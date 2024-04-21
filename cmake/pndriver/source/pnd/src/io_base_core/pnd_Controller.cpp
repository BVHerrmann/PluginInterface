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
/*  F i l e               &F: pnd_Controller.cpp                        :F&  */
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
#define LTRC_ACT_MODUL_ID   1122
#define PND_MODULE_ID       1122

#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnd_Controller.h"
#include "pnd_IOSystemReconfig.h"

#ifdef PND_CFG_ISO_SUPPORT
#include "pnd_ISOUser.h"
#endif

CController::CController(PNIO_UINT32 UserHandle, IPndAdapter* pPndAdapter) : CIOBaseCoreCommon(PNIO_UINT16(UserHandle), PND_CLOSED, 0, 0, pPndAdapter),
    m_pUserCbf_Mode(PNIO_NULL),
    m_pUserCbf_DevAct(PNIO_NULL),
    m_pUserCbf_Alarm(PNIO_NULL),
    m_pUserCbf_DR_Read(PNIO_NULL),
    m_pUserCbf_DR_Write(PNIO_NULL),
    m_pCbf_AlarmInd(PNIO_NULL),
    m_pUserCbf_DiagReq(PNIO_NULL),
    m_pUserCbf_IosytemReconfig(PNIO_NULL),
    m_ExtPar(0),
    m_Mode(PNIO_MODE_OFFLINE),
	m_pIOSystemReconfig(0) 
#ifdef PND_CFG_ISO_SUPPORT
    ,m_pUserCbf_StartOpInd(PNIO_NULL)
    ,m_pUserCbf_OpFaultInd(PNIO_NULL)
    ,m_isIsoDataConsistent(PNIO_FALSE)
#endif
	,m_nr_of_alarm_res(0)
	,m_set_mode_req_running(PNIO_FALSE)
	,m_set_mode_sync_req_running(PNIO_FALSE)
	,m_iosys_reconfig_running(PNIO_FALSE)
{
    pnd_memset(&m_pndDevNumberList, 0, sizeof(m_pndDevNumberList));         // PND_CORE Device Number List
    pnd_memset(&m_pndSlotNumberList, 0, sizeof(m_pndSlotNumberList));       // PND_CORE Slot Number List
    pnd_memset(&m_pndSubslotNumberList, 0, sizeof(m_pndSubslotNumberList)); // PND_CORE Subslot Number List

    m_PendingActivationList.clear();
    m_InProgressActivationList.clear();

    pnd_memset(s_PndLogAddr.logAddrList, 0, sizeof(s_PndLogAddr.logAddrList));
    s_PndLogAddr.isInitialized = PNIO_FALSE;

    pnd_rqb_list.size = PND_PNIO_MAX_RESOURCES;
    pnd_alloc_rqb_list(pnd_rqb_list);

    pnd_datarecord_read_rqb_list.size = PND_PNIO_MAX_RECORD_READ_WRITE_RESOURCES;
    pnd_alloc_rqb_list(pnd_datarecord_read_rqb_list);

    pnd_datarecord_write_rqb_list.size = PND_PNIO_MAX_RECORD_READ_WRITE_RESOURCES;
    pnd_alloc_rqb_list(pnd_datarecord_write_rqb_list);
}

CController::~CController(void)
{
    pnd_free_rqb_list(pnd_rqb_list);
    pnd_free_rqb_list(pnd_datarecord_read_rqb_list);
    pnd_free_rqb_list(pnd_datarecord_write_rqb_list);
	m_pIOSystemReconfig = 0;
}

CController* CController::get_instance(PNIO_UINT32 CpIndex)
{
    if (CpIndex < PND_MAX_INSTANCES)
    {
        if (s_PndUserCoreCpList[CpIndex].instance_pointer != PNIO_NULL)
            return s_PndUserCoreCpList[CpIndex].instance_pointer;
        else
            return PNIO_NULL;
    }
    else
        return PNIO_NULL;
}

IIODU* CController::get_IODU(PNIO_UINT32 CpIndex)
{
    if (CpIndex < PND_MAX_INSTANCES)
    {
        if (s_PndUserCoreCpList[CpIndex].instance_pointer != PNIO_NULL)
            return s_PndUserCoreCpList[CpIndex].IODU_pointer;
        else
            return PNIO_NULL;
    }
    else
        return PNIO_NULL;
}

PNIO_VOID CController::remove_instance(PNIO_UINT16 UserHandle)
{
    delete s_PndUserCoreCpList[UserHandle].instance_pointer;
}

PND_RQB_LIST_TYPE *CController::getRQBList(LSA_OPCODE_TYPE opcode)
{
    PND_RQB_LIST_TYPE* pRqbList;
    if (opcode == PND_OPC_REC_READ)
    {
        pRqbList = &pnd_datarecord_read_rqb_list;
    }
    else if (opcode == PND_OPC_REC_WRITE)
    {
        pRqbList = &pnd_datarecord_write_rqb_list;
    }
    else
    {
        pRqbList = &pnd_rqb_list;
    }

    return pRqbList;
}

PNIO_VOID CController::setExtPar( PNIO_UINT32 extPar )
{
    m_ExtPar = extPar;
}

PNIO_UINT32 CController::getExtPar() const
{
    return m_ExtPar;
}

PNIO_VOID CController::controller_open_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    CController *pThis = (CController *)pInst;

    LSA_UNUSED_ARG(pRqb);

    pnd_set_event(pThis->m_PndWaitEvent);
}

PNIO_VOID CController::controller_close_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    CController *pThis = (CController *)pInst;

    LSA_UNUSED_ARG(pRqb);

    pnd_set_event(pThis->m_PndWaitEvent);
}

PNIO_VOID CController::alarm_indication(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    CController *pThis = (CController *)pInst;
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    PND_ALARM_PTR_TYPE     pAlarm;

    pAlarm = &rqb_ptr->args.iob_alarm;

    /* fill in alarm data */

    if (LSA_RQB_GET_RESPONSE(rqb_ptr) == PNIO_OK_CANCEL)
    {
        // pThis->pnd_lower_rqb_free(rqb_ptr);
        pnd_mem_free(rqb_ptr);
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
            cbf_prm.CbeType = PNIO_CBE_ALARM_IND;

            cbf_prm.u.AlarmInd.IndRef = pAlarm->ind_ref;

            switch (pAlarm->alarm.AlarmType)
            {
            case PNIO_ALARM_DEV_FAILURE:
            {
                IIODU *pIODU = pThis->get_IODU(cbf_prm.Handle);

                pIODU->update_IQ_table(pAlarm->alarm.DeviceNum, PNIO_FALSE/*offline*/);
                break;
            }
            case PNIO_ALARM_DEV_RETURN:
            {
                IIODU *pIODU = pThis->get_IODU(cbf_prm.Handle);

                pIODU->update_IQ_table(pAlarm->alarm.DeviceNum, PNIO_TRUE/*online*/);
                break;
            }
            default:
            {
                // Do nothing;
                break;
            }
            } // end switch


            switch (pAlarm->alarm.AlarmType)
            {
            case PNIO_ALARM_DEV_FAILURE:
            case PNIO_ALARM_DEV_RETURN:
            case PNIO_ALARM_DIAGNOSTIC:
            case PNIO_ALARM_PROCESS:
            case PNIO_ALARM_PULL:
            case PNIO_ALARM_PLUG:
            case PNIO_ALARM_STATUS:
            case PNIO_ALARM_UPDATE:
            case PNIO_ALARM_REDUNDANCY:
            case PNIO_ALARM_CONTROLLED_BY_SUPERVISOR:
            case PNIO_ALARM_RELEASED_BY_SUPERVISOR:
            case PNIO_ALARM_PLUG_WRONG:
            case PNIO_ALARM_RETURN_OF_SUBMODULE:
            case PNIO_ALARM_DIAGNOSTIC_DISAPPEARS:
            case PNIO_ALARM_MCR_MISMATCH:
            case PNIO_ALARM_PORT_DATA_CHANGED:
            case PNIO_ALARM_SYNC_DATA_CHANGED:
            case PNIO_ALARM_ISOCHRONE_MODE_PROBLEM:
            case PNIO_ALARM_NETWORK_COMPONENT_PROBLEM:
            case PNIO_ALARM_UPLOAD_AND_STORAGE:
            case PNIO_ALARM_PULL_MODULE:
            {
                AlarmData.AlarmType = pAlarm->alarm.AlarmType;
                AlarmData.AlarmPriority = pAlarm->alarm.AlarmPriority;
                AlarmData.DeviceNum = pAlarm->alarm.DeviceNum;
                AlarmData.SlotNum = pAlarm->alarm.SlotNum;
                AlarmData.SubslotNum = pAlarm->alarm.SubslotNum;
                AlarmData.CpuAlarmPriority = pAlarm->alarm.CpuAlarmPriority;
                AlarmData.PnioCompatModtype = pAlarm->alarm.PnioCompatModtype;
                AlarmData.ModDiffBlock.ModDiffBlockLength = pAlarm->alarm.ModDiffBlock.ModDiffBlockLength;
                AlarmData.ModDiffBlock.pModDiffBlock = pAlarm->alarm.ModDiffBlock.pModDiffBlock;

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

PNIO_UINT32 CController::alarm_provide(PNIO_UINT16 nrOfAlarmResource)
{
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>alarm_provide");
    
    CIOBaseCoreCommon::alarm_provide(nrOfAlarmResource, PNIO_REF_IOC, CController::alarm_indication);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<alarm_provide");

    return PNIO_OK;
}

PNIO_VOID CController::alarm_response(PNIO_VOID_PTR_TYPE  rqb_ptr)
{
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>alarm_response");

    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;
    PND_ASSERT(pRQB);

    CIOBaseCoreCommon::alarm_response(pRQB, PNIO_REF_IOC, CController::alarm_indication);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<alarm_response");
}

PNIO_VOID CController::write_snmp_record(PNIO_VOID_PTR_TYPE pInst)
{
    PND_RQB_PTR_TYPE          pRQB;
    PND_PRM_WRITE_DATA_TYPE*  pPrmWrite;
    CController *pThis = (CController *)pInst;

    pRQB = (PND_RQB_PTR_TYPE)pnd_mem_alloc(sizeof(*pRQB));
    PND_ASSERT(pRQB);

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_PRM_WRITE);

    pPrmWrite = &pRQB->args.prm_write;

    pPrmWrite->pnd_handle.cp_nr = (PNIO_UINT16)pThis->m_CpIndex;
    pPrmWrite->edd_port_id = 0;
    pPrmWrite->mk_rema = 1;           // mk_rema have to be SET.
    pPrmWrite->record_index = 0xF8000002;  /* MultipleSNMPRecord */
    pPrmWrite->slot_number = 0;
    pPrmWrite->subslot_number = 0x8000;

    pnd_request_start(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request, (PNIO_VOID_PTR_TYPE)pRQB);
}

PNIO_UINT32 CController::register_cbf(PNIO_CBE_TYPE CbeType, PNIO_CBF cbf)
{
    PNIO_UINT32 Ret = PNIO_OK;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>register_cbf");

    if (m_Status != PND_OPENED)
        return PNIO_ERR_WRONG_HND;

    if (!cbf)
        return PNIO_ERR_PRM_CALLBACK;

    if (m_Mode != PNIO_MODE_OFFLINE)
        return PNIO_ERR_MODE_VALUE;

    switch (CbeType) {
    case PNIO_CBE_MODE_IND:
        if (m_pUserCbf_Mode) {
            Ret = PNIO_ERR_ALREADY_DONE;
        }
        else {
            m_pUserCbf_Mode = cbf;
            Ret = PNIO_OK;
        }
        break;
    case PNIO_CBE_DEV_ACT_CONF:
        if (m_pUserCbf_DevAct) {
            Ret = PNIO_ERR_ALREADY_DONE;
        }
        else {
            m_pUserCbf_DevAct = cbf;
            Ret = PNIO_OK;
        }
        break;
    case PNIO_CBE_CP_STOP_REQ:

        break;

    case PNIO_CBE_CTRL_DIAG_CONF:
        if (m_pUserCbf_DiagReq) {
            Ret = PNIO_ERR_ALREADY_DONE;
        }
        else {
            m_pUserCbf_DiagReq = cbf;
            Ret = PNIO_OK;
        }
        break;

    case PNIO_CBE_IOSYSTEM_RECONFIG:
        if (m_pUserCbf_IosytemReconfig) {
            Ret = PNIO_ERR_ALREADY_DONE;
        }
        else {
            m_pUserCbf_IosytemReconfig = cbf;
            Ret = PNIO_OK;
        }
        break;
    default:
        Ret = PNIO_ERR_PRM_TYPE;
        break;
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<register_cbf");

    return Ret;
}

PNIO_VOID CController::de_register_all_cbf()
{
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>de_register_cbf");

    m_pUserCbf_Mode = 0;
    m_pUserCbf_DevAct = 0;
    m_pUserCbf_Alarm = 0;
    m_pUserCbf_DR_Read = 0;
    m_pUserCbf_DR_Write = 0;
    m_pCbf_AlarmInd = 0;
    m_pUserCbf_DiagReq = 0;
    m_pUserCbf_IosytemReconfig = 0;
#ifdef PND_CFG_ISO_SUPPORT
    m_pUserCbf_StartOpInd = 0;
    m_pUserCbf_OpFaultInd = 0;
#endif  
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<de_register_cbf");
}

PNIO_UINT32 CController::controller_open(PNIO_UINT32 CpIndex,
    PNIO_UINT32 ExtPar, PNIO_CBF cbf_RecReadConf,
    PNIO_CBF cbf_RecWriteConf, PNIO_CBF cbf_AlarmInd,
    PNIO_UINT32 * pApplHandle)
{
    PNIO_UINT32 result = PNIO_OK;
    CController *pThis = NULL;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>controller_open");

    if (!pApplHandle)
        return PNIO_ERR_PRM_HND;

    if (!cbf_RecReadConf || !cbf_RecWriteConf)
        return PNIO_ERR_PRM_CALLBACK;

    if (!(ExtPar & PNIO_CEP_MODE_CTRL))
        return PNIO_ERR_PRM_EXT_PAR;

    if (ExtPar & PNIO_CEP_MODE_CTRL) {
        // to do : from SCCI
        // only one instance mit PNIO_CEP_MODE_CTRL on CP can exist
    }

    *pApplHandle = CController::get_handle_by_index(CpIndex);
    if (*pApplHandle == PND_INVALID_HANDLE)
        return PNIO_ERR_PRM_CP_ID;

    pThis = CController::get_instance(CpIndex);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_CREATE_INSTANCE;

    if (pThis->m_Status != PND_CLOSED)
        return PNIO_ERR_WRONG_HND;

    pThis->m_ExtPar = ExtPar;
    pThis->m_CpIndex = CpIndex;
    pThis->m_pUserCbf_Alarm = cbf_AlarmInd;
    pThis->m_pUserCbf_DR_Read = cbf_RecReadConf;
    pThis->m_pUserCbf_DR_Write = cbf_RecWriteConf;
    pThis->m_Mode = PNIO_MODE_OFFLINE;

    // Wait on callback, after request 
    /*******/

    PND_RQB_PTR_TYPE               pRQB;
    PND_IOB_CORE_OPEN_PTR_TYPE     pOpen;
    PNIO_VOID_PTR_TYPE  pnd_laddr_list_ptr;
    PNIO_VOID_PTR_TYPE  pnd_dev_number_list_ptr;
    PNIO_VOID_PTR_TYPE  pnd_slot_number_list_ptr;
    PNIO_VOID_PTR_TYPE  pnd_subslot_number_list_ptr;

    pRQB = (PND_RQB_PTR_TYPE)pnd_mem_alloc(sizeof(*pRQB));
    // pRQB = pThis->pnd_lower_rqb_alloc(PND_OPC_CONTROLLER_OPEN, 0, pThis, CController::controller_open_done);

    if (!pRQB)
    {
        return PNIO_ERR_NO_RESOURCE;
    }

    if (pThis->s_PndLogAddr.isInitialized != PNIO_TRUE)
    {
        pnd_laddr_list_ptr = pnd_mem_alloc(sizeof(pThis->s_PndLogAddr.logAddrList));
    }
    else
    {
        pnd_laddr_list_ptr = PNIO_NULL;
    }

    pnd_dev_number_list_ptr = pnd_mem_alloc(sizeof(pThis->m_pndDevNumberList));
    pnd_slot_number_list_ptr = pnd_mem_alloc(sizeof(pThis->m_pndSlotNumberList));
    pnd_subslot_number_list_ptr = pnd_mem_alloc(sizeof(pThis->m_pndSubslotNumberList));

    if (pRQB == 0)
        PND_FATAL("mem alloc failed");

    pOpen = &pRQB->args.iob_open;

    pnd_memset(pOpen, 0, sizeof(PND_IOB_CORE_OPEN_TYPE));

    pOpen->laddr_list_ptr = pnd_laddr_list_ptr;
    pOpen->dev_number_list_ptr = pnd_dev_number_list_ptr;
    pOpen->slot_number_list_ptr = pnd_slot_number_list_ptr;
    pOpen->subslot_number_list_ptr = pnd_subslot_number_list_ptr;

    pOpen->pnd_handle.cp_nr = (PNIO_UINT16)CpIndex;

    PND_ASSERT(pRQB);

    result = pnd_alloc_event(&pThis->m_PndWaitEvent);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }


    pnd_reset_event(pThis->m_PndWaitEvent);

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_CONTROLLER_OPEN);
    LSA_RQB_SET_HANDLE(pRQB, 0);
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
    PND_RQB_SET_USER_CBF_PTR(pRQB, CController::controller_open_done);

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event(pThis->m_PndWaitEvent);

    result = pnd_free_event(pThis->m_PndWaitEvent);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pThis->m_nr_of_alarm_res = pOpen->nr_of_alarm_res;

    result = pRQB->_response;
    if (result != PNIO_RET_OK)
        return PNIO_ERR_INTERNAL;
    else
        result = PNIO_OK;

    /******/
    result = pOpen->pnio_err;
    if (result != PNIO_OK)
        return result;

    if (pThis->s_PndLogAddr.isInitialized != PNIO_TRUE)
    {
        pnd_memcpy(pThis->s_PndLogAddr.logAddrList, pOpen->laddr_list_ptr, sizeof(pThis->s_PndLogAddr.logAddrList));
        pThis->s_PndLogAddr.isInitialized = PNIO_TRUE;

        pnd_mem_free(pnd_laddr_list_ptr);
    }
    pnd_memcpy(pThis->m_pndDevNumberList, pOpen->dev_number_list_ptr, sizeof(pThis->m_pndDevNumberList));
    pnd_memcpy(pThis->m_pndSlotNumberList, pOpen->slot_number_list_ptr, sizeof(pThis->m_pndSlotNumberList));
    pnd_memcpy(pThis->m_pndSubslotNumberList, pOpen->subslot_number_list_ptr, sizeof(pThis->m_pndSubslotNumberList));

    // Create addressMap
    for (PNIO_UINT32 i = 0; i < PND_LADDR_MAX; i++)
    {
        if (pThis->s_PndLogAddr.logAddrList[i] >= PDSUBSLOTINTERFACE && pThis->s_PndLogAddr.logAddrList[i] <= IOCSUBSLOT)
        {
            GEO_ADDR geoAddr;
            geoAddr.device = pThis->m_pndDevNumberList[i];
            geoAddr.slot = pThis->m_pndSlotNumberList[i];
            geoAddr.subslot = pThis->m_pndSubslotNumberList[i];

            pThis->AddNode(i, geoAddr);
        }
    }

    pnd_mem_free(pnd_dev_number_list_ptr);
    pnd_mem_free(pnd_slot_number_list_ptr);
    pnd_mem_free(pnd_subslot_number_list_ptr);

    pnd_mem_free(pRQB);

    result = pThis->alarm_provide(pThis->m_nr_of_alarm_res);
    if (result != PNIO_OK)
        return  PNIO_ERR_NO_RESOURCE;

    pThis->write_snmp_record(pThis);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<controller_open");

    pThis->m_set_status(PND_OPENED);

    result = pnd_alloc_event(&pThis->pnd_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_set_event(pThis->pnd_rqb_list.resource_free_event);

    result = pnd_alloc_event(&pThis->pnd_datarecord_read_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_set_event(pThis->pnd_datarecord_read_rqb_list.resource_free_event);

    result = pnd_alloc_event(&pThis->pnd_datarecord_write_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_set_event(pThis->pnd_datarecord_write_rqb_list.resource_free_event);

    return PNIO_OK;
}

PNIO_UINT32 CController::controller_close(PNIO_UINT32 ApplHandle)
{
    PNIO_UINT32 result = PNIO_OK;
    CController *pThis = NULL;
    PND_RQB_PTR_TYPE                pRQB;
    PND_IOB_CORE_CLOSE_PTR_TYPE     pClose;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>controller_close");

    pThis = CController::get_instance(ApplHandle);

    PND_ASSERT(pThis != PNIO_NULL);

    if (!pThis)
        return PNIO_ERR_WRONG_HND;

    if (PND_CLOSED == pThis->m_get_status())
    {
        return PNIO_ERR_WRONG_HND;
    }

    pThis->de_register_all_cbf();

    pnd_wait_event(pThis->pnd_rqb_list.resource_free_event);
    pnd_wait_event(pThis->pnd_datarecord_read_rqb_list.resource_free_event);
    pnd_wait_event(pThis->pnd_datarecord_write_rqb_list.resource_free_event);

    if (pThis->m_Mode != PNIO_MODE_OFFLINE)
    {
        result = pThis->set_mode(PNIO_MODE_OFFLINE);
        if (result != PNIO_OK)
        {
            return result;
        }

        pnd_wait_event(pThis->pnd_rqb_list.resource_free_event);
    }

    PND_ASSERT(pThis->m_Mode == PNIO_MODE_OFFLINE);

    result = pnd_alloc_event(&pThis->m_PndWaitEvent);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }


    pRQB = (PND_RQB_PTR_TYPE)pnd_mem_alloc(sizeof(*pRQB));
    // pRQB = pnd_lower_rqb_alloc(PND_OPC_CONTROLLER_CLOSE,0,pThis,CController::controller_close_done);

    if (!pRQB)
    {
        return PNIO_ERR_INTERNAL;
    }

    pClose = &pRQB->args.iob_close;

    pnd_memset(pClose, 0, sizeof(PND_IOB_CORE_CLOSE_TYPE));

    pClose->pnd_handle.cp_nr = (PNIO_UINT16)pThis->m_CpIndex;

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_CONTROLLER_CLOSE);
    LSA_RQB_SET_HANDLE(pRQB, 0);
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
    PND_RQB_SET_USER_CBF_PTR(pRQB, CController::controller_close_done);

    pnd_reset_event(pThis->m_PndWaitEvent);

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event(pThis->m_PndWaitEvent);

    result = pClose->pnio_err;

    pnd_mem_free(pRQB);

    result = pnd_free_event(pThis->m_PndWaitEvent);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }


    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<controller_close");

    result = pnd_free_event(pThis->pnd_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    result = pnd_free_event(pThis->pnd_datarecord_read_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    result = pnd_free_event(pThis->pnd_datarecord_write_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    // empty addressMap
    pThis->EmptyMap();

    pThis->m_set_status(PND_CLOSED);

    return PNIO_OK;
}

PNIO_VOID CController::set_mode_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    CController *pThis = (CController *)pInst;
    PNIO_CBE_PRM cbf_prm;

    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CController::set_mode_done()");

    if (LSA_RQB_GET_OPCODE(rqb_ptr) != PND_OPC_SET_MODE)
    {
        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_MODE_IND Wrong Opcode ; %d", LSA_RQB_GET_OPCODE(rqb_ptr));
        PND_FATAL("wrong OPCODE");
    }

    cbf_prm.Handle = pThis->get_handle(pThis);

    if (pThis->m_pUserCbf_Mode)
    {

        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_MODE_IND call user callback() ends ApplHandle= %d Mode= %d", cbf_prm.Handle, pThis->m_pUserCbf_Mode);

        cbf_prm.CbeType = PNIO_CBE_MODE_IND;
        cbf_prm.u.ModeInd.Mode = (PNIO_MODE_TYPE)rqb_ptr->args.iob_set_mode.mode;

        pThis->m_pUserCbf_Mode(&cbf_prm);
    }
    else
    {
        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF_MODE_IND user callback() defined. ApplHandle= %d Mode= %d", cbf_prm.Handle, pThis->m_pUserCbf_Mode);
    }
#ifdef PND_CFG_ISO_SUPPORT
    if(rqb_ptr->args.iob_set_mode.pnio_err!= PNIO_OK && pThis->m_pUserCbf_OpFaultInd != PNIO_NULL)
    {
        pThis->setISOUserSynchronization(PNIO_MODE_OFFLINE);
    }
#endif
    pThis->m_Mode = (PNIO_MODE_TYPE)rqb_ptr->args.iob_set_mode.mode;

    pThis->m_set_mode_req_running = PNIO_FALSE;

    pThis->pnd_lower_rqb_free(pRqb);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CController::set_mode_done()");
}

PNIO_UINT32 CController::set_mode(PNIO_MODE_TYPE Mode)
{
    PND_RQB_PTR_TYPE          pRQB;
    PND_SET_MODE_PTR_TYPE     pSetMode;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>set_mode");

    if (!(m_ExtPar & PNIO_CEP_MODE_CTRL))
        return PNIO_ERR_SET_MODE_NOT_ALLOWED;

    if (Mode != PNIO_MODE_OFFLINE &&
        Mode != PNIO_MODE_CLEAR &&
        Mode != PNIO_MODE_OPERATE)
        return PNIO_ERR_MODE_VALUE;

    if (m_Mode == Mode)
        return PNIO_OK;

    if (m_Status != PND_OPENED)
        return PNIO_ERR_WRONG_HND;

#ifdef PND_CFG_ISO_SUPPORT
    // PNIO_CBE_OPFAULT_IND must be registered.
    if(this->m_pUserCbf_StartOpInd != PNIO_NULL && this->m_pUserCbf_OpFaultInd == PNIO_NULL)
    {
        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, ">>Wrong callback sequence");
    }

	CISOUser* instance = CISOUser::getInstance(m_CpIndex);
	
	// If ISO User does not exist ISO User syncronization can not be called. And in case it exists, during shutdown 
	// (i.e. Mode == PNIO_MODE_OFFLINE) even if the opfault cbf is nulled in CController::de_register_all_cbf()
	// ISO User syncronization have to be called.
    if( instance != PNIO_NULL && ( Mode == PNIO_MODE_OFFLINE || this->m_pUserCbf_OpFaultInd != PNIO_NULL ))
	{        
        PNIO_UINT32 result = setISOUserSynchronization(Mode);
        if(result != PNIO_OK) 
        { 
            PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, ">>ISO synchronization start failed. Error Code: %x", result);
        }
    }  
#endif
    pRQB = pnd_lower_rqb_alloc(PND_OPC_SET_MODE, 0, this, CController::set_mode_done);

    if (!pRQB)
    {
        return PNIO_ERR_NO_RESOURCE;
    }

    pSetMode = &pRQB->args.iob_set_mode;

    pnd_memset(pSetMode, 0, sizeof(PND_SET_MODE_TYPE));

    pSetMode->mode = Mode; // Set Mode in RQB

    pSetMode->pnd_handle.cp_nr = this->m_hInstanceHandle;

    if (m_set_mode_req_running == PNIO_TRUE)
    {
        pnd_lower_rqb_free(pRQB);
        return PNIO_ERR_SEQUENCE;
        // pnd_add_to_rqb_queue( &m_pnd_lower_rqb_queu,  pRQB);
    }
    else
    {
        m_set_mode_req_running = PNIO_TRUE;
        pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<set_mode");
    
    return PNIO_OK;
}

PNIO_VOID CController::set_synchronization_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    CController *pThis = (CController *)pInst;

    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CController::set_synchronization_done()");

    if (LSA_RQB_GET_OPCODE(rqb_ptr) != PND_OPC_ISO_SYNC)
    {
        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_MODE_IND Wrong Opcode ; %d", LSA_RQB_GET_OPCODE(rqb_ptr));
        PND_FATAL("wrong OPCODE");
    }

    if (rqb_ptr->args.iob_set_mode.pnio_err != PNIO_OK)
    {
        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "Failed to start synchronization. Errorcode; %x", rqb_ptr->args.iob_set_mode.pnio_err);
    }
    
    pThis->m_set_mode_sync_req_running = PNIO_FALSE;
    
    pThis->pnd_lower_rqb_free(pRqb);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CController::set_synchronization_done()");
}

#ifdef PND_CFG_ISO_SUPPORT
/**
* \brief This method starts isochronous sychronization separately (standalone).
*		 In case of a isochronous callback registration, this method is called from CController::set_mode(PNIO_MODE_TYPE Mode).
* \note  If it is re-called before a previous call is completed, it returns sequence error.
*
* \param[in] Mode : PNIO_MODE_OFFLINE, PNIO_MODE_ONLINE or PNIO_MODE_CLEAR
*
* \return If successful, "PNIO_OK" is returned. If an error occurs,
*		   the following values are possible (for the meaning, refer to the comments in the header file "pnioerrx.h"):
* \return PNIO_ERR_SEQUENCE
* \return PNIO_ERR_NO_RESOURCE
*/
PNIO_UINT32 CController::setISOUserSynchronization(PNIO_MODE_TYPE Mode)
{
    PND_RQB_PTR_TYPE          pRQB;
    PND_SET_MODE_PTR_TYPE     pSetMode;

	if (m_set_mode_sync_req_running == PNIO_TRUE)
	{
		PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "<<CController::setISOUserSynchronization is recalled before completed.");
		return PNIO_ERR_SEQUENCE;
	}

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CController::setISOUserSynchronization");    

    pRQB = pnd_lower_rqb_alloc(PND_OPC_ISO_SYNC, 0, this, CController::set_synchronization_done);

    if (!pRQB)
    {
        return PNIO_ERR_NO_RESOURCE;
    }
    
    pSetMode = &pRQB->args.iob_set_mode;

    pnd_memset(pSetMode, 0, sizeof(PND_SET_MODE_TYPE));

    pSetMode->mode = Mode; // Set Mode in RQB
    pSetMode->pnd_handle.cp_nr = this->m_hInstanceHandle;

    m_set_mode_sync_req_running = PNIO_TRUE;
    pnd_request_start (PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
    
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CController::setISOUserSynchronization");
    
    return PNIO_OK;
}
#endif

PNIO_UINT32 CController::ctrl_diag_request(LSA_HANDLE_TYPE handle, PNIO_CTRL_DIAG *pDiagInfo)
{
    PND_RQB_PTR_TYPE       pRQB = PNIO_NULL;
    PND_DIAG_REQ_PTR_TYPE  pDiag = PNIO_NULL;
    PNIO_ADDR *            pAddr = &(pDiagInfo->u.Addr);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>ctrl_diag_request");

    if (this->m_Status != PND_OPENED)
        return PNIO_ERR_WRONG_HND;

    if (!pDiagInfo) {
        return PNIO_ERR_PRM_POINTER;
    }

    if (pDiagInfo->DiagService != PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST &&
        pDiagInfo->DiagService != PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC &&
        pDiagInfo->DiagService != PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO) {
        return PNIO_ERR_PRM_INVALIDARG;
    }

    // LADDR is given only in PNIO_CTRL_DIAG_DEVICE_STATE service.
    if (pDiagInfo->DiagService == PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC)
    {
        PNIO_UINT32 result = PNIO_OK;

        if (!pAddr ||
            pAddr->AddrType != PNIO_ADDR_LOG ||
            (pAddr->IODataType != PNIO_IO_IN && pAddr->IODataType != PNIO_IO_OUT))
            return PNIO_ERR_PRM_ADD;

        result = check_log_adress(pAddr);
        if (result != PNIO_OK)
        {
            return result;
        }
    }

    pRQB = (PND_RQB_PTR_TYPE)pnd_mem_alloc(sizeof(*pRQB));

    if (!pRQB)
    {
        return PNIO_ERR_INTERNAL;
    }

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_DIAG_REQ);
    LSA_RQB_SET_HANDLE(pRQB, handle);
    PND_RQB_SET_USER_INST_PTR(pRQB, this);
    PND_RQB_SET_USER_CBF_PTR(pRQB, CController::ctrl_diag_req_done);

    pDiag = &pRQB->args.diag_req;

    pDiag->pnio_err = PNIO_OK;
    pDiag->DiagReq = (PNIO_CTRL_DIAG *)pnd_mem_alloc(sizeof(PNIO_CTRL_DIAG));
    PND_ASSERT(pDiag->DiagReq);

    pnd_memset(pDiag->DiagReq, 0, sizeof(PNIO_CTRL_DIAG));

    // transferring the diagnostic information
    pnd_memcpy(pDiag->DiagReq, pDiagInfo, sizeof(PNIO_CTRL_DIAG));

    pDiag->pnd_handle.cp_nr = (PNIO_UINT16)m_CpIndex;

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<ctrl_diag_request");

    return PNIO_OK;
}

PNIO_UINT32 CController::checkParametersForIOSystemReconfig(PNIO_UINT32 handle, const CIOSystemReconfig *ioSystemReconfig) const
{
    if (this->m_Status != PND_OPENED)
    {
        return PNIO_ERR_WRONG_HND;
    }

    if (!(m_ExtPar & PNIO_CEP_MODE_CTRL))
    {
        return PNIO_ERR_DEV_ACT_NOT_ALLOWED;
    }

    if (handle != get_handle(this))
    {
        return PNIO_ERR_PRM_HND;
    }

    PNIO_IOS_RECONFIG_MODE mode = ioSystemReconfig->getMode();

    if ((mode != PNIO_IOS_RECONFIG_MODE_DEACT) && (mode != PNIO_IOS_RECONFIG_MODE_TAILOR))
    {
        return PNIO_ERR_PRM_TYPE;
    }

    if (m_iosys_reconfig_running == PNIO_TRUE)
    {
        return PNIO_ERR_NOT_REENTERABLE;
    }

    if (mode == PNIO_IOS_RECONFIG_MODE_TAILOR)
    {
        PNIO_UINT32 result = ioSystemReconfig->validateReconfigParams();

        if (result != PNIO_TRUE)
        {
            return PNIO_ERR_PRM_BUF;
        }
    }
    return PNIO_OK;
}

PNIO_UINT32 CController::iosystem_reconfig(PNIO_UINT32 handle, CIOSystemReconfig *ioSystemReconfig)
{
    PND_RQB_PTR_TYPE pRQB;
    PNIO_UINT32 result;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>iosystem_reconfig");

    if(ioSystemReconfig == PNIO_NULL)
    {
        return PNIO_ERR_PRM_ADD; // TODO: replace with suitable error code.
    }

    result = checkParametersForIOSystemReconfig(handle, ioSystemReconfig);

    if (result != PNIO_OK)
    {
        delete ioSystemReconfig;

        return result;
    }

    m_iosys_reconfig_running = PNIO_TRUE;

    pRQB = (PND_RQB_PTR_TYPE)m_pPndAdapter->memAlloc(sizeof(*pRQB));

    if (!pRQB)
    {
        delete ioSystemReconfig;

        return PNIO_ERR_INTERNAL;
    }
    pnd_memset(pRQB, 0, sizeof(PND_RQB_TYPE));

    m_pIOSystemReconfig = ioSystemReconfig;
    m_pIOSystemReconfig->setControllerInstance(this);
    m_pIOSystemReconfig->processStateMachine(pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<iosystem_reconfig");

    return PNIO_OK;
}

PNIO_UINT32 CController::device_activate(const PNIO_ADDR * pAddr, PNIO_DEV_ACT_TYPE mode)
{
    PNIO_UINT32 result = PNIO_OK;

    PND_RQB_PTR_TYPE          pRQB;
    PND_DEV_ACTIVATE_PTR_TYPE     pActivate;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>device_activate");

    if (this->m_Status != PND_OPENED)
        return PNIO_ERR_WRONG_HND;

    if (!pAddr ||
        pAddr->AddrType != PNIO_ADDR_LOG ||
        (pAddr->IODataType != PNIO_IO_IN && pAddr->IODataType != PNIO_IO_OUT))
        return PNIO_ERR_PRM_ADD;

    // sanity addres bound
    if (pAddr->u.Addr > 0x7FFF)
        return PNIO_ERR_PRM_ADD;

    if (!(m_ExtPar & PNIO_CEP_MODE_CTRL))
        return PNIO_ERR_DEV_ACT_NOT_ALLOWED;

    if (mode != PNIO_DA_FALSE && mode != PNIO_DA_TRUE)
        return PNIO_ERR_MODE_VALUE;

    if (IsInProgressListNotFull() == PNIO_FALSE)
    {
        return  PNIO_ERR_NO_RESOURCE;
    }

    pRQB = pnd_lower_rqb_alloc(PND_OPC_DEVICE_ACTIVATE, 0, this, device_activate_done);

    if (!pRQB)
    {
        return PNIO_ERR_NO_RESOURCE;
    }

    AddDeviceToInProgressList(pAddr->u.Addr);

    pActivate = &pRQB->args.iob_dev_act;

    pnd_memset(pActivate, 0, sizeof(PND_DEV_ACTIVATE_TYPE));

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_DEVICE_ACTIVATE);
    PND_RQB_SET_USER_CBF_PTR(pRQB, device_activate_done);
    PND_RQB_SET_USER_INST_PTR(pRQB, this);

    pActivate->Addr.AddrType = pAddr->AddrType;
    pActivate->Addr.IODataType = pAddr->IODataType;
    pActivate->Addr.u.Addr = pAddr->u.Addr;

    pActivate->pnd_handle.cp_nr = this->m_hInstanceHandle;

    pActivate->DevActMode = mode;

    result = pnd_alloc_event(&this->m_PndWaitEvent);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    result = pnd_free_event(this->m_PndWaitEvent);
    if (result != PNIO_RET_OK)          // PNIO_RET_OK != PNIO_OK !!!
    {
        PND_FATAL("event free failed");
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<device_activate");

    return PNIO_OK;
}

PNIO_UINT32 CController::record_read(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex, PNIO_UINT32 Length)
{
    PNIO_UINT32 result = PNIO_OK;
    PND_RQB_PTR_TYPE                pRQB;
    PND_RECORD_READ_WRITE_PTR_TYPE  pRecRead;
    PNIO_UINT8 * pRecBuffer;

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

    // check if not controller address
    if (s_PndLogAddr.logAddrList[pAddr->u.Addr] != IOCDEVICE &&
        s_PndLogAddr.logAddrList[pAddr->u.Addr] != IOCSLOT   &&
        s_PndLogAddr.logAddrList[pAddr->u.Addr] != IOCSUBSLOT)
    {
        return PNIO_ERR_PRM_ADD;
    }

    if (Length == 0 || Length > PNIOI_DREC_MAX_SIZE)
        return PNIO_ERR_VALUE_LEN;

    if (RecordIndex > 0xFFFF)
        return PNIO_ERR_PRM_REC_INDEX;

    pRecBuffer = (PNIO_UINT8 *)pnd_mem_alloc(Length);
    if (pRecBuffer == 0)
        PND_FATAL("mem alloc failed");

    pnd_memset(pRecBuffer, 0, Length);

    pRQB = pnd_lower_rqb_alloc(PND_OPC_REC_READ, 0, this, read_write_record_done);

    if (!pRQB)
    {
        PNIO_CBE_PRM cbf_prm;

        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>read_read CBF");

        cbf_prm.CbeType = PNIO_CBE_REC_READ_CONF;
        cbf_prm.Handle = get_handle(this);
        cbf_prm.u.RecReadConf.Err.ErrCode = 0xDE; // IODReadRes
        cbf_prm.u.RecReadConf.Err.ErrDecode = 0x80; // PNIORW
        cbf_prm.u.RecReadConf.Err.ErrCode1 = 0xC3; // Resource  + resource unavailable
        cbf_prm.u.RecReadConf.Err.ErrCode2 = 0;
        cbf_prm.u.RecReadConf.Err.AddValue1 = 0;
        cbf_prm.u.RecReadConf.Err.AddValue2 = 0;

        cbf_prm.u.RecReadConf.pAddr = pAddr;
        cbf_prm.u.RecReadConf.RecordIndex = RecordIndex;
        cbf_prm.u.RecReadConf.ReqRef = ReqRef;
        cbf_prm.u.RecReadConf.Length = Length;
        cbf_prm.u.RecReadConf.pBuffer = pRecBuffer;

        if (this->m_pUserCbf_DR_Read)
        {
            this->m_pUserCbf_DR_Read(&cbf_prm);
        }
        else
        {
            PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "NO CBF for Data Record Read defined. ");
        }

        pnd_mem_free(pRecBuffer);

        return PNIO_OK;
    }

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

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<record_read");

    return result;
}
PNIO_UINT32 CController::record_write(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex,
    PNIO_UINT32 Length, const PNIO_UINT8 * pBuffer)
{
    PNIO_UINT32 result = PNIO_OK;
    PND_RQB_PTR_TYPE                pRQB;
    PND_RECORD_READ_WRITE_PTR_TYPE  pRecWrite;
    PNIO_UINT8 * pRecBuffer;

    // Buffer[PNIOI_DREC_MAX_SIZE]; // in, out

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>record_write");

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

    // check if not controller address
    if (s_PndLogAddr.logAddrList[pAddr->u.Addr] != IOCDEVICE &&
        s_PndLogAddr.logAddrList[pAddr->u.Addr] != IOCSLOT   &&
        s_PndLogAddr.logAddrList[pAddr->u.Addr] != IOCSUBSLOT)
    {
        return PNIO_ERR_PRM_ADD;
    }

    if (pBuffer == PNIO_NULL)
    {
        return PNIO_ERR_PRM_BUF;
    }

    if (Length == 0 || Length > PNIOI_DREC_MAX_SIZE)
        return PNIO_ERR_VALUE_LEN;

    if (RecordIndex > 0xFFFF)
        return PNIO_ERR_PRM_REC_INDEX;

    pRQB = pnd_lower_rqb_alloc(PND_OPC_REC_WRITE, 0, this, read_write_record_done);

    if (!pRQB)
    {
        PNIO_CBE_PRM cbf_prm;

        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>read_write CBF");

        cbf_prm.CbeType = PNIO_CBE_REC_WRITE_CONF;
        cbf_prm.Handle = get_handle(this);
        cbf_prm.u.RecWriteConf.Err.ErrCode = 0xDF; // IODWriteRes
        cbf_prm.u.RecWriteConf.Err.ErrDecode = 0x80; // PNIORW
        cbf_prm.u.RecWriteConf.Err.ErrCode1 = 0xC3; // Resource  + resource unavailable
        cbf_prm.u.RecWriteConf.Err.ErrCode2 = 0;
        cbf_prm.u.RecWriteConf.Err.AddValue1 = 0;
        cbf_prm.u.RecWriteConf.Err.AddValue2 = 0;

        cbf_prm.u.RecWriteConf.pAddr = pAddr;
        cbf_prm.u.RecWriteConf.RecordIndex = RecordIndex;
        cbf_prm.u.RecWriteConf.ReqRef = ReqRef;

        if (this->m_pUserCbf_DR_Write)
        {
            this->m_pUserCbf_DR_Write(&cbf_prm);
        }
        else
        {
            PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "NO CBF for Data Record Write defined. ");
        }
        return PNIO_OK;
    }

    pRecWrite = &pRQB->args.rec_write;

    pnd_memset(pRecWrite, 0, sizeof(PND_RECORD_READ_WRITE_TYPE));

    pRecBuffer = (PNIO_UINT8 *)pnd_mem_alloc(Length);
    if (pRecBuffer == 0)
        PND_FATAL("mem alloc failed");

    pnd_memcpy(pRecBuffer, pBuffer, Length);

    pnd_memset(pRQB, 0, sizeof(PND_RQB_TYPE));

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_REC_WRITE);
    PND_RQB_SET_USER_CBF_PTR(pRQB, read_write_record_done);
    PND_RQB_SET_USER_INST_PTR(pRQB, this);

    pRecWrite->Addr.AddrType = pAddr->AddrType;
    pRecWrite->Addr.IODataType = pAddr->IODataType;
    pRecWrite->Addr.u.Addr = pAddr->u.Addr;

    pRecWrite->UserRef = ReqRef;

    pRecWrite->RecordIndex = RecordIndex;
    pRecWrite->Length = Length;
    pRecWrite->pBuffer = pRecBuffer;

    pRecWrite->pnd_handle.cp_nr = this->m_hInstanceHandle;

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<record_write");

    return result;
}

/* callbacks */
PNIO_VOID CController::iosystem_reconfig_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    CController *pThis = (CController *)pInst;
    CIOSystemReconfig::SM_PROCESS_RESULT result;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>iosystem_reconfig_done");

    result = pThis->m_pIOSystemReconfig->processStateMachine(rqb_ptr);

    if (result == CIOSystemReconfig::FINISHED)
    {
        PNIO_CBE_PRM cbf_prm;
        PNIO_ADDR addr;

        cbf_prm.Handle = get_handle(pThis);
        cbf_prm.CbeType = PNIO_CBE_IOSYSTEM_RECONFIG;
        cbf_prm.u.IoSystemReconf.pAddr = &addr;
        cbf_prm.u.IoSystemReconf.pAddr->u.Addr = rqb_ptr->args.iosys_recon.Addr.u.Addr;
        cbf_prm.u.IoSystemReconf.pAddr->AddrType = rqb_ptr->args.iosys_recon.Addr.AddrType;
        cbf_prm.u.IoSystemReconf.pAddr->IODataType = rqb_ptr->args.iosys_recon.Addr.IODataType;
        cbf_prm.u.IoSystemReconf.Mode = rqb_ptr->args.iosys_recon.Mode;
        cbf_prm.u.IoSystemReconf.Status = rqb_ptr->args.iosys_recon.pnio_err;

        if (pThis->m_pUserCbf_IosytemReconfig)
        {
            PND_IOBC_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_IOSYSTEM_RECONFIG call user callback() finished with Result = %d. ApplHandle = %d Cbf = %d",
                cbf_prm.u.IoSystemReconf.Status,
                cbf_prm.Handle,
                pThis->m_pUserCbf_IosytemReconfig);

            pThis->m_pUserCbf_IosytemReconfig(&cbf_prm);
        }
        else
        {
            PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF_IOSYSTEM_RECONFIG user callback() defined. ApplHandle = %d Cbf = %d",
                cbf_prm.Handle,
                pThis->m_pUserCbf_IosytemReconfig);
        }

        pThis->m_pPndAdapter->memFree(rqb_ptr);

        if(pThis->m_pIOSystemReconfig != PNIO_NULL)
        {
            delete pThis->m_pIOSystemReconfig;
            pThis->m_pIOSystemReconfig = PNIO_NULL;
        }

        pThis->m_iosys_reconfig_running = PNIO_FALSE;
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<iosystem_reconfig_done");
}

PNIO_VOID CController::device_activate_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    CController *pThis = (CController *)pInst;
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>device_activate_done");

    if (LSA_RQB_GET_OPCODE(rqb_ptr) != PND_OPC_DEVICE_ACTIVATE) {
        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "wrong OPCODE : %d", LSA_RQB_GET_OPCODE(rqb_ptr));
        PND_FATAL("wrong OPCODE");
    }

    pThis->RemoveDeviceFromInProgressList(rqb_ptr->args.iob_dev_act.Addr);

    if (pThis->m_pUserCbf_DevAct)
    {
        PND_DEV_ACTIVATE_PTR_TYPE pDevActivate = &rqb_ptr->args.iob_dev_act;
        PNIO_ADDR Addr;
        /*TRC_OUT01(GR_STATE, LV_FCTCLBF,
        "-> CBF_DEV_ACT_IND call user callback() ApplHandle=0x%x",
        Handle); */

        PNIO_CBE_PRM cbf_prm;
        cbf_prm.Handle = pThis->get_handle(pThis);

        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "-> CBF_DEV_ACT_IND call user callback() ApplHandle= %d", cbf_prm.Handle);

        cbf_prm.CbeType = PNIO_CBE_DEV_ACT_CONF;

        Addr.AddrType = (PNIO_ADDR_TYPE)(pDevActivate->Addr.AddrType);
        Addr.IODataType = (PNIO_IO_TYPE)(pDevActivate->Addr.IODataType);
        Addr.u.Addr = (pDevActivate->Addr.u.Addr);
        cbf_prm.u.DevActConf.pAddr = &Addr;

        cbf_prm.u.DevActConf.Mode = (PNIO_DEV_ACT_TYPE)(pDevActivate->DevActMode);
        cbf_prm.u.DevActConf.Result = pDevActivate->pnio_err;

        pThis->m_pUserCbf_DevAct(&cbf_prm);
        /*    TRC_OUT01(GR_STATE, LV_FCTCLBF,
        "<- CBF_DEV_ACT_IND call user callback() ends ApplHandle=0x%x", Handle); */
        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_DEV_ACT_IND call user callback() finished.");
    }
    else
    {
        /* no callback fct registered */
        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_DEV_ACT_IND : No user callback fct registered");
    }

    pThis->pnd_lower_rqb_free(pRqb);


    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<device_activate_done");
}

PNIO_VOID CController::read_write_record_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    CController *pThis = (CController *)pInst;

    PNIO_ADDR Addr;
    PNIO_CBE_PRM cbf_prm;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>read_write_record_done");

    switch (LSA_RQB_GET_OPCODE(rqb_ptr))
    {
    case PND_OPC_REC_READ:
    {
        PND_RECORD_READ_WRITE_PTR_TYPE pRecRead = &rqb_ptr->args.rec_read;

        cbf_prm.CbeType = PNIO_CBE_REC_READ_CONF;
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

        pnd_mem_free(pRecRead->pBuffer); // free record read Buffer allocated by record_read
        break;
    }
    case PND_OPC_REC_WRITE:
    {
        PND_RECORD_READ_WRITE_PTR_TYPE pRecWrite = &rqb_ptr->args.rec_write;
        cbf_prm.CbeType = PNIO_CBE_REC_WRITE_CONF;
        cbf_prm.Handle = get_handle(pThis);

        cbf_prm.CbeType = PNIO_CBE_REC_WRITE_CONF;

        Addr.AddrType = (PNIO_ADDR_TYPE)(pRecWrite->Addr.AddrType);
        Addr.IODataType = (PNIO_IO_TYPE)(pRecWrite->Addr.IODataType);
        Addr.u.Addr = pRecWrite->Addr.u.Addr;
        cbf_prm.u.RecWriteConf.pAddr = &Addr;

        cbf_prm.u.RecWriteConf.RecordIndex = pRecWrite->RecordIndex;
        cbf_prm.u.RecWriteConf.ReqRef = pRecWrite->UserRef;

        cbf_prm.u.RecWriteConf.Err.ErrCode = pRecWrite->err.ErrCode;
        cbf_prm.u.RecWriteConf.Err.ErrDecode = pRecWrite->err.ErrDecode;
        cbf_prm.u.RecWriteConf.Err.ErrCode1 = pRecWrite->err.ErrCode1;
        cbf_prm.u.RecWriteConf.Err.ErrCode2 = pRecWrite->err.ErrCode2;
        cbf_prm.u.RecWriteConf.Err.AddValue1 = (PNIO_UINT16)pRecWrite->pnio_err;
        cbf_prm.u.RecWriteConf.Err.AddValue2 = (PNIO_UINT16)pRecWrite->pnio_err;

        if (pThis->m_pUserCbf_DR_Write)
        {
            pThis->m_pUserCbf_DR_Write(&cbf_prm);
        }
        else
        {
            PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "NO CBF for Data Record Read/Write defined. ");
        }

        pnd_mem_free(pRecWrite->pBuffer); // free record write Buffer allocated by record_write
        break;
    }
    default:
        PND_FATAL("wrong OPCODE");
    }

    pThis->pnd_lower_rqb_free(pRqb);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<read_write_record_done");
}

PNIO_VOID CController::ctrl_diag_req_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    CController *pThis = (CController *)pInst;

    PNIO_CBE_PRM cbf_prm;
    cbf_prm.Handle = pThis->get_handle(pThis);
    cbf_prm.CbeType = PNIO_CBE_CTRL_DIAG_CONF;

    if (pThis->m_pUserCbf_DiagReq)
    {
        cbf_prm.u.CtrlDiagConf.pDiagData = (PNIO_CTRL_DIAG *)rqb_ptr->args.diag_req.DiagReq;
        cbf_prm.u.CtrlDiagConf.DiagDataBufferLen = (PNIO_UINT32)rqb_ptr->args.diag_req.DiagDataBufferLen;
        cbf_prm.u.CtrlDiagConf.pDiagDataBuffer = (PNIO_UINT8 *)rqb_ptr->args.diag_req.pDiagDataBuffer;
        cbf_prm.u.CtrlDiagConf.ErrorCode = (PNIO_UINT32)rqb_ptr->args.diag_req.pnio_err;

        pThis->m_pUserCbf_DiagReq(&cbf_prm);
    }
    else
    {
        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF for Diagnostic Request user callback() defined. ApplHandle= %d Mode= %d", cbf_prm.Handle, pThis->m_pUserCbf_DiagReq);
    }

    if (rqb_ptr->args.diag_req.pDiagDataBuffer != PNIO_NULL)
    {
        pnd_mem_free(rqb_ptr->args.diag_req.pDiagDataBuffer);
    }

    pnd_mem_free(rqb_ptr->args.diag_req.DiagReq); // free DiagReq allocated by ctrl_diag_request
    pnd_mem_free(rqb_ptr);

}



PNIO_UINT32 CController::GetLogAddrOfInterfaceSubmodule() const
{
    for (PNIO_UINT16 idx = 0; idx < PND_LADDR_MAX; idx++)
    {
        if (s_PndLogAddr.logAddrList[idx] == PDSUBSLOTINTERFACE)
        {
            return idx;
        }
    }

    return 0;
}

PNIO_UINT32 CController::IsInProgressListNotFull() const
{
    // Check if InProgress List is empty
    if (m_InProgressActivationList.size() >= MAX_DEVICE_ACTIVATION_SIZE)
    {
        return PNIO_FALSE;
    }

    return PNIO_TRUE;
}

PNIO_VOID CController::AddDeviceToInProgressList(PNIO_UINT32 laddr)
{
    lockSharedList();
    m_InProgressActivationList.push_back(laddr);
    unlockSharedList();
}

/*lint -e{1764) Reference parameter 'laddr' could be declared const ref */
PNIO_VOID CController::RemoveDeviceFromInProgressList(PNIO_ADDR &laddr)
{
    lockSharedList();

    LogAddrVector::iterator listIter = m_InProgressActivationList.begin();

    while (listIter != m_InProgressActivationList.end())
    {
        if (*listIter == laddr.u.Addr)
        {
            listIter = m_InProgressActivationList.erase(listIter);
        }
        else
        {
            ++listIter;
        }
    }

    unlockSharedList();
}

PNIO_UINT16 CController::getDeviceNumber(PNIO_UINT32 laddr) const
{
    return m_pndDevNumberList[laddr];
}

PNIO_UINT16 CController::getSlotNumber(PNIO_UINT32 laddr) const
{
    return m_pndSlotNumberList[laddr];
}

PNIO_UINT16 CController::getSubslotNumber(PNIO_UINT32 laddr) const
{
    return m_pndSubslotNumberList[laddr];
    
}

PNIO_BOOL CController::isInProgressActivationListEmpty() const
{
    return m_InProgressActivationList.empty();
}

PNIO_BOOL CController::isPendingActivationListEmpty() const
{
    return m_PendingActivationList.empty();
}

PNIO_UINT16 CController::getSharedListLockId() const
{
    return s_PndSharedListLockId;
}

PNIO_VOID CController::pendingListPushBack(PNIO_UINT32 laddr)
{
    m_PendingActivationList.push_back(laddr);
}

PNIO_VOID CController::pendingListPopBack()
{
    m_PendingActivationList.pop_back();
}

PNIO_UINT32 CController::pendingListGetLast()
{
    return m_PendingActivationList.at(m_PendingActivationList.size() - 1);
}

PNIO_VOID CController::inProgressListPushBack(PNIO_UINT32 laddr)
{
    m_InProgressActivationList.push_back(laddr);
}

PNIO_BOOL CController::isIOCDeviceLogAddr(PNIO_UINT32 laddr) const
{
    return s_PndLogAddr.logAddrList[laddr] == IOCDEVICE ? PNIO_TRUE : PNIO_FALSE;
}

PNIO_VOID CController::lockSharedList()
{
    m_pPndAdapter->enterCriticalSection(s_PndSharedListLockId);
}

PNIO_VOID CController::unlockSharedList()
{
    m_pPndAdapter->exitCriticalSection(s_PndSharedListLockId);
}

#ifdef PND_CFG_ISO_SUPPORT
PNIO_UINT32 CController::register_cp_cbf(PNIO_CP_CBE_TYPE CbeType, PNIO_CP_CBF cbf)
{
    PNIO_UINT32 Ret = PNIO_OK;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>register_cp_cbf");
    
    if (m_Status != PND_OPENED)
        return PNIO_ERR_WRONG_HND;

    if (!cbf)
        return PNIO_ERR_PRM_CALLBACK;

    if (m_Mode != PNIO_MODE_OFFLINE)
        return PNIO_ERR_MODE_VALUE;

    switch(CbeType)
    {
        case PNIO_CP_CBE_OPFAULT_IND:
            if(m_pUserCbf_OpFaultInd != NULL)
            {
                Ret = PNIO_ERR_ALLREADY_DONE;
            }
            else
            {
                m_pUserCbf_OpFaultInd = cbf;
            }
            break;
        case PNIO_CP_CBE_STARTOP_IND:
            if(m_pUserCbf_OpFaultInd == NULL)
            {
                Ret = PNIO_ERR_SEQUENCE;
            }
            else if(m_pUserCbf_StartOpInd != NULL)
            {
                Ret = PNIO_ERR_ALLREADY_DONE;
            }
            else
            {
                m_pUserCbf_StartOpInd = cbf;
            }            
            break;
        default:            
            Ret = PNIO_ERR_PRM_TYPE;
            break;
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<register_cp_cbf");

    return Ret;
}

PNIO_VOID CController::opFault(const PNIO_CYCLE_INFO* pCycleInfo)
{
    PNIO_CP_CBE_PRM prm;

    if(m_pUserCbf_OpFaultInd != NULL)
    {
        buildIsoCallbackPrm(&prm, PNIO_CP_CBE_OPFAULT_IND, pCycleInfo);
        m_pUserCbf_OpFaultInd(&prm);  
    }
}

PNIO_VOID CController::startOp(const PNIO_CYCLE_INFO* pCycleInfo)
{
    PNIO_CP_CBE_PRM prm;

    if(m_pUserCbf_StartOpInd != NULL)
    {
        m_isIsoDataConsistent = PNIO_TRUE;
        buildIsoCallbackPrm(&prm, PNIO_CP_CBE_STARTOP_IND, pCycleInfo);
        m_pUserCbf_StartOpInd(&prm);          
    }    
}

PNIO_UINT32 CController::setOpDone(PNIO_CYCLE_INFO* pCycleInfo)
{
    PNIO_UINT32 result = PNIO_OK;
    CISOUser* instance;

    instance = CISOUser::getInstance(m_CpIndex);

    if(instance == NULL)
    {
        result = PNIO_ERR_NO_FW_COMMUNICATION;  //TO-DO: ???
    }
    else
    {
        result = instance->setOpDone(pCycleInfo);
    }

    m_isIsoDataConsistent = PNIO_FALSE;

    return result;
}

PNIO_BOOL CController::isIsoDataConsistent() const
{
    return m_isIsoDataConsistent;
}

PNIO_VOID CController::buildIsoCallbackPrm(PNIO_CP_CBE_PRM* pPrm, const PNIO_CP_CBE_TYPE cbeType, const PNIO_CYCLE_INFO* pCycleInfo)
{
    PNIO_UINT32 appHandle;
    
    pPrm->CbeType = cbeType;
    appHandle = get_handle_by_index(m_CpIndex);    

    switch(cbeType)
    {
    case PNIO_CP_CBE_STARTOP_IND:
        pPrm->u.StartOp.AppHandle = appHandle;
        pPrm->u.StartOp.CycleInfo = *pCycleInfo;
        break;
    case PNIO_CP_CBE_OPFAULT_IND:
        pPrm->u.OpFault.AppHandle = appHandle;
        pPrm->u.OpFault.CycleInfo = *pCycleInfo;
        break;
    default:
        PND_FATAL("CController::buildIsoCallbackPrm(): unknown CBE type");
        break;
    }
}

#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

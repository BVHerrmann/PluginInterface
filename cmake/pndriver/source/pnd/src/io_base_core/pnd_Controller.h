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
/*  F i l e               &F: pnd_Controller.h                          :F&  */
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
#ifndef PND_CONTROLLER_H
#define PND_CONTROLLER_H

#include "pnd_int.h"
#include "pnd_IOBaseCoreCommon.h"
#include "pnd_IPndAdapter.h"

#include <vector>

class CIOSystemReconfig;

class CController : public CIOBaseCoreCommon {

    /* misc bits */
    PNIO_CBF m_pUserCbf_Mode;
    PNIO_CBF m_pUserCbf_DevAct;
    PNIO_CBF m_pUserCbf_Alarm;
    PNIO_CBF m_pUserCbf_DR_Read;
    PNIO_CBF m_pUserCbf_DR_Write;
    PNIO_CBF m_pCbf_AlarmInd;
    PNIO_CBF m_pUserCbf_DiagReq;
    PNIO_CBF m_pUserCbf_IosytemReconfig;
    PNIO_UINT32 m_ExtPar;
    PNIO_MODE_TYPE  m_Mode;      /* Actual mode */
	CIOSystemReconfig *m_pIOSystemReconfig;
    PND_RQB_LIST_TYPE pnd_rqb_list;
    PND_RQB_LIST_TYPE pnd_datarecord_read_rqb_list;
    PND_RQB_LIST_TYPE pnd_datarecord_write_rqb_list;
#ifdef PND_CFG_ISO_SUPPORT  
    PNIO_CP_CBF m_pUserCbf_StartOpInd;
    PNIO_CP_CBF m_pUserCbf_OpFaultInd;
	PNIO_BOOL m_isIsoDataConsistent;
#endif
    
public:

    CController(PNIO_UINT32, IPndAdapter*);
    ~CController(void);

    virtual PND_RQB_LIST_TYPE *getRQBList(LSA_OPCODE_TYPE opcode);

    PNIO_VOID setExtPar(PNIO_UINT32 extPar);

    PNIO_UINT32 getExtPar() const;
    PNIO_UINT16 getDeviceNumber(PNIO_UINT32 laddr) const;
    PNIO_UINT16 getSlotNumber(PNIO_UINT32 laddr) const;
    PNIO_UINT16 getSubslotNumber(PNIO_UINT32 laddr) const;
    PNIO_UINT8 isInProgressActivationListEmpty() const;
    PNIO_UINT8 isPendingActivationListEmpty() const;
    PNIO_UINT16 getSharedListLockId() const;
    PNIO_VOID pendingListPushBack(PNIO_UINT32 laddr);
    PNIO_VOID pendingListPopBack();
    PNIO_UINT32 pendingListGetLast();
    PNIO_VOID inProgressListPushBack(PNIO_UINT32 laddr);
    PNIO_UINT8 isIOCDeviceLogAddr(PNIO_UINT32 laddr) const;
    PNIO_UINT32 GetLogAddrOfInterfaceSubmodule() const;
    PNIO_UINT32 IsInProgressListNotFull() const;
    PNIO_VOID RemoveDeviceFromInProgressList(PNIO_ADDR& laddr);
    PNIO_VOID AddDeviceToInProgressList(PNIO_UINT32 laddr);
    PNIO_VOID lockSharedList();
    PNIO_VOID unlockSharedList();
#ifdef PND_CFG_ISO_SUPPORT
    PNIO_VOID opFault(const PNIO_CYCLE_INFO* pCycleInfo);
    PNIO_VOID startOp(const PNIO_CYCLE_INFO* pCycleInfo);
    PNIO_UINT32 setOpDone(PNIO_CYCLE_INFO* pCycleInfo);
    PNIO_BOOL isIsoDataConsistent() const;
#endif

    static PNIO_VOID remove_instance(PNIO_UINT16);
    static CController *get_instance(PNIO_UINT32 CpIndex);
    static IIODU *get_IODU(PNIO_UINT32 CpIndex);

    static PNIO_UINT32 controller_open(PNIO_UINT32 CpIndex,
        PNIO_UINT32 ExtPar,
        PNIO_CBF cbf_RecReadConf,
        PNIO_CBF cbf_RecWriteConf,
        PNIO_CBF cbf_AlarmInd,
        PNIO_UINT32 *UserHandle);
    static PNIO_UINT32 controller_close(PNIO_UINT32 UserHandle);

    PNIO_UINT32 register_cbf(PNIO_CBE_TYPE CbeType, PNIO_CBF cbf);
    PNIO_UINT32 register_cp_cbf(PNIO_CP_CBE_TYPE CbeType, PNIO_CP_CBF cbf);
    PNIO_VOID   de_register_all_cbf();
    PNIO_UINT32 set_mode(PNIO_MODE_TYPE Mode);
    PNIO_UINT32 device_activate(const PNIO_ADDR * pAddr, PNIO_DEV_ACT_TYPE mode);
    PNIO_UINT32 record_read(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex, PNIO_UINT32 Length);
    PNIO_UINT32 record_write(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex, PNIO_UINT32 Length, const PNIO_UINT8 * pBuffer);
    PNIO_UINT32	ctrl_diag_request(LSA_HANDLE_TYPE handle, PNIO_CTRL_DIAG * pDiagInfo);
    PNIO_UINT32 iosystem_reconfig(PNIO_UINT32 handle, CIOSystemReconfig *ioSystemReconfig);

    PNIO_UINT32 alarm_provide(PNIO_UINT16 nrOfAlarmResource);
    PNIO_VOID   alarm_response(PNIO_VOID_PTR_TYPE  pRqb);

    static PNIO_VOID alarm_indication(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_VOID write_snmp_record(PNIO_VOID_PTR_TYPE pInst);

    static PNIO_VOID controller_open_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE rqb_ptr);
    static PNIO_VOID controller_close_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE rqb_ptr);
    static PNIO_VOID set_mode_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE pRqb);
    static PNIO_VOID set_synchronization_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE pRqb);
    static PNIO_VOID device_activate_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_VOID read_write_record_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_VOID ctrl_diag_req_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_VOID iosystem_reconfig_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);

protected:
    PNIO_UINT16    m_nr_of_alarm_res;
    PNIO_UINT16    m_pndDevNumberList[PND_LADDR_MAX];     // Device number list for each HD/IF
    PNIO_UINT16    m_pndSlotNumberList[PND_LADDR_MAX];    // Slot number list for each HD/IF
    PNIO_UINT16    m_pndSubslotNumberList[PND_LADDR_MAX]; // Subslot number list for each HD/IF
    PNIO_BOOL      m_set_mode_req_running;
    PNIO_BOOL      m_set_mode_sync_req_running;           // ISOUser synchronization request check

private:
    typedef std::vector<PNIO_UINT32> LogAddrVector;
    LogAddrVector              m_PendingActivationList;
    LogAddrVector              m_InProgressActivationList;
    PNIO_BOOL                  m_iosys_reconfig_running;

    PNIO_UINT32 checkParametersForIOSystemReconfig(PNIO_UINT32 handle, const CIOSystemReconfig *ioSystemReconfig) const;
#ifdef PND_CFG_ISO_SUPPORT
    PNIO_VOID buildIsoCallbackPrm(PNIO_CP_CBE_PRM* pPrm, const PNIO_CP_CBE_TYPE cbeType, const PNIO_CYCLE_INFO* pCycleInfo);
    PNIO_UINT32 setISOUserSynchronization(PNIO_MODE_TYPE Mode);
#endif
};



#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

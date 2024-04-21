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
/*  F i l e               &F: pnd_IOInterface.h                         :F&  */
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
#ifndef PND_IIOINTERFACE_H
#define PND_IIOINTERFACE_H

#include "pnd_IOBaseCoreCommon.h"

class CIOInterface : public CIOBaseCoreCommon {

    /* misc bits */
    PNIO_CBF m_pUserCbf_Alarm;
    PNIO_CBF m_pUserCbf_DR_Read;
    PNIO_CBF m_pUserCbf_SetIpAndNos;
    PNIO_CBF m_pUserCbf_RemaRead;
    PNIO_BOOL m_set_ip_and_nos_running;
    PND_RQB_LIST_TYPE pnd_ifc_datarecord_read_rqb_list;

public:

    CIOInterface(PNIO_UINT32, IPndAdapter* pPndAdapter);
    ~CIOInterface(void);

    virtual PND_RQB_LIST_TYPE *getRQBList(LSA_OPCODE_TYPE opcode);
    
    static PNIO_VOID remove_instance(PNIO_UINT16);
    static CIOInterface *get_instance(PNIO_UINT32 CpIndex);

    static PNIO_UINT32 interface_open(PNIO_UINT32 CpIndex, PNIO_CBF cbf_RecReadConf,
        PNIO_CBF cbf_AlarmInd, PNIO_UINT32 *UserHandle);
    static PNIO_UINT32 interface_close(PNIO_UINT32 UserHandle);

    PNIO_UINT32 register_cbf(PNIO_CBE_TYPE CbeType, PNIO_CBF cbf);
    PNIO_VOID de_register_all_cbf();
    PNIO_UINT32 record_read(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex, PNIO_UINT32 Length);
    PNIO_UINT32	set_ip_and_nos(PNIO_UINT32 handle, PNIO_SET_IP_NOS_MODE_TYPE Mode, const PNIO_IPv4 &IpV4, const PNIO_NOS &NoS);

    PNIO_UINT32 alarm_provide(PNIO_UINT16 nrOfAlarmResource);
    PNIO_VOID   alarm_response(PNIO_VOID_PTR_TYPE  pRqb);
    
    static PNIO_VOID alarm_indication(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_UINT32 rema_provide(PNIO_VOID_PTR_TYPE pInst, PNIO_UINT16 nrOfRemaResource);
    static PNIO_VOID rema_response(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  rqb_ptr);

    static PNIO_VOID interface_open_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE rqb_ptr);
    static PNIO_VOID interface_close_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE rqb_ptr);
    static PNIO_VOID record_read_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_VOID set_ip_and_nos_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_VOID ifc_rema_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);

protected:
    PNIO_UINT16   m_nr_of_alarm_res;
    PNIO_UINT16   m_nr_of_rema_res;

};

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
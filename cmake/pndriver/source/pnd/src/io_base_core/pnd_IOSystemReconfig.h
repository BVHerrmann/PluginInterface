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
#ifndef PND_IOSYSTEMRECONFIG_H
#define PND_IOSYSTEMRECONFIG_H

#include "pnd_int.h"
#include "pnd_Controller.h"
#include "pnd_IPndAdapter.h"

#include <vector>

class CIOSystemReconfig
{
public:
    typedef enum
    {
        ONGOING,
        FINISHED
    } SM_PROCESS_RESULT;

    PNIO_VOID setControllerInstance(CController *pController);
    SM_PROCESS_RESULT processStateMachine( PND_RQB_PTR_TYPE pRqb );
    PNIO_IOS_RECONFIG_MODE getMode() const;
    PNIO_BOOL validateReconfigParams() const;

    CIOSystemReconfig(PNIO_IOS_RECONFIG_MODE mode, PNIO_UINT32 deviceCount, const PNIO_ADDR *deviceList, 
                      PNIO_UINT32 portInterconnectionCount, PNIO_ADDR *portInterconnectionList, 
                      IPndAdapter *pPndAdapter);
    ~CIOSystemReconfig();
private:
    typedef std::vector<PNIO_UINT32> LogAddrVector;
    LogAddrVector m_mandatoryDeviceList;
    LogAddrVector m_optionalDeviceList;
    PNIO_IOS_RECONFIG_PTR_TYPE m_reconfigParams;
    IOSYSTEM_RECON_ENUM m_state;
    IPndAdapter *m_pPndAdapter;
    CController *m_pController;
    PNIO_IOS_RECONFIG_MODE m_mode;
    PNIO_UINT32 m_pnioErrorCodeOfLastFailedDevice;
    PNIO_ADDR m_laddrOfLastFailedDevice;

    PNIO_VOID prepareDeactPendingList();
    PNIO_VOID prepareDeactReq(const PNIO_ADDR &laddr, PNIO_DEV_ACT_TYPE type, PNIO_VOID *rqb);
    PNIO_VOID prepareReadReq(const PNIO_ADDR &laddr, PNIO_VOID *rqb);
    PNIO_UINT32 evaluateReadReq(PNIO_VOID *rqb);
    PNIO_VOID prepareWriteReq(const PNIO_ADDR &laddr, PNIO_VOID *rqb);
    PNIO_UINT32 evaluateWriteReq(PNIO_VOID *rqb);
    PNIO_VOID prepareActPendingList();
    PNIO_VOID freeUserParams();
    PNIO_UINT32 mapCmErrorToPnioError(const PNIO_ERR_STAT &error) const;
    PNIO_VOID createMandatoryAndOptionalDeviceList(PNIO_VOID *record);
    PNIO_VOID createPDInstanceTailorData(PNIO_VOID **ppPDInstTailorDataBuff, PNIO_UINT32 *pPDInstTailorDataBufLen);
    PNIO_UINT32 getNextDeviceFromPendingList(PNIO_ADDR *laddr);
};



#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
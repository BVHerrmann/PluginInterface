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
/*  F i l e               &F: pnd_ISOUser.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  ISOUser class which manages isochronous mode data read/write operations  */
/*  and checks application coupling status                                   */
/*                                                                           */
/*****************************************************************************/
#ifndef PND_ISOUSER_H
#define PND_ISOUSER_H

#include "pnd_IISOUser.h"
#include "pnd_IPndAdapter.h"
#include "pnd_int.h"
#include "pnd_iodu_eddi.h"

#include "eps_tasks.h"

class CISOUser : public IISOUser
{
public:
    virtual ~CISOUser();
    CISOUser( PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent);
       
    PNIO_VOID Create(PND_PARAM_ISO_HD_IF_PTR_TYPE params);
    PNIO_UINT32 setMode(PNIO_MODE_TYPE mode);
    PNIO_UINT32 startSync();
    PNIO_UINT32 stopSync();  
    PNIO_UINT32 restartSync();
    PNIO_UINT32 freerunSync();
    PNIO_UINT32 setOpDone(PNIO_CYCLE_INFO* pCycleInfo);
    PNIO_UINT32 syncStateChanged(PNIO_VOID* pRqb);
    PNIO_VOID setRxLocalTransferEndNs(PNIO_UINT32 rxLocalTransferEndNs);
    
	virtual string GetKey();
	virtual LSA_UINT GetNumber();
    virtual PNIO_VOID Startup(PNIO_VOID* pRqb);
    virtual PNIO_VOID Shutdown(PNIO_VOID* pRqb);    

    static CISOUser* getInstance(PNIO_UINT32 CpIndex);

protected:
    virtual IPndAdapter* createPndAdapter();
    PNIO_VOID deletePndAdapter();

private:
    typedef enum
    {
        START,
        STOP,
        RESTART,
        FREERUN
    } PND_SYNC_EVENT_TYPE;

    typedef enum
    {
        INIT,
        STARTED,
        FREERUNNING
    } STATE_TYPE;
    
    PND_RQB_PTR_TYPE m_responseRQB;
    PND_PARAM_ISO_HD_IF_TYPE m_isoParam;
    PNIO_UINT16 m_syncHandle;
    PNIO_BOOL m_initialized;    
    IIODU_EDDI* m_pIoduEddiInst;
    PNIO_BOOL m_obLoss;
    PNIO_BOOL m_syncStarted;
    PNIO_VOID_PTR_TYPE m_pIrteBasePtr;
    STATE_TYPE m_currentSyncState;
    PNIO_UINT32 m_rxLocalTransferEndNs;
    IPndAdapter* m_pPndAdapter;
	PNIO_BOOL m_handleTm;
	PNIO_UINT8 m_validEarlyCount;

    static CISOUser* m_pInstances[PSI_CFG_MAX_HD_CNT];
    static const PNIO_UINT32 cycleInfoDivider = 10UL;
    static const PNIO_UINT32 INVALID_RX_LOCAL_TRANSFER_END_NS = 0;
             
    PNIO_VOID startupDone(PNIO_UINT32 result);
    PNIO_VOID shutdownDone(PNIO_UINT32 pnioErr);
    PNIO_BOOL checkIfInTime();  
    PNIO_VOID getCycleInfo(PNIO_CYCLE_INFO* cycleInfo);
    PNIO_UINT32 changeSyncState(PND_SYNC_EVENT_TYPE eventType);

    static PNIO_VOID isoIrqIsr(LSA_INT param, PNIO_VOID_PTR_TYPE args);
    static LSA_VOID eddiTmIntReceiver(LSA_USER_ID_TYPE UserID, LSA_BOOL bOBLoss);
    static PNIO_VOID handleTmInterrupt(PNIO_UINT32 param, PNIO_VOID_PTR_TYPE pArgs);
	static PNIO_VOID setNewCycle(PNIO_VOID *basePtr, PNIO_BOOL enable);

        
};

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

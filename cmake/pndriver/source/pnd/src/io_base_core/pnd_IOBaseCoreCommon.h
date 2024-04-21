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
/*  F i l e               &F: pnd_IOBaseCoreCommon.h                    :F&  */
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
#ifndef PND_IOB_CORE_H                     /* ----- reinclude-protection ----- */
#define PND_IOB_CORE_H

#include "pnd_iodu.h"

#if (PSI_CFG_USE_EDDI == 1)
#include "pnd_IIODU_EDDI.h"
#endif

#include "pnd_pnio_user_core.h"
#include "pnd_IPndAdapter.h"

#include "pniobase.h"
#include "pniousrx.h"

#include <map>
#include <mutex>

#define PND_INVALID_HANDLE  0xFFFF
#define MAX_DEVICE_ACTIVATION_SIZE 8
#define PND_MAX_INSTANCES  PSI_CFG_MAX_HD_CNT
/*****************************************************************************
Class for controller Instances
*****************************************************************************/
class CController;
class CIOInterface;

typedef enum pnd_iob_status
{
    PND_CLOSED = 0,
    PND_OPENED
} PND_IOB_STATUS;

typedef struct {
    PNIO_UINT16 ios;       // IOSystem Number
    PNIO_UINT16 device;
    PNIO_UINT16 slot;
    PNIO_UINT16 subslot;
} GEO_ADDR;

typedef struct pnd_user_core_cp
{
    PNIO_UINT32  user_handle;
    CController  *instance_pointer;
    IIODU        *IODU_pointer;
    #if (PSI_CFG_USE_EDDI == 1)
    IIODU_EDDI   *IODU_EDDI_pointer;
    #endif
    CIOInterface *interface_instance_pointer;

    pnd_user_core_cp()
    {
        user_handle = PND_INVALID_HANDLE;
        instance_pointer = 0;
        IODU_pointer = 0;
        interface_instance_pointer = 0;
    };
}PND_USER_CORE_CP_TYPE;

typedef struct
{
    PNIO_BOOL  isInitialized;
    PNIO_UINT8 logAddrList[PND_LADDR_MAX]; // LADDR list for each HD/IF
}PND_LADDR_LIST_TYPE;

typedef struct pnd_rqb_list_tag
{
    PNIO_UINT16 size;
    PNIO_UINT16 index;
    PND_RQB_PTR_TYPE* pnd_rqb;
    PNIO_UINT16 resource_free_event;
} PND_RQB_LIST_TYPE;

typedef enum {
    IDLE = 0,
    DEACT = 1,
    TAILOR = 2,
    TAILOR_WRITE = 3,
    TAILOR_ACT = 4
} IOSYSTEM_RECON_ENUM;


class CIOBaseCoreCommon
{
public:
    static std::map<PNIO_UINT32, GEO_ADDR> s_AddressMap;
    static PND_USER_CORE_CP_TYPE s_PndUserCoreCpList[PND_MAX_INSTANCES];
    static PNIO_UINT16 s_PndIobCoreLockId;
    static PNIO_UINT16 s_PndSharedListLockId;
    static PNIO_UINT16 s_PndAllocatedResources;
    static PND_LADDR_LIST_TYPE s_PndLogAddr;
    
    CIOBaseCoreCommon(PNIO_UINT16 instance_handle, PND_IOB_STATUS status,
        PNIO_UINT32 cpIndex, PNIO_UINT16 waitEvent, IPndAdapter*);

    virtual ~CIOBaseCoreCommon();

    PND_IOB_STATUS m_get_status() const;
    PNIO_VOID m_set_status(PND_IOB_STATUS status);
    PNIO_UINT32 check_log_adress(const PNIO_ADDR * pAddr) const;
    static PNIO_UINT32 get_handle(const CIOBaseCoreCommon *ptr);
    static PNIO_UINT32 get_handle_by_index(PNIO_UINT32 CpIndex);
    PNIO_UINT32 ConvertGeoToLogAddr(PNIO_UINT16 stationNumber, PNIO_UINT16 slotNumber, PNIO_UINT16 subslotNumber) const;
    PNIO_VOID AddNode(PNIO_UINT32 laddr, GEO_ADDR &geoaddr) const;
    PNIO_VOID setLogAddrTypeListItem(PNIO_UINT16 index, PNIO_UINT8 laddrType) const;

protected:
    PNIO_UINT16 m_hInstanceHandle;
    PND_IOB_STATUS m_Status;
    PNIO_UINT32 m_CpIndex;
    PNIO_UINT16 m_PndWaitEvent;
    IPndAdapter* m_pPndAdapter; 

    GEO_ADDR ConvertLogToGeoAddr(const PNIO_ADDR &laddr) const;
    PNIO_VOID EmptyMap() const;
    PNIO_VOID pnd_alloc_rqb_list(PND_RQB_LIST_TYPE &rqb_list);
    PNIO_VOID pnd_free_rqb_list(PND_RQB_LIST_TYPE &rqb_list);
    PND_RQB_PTR_TYPE pnd_get_rqb_from_list(PND_RQB_LIST_TYPE &rqb_list) const;
    PNIO_VOID pnd_put_rqb_to_list(PND_RQB_PTR_TYPE pRqb, PND_RQB_LIST_TYPE &rqb_list) const;
    PND_RQB_PTR_TYPE pnd_lower_rqb_alloc(LSA_OPCODE_TYPE opcode, LSA_HANDLE_TYPE handle, PNIO_VOID_PTR_TYPE pInst, PNIO_VOID(*func)(PNIO_VOID*_this, PNIO_VOID*_rqb));
    PNIO_VOID pnd_lower_rqb_free(PNIO_VOID_PTR_TYPE pRQB);
    PNIO_UINT32 alarm_provide(PNIO_UINT16 nrOfAlarmResource, PNIO_REF indRef, PNIO_VOID(*alarmIndication)(PNIO_VOID_PTR_TYPE, PNIO_VOID_PTR_TYPE));
    PNIO_VOID alarm_response(PNIO_VOID_PTR_TYPE  rqb_ptr, PNIO_REF indRef, PNIO_VOID(*alarmIndication)(PNIO_VOID_PTR_TYPE, PNIO_VOID_PTR_TYPE));

    virtual PND_RQB_LIST_TYPE *getRQBList(LSA_OPCODE_TYPE opcode) = 0;           
};

/*---------------------------------------------------------------------*/

void pnd_iobase_core_init(void);
void pnd_iobase_core_undo_init(void);

void pnd_iobase_core_startup(PNIO_VOID_PTR_TYPE rqb_ptr);

void pnd_iobase_core_usr_enter(void);
void pnd_iobase_core_usr_exit(void);

PNIO_VOID pnd_iob_core_request( PND_RQB_PTR_TYPE  rqb_ptr );
PNIO_VOID pnd_iob_core_request_done( PND_RQB_PTR_TYPE  rqb_ptr );

PNIO_VOID pnio_user_request_start(PNIO_VOID_PTR_TYPE  pRQB);
/*---------------------------------------------------------------------*/
/*macros */
/*---------------------------------------------------------------------*/

#define PND_IOB_USR_ENTER pnd_iobase_core_usr_enter();
#define PND_IOB_USR_EXIT  pnd_iobase_core_usr_exit();

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
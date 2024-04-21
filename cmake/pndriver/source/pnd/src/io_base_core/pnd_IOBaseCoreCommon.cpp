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
/*  F i l e               &F: pnd_IOBaseCoreCommon.cpp                  :F&  */
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
#define LTRC_ACT_MODUL_ID   1121
#define PND_MODULE_ID        1121

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnd_IOBaseCoreCommon.h"
#include "pnd_pnio_user_core.h"
#include "pnd_Adapter.h"
#include "pnd_iodu.h"

#include "pniobase.h"
#include "pniousrx.h"

#if (PSI_CFG_USE_EDDS == 1)
#include "pnd_iodu_edds.h"
#endif
#if (PSI_CFG_USE_EDDI == 1)
#include "pnd_iodu_eddi.h"
#include "eddi_iocc_cfg.h"
#include "eddi_iocc.h"
#endif

#include "pnd_Controller.h"
#include "pnd_IOInterface.h"

/* StdLib++ */
#include <map>
#include <vector>

using namespace std;

map<PNIO_UINT32, GEO_ADDR> CIOBaseCoreCommon::s_AddressMap;
PND_USER_CORE_CP_TYPE CIOBaseCoreCommon::s_PndUserCoreCpList[PND_MAX_INSTANCES];
PNIO_UINT16 CIOBaseCoreCommon::s_PndIobCoreLockId = 0;
PNIO_UINT16 CIOBaseCoreCommon::s_PndSharedListLockId = 0;
PNIO_UINT16 CIOBaseCoreCommon::s_PndAllocatedResources = 0;
PND_LADDR_LIST_TYPE CIOBaseCoreCommon::s_PndLogAddr;
std::mutex mtx_index;

CIOBaseCoreCommon::CIOBaseCoreCommon(PNIO_UINT16 instance_handle, PND_IOB_STATUS status,
    PNIO_UINT32 cpIndex, PNIO_UINT16 waitEvent, IPndAdapter* pPndAdapter) :
    m_hInstanceHandle(instance_handle),
    m_Status(status),
    m_CpIndex(cpIndex),
    m_PndWaitEvent(waitEvent),
    m_pPndAdapter(pPndAdapter)
{
    
};

CIOBaseCoreCommon::~CIOBaseCoreCommon()
{
    if(m_pPndAdapter != NULL)
    {
        delete m_pPndAdapter;
        m_pPndAdapter = NULL;
    }    
};

PNIO_VOID CIOBaseCoreCommon::pnd_alloc_rqb_list(PND_RQB_LIST_TYPE &rqb_list)
{
    PNIO_UINT16 idx = 0;

    rqb_list.index = 0;
    rqb_list.resource_free_event = 0;

    rqb_list.pnd_rqb = (PND_RQB_PTR_TYPE*) m_pPndAdapter->memAlloc( sizeof(PND_RQB_TYPE) * rqb_list.size );

    for (idx=0;idx<rqb_list.size;idx++)
    {
        rqb_list.pnd_rqb[idx] = (PND_RQB_PTR_TYPE) m_pPndAdapter->memAlloc( sizeof(PND_RQB_TYPE) );
        pnd_memset( rqb_list.pnd_rqb[idx], 0,  sizeof(PND_RQB_TYPE) );
    }
}
/*lint -e{1764} Reference parameter 'rqb_list' could be declared const ref*/
PNIO_VOID CIOBaseCoreCommon::pnd_free_rqb_list(PND_RQB_LIST_TYPE &rqb_list)
{
    PNIO_UINT16 idx = 0;

    for (idx=0;idx<rqb_list.size;idx++)
    {
        m_pPndAdapter->memFree(rqb_list.pnd_rqb[idx]);
    }

    m_pPndAdapter->memFree(rqb_list.pnd_rqb);
}

PND_RQB_PTR_TYPE CIOBaseCoreCommon::pnd_get_rqb_from_list(PND_RQB_LIST_TYPE &rqb_list) const
{
	/*lint -e(530) */
    mtx_index.lock();
    PND_RQB_PTR_TYPE pRqb = PNIO_NULL;

    if (rqb_list.index < rqb_list.size)
    {
        pRqb = rqb_list.pnd_rqb[rqb_list.index];

        PND_ASSERT(pRqb != 0);

        rqb_list.pnd_rqb[rqb_list.index] = PNIO_NULL;

        rqb_list.index ++;

        pnd_reset_event(rqb_list.resource_free_event);

        mtx_index.unlock();

        return pRqb;
    }

    mtx_index.unlock();
    return PNIO_NULL;
}

PNIO_VOID CIOBaseCoreCommon::pnd_put_rqb_to_list(PND_RQB_PTR_TYPE pRqb, PND_RQB_LIST_TYPE &rqb_list) const
{
    mtx_index.lock();
    PND_ASSERT(pRqb != 0);

    if ( (rqb_list.index > 0) && (rqb_list.index <= rqb_list.size) )
    {
        rqb_list.index --;

        PND_ASSERT(rqb_list.pnd_rqb[rqb_list.index] == 0);

        rqb_list.pnd_rqb[rqb_list.index] = pRqb;

        pRqb = 0;
    }
    else
    {
        PND_ASSERT(pRqb != 0);
    }

    // set event - all resource free (close controller waits till all resourcecs are free)
    if (rqb_list.index == 0)
    {
        pnd_set_event( rqb_list.resource_free_event );
    }
    mtx_index.unlock();
}

PND_RQB_PTR_TYPE CIOBaseCoreCommon::pnd_lower_rqb_alloc( LSA_OPCODE_TYPE opcode, LSA_HANDLE_TYPE handle, PNIO_VOID_PTR_TYPE pInst,PNIO_VOID(*func)(PNIO_VOID*_this, PNIO_VOID*_rqb) )
{
    // Allocate an user RQB for opcode
    PND_RQB_PTR_TYPE   pRQB;
    PND_RQB_LIST_TYPE *pRqbList = PNIO_NULL;

    pRqbList = getRQBList(opcode);

    pRQB = pnd_get_rqb_from_list(*pRqbList);

    if (!pRQB)
    {
        return PNIO_NULL;
    }

    pnd_memset( pRQB, 0,  sizeof(PND_RQB_TYPE) );

    LSA_RQB_SET_OPCODE( pRQB, opcode );
    LSA_RQB_SET_HANDLE( pRQB, handle );
    PND_RQB_SET_USER_INST_PTR(pRQB, pInst);
    PND_RQB_SET_USER_CBF_PTR(pRQB, func);

    s_PndAllocatedResources++;

    return pRQB;

}

PNIO_VOID CIOBaseCoreCommon::pnd_lower_rqb_free( PNIO_VOID_PTR_TYPE rqb )
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb;
    PND_RQB_LIST_TYPE *pRqbList;

    pRqbList = getRQBList(LSA_RQB_GET_OPCODE(pRQB));
    
    pnd_put_rqb_to_list((PND_RQB_PTR_TYPE)pRQB, *pRqbList);
    s_PndAllocatedResources--;
}

void CIOBaseCoreCommon::setLogAddrTypeListItem(PNIO_UINT16 index, PNIO_UINT8 laddrType) const
{
    if (index < PND_LADDR_MAX)
    {
        s_PndLogAddr.logAddrList[index] = laddrType;
    }
}

PNIO_UINT32 CIOBaseCoreCommon::ConvertGeoToLogAddr( PNIO_UINT16 stationNumber, PNIO_UINT16 slotNumber, PNIO_UINT16 subslotNumber ) const
{
    PNIO_UINT32 laddr = 0;

	for(map<PNIO_UINT32, GEO_ADDR>::iterator m_it = s_AddressMap.begin(); m_it != s_AddressMap.end(); m_it++)
	{
		GEO_ADDR ga = m_it->second;
		if( ga.device == stationNumber
			&& ga.slot == slotNumber 
			&& ga.subslot == subslotNumber)
		{
            laddr = m_it->first;
			break;
		}
	}

	return laddr;
}

GEO_ADDR CIOBaseCoreCommon::ConvertLogToGeoAddr( const PNIO_ADDR &laddr ) const
{
    map<PNIO_UINT32, GEO_ADDR>::iterator m_it;
    GEO_ADDR ga = {0};
	
	if( !s_AddressMap.empty() )
	{
		m_it = s_AddressMap.find( laddr.u.Addr );
		
		if( m_it != s_AddressMap.end() )
		{
			ga = m_it->second;
		}
	}
	
	return ga;
}
/*lint -e{1764} parameter geoaddr coud be made const ref*/
PNIO_VOID CIOBaseCoreCommon::AddNode(PNIO_UINT32 laddr, GEO_ADDR &geoaddr ) const
{
	map<PNIO_UINT32, GEO_ADDR>::iterator m_it;

	m_it = s_AddressMap.find( laddr );

	PND_ASSERT( m_it == s_AddressMap.end() );  // in MAP ?
    s_AddressMap[laddr] = geoaddr;
} 

PNIO_VOID CIOBaseCoreCommon::EmptyMap() const
{
    s_AddressMap.clear();
}
PNIO_UINT32 CIOBaseCoreCommon::check_log_adress(const PNIO_ADDR * pAddr) const
{
    if (pAddr == PNIO_NULL)
    {
        return PNIO_ERR_PRM_ADD;
    }

    PNIO_UINT32 laddr = pAddr->u.Addr;

    if ((laddr > PND_LADDR_MAX) || (laddr <= 0))
    {
        return PNIO_ERR_PRM_ADD;
    }

    if ((s_PndLogAddr.logAddrList[laddr] > IOCSUBSLOT) || (s_PndLogAddr.logAddrList[laddr] <= UNKNOWN))
    {
        return PNIO_ERR_PRM_ADD;
    }
    else
    {
        return PNIO_OK;
    }
}

PNIO_VOID CIOBaseCoreCommon::alarm_response(PNIO_VOID_PTR_TYPE rqb_ptr, PNIO_REF indRef, PNIO_VOID(*alarmIndication)(PNIO_VOID_PTR_TYPE, PNIO_VOID_PTR_TYPE))
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;
    PND_ASSERT(pRQB);

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_ALARM_RESPONSE);
    LSA_RQB_SET_HANDLE(pRQB, 0);
    PND_RQB_SET_USER_INST_PTR(pRQB, this);
    PND_RQB_SET_USER_CBF_PTR(pRQB, alarmIndication);

    pRQB->args.iob_alarm.ind_ref = indRef;

    m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
}

PNIO_UINT32 CIOBaseCoreCommon::alarm_provide(PNIO_UINT16 nrOfAlarmResource, PNIO_REF indRef, PNIO_VOID(*alarmIndication)(PNIO_VOID_PTR_TYPE, PNIO_VOID_PTR_TYPE))
{
    PND_RQB_PTR_TYPE       pRQB = 0;
    PND_ALARM_PTR_TYPE     pAlarm = 0;
    PNIO_UINT16            i = 0;

    for (i = 0; i< nrOfAlarmResource; i++)
    {
        pRQB = (PND_RQB_PTR_TYPE)m_pPndAdapter->memAlloc( sizeof(*pRQB));

        if (!pRQB)
        {
            return PNIO_ERR_INTERNAL;
        }

        pAlarm = &pRQB->args.iob_alarm;

        pnd_memset(pAlarm, 0, sizeof(PND_ALARM_TYPE));

        pAlarm->pnd_handle.cp_nr = m_hInstanceHandle;
        pAlarm->ind_ref = indRef;

        LSA_RQB_SET_OPCODE(pRQB, PND_OPC_ALARM_PROVIDE);
        LSA_RQB_SET_HANDLE(pRQB, 0);
        PND_RQB_SET_USER_INST_PTR(pRQB, this);
        PND_RQB_SET_USER_CBF_PTR(pRQB, alarmIndication);

        m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
    }

    return PNIO_OK;
}

/* instances & handles */
PNIO_UINT32 CIOBaseCoreCommon::get_handle(const CIOBaseCoreCommon *ptr)
{
    if(ptr)
        return ptr->m_hInstanceHandle;
        else{
            PND_ASSERT(ptr);
        return 0xFFFFFFFF;
    }
}

PNIO_UINT32 CIOBaseCoreCommon::get_handle_by_index(PNIO_UINT32 CpIndex)
{
    if (CpIndex < PND_MAX_INSTANCES)
    {
        if (s_PndUserCoreCpList[CpIndex].user_handle != PND_INVALID_HANDLE)
            return s_PndUserCoreCpList[CpIndex].user_handle;
        else
            return PND_INVALID_HANDLE;

    }
    else 
        return PND_INVALID_HANDLE;
}

PND_IOB_STATUS CIOBaseCoreCommon::m_get_status() const
{
    return this->m_Status;
}

PNIO_VOID CIOBaseCoreCommon::m_set_status(PND_IOB_STATUS status)
{
    this->m_Status = status;
}

/* requests */
// poste request to pnio_user_core
PNIO_VOID pnio_user_request_start(PNIO_VOID_PTR_TYPE  pRQB)
{
    pnd_request_start(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request, (PNIO_VOID_PTR_TYPE)pRQB);
}

PNIO_VOID pnd_iob_core_request( PND_RQB_PTR_TYPE  rqb_ptr )
{
    PND_ASSERT( rqb_ptr != 0 );

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>pnd_iob_core_request");
    
    switch (LSA_RQB_GET_OPCODE(rqb_ptr))
    {
        case PND_OPC_STARTUP :
            pnd_iobase_core_startup(rqb_ptr);
            rqb_ptr->args.startup.cbf(rqb_ptr);
        break;
        case PND_OPC_SHUTDOWN:
            {
                // pnd_iobase_core_undo_init();
                PNIO_UINT16 i = 0;
                CController *pInsIController;
                CIOInterface *pInsIIOInterface;

                while ((i < PND_MAX_INSTANCES) && (CIOBaseCoreCommon::s_PndUserCoreCpList[i].user_handle != PND_INVALID_HANDLE))
                {
                    pInsIController = CIOBaseCoreCommon::s_PndUserCoreCpList[i].instance_pointer;
                    pInsIIOInterface = CIOBaseCoreCommon::s_PndUserCoreCpList[i].interface_instance_pointer;

                    if (pInsIController->m_get_status() == PND_OPENED || pInsIIOInterface->m_get_status() == PND_OPENED)
                    {
                        rqb_ptr->args.shutdown.pnio_err = PNIO_ERR_SEQUENCE;
                        break;
                    }
                    i++;
                }
                rqb_ptr->args.shutdown.cbf(rqb_ptr);

             break;
            }
        case PND_OPC_CONTROLLER_OPEN :
            pnio_user_request_start(rqb_ptr);
        break;
        case PND_OPC_CONTROLLER_CLOSE :
            pnio_user_request_start(rqb_ptr);
        break;
        case PND_OPC_PI_ALLOC :
#if (PSI_CFG_USE_EDDS == 1 || PSI_CFG_USE_EDDI == 1)
            PND_ASSERT(rqb_ptr->args.pi_alloc.pnd_handle.cp_nr < PND_MAX_INSTANCES);
            PND_ASSERT(CIOBaseCoreCommon::s_PndUserCoreCpList[(rqb_ptr->args.pi_alloc.pnd_handle.cp_nr)].IODU_pointer);

            CIOBaseCoreCommon::s_PndUserCoreCpList[(rqb_ptr->args.pi_alloc.pnd_handle.cp_nr)].IODU_pointer->PI_Alloc(rqb_ptr);
#endif
        break;
        case PND_OPC_PI_FREE :
#if (PSI_CFG_USE_EDDS == 1 || PSI_CFG_USE_EDDI == 1)
            PND_ASSERT(rqb_ptr->args.pi_free.pnd_handle.cp_nr < PND_MAX_INSTANCES);
            PND_ASSERT(CIOBaseCoreCommon::s_PndUserCoreCpList[(rqb_ptr->args.pi_free.pnd_handle.cp_nr)].IODU_pointer);

            CIOBaseCoreCommon::s_PndUserCoreCpList[(rqb_ptr->args.pi_free.pnd_handle.cp_nr)].IODU_pointer->PI_Free(rqb_ptr);
#endif
        break;
        case PND_OPC_LL_ALLOC :
#if (PSI_CFG_USE_EDDI == 1)
            PND_ASSERT(rqb_ptr->args.ll_alloc.pnd_handle.cp_nr < PND_MAX_INSTANCES);
            PND_ASSERT(CIOBaseCoreCommon::s_PndUserCoreCpList[(rqb_ptr->args.ll_alloc.pnd_handle.cp_nr)].IODU_EDDI_pointer);

            CIOBaseCoreCommon::s_PndUserCoreCpList[(rqb_ptr->args.ll_alloc.pnd_handle.cp_nr)].IODU_EDDI_pointer->allocateLinkLists(rqb_ptr);
#endif
        break;
        case PND_OPC_LL_FREE :
#if (PSI_CFG_USE_EDDI == 1)
            PND_ASSERT(rqb_ptr->args.ll_free.pnd_handle.cp_nr < PND_MAX_INSTANCES);
            PND_ASSERT(CIOBaseCoreCommon::s_PndUserCoreCpList[(rqb_ptr->args.ll_free.pnd_handle.cp_nr)].IODU_EDDI_pointer);

            CIOBaseCoreCommon::s_PndUserCoreCpList[(rqb_ptr->args.ll_free.pnd_handle.cp_nr)].IODU_EDDI_pointer->freeLinkLists(rqb_ptr);
#endif
        break;
		case PND_OPC_INTERFACE_OPEN :
            pnio_user_request_start(rqb_ptr);
        break;
        case PND_OPC_INTERFACE_CLOSE :
            pnio_user_request_start(rqb_ptr);
        break;
        default:
            PND_FATAL("not supported opcode received !");
    }
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<pnd_iob_core_request");
}

PNIO_VOID pnd_iob_core_request_done( PND_RQB_PTR_TYPE  rqb_ptr )
{
    PND_ASSERT( rqb_ptr != 0 );

    /* main CBF function PNIO user core, called in context of PNIO user core task */
    /* note: addressed instance is stored in extended RQB Header, for callback    */
    PNIO_VOID_PTR_TYPE   p_inst = PND_RQB_GET_USER_INST_PTR( rqb_ptr );
    EPS_LOWER_DONE_FCT  p_fct  = PND_RQB_GET_USER_CBF_PTR( rqb_ptr );

    // execute the CBF for instance
    p_fct( p_inst, rqb_ptr );
}

/* startup - shut down */
void pnd_iobase_core_init(PNIO_VOID)
{
    PNIO_UINT32 i;
    PNIO_UINT32 result = PNIO_OK;

    // void pnd_iobase_core_init(controller instanze and interfaces)
    result = pnd_alloc_critical_section(&CIOBaseCoreCommon::s_PndIobCoreLockId, PNIO_FALSE);
    if (result != PNIO_RET_OK)          // PNIO_RET_OK != PNIO_OK !!!
        PND_FATAL("event alloc failed");

    result = pnd_alloc_critical_section(&CIOBaseCoreCommon::s_PndSharedListLockId, LSA_FALSE);
    if (result != LSA_RET_OK)          // LSA_RET_OK != PNIO_OK !!!
        PND_FATAL("event alloc failed");

    for (i = 0; i<PND_MAX_INSTANCES; i++)
    {
        CIOBaseCoreCommon::s_PndUserCoreCpList[i].instance_pointer = 0;
        CIOBaseCoreCommon::s_PndUserCoreCpList[i].IODU_pointer = 0;        
        CIOBaseCoreCommon::s_PndUserCoreCpList[i].user_handle = PND_INVALID_HANDLE;
        CIOBaseCoreCommon::s_PndUserCoreCpList[i].interface_instance_pointer = 0;       
        
        #if (PSI_CFG_USE_EDDI == 1)
        CIOBaseCoreCommon::s_PndUserCoreCpList[i].IODU_EDDI_pointer = 0;
        #endif
    }

#if (PSI_CFG_USE_EDDI == 1)
	pnd_eddi_alloc_applsync_lock();
    eddi_IOCC_Init();
#endif

}

/* startup - shut down */
void pnd_iobase_core_startup(PNIO_VOID_PTR_TYPE rqb_ptr)
{
    PNIO_UINT32 i;
    PND_RQB_PTR_TYPE              pRQB;
    PND_STARTUP_PTR_TYPE          pStart;
    pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;

    pStart = &pRQB->args.startup;

    for (i = 0; i<pStart->startup_param.nr_of_cp; i++)
    {
        CIOBaseCoreCommon::s_PndUserCoreCpList[i].instance_pointer = new CController(i, new CPndAdapter());
        CIOBaseCoreCommon::s_PndUserCoreCpList[i].interface_instance_pointer = new CIOInterface(i, new CPndAdapter());

        if (LSA_COMP_ID_EDDP == pStart->startup_param.pnd_handle[i].edd_comp_id)
        {
            //s_PndUserCoreCpList[i].IODU_pointer = new CIODU_PERIF(pStart->startup_param.pnd_handle[i]);
            PND_FATAL("PNIP is no longer supported!");
        }
        else if (LSA_COMP_ID_EDDS == pStart->startup_param.pnd_handle[i].edd_comp_id)
        {
#if (PSI_CFG_USE_EDDS == 1)
            CIOBaseCoreCommon::s_PndUserCoreCpList[i].IODU_pointer = new CIODU_EDDS(pStart->startup_param.pnd_handle[i], new CPndAdapter());
#endif
        }
        else if (LSA_COMP_ID_EDDI == pStart->startup_param.pnd_handle[i].edd_comp_id)
        {
#if (PSI_CFG_USE_EDDI == 1)
            CIODU_EDDI *pEddi = new CIODU_EDDI(pStart->startup_param.pnd_handle[i], new CPndAdapter());

            CIODU_EDDI::INIT_PARAMS params;
            params.ioccPhyAddr = pStart->startup_param.ioAddress[i].io_iocc.uPhyAddr;
            params.pIoccBase = pStart->startup_param.ioAddress[i].io_iocc.pBase;
            params.ioMemPhyAddr = pStart->startup_param.ioAddress[i].io_mem.uPhyAddr;
            params.pIoMemBase = pStart->startup_param.ioAddress[i].io_mem.pBase;
            params.ioMemSize = pStart->startup_param.ioAddress[i].io_mem.lSize;

            pEddi->initEddi(&params);

            CIOBaseCoreCommon::s_PndUserCoreCpList[i].IODU_pointer = pEddi;
            CIOBaseCoreCommon::s_PndUserCoreCpList[i].IODU_EDDI_pointer = pEddi;
#endif
        }

        CIOBaseCoreCommon::s_PndUserCoreCpList[i].user_handle = i;
    }
}

PNIO_VOID pnd_iobase_core_undo_init(PNIO_VOID)
{
    PNIO_UINT32 i;

    for (i = 0; i< PND_MAX_INSTANCES; i++)
    {
        if (CIOBaseCoreCommon::s_PndUserCoreCpList[i].instance_pointer)
            delete CIOBaseCoreCommon::s_PndUserCoreCpList[i].instance_pointer;

        if (CIOBaseCoreCommon::s_PndUserCoreCpList[i].interface_instance_pointer)
            delete CIOBaseCoreCommon::s_PndUserCoreCpList[i].interface_instance_pointer;

        if (CIOBaseCoreCommon::s_PndUserCoreCpList[i].IODU_pointer)
        {
#if (PSI_CFG_USE_EDDS == 1 || PSI_CFG_USE_EDDI == 1)
            delete CIOBaseCoreCommon::s_PndUserCoreCpList[i].IODU_pointer;
#endif
        }

        CIOBaseCoreCommon::s_PndUserCoreCpList[i].user_handle = PND_INVALID_HANDLE;
        CIOBaseCoreCommon::s_PndUserCoreCpList[i].instance_pointer = 0;
        CIOBaseCoreCommon::s_PndUserCoreCpList[i].IODU_pointer = 0;

        #if (PSI_CFG_USE_EDDI == 1)
        CIOBaseCoreCommon::s_PndUserCoreCpList[i].IODU_EDDI_pointer = 0;
        #endif
    }

#if (PSI_CFG_USE_EDDI == 1)
    eddi_IOCC_Deinit();
	pnd_eddi_free_applsync_lock();
#endif

    pnd_free_critical_section(CIOBaseCoreCommon::s_PndIobCoreLockId);
    pnd_free_critical_section(CIOBaseCoreCommon::s_PndSharedListLockId);
}

/* locks */
void pnd_iobase_core_usr_enter(void)
{
    pnd_enter_critical_section(CIOBaseCoreCommon::s_PndIobCoreLockId);
}

void pnd_iobase_core_usr_exit(void)
{
    pnd_exit_critical_section(CIOBaseCoreCommon::s_PndIobCoreLockId);
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
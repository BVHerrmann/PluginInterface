
#ifndef EDDS_LLIF_H                    /* ----- reinclude-protection ----- */
#define EDDS_LLIF_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_llif.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile for EDDS. Lower Layer Interface (LLIF)               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                             some configs for LL                           */
/*===========================================================================*/
    
/* bitfields for EDDS_LL_RECURRING_TASK reporting actions/events to EDDS scheduler */
#define EDDS_LL_RECURRING_TASK_NOTHING                     0x00000000U
#define EDDS_LL_RECURRING_TASK_LINK_CHANGED                0x00000001U
#define EDDS_LL_RECURRING_TASK_STATISTIC_COUNTER           0x00000002U
#define EDDS_LL_RECURRING_TASK_CHIP_RESET                  0x80000000U


/*===========================================================================*/
/*                   Low-Level-Handle-Type                                   */
/*                                                                           */
/* The Handle Type EDDS_LL_HANDLE_TYPE has to be defined in a header of LLIF */
/*===========================================================================*/

/*===========================================================================*/
/*                   Low-Level ethernetcontroller interface                  */
/*                                                                           */
/* This will map the functions used in edds to hardware specific ones        */
/*                                                                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_OPEN_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 		            pLLManagement,
	EDDS_HANDLE				            hDDB,
	EDDS_UPPER_DPB_PTR_TO_CONST_TYPE    pDPB,
	LSA_UINT32				            TraceIdx,
	EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE	pCaps);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SETUP_FUNC_PTR)(
	LSA_VOID_PTR_TYPE                         pLLManagement,
    EDDS_UPPER_DSB_PTR_TYPE                   pDSB,
    EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE           pHwParam);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SHUTDOWN_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_CLOSE_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SEND_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 		pLLManagement,
    EDD_UPPER_MEM_PTR_TYPE  pBufferAddr,
    LSA_UINT32              Length,
    LSA_UINT32              PortID);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SEND_STS_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_VOID    EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SEND_TRIGGER_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_RECV_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 							pLLManagement,
    EDD_UPPER_MEM_PTR_TYPE  EDDS_LOCAL_MEM_ATTR *pBufferAddr,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                 pLength,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                 pPortID);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_RECV_PROVIDE_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 				pLLManagement,
    EDD_UPPER_MEM_PTR_TYPE          pBufferAddr);

typedef LSA_VOID    EDDS_LOCAL_FCT_ATTR (*EDDS_LL_RECV_TRIGGER_FUNC_PTR) (
    LSA_VOID_PTR_TYPE               pLLManagement);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_GET_STATS_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 				pLLManagement,
    LSA_UINT32                      PortID,
    EDDS_LOCAL_STATISTICS_PTR_TYPE  pStats);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_GET_LINK_STATE_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 					pLLManagement,
    LSA_UINT32                          PortID,
    EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  pLinkStat,
    EDDS_LOCAL_MEM_U16_PTR_TYPE         pMAUType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pMediaType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pIsPOF,
    EDDS_LOCAL_MEM_U32_PTR_TYPE         pPortStatus,
    EDDS_LOCAL_MEM_U32_PTR_TYPE         pAutonegCapAdvertised,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pLinkSpeedModeConfigured);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SET_LINK_STATE_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 				pLLManagement,
    LSA_UINT32                      PortID,
    LSA_UINT8    * const            LinkStat,
    LSA_UINT8    * const            PHYPower);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_MC_ENABLE_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 						pLLManagement,
    EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR		*pMCAddr);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_MC_DISABLE_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 						pLLManagement,
    LSA_BOOL                             	DisableAll,
    EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR 	*pMCAddr);

typedef LSA_UINT32  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_RECURRING_TASK_PTR)(
    LSA_VOID_PTR_TYPE       pLLManagement, LSA_BOOL hwTimeSlicing);

typedef LSA_VOID  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_LED_BACKUP_MODE_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 				pLLManagement);

typedef LSA_VOID  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_LED_RESTORE_MODE_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 				pLLManagement);

typedef LSA_VOID  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_LED_SET_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 				pLLManagement,
    LSA_BOOL						On);

typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SWITCH_MULTICAST_FWD_CTRL_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 					    pLLManagement,
    EDD_UPPER_PORTID_MODE_PTR_TO_CONST_TYPE pPortIDModeArray,
    LSA_UINT16                              PortIDModeCnt,
    LSA_UINT16                              MACAddrPrio,
    LSA_UINT16                              MACAddrGroup,
    LSA_UINT32                              MACAddrLow);

typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SWITCH_SET_PORT_STATE_FUNC_PTR)(
	LSA_VOID_PTR_TYPE 						pLLManagement,
    EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE   pPortStates);

typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SWITCH_FLUSH_FILTERING_DB_FUNC_PTR)(
LSA_VOID_PTR_TYPE 		                  pLLManagement,
EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE pFlushFilterDB);

typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR(*EDDS_LL_SWITCH_CHANGE_PORT_FUNC_PTR)(
LSA_VOID_PTR_TYPE                         pLLManagement,
LSA_UINT16                                PortID,
LSA_UINT8                                 IsPulled);

#ifdef LLIF_CFG_USE_LL_ARP_FILTER
typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SET_ARP_FILTER_FUNC_PTR)(
    LSA_VOID_PTR_TYPE 		            pLLManagement,
    LSA_UINT8                           Mode,
    LSA_UINT32                          UserIndex,
    EDD_IP_ADR_TYPE                     IPAddr);
#endif /* ifdef LLIF_CFG_USE_LL_ARP_FILTER */

typedef struct _EDDS_LL_TABLE_TYPE
{
	/* EDDS init/setup/shutdown/close functions --> */
	EDDS_LL_OPEN_FUNC_PTR						open;						/* low */
	EDDS_LL_SETUP_FUNC_PTR						setup;						/* low */
	EDDS_LL_SHUTDOWN_FUNC_PTR					shutdown;					/* low */
	EDDS_LL_CLOSE_FUNC_PTR						close;						/* low */
	/* <-- EDDS init/setup/shutdown/close functions */
	/* Transmit buffer handling functions --> */
	EDDS_LL_SEND_FUNC_PTR						enqueueSendBuffer;			/*HIGH!*/
	EDDS_LL_SEND_STS_FUNC_PTR					getNextFinishedSendBuffer;	/*HIGH!*/
	EDDS_LL_SEND_TRIGGER_FUNC_PTR				triggerSend;				/*HIGH!*/
	/* <-- Transmit buffer handling functions */
	/* Receive buffer handling functions --> */
	EDDS_LL_RECV_FUNC_PTR						getNextReceivedBuffer;		/*!ISR!*/
	EDDS_LL_RECV_PROVIDE_FUNC_PTR				provideReceiveBuffer;		/*!ISR!*/
	EDDS_LL_RECV_TRIGGER_FUNC_PTR               triggerReceive;             /*!ISR!*/
	/* <-- Receive buffer handling functions */
	/* Information functions --> */
	EDDS_LL_GET_LINK_STATE_FUNC_PTR				getLinkState;				/* low */
	EDDS_LL_GET_STATS_FUNC_PTR					getStatistics;				/* low */
	/* <-- Information functions */
	/* MAC address management functions --> */
	EDDS_LL_MC_ENABLE_FUNC_PTR					enableMC;					/* low */
	EDDS_LL_MC_DISABLE_FUNC_PTR					disableMC;					/* low */
	/* <-- MAC address management functions */
	EDDS_LL_RECURRING_TASK_PTR                  recurringTask;              /* scheduler */
	/* Control functions --> */
	EDDS_LL_SET_LINK_STATE_FUNC_PTR				setLinkState;				/* low */
	EDDS_LL_LED_BACKUP_MODE_FUNC_PTR			backupLocationLEDs;			/*!ANY!*/
	EDDS_LL_LED_RESTORE_MODE_FUNC_PTR			restoreLocationLEDs;	    /*!ANY!*/
	EDDS_LL_LED_SET_FUNC_PTR					setLocationLEDs;			/*!ANY!*/
	/* <-- Control functions */
	/* Switch functions --> */
	EDDS_LL_SWITCH_SET_PORT_STATE_FUNC_PTR		setSwitchPortState;			/* low */
	EDDS_LL_SWITCH_MULTICAST_FWD_CTRL_FUNC_PTR	controlSwitchMulticastFwd;	/* low */
	EDDS_LL_SWITCH_FLUSH_FILTERING_DB_FUNC_PTR	flushSwitchFilteringDB;		/* low */
    EDDS_LL_SWITCH_CHANGE_PORT_FUNC_PTR         changePort;
	/* <-- Switch functions */
	/* EDDS set arp filter function --> */
#ifdef LLIF_CFG_USE_LL_ARP_FILTER
	EDDS_LL_SET_ARP_FILTER_FUNC_PTR             setArpFilter;               /* low */
#endif /* ifdef LLIF_CFG_USE_LL_ARP_FILTER */
	/* <-- EDDS set arp filter function */	
} EDDS_LL_TABLE_TYPE;



#define EDDS_LL_FUNC(pDDB,func) (pDDB)->pGlob->pLLFunctionTable->func((pDDB)->pGlob->pLLManagementData



#define EDDS_LL_OPEN(pDDB,hDDB,pDPB,TraceIdx,pCaps) \
    EDDS_LL_FUNC(pDDB,open),hDDB,pDPB,TraceIdx,pCaps)

#define EDDS_LL_SETUP(pDDB,pDSB,pHwParam) \
    EDDS_LL_FUNC(pDDB,setup),pDSB,pHwParam)

#define EDDS_LL_SHUTDOWN(pDDB) \
    EDDS_LL_FUNC(pDDB,shutdown))

#define EDDS_LL_CLOSE(pDDB) \
    EDDS_LL_FUNC(pDDB,close))

#define EDDS_LL_SEND(pDDB,pBufferAddr,Length,PortID) \
    EDDS_LL_FUNC(pDDB,enqueueSendBuffer),pBufferAddr,Length,PortID)

#define EDDS_LL_SEND_STS(pDDB) \
    EDDS_LL_FUNC(pDDB,getNextFinishedSendBuffer))

#define EDDS_LL_SEND_TRIGGER(pDDB) \
    EDDS_LL_FUNC(pDDB,triggerSend))

#define EDDS_LL_RECV(pDDB,pBufferAddr,pLength,pPortID) \
    EDDS_LL_FUNC(pDDB,getNextReceivedBuffer),pBufferAddr,pLength,pPortID)

#define EDDS_LL_RECV_PROVIDE(pDDB,pBufferAddr) \
    EDDS_LL_FUNC(pDDB,provideReceiveBuffer),pBufferAddr)

#define EDDS_LL_RECV_TRIGGER(pDDB) \
    EDDS_LL_FUNC(pDDB,triggerReceive))

#define EDDS_LL_GET_STATS(pDDB,PortID,pStats) \
    EDDS_LL_FUNC(pDDB,getStatistics),PortID,pStats)

#define EDDS_LL_GET_LINK_STATE(pDDB,PortID,pLinkStat,pMAUType,pMediaType,pIsPOF,pPortStatus,pAutonegCapAdvertised,pLinkSpeedModeConfigured) \
		EDDS_LL_FUNC(pDDB,getLinkState),PortID,pLinkStat,pMAUType,pMediaType,pIsPOF,pPortStatus,pAutonegCapAdvertised,pLinkSpeedModeConfigured)

#define EDDS_LL_SET_LINK_STATE(pDDB,PortID,pLinkStat,pPHYPower) \
    EDDS_LL_FUNC(pDDB,setLinkState),PortID,pLinkStat,pPHYPower)

#define EDDS_LL_MC_ENABLE(pDDB,pMCAddr) \
    EDDS_LL_FUNC(pDDB,enableMC),pMCAddr)

#define EDDS_LL_MC_DISABLE(pDDB,DisableAll,pMCAddr) \
    EDDS_LL_FUNC(pDDB,disableMC),DisableAll,pMCAddr)

#define EDDS_LL_RECURRING_TASK(pDDB,hwTimeSlicing) \
        EDDS_LL_FUNC(pDDB,recurringTask),hwTimeSlicing)

#define EDDS_LL_LED_BACKUP_MODE(pDDB) \
    EDDS_LL_FUNC(pDDB,backupLocationLEDs))

#define EDDS_LL_LED_RESTORE_MODE(pDDB) \
    EDDS_LL_FUNC(pDDB,restoreLocationLEDs))

#define EDDS_LL_LED_SET_MODE(pDDB,On) \
    EDDS_LL_FUNC(pDDB,setLocationLEDs),On)

#define EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB,pPortIDModeArray,PortIDModeCnt,MACAddrPrio,MACAddrGroup,MACAddrLow) \
    EDDS_LL_FUNC(pDDB,controlSwitchMulticastFwd),pPortIDModeArray,PortIDModeCnt,MACAddrPrio,MACAddrGroup,MACAddrLow)

#define EDDS_LL_SWITCH_SET_PORT_STATE(pDDB,pPortStates) \
    EDDS_LL_FUNC(pDDB,setSwitchPortState),pPortStates)

#define EDDS_LL_SWITCH_FLUSH_FILTERING_DB(pDDB,pFlushFilterDB) \
    EDDS_LL_FUNC(pDDB,flushSwitchFilteringDB),pFlushFilterDB)

#define EDDS_LL_SWITCH_CHANGE_PORT(pDDB,PortID,isPulled) \
    EDDS_LL_FUNC(pDDB,changePort),PortID,isPulled)

#ifdef LLIF_CFG_USE_LL_ARP_FILTER
#define EDDS_LL_SET_ARP_FILTER(pDDB,Mode,UserIndex,IPAddr) \
    EDDS_LL_FUNC(pDDB,setArpFilter),Mode,UserIndex,IPAddr)
#endif /* ifdef LLIF_CFG_USE_LL_ARP_FILTER */

#define EDDS_LL_AVAILABLE(pDDB,func) (0 != ((pDDB)->pGlob->pLLFunctionTable->func))


/*****************************************************************************/
/*  end of file EDDS_LLIF.H                                                  */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_LLIF_H */

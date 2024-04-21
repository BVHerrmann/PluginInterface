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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_wpcapdrv.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS WPCAP Interface Adaption                                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20037
#define EPS_MODULE_ID      20037

/* - Includes ------------------------------------------------------------------------------------- */

#include <eps_sys.h>           /* Types / Prototypes / Funcs               */

#if (PSI_CFG_USE_EDDS == 1)
/*-------------------------------------------------------------------------*/
#include <eps_trc.h>           /* Tracing                                  */
#include <eps_locks.h>         /* EPS Locks                                */
#include <eps_cp_hw.h>         /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>     /* PN Device Driver Interface               */
#include <eps_wpcapdrv.h>      /* Own Header                               */
#include <eps_plf.h>           /* EPS PCI Interface functions              */
#include <eps_timer.h>         /* EPS Timer                                */
#include <eps_mem.h>           /* EPS Memory                               */

#include <edds_int.h>          /* EDDS Types                               */
#include <pcap.h>
#include <packet32_inc.h>      /* EDDS Lower Layer Intel Driver            */

/*----------------------------------------------------------------------------*/
/*  Defines                                                                   */
/*----------------------------------------------------------------------------*/

// Partitioning of the DEV and NRT memory, all values in percent
#define CP_MEM_WPCAPDRV_PERCENT_DEV         10
#define CP_MEM_WPCAPDRV_PERCENT_NRT_TX      45
#define CP_MEM_WPCAPDRV_PERCENT_NRT_RX      45

#define CP_MEM_WPCAPDRV_PERCENT_TOTAL (CP_MEM_WPCAPDRV_PERCENT_DEV + CP_MEM_WPCAPDRV_PERCENT_NRT_TX + CP_MEM_WPCAPDRV_PERCENT_NRT_RX)

#if (CP_MEM_WPCAPDRV_PERCENT_TOTAL != 100)
#error "Sum has to be 100 percent!"
#endif

/* - Local Defines -------------------------------------------------------------------------------- */

#define EPS_CFG_WPCAPDRV_MAX_BOARDS             50

#define EPS_WPCAPDRV_CFG_TIMER_OVERSAMPLING_VALUE_MS     ((LSA_UINT32) 1)         // 1 ms for systimer -> Oversampling factor 32
#define EPS_WPCAPDRV_CFG_TIMER_MIN_CBF_START_DELAY_NS    ((LSA_UINT64)31000000)   // 31ms // minimum delay between 2 Cbf executions 
#define EPS_WPCAPDRV_CFG_TIMER_MAX_CBF_START_DELAY_NS    ((LSA_UINT64)64000000)   // 64ms // maximum delay between 2 Cbf executions
#define EPS_WPCAPDRV_CFG_TIMER_MAX_CBF_DURATION_NS       ((LSA_UINT64)10000000)   // 10ms // maximum duration Cbf should last

#ifndef EPS_SUPPRESS_PRINTF
#define EPS_WPCAPDRV_PRINTF_OPEN_CLOSE_INFO 
#define EPS_WPCAPDRV_PRINTF_FOUND_BOARDS    
#endif

/* - Function Forward Declaration ----------------------------------------------------------------- */

static LSA_UINT16  eps_wpcapdrv_enable_interrupt  (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf);
static LSA_UINT16  eps_wpcapdrv_disable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);
static LSA_UINT16  eps_wpcapdrv_set_gpio          (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_wpcapdrv_clear_gpio        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_wpcapdrv_timer_ctrl_start  (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf);
static LSA_UINT16  eps_wpcapdrv_timer_ctrl_stop   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_wpcapdrv_read_trace_data   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
static LSA_UINT16  eps_wpcapdrv_write_trace_data  (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size);
static LSA_UINT16  eps_wpcapdrv_save_dump         (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_wpcapdrv_enable_hw_interrupt    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_wpcapdrv_disable_hw_interrupt   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_wpcapdrv_read_hw_interrupt      (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts);
static LSA_UINT16  eps_wpcapdrv_write_sync_time_lower  (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId);

static LSA_UINT16 eps_wpcapdrv_open(EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
static LSA_UINT16 eps_wpcapdrv_close(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
/* - Typedefs ------------------------------------------------------------------------------------- */

struct eps_wpcapdrv_store_tag;

typedef struct eps_stdmacdrv_device_info_tag
{
	LSA_CHAR* pName;
	LSA_CHAR Name[300];
	EPS_PNDEV_MAC_TYPE uMac;
} EPS_WPCAPDRV_DEVICE_INFO_TYPE, *EPS_WPCAPDRV_DEVICE_INFO_PTR_TYPE;

typedef struct eps_wpcapdrv_tmr_par
{
	LSA_UINT16 TmrID;
	EPS_PNDEV_TMR_TICK_PAR_TYPE TmrPar; // tmr tick parameters
} EPS_WPCAPDRV_TMR_PAR_TYPE, *EPS_WPCAPDRV_TMR_PAR_PTR_TYPE;

typedef struct eps_wpcapdrv_board_tag
{	
	LSA_BOOL bUsed;                             /// flag: Board used
	EPS_PNDEV_HW_TYPE sHw;                      /// storage for EPS PN Dev if management info
	LSA_UINT32 uCountIsrEnabled;	            /// Number of enabled isr / pollthreads. Value is checked at shutdown -> disable all ISRs / Pollthreads before shutdown!
	EPS_PNDEV_CALLBACK_TYPE sIsrPnGathered;     /// Callback function that triggers the poll function of EDDS (edds_scheduler)
	EPS_WPCAPDRV_TMR_PAR_TYPE TmrParPnGathered; /// Wrapper structure in order to use eps_pndev_if_timer_tick
    EPS_SYS_TYPE sysDev;                        /// Sys handle
	EPS_WPCAPDRV_DEVICE_INFO_TYPE sDeviceInfo;
	struct
	{
		PACKET32_LL_HANDLE_TYPE hLL;            /// Lower layer handle
		EDDS_LL_TABLE_TYPE tLLFuncs;            /// Lower layer table
	} EDDS_LL;
	struct eps_wpcapdrv_store_tag* pBackRef;
} EPS_WPCAPDRV_BOARD_TYPE, *EPS_WPCAPDRV_BOARD_PTR_TYPE;

typedef struct eps_wpcapdrv_store_tag
{
	LSA_BOOL bInit;
	LSA_UINT16 hEnterExit;
	EPS_WPCAPDRV_BOARD_TYPE board[EPS_CFG_WPCAPDRV_MAX_BOARDS];
	EPS_WPCAPDRV_DEVICE_INFO_TYPE foundDevices[EPS_CFG_WPCAPDRV_MAX_BOARDS];
	LSA_UINT32 uCntFoundDevices;
} EPS_WPCAPDRV_STORE_TYPE, *EPS_WPCAPDRV_STORE_PTR_TYPE;

/* - Global Data ---------------------------------------------------------------------------------- */
static EPS_WPCAPDRV_STORE_TYPE g_EpsWpcapDrv;
static EPS_WPCAPDRV_STORE_PTR_TYPE g_pEpsWpcapDrv = LSA_NULL;

/* - Source --------------------------------------------------------------------------------------- */

#ifdef EPS_WPCAPDRV_PRINTF_FOUND_BOARDS
static LSA_VOID eps_wpcapdrv_printf_found_boards(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_WPCAPDRV_DEVICE_INFO_PTR_TYPE pDevice;

	printf("\r\nEPS WpcapDrv Found Network Boards:\r\n");
	printf("----------------------------------------");

	for (i=0; i<g_pEpsWpcapDrv->uCntFoundDevices; i++)
	{
		pDevice = &g_pEpsWpcapDrv->foundDevices[i];

		printf("\r\n%02x:%02x:%02x:%02x:%02x:%02x  -  %s",pDevice->uMac[0],pDevice->uMac[1],pDevice->uMac[2],pDevice->uMac[3],pDevice->uMac[4],pDevice->uMac[5],pDevice->pName);
	}

	printf("\r\n----------------------------------------\r\n");
	printf("\r\n");
}
#endif

#ifdef EPS_WPCAPDRV_PRINTF_OPEN_CLOSE_INFO
static LSA_VOID eps_wpcapdrv_printf_openclose_board(LSA_BOOL bOpen, EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard)
{
	if (bOpen)
	{
		printf("\r\nEPS WpcapDrv Board Opened: %s", pBoard->sDeviceInfo.pName );
	}
	else
	{
		printf("\r\nEPS WpcapDrv Board Closed: %s", pBoard->sDeviceInfo.pName );
	}
	printf("\r\n");
}
#endif

/**
* Lock implementation
*/
static LSA_VOID eps_wpcapdrv_undo_init_critical_section(LSA_VOID)
{
    LSA_RESPONSE_TYPE retVal;
    retVal = eps_free_critical_section(g_pEpsWpcapDrv->hEnterExit);
    EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
* Lock implementation
*/
static LSA_VOID eps_wpcapdrv_init_critical_section(LSA_VOID)
{
    LSA_RESPONSE_TYPE retVal;
    retVal = eps_alloc_critical_section(&g_pEpsWpcapDrv->hEnterExit, LSA_FALSE);
    EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
* Lock implementation
*/
static LSA_VOID eps_wpcapdrv_enter(LSA_VOID)
{
    eps_enter_critical_section(g_pEpsWpcapDrv->hEnterExit);
}

/**
* Lock implementation
*/
static LSA_VOID eps_wpcapdrv_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsWpcapDrv->hEnterExit);
}

/**
 * alloc board entry from the internal management structure.
 * - create new board
 * - initialize board structure with function pointers
 * 
 * @param LSA_VOID
 * @return [out] pBoard       Pointer to board structure
 * @return LSA_NULL           if all accessible boards are already in use.
 */
static EPS_WPCAPDRV_BOARD_PTR_TYPE eps_wpcapdrv_alloc_board(LSA_VOID)
{
	LSA_UINT32 ctr;

	for (ctr=0; ctr<EPS_CFG_WPCAPDRV_MAX_BOARDS; ctr++)
	{
	    EPS_WPCAPDRV_BOARD_PTR_TYPE const pBoard = &g_pEpsWpcapDrv->board[ctr];

		if //free board entry available?
		   (!pBoard->bUsed)
		{
			pBoard->sHw.hDevice             = (LSA_VOID*)pBoard;
			pBoard->sHw.EnableIsr           = eps_wpcapdrv_enable_interrupt;
			pBoard->sHw.DisableIsr          = eps_wpcapdrv_disable_interrupt;
			pBoard->sHw.EnableHwIr          = eps_wpcapdrv_enable_hw_interrupt;
            pBoard->sHw.DisableHwIr         = eps_wpcapdrv_disable_hw_interrupt;
            pBoard->sHw.ReadHwIr            = eps_wpcapdrv_read_hw_interrupt;
			pBoard->sHw.SetGpio             = eps_wpcapdrv_set_gpio;
			pBoard->sHw.ClearGpio           = eps_wpcapdrv_clear_gpio;
			pBoard->sHw.TimerCtrlStart      = eps_wpcapdrv_timer_ctrl_start;
			pBoard->sHw.TimerCtrlStop       = eps_wpcapdrv_timer_ctrl_stop;
			pBoard->sHw.ReadTraceData       = eps_wpcapdrv_read_trace_data;
			pBoard->sHw.WriteTraceData      = eps_wpcapdrv_write_trace_data;
            pBoard->sHw.WriteSyncTimeLower  = eps_wpcapdrv_write_sync_time_lower;
            pBoard->sHw.SaveDump            = eps_wpcapdrv_save_dump;
			pBoard->pBackRef                = g_pEpsWpcapDrv;
			pBoard->bUsed                   = LSA_TRUE;

			return pBoard;
		}
	}

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_wpcapdrv_alloc_board: no free board entry available");

	return LSA_NULL;
}

/**
* Stores all available wpcap drivers into a management structure.
*/
static LSA_VOID eps_wpcapdrv_register_device_infos(LSA_VOID)
{
	EPS_PNDEV_IF_DEV_INFO_TYPE sPnDevInfo;

	int nResult;
	pcap_if_t* alldevs;
	pcap_if_t* dev;
	char errbuf[PCAP_ERRBUF_SIZE];
	LPADAPTER lpAdapter;

	struct 
	{
		PACKET_OID_DATA pkt;
		char more[6]; // used to spread the length of the structure buf by 6 x sizeof(char). Used since PACKET_OID_DATA.Data[1] is declared, but the length of the array is PACKET_OID_DATA.Length
	} buf;
    LSA_UNUSED_ARG(buf.more); // prevent lint warning 754 : local structure member 'more' not referenced
	g_pEpsWpcapDrv->uCntFoundDevices = 0;

	// get all available devices
	nResult = pcap_findalldevs(&alldevs, errbuf);
	EPS_ASSERT( 0 == nResult );

	// iterate through all found devices
	for (dev = alldevs ; (NULL != dev) && (g_pEpsWpcapDrv->uCntFoundDevices<EPS_CFG_WPCAPDRV_MAX_BOARDS) ; dev = dev->next)
	{
		// query the mac-address
		lpAdapter = PacketOpenAdapter(dev->name);
		EPS_ASSERT( 0 != lpAdapter );

		buf.pkt.Oid    = OID_802_3_CURRENT_ADDRESS;
		buf.pkt.Length = 6;

		if (PacketRequest(lpAdapter, LSA_FALSE, &buf.pkt) && buf.pkt.Length == 6)
		{
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].pName = &g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].Name[0];

			eps_strcpy((LSA_UINT8*)g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].pName, (LSA_UINT8*)dev->name);

            //Note: PACKET_OID_DATA.Data[1] is an array of the length 1, but the actual length of the array is PACKET_OID_DATA.Length. Surpress lint warnings when accessing the array with values >= 1
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[0]  = buf.pkt.Data[0];
            //lint --e(415) Likely access of out-of-bounds pointer (1 beyond end of data)
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[1]  = buf.pkt.Data[1];
            //lint --e(415) --e(416) Likely access of out-of-bounds pointer (n beyond end of data)
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[2]  = buf.pkt.Data[2]; 
            //lint --e(415) --e(416) Likely access of out-of-bounds pointer (n beyond end of data)
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[3]  = buf.pkt.Data[3]; 
            //lint --e(415) --e(416) Likely access of out-of-bounds pointer (n beyond end of data)
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[4]  = buf.pkt.Data[4]; 
            //lint --e(415) --e(416) Likely access of out-of-bounds pointer (n beyond end of data)
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[5]  = buf.pkt.Data[5]; 

			sPnDevInfo.uPciAddrValid = 0;
			sPnDevInfo.uMacAddrValid = 1;
			sPnDevInfo.uMacAddr[0] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[0];
			sPnDevInfo.uMacAddr[1] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[1];
			sPnDevInfo.uMacAddr[2] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[2];
			sPnDevInfo.uMacAddr[3] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[3];
			sPnDevInfo.uMacAddr[4] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[4];
			sPnDevInfo.uMacAddr[5] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[5];

			sPnDevInfo.uEddType      = LSA_COMP_ID_EDDS;

			eps_pndev_if_register_device(&sPnDevInfo);

			g_pEpsWpcapDrv->uCntFoundDevices++;
		}

		PacketCloseAdapter(lpAdapter);
	}

	if (NULL != dev)
	{
		EPS_FATAL("there are more network adapters installed than defined by EPS_CFG_WPCAPDRV_MAX_BOARDS"); /* there are more network adapters installed than defined by EPS_CFG_WPCAPDRV_MAX_BOARDS => increase EPS_CFG_WPCAPDRV_MAX_BOARDS */
	}

	pcap_freealldevs(alldevs);
}

/**
 * free board entry
 * 
 * @param [in] pBoard	ptr to current board structure
 * @return LSA_VOID
 */
static LSA_VOID eps_wpcapdrv_free_board(EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard)
{
	EPS_ASSERT(pBoard != LSA_NULL);

	EPS_ASSERT(pBoard->bUsed);
	pBoard->bUsed = LSA_FALSE;
}

/**
* Callback function that is called by the systimer of EPS. This function calls eps_pndev_if_timer_tick that handles overruns / underruns.
* Checks the current time as well as the last time this function was called. Since we do an oversampling, we do nothing if the uTickUnderrunMarkNs is not reached.
*
* @see eps_pndev_if_timer_tick - function is called to trigger the real cbf. Also, this function checks for overruns and underruns. A underrun should never occur!
*
* @param   [in] sigval args - wrapper structure, casted to pTmrPar
*/
static LSA_VOID eps_wpcapdrv_timer_cbf (union sigval args)
{
	EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE pTmrPar = (EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE) args.sival_ptr;
	LSA_UINT64  uStartTimeNs;
	LSA_UINT64  uDiffTimeNs;

	// read current StartTime
    uStartTimeNs = eps_get_ticks_100ns();
    // calc diff to last call to see if we need to call eps_pndev_if_timer_tick or skip this tick
	uDiffTimeNs = (uStartTimeNs - pTmrPar->uTickCbfLastCallTimeNs) * 100;

	if	(uDiffTimeNs > pTmrPar->uTickUnderrunMarkNs)
		// tick is not too early.
	{
		// call actual timer Cbf if tick is not too early.
		eps_pndev_if_timer_tick(pTmrPar);
	}
    else
    {
        // skip this tick. Since we do a oversampling, this path is called way more often then the if path.
    }
}

/**
 * open pndevdrv timer for EPS_WPCAPDRV_CFG_TIMER_OVERSAMPLING_VALUE_MS (e.g. 1 ms) eps_wpcapdrv_timer_cbf callbacks

 * Important note: We start the systimer with a shorter cycle, e.g. 1 ms, but we call the eps_pndev_if_timer_tick only if the uTickUnderrunMarkNs is reached (e.g. 32 ms) -> Oversampling.
 * That way, we have up to 31 useless calls if the systimer is able to call this function every millisecond. 
 * The experience showed, that 
 * 1. The Windows OS is not able to achieve a 1 ms timer. Oversampling with 1 ms is better than a 32 ms timer that comes too late
 * 2. If the system is in stress and the multimedia timer is not able to call the cbf in time, windows calls the cbf two or more times in a row to make up for the missed call. 
 *    The EDDS function that is triggered by this function shall _never_ be called too soon 
 * @see eps_wpcapdrv_timer_cbf - implements oversampling
 * 
 * @param   [in] pBoard 		    ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK,       upon successful execution		
 */
static LSA_UINT16 eps_wpcapdrv_start_pn_gathered_timer(EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard)
{
	EPS_WPCAPDRV_TMR_PAR_PTR_TYPE pTmrPar = &pBoard->TmrParPnGathered;

	eps_memset(pTmrPar, 0, sizeof(EPS_WPCAPDRV_TMR_PAR_TYPE));
	
	pTmrPar->TmrPar.pTickCbf				= &pBoard->sIsrPnGathered;
	pTmrPar->TmrPar.uTickCbfOverrunMarkNs	= EPS_WPCAPDRV_CFG_TIMER_MAX_CBF_DURATION_NS;
	pTmrPar->TmrPar.uTickUnderrunMarkNs		= EPS_WPCAPDRV_CFG_TIMER_MIN_CBF_START_DELAY_NS;
	pTmrPar->TmrPar.uTickOverrunMarkNs		= EPS_WPCAPDRV_CFG_TIMER_MAX_CBF_START_DELAY_NS;
	// save ref time when timer was started
	pTmrPar->TmrPar.uTickCbfLastCallTimeNs  = eps_get_ticks_100ns();
    
    /// Allocate a sytimer with 1 ms. Note: This will do a oversampling
	return eps_timer_allocate_sys_timer(&pTmrPar->TmrID, "EPS_WPCAPGPISR", EPS_WPCAPDRV_CFG_TIMER_OVERSAMPLING_VALUE_MS, eps_wpcapdrv_timer_cbf, &pTmrPar->TmrPar);
}

/**
 * close systimer timer for EPS_WPCAPDRV_CFG_TIMER_OVERSAMPLING_VALUE_MS eps_wpcapdrv_timer_cbf callbacks
 * 
 * @param   [in] pBoard 		    ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK,       upon successful execution		
 */
static LSA_UINT16 eps_wpcapdrv_shutdown_pn_gathered_timer(EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 result;
	EPS_WPCAPDRV_TMR_PAR_PTR_TYPE pTmrPar = &pBoard->TmrParPnGathered;

	result = eps_timer_free_sys_timer(pTmrPar->TmrID);

	if (result == EPS_PNDEV_RET_OK)
	{
		// show statistics
		EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_wpcapdrv_shutdown_pn_gathered_timer: Ticks: %u, TickOverruns: %u, TickUnderruns: %u, CbfOverruns: %u", 
							pTmrPar->TmrPar.uTickCount, pTmrPar->TmrPar.uTickOverrunCount, pTmrPar->TmrPar.uTickUnderrunCount,pTmrPar->TmrPar.uTickCbfOverrunCount);

		eps_memset(pTmrPar, 0, sizeof(EPS_WPCAPDRV_TMR_PAR_TYPE));
	}

	return result;
}

/**
 * enable interrupt in wpcap driver. Currently, we pnly support polling mode for wpcap variants.
 * 
 * @param [in] pHwInstIn                - handle to board
 * @param [in] pInterrupt               - event mode (interrupt, polling)
 * @return EPS_PNDEV_RET_ERR            - something went wrong
 * @return EPS_PNDEV_RET_OK             - everything went better than expected
 * @return EPS_PNDEV_RET_UNSUPPORTED    - mode not supported
 */
static LSA_UINT16 eps_wpcapdrv_enable_interrupt_intern (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf)
{
	EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	pBoard = (EPS_WPCAPDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	switch (*pInterrupt)
	{
		case EPS_PNDEV_ISR_ISOCHRONOUS:
			 EPS_FATAL("EPS_PNDEV_ISR_ISOCHRONOUS is not implemented in wpcap driver"); /* not supported */
			break;
		case EPS_PNDEV_ISR_PN_GATHERED:
			if (pBoard->sIsrPnGathered.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			pBoard->sIsrPnGathered = *pCbf;

            result = eps_wpcapdrv_start_pn_gathered_timer(pBoard);

			if (result != EPS_PNDEV_RET_OK)
			{
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_wpcapdrv_enable_interrupt_intern: Error while enabling the polling mode!");
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
            else
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_wpcapdrv_enable_interrupt_intern: Polling mode enabled");
            }
			break;
		case EPS_PNDEV_ISR_PN_NRT:
		  EPS_FATAL("enabling NRT interrupts is not supported in wpcap driver"); /* not supported */
			break;
        case EPS_PNDEV_ISR_INTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_INTERRUPT is not implemented in wpcap driver");            
            break;
        case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_POLLINTERRUPT is not implemented in wpcap driver");            
            break;
        case EPS_PNDEV_ISR_CPU:
            EPS_FATAL("EPS_PNDEV_ISR_CPU is not implemented in wpcap driver");            
            break;
        case EPS_PNDEV_ISR_IPC_RX:
            EPS_FATAL("EPS_PNDEV_ISR_IPC_RX is not implemented in wpcap driver");
            break;
        case EPS_PNDEV_ISR_UNDEFINED:
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
	}

	if (result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled++;
	}

	return result;
}

/**
 * disable interrupt in wpcap driver. Currently, we pnly support polling mode for wpcap variants.
 * 
 * @param [in] pHwInstIn                - handle to board
 * @param [in] pInterrupt               - event mode (interrupt, polling)
 * @return EPS_PNDEV_RET_ERR            - something went wrong
 * @return EPS_PNDEV_RET_OK             - everything went better than expected
 * @return EPS_PNDEV_RET_UNSUPPORTED    - mode not supported
 */
static LSA_UINT16 eps_wpcapdrv_disable_interrupt_intern (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt)
{
	EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	pBoard = (EPS_WPCAPDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(pBoard->uCountIsrEnabled != 0);

	switch (*pInterrupt)
	{
		case EPS_PNDEV_ISR_ISOCHRONOUS:
			EPS_FATAL("EPS_PNDEV_ISR_ISOCHRONOUS is not implemented in wpcap driver");     
			break;
		case EPS_PNDEV_ISR_PN_GATHERED:
			if (pBoard->sIsrPnGathered.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			result = eps_wpcapdrv_shutdown_pn_gathered_timer(pBoard);

			if (result == EPS_PNDEV_RET_OK)
			{
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_wpcapdrv_enable_interrupt_intern: Error while disabling the polling mode!");
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
            else
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_wpcapdrv_disable_interrupt_intern: Polling mode disabled");
            }
			break;
		case EPS_PNDEV_ISR_PN_NRT:
		  EPS_FATAL("disabling NRT interrupts is not supported in wpcap driver"); /* not supported */
			break;
        case EPS_PNDEV_ISR_INTERRUPT:
            EPS_FATAL("disabling EPS_PNDEV_ISR_INTERRUPT is not implemented in wpcap driver");            
            break;
        case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("disabling EPS_PNDEV_ISR_POLLINTERRUPT is not implemented in wpcap driver");            
            break;
        case EPS_PNDEV_ISR_CPU:
            EPS_FATAL("disabling EPS_PNDEV_ISR_CPU is not implemented in wpcap driver");            
            break;
        case EPS_PNDEV_ISR_IPC_RX:
            EPS_FATAL("disabling EPS_PNDEV_ISR_IPC_RX is not implemented in wpcap driver");
            break;
        case EPS_PNDEV_ISR_UNDEFINED:
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
	}

	if (result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled--;
	}

	return result;
}

/**
 * Setup DEV and NRT memory pools
 *
 * @param [in] pboardInfo       - pointer to board info
 * @return LSA_VOID
 */
static LSA_VOID eps_wpcapdrv_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid)
{
	EPS_BOARD_INFO_TYPE	* const pboardInfo           = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;
    LSA_UINT32            const dev_nrt_mem_size_all = pboardInfo->dev_mem.size;

    // setup DEV and NRT memory pools: divide the already allocated DEV memory pool into DEV and NRT memory pools

    //pboardInfo->dev_mem.base_ptr  = already valid
    //pboardInfo->dev_mem.phy_addr  = already valid
    pboardInfo->dev_mem.size        = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_WPCAPDRV_PERCENT_DEV);

    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->dev_mem.base_ptr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->dev_mem.phy_addr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_WPCAPDRV_PERCENT_NRT_TX);

    pboardInfo->nrt_rx_mem.base_ptr = pboardInfo->nrt_tx_mem.base_ptr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.phy_addr = pboardInfo->nrt_tx_mem.phy_addr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_WPCAPDRV_PERCENT_NRT_RX);

	EPS_ASSERT(pboardInfo->dev_mem.base_ptr != LSA_NULL);
}


/**
 * Installing EPS WinPCap driver for windows OS. The EPS WPcap PN Dev Driver implements the eps_pndev_if.
 * 
 * The EPS WinPCap driver requires Wpcap installed in the system.
 * 
 * This function links the functions required by the EPS PN Dev IF to internal functions
 *  eps_pndev_if_open      -> eps_wpcapdrv_open
 *  eps_pndev_if_close     -> eps_wpcapdrv_close
 *  eps_pndev_if_uninstall -> eps_wpcapdrv_uninstall
 * 
 * @see eps_pndev_if_register - this function is called to register the PN Dev implementation into the EPS PN Dev IF.
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_wpcapdrv_install(LSA_VOID)
{
	EPS_PNDEV_IF_TYPE sPnWpcapDrvIf;

	eps_memset(&g_EpsWpcapDrv, 0, sizeof(g_EpsWpcapDrv));
	g_pEpsWpcapDrv = &g_EpsWpcapDrv;

	g_pEpsWpcapDrv->bInit = LSA_TRUE;

	eps_wpcapdrv_init_critical_section();

	eps_wpcapdrv_register_device_infos();

	#ifdef EPS_WPCAPDRV_PRINTF_FOUND_BOARDS
	eps_wpcapdrv_printf_found_boards();
	#endif

	//Init Interface
	sPnWpcapDrvIf.open      = eps_wpcapdrv_open;
	sPnWpcapDrvIf.close     = eps_wpcapdrv_close;
	sPnWpcapDrvIf.uninstall = eps_wpcapdrv_uninstall;

	eps_pndev_if_register(&sPnWpcapDrvIf);
}

/**
 * uninstall EPS WPcap driver. Deletes management structures.
 * 
 * @param	LSA_VOID
 * @return 	LSA_VOID
 */
LSA_VOID eps_wpcapdrv_uninstall(LSA_VOID)
{
	EPS_ASSERT(g_pEpsWpcapDrv->bInit);

	eps_wpcapdrv_undo_init_critical_section();

	g_pEpsWpcapDrv->bInit = LSA_FALSE;

	g_pEpsWpcapDrv = LSA_NULL;
}

/**
 * Open a PN board supported by WPCal for the given MAC Address
 * 
 * This function implements the eps_pndev_if function eps_pndev_if_open. 
 * The function may only be called once since there is only one HD.
 * The lower layer table from the edds is linked into the lower layer table here.
 * 
 * @see eps_pndev_if_open             - calls this function
 * 
 * @param	[in] pLocation 		            ptr including MAC Address
 * @param	[in] pOption			        ptr to options (not used)
 * @param	[out] ppHwInstOut		        pointer to ptr to structure with hardware configuration
 * @param 	[in] hd_id			            index of hd
 * @return 	EPS_PNDEV_RET_OK                everything went better then expected.
 * @return  EPS_PNDEV_RET_DEV_NOT_FOUND     device was not found (wrong MAC address / did you disabled the board in device manager without reboot?)
 */
static LSA_UINT16 eps_wpcapdrv_open(EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id)
{
	EPS_WPCAPDRV_BOARD_PTR_TYPE       pBoard;
	EPS_WPCAPDRV_DEVICE_INFO_PTR_TYPE pDeviceInfo = LSA_NULL;
	EDDS_LL_TABLE_TYPE *              pLLTable;
	EPS_BOARD_INFO_PTR_TYPE           pBoardInfo;
	LSA_UINT16                        uPortMacEnd;
	LSA_UINT16                        i;
	LSA_UINT16                        result = EPS_PNDEV_RET_OK;

	LSA_UNUSED_ARG(pOption);

	EPS_ASSERT(g_pEpsWpcapDrv->bInit);
	EPS_ASSERT(pLocation   != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	eps_wpcapdrv_enter();

	pBoard = eps_wpcapdrv_alloc_board();
	EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */

	/* Connect Board HW / Map Memory */
	
	/* NOTE: EPS_APP_GET_MAC_ADDR is not called within eps_wpcapdriver
	 * The board is selected by MAC-Address. This MAC-Address is used.
	 */

	if (pLocation->eLocationSelector == EPS_PNDEV_LOCATION_MAC)
	{
		for (i=0; i<g_pEpsWpcapDrv->uCntFoundDevices; i++)
		{
			pDeviceInfo = &g_pEpsWpcapDrv->foundDevices[i];
			if ( (pDeviceInfo->uMac[0] == pLocation->Mac[0]) &&
			     (pDeviceInfo->uMac[1] == pLocation->Mac[1]) &&
				 (pDeviceInfo->uMac[2] == pLocation->Mac[2]) &&
				 (pDeviceInfo->uMac[3] == pLocation->Mac[3]) &&
				 (pDeviceInfo->uMac[4] == pLocation->Mac[4]) &&
				 (pDeviceInfo->uMac[5] == pLocation->Mac[5]) )
			{
				break; /* leave loop */
			}
			else
			{
				pDeviceInfo = LSA_NULL;
			}
		}
	}

	if (pDeviceInfo != LSA_NULL)
	{
		/* Fill EDDS LL */
		pBoard->sDeviceInfo = *pDeviceInfo;

		pLLTable = &pBoard->EDDS_LL.tLLFuncs;

		/* EDDS init/setup/shutdown/close functions */
		pLLTable->open                      = PACKET32_LL_OPEN;
		pLLTable->setup                     = PACKET32_LL_SETUP;
		pLLTable->shutdown                  = PACKET32_LL_SHUTDOWN;
		pLLTable->close                     = PACKET32_LL_CLOSE;

		/* Transmit buffer handling functions */
		pLLTable->enqueueSendBuffer         = PACKET32_LL_SEND;
		pLLTable->getNextFinishedSendBuffer = PACKET32_LL_SEND_STS;
		pLLTable->triggerSend               = PACKET32_LL_SEND_TRIGGER;

		/* Receive buffer handling functions */
		pLLTable->getNextReceivedBuffer     = PACKET32_LL_RECV;
		pLLTable->provideReceiveBuffer      = PACKET32_LL_RECV_PROVIDE;
		pLLTable->triggerReceive            = PACKET32_LL_RECV_TRIGGER;

		/* Information functions */
		pLLTable->getLinkState              = PACKET32_LL_GET_LINK_STATE;
		pLLTable->getStatistics             = PACKET32_LL_GET_STATS;

		/* MAC address management functions */
		pLLTable->enableMC                  = PACKET32_LL_MC_ENABLE;
		pLLTable->disableMC                 = PACKET32_LL_MC_DISABLE;

		/* Scheduler function */
        pLLTable->recurringTask             = PACKET32_LL_RECURRING_TASK;

		/* Control functions */
		pLLTable->setLinkState              = PACKET32_LL_SET_LINK_STATE;
		pLLTable->backupLocationLEDs        = PACKET32_LL_LED_BACKUP_MODE;
		pLLTable->restoreLocationLEDs       = PACKET32_LL_LED_RESTORE_MODE;
		pLLTable->setLocationLEDs           = PACKET32_LL_LED_SET_MODE;

		/* Switch functions */
        #ifdef LLIF_CFG_SWITCH_SUPPORT
		pLLTable->setSwitchPortState        = PACKET32_LL_SWITCH_SET_PORT_STATE;
		pLLTable->controlSwitchMulticastFwd = PACKET32_LL_SWITCH_MULTICAST_FWD_CTRL;
		pLLTable->flushSwitchFilteringDB    = PACKET32_LL_SWITCH_FLUSH_FILTERING_DB;
        #else
		pLLTable->setSwitchPortState        = LSA_NULL;
		pLLTable->controlSwitchMulticastFwd = LSA_NULL;
		pLLTable->flushSwitchFilteringDB    = LSA_NULL;
        #endif

        pLLTable->changePort    			= PACKET32_LL_SWITCH_CHANGE_PORT;

	    /* EDDS set arp filter function */
		#ifdef LLIF_CFG_USE_LL_ARP_FILTER
		pLLTable->setArpFilter = LSA_NULL;
		#endif

		// fill Device Info

		pBoardInfo = &pBoard->sHw.EpsBoardInfo;
		eps_memset(pBoardInfo, 0, sizeof(*pBoardInfo));

		pBoard->sysDev.hd_nr        = hd_id;
		pBoard->sysDev.pnio_if_nr   = 0;	// don't care
		pBoard->sysDev.edd_comp_id  = LSA_COMP_ID_EDDS;

		// Common HD settings
		pBoardInfo->edd_type        = LSA_COMP_ID_EDDS;
		pBoardInfo->hd_sys_handle   = &pBoard->sysDev;

		// init User to HW port mapping (used PSI GET HD PARAMS)
		eps_hw_init_board_port_param( pBoardInfo );

		// Port specific setup
		pBoardInfo->nr_of_ports = 1;

		// Interface Mac
		pBoardInfo->if_mac.mac[0] = pBoard->sDeviceInfo.uMac[0];
		pBoardInfo->if_mac.mac[1] = pBoard->sDeviceInfo.uMac[1];
		pBoardInfo->if_mac.mac[2] = pBoard->sDeviceInfo.uMac[2];
		pBoardInfo->if_mac.mac[3] = pBoard->sDeviceInfo.uMac[3];
		pBoardInfo->if_mac.mac[4] = pBoard->sDeviceInfo.uMac[4];
		pBoardInfo->if_mac.mac[5] = pBoard->sDeviceInfo.uMac[5];

		// Port MAC - Ersatz Mac Adressen laut Norm bei Standard Hw
		for (i = 0; i < pBoardInfo->nr_of_ports; i++ )
		{
			uPortMacEnd = 0x3840 + i;

			pBoardInfo->port_mac[i+1].mac[0] = 0x08;
			pBoardInfo->port_mac[i+1].mac[1] = 0x00;
			pBoardInfo->port_mac[i+1].mac[2] = 0x06;
			pBoardInfo->port_mac[i+1].mac[3] = 0x9D;
			pBoardInfo->port_mac[i+1].mac[4] = (LSA_UINT8)(uPortMacEnd>>8);
			pBoardInfo->port_mac[i+1].mac[5] = (LSA_UINT8)(uPortMacEnd&0xff);

			// setup portmapping (=1:1)
			pBoardInfo->port_map[i+1].hw_phy_nr  = i;
			pBoardInfo->port_map[i+1].hw_port_id = i+1;
		}

	    // setup DEV and NRT memory pools (local mem is used)
        // 1. only one memory pool is allocated and stored in DEV parameters
		pBoardInfo->dev_mem.size     = 0x2000000;
		pBoardInfo->dev_mem.base_ptr = (LSA_UINT8*)eps_mem_alloc(pBoardInfo->dev_mem.size, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_NRT_MEM);
		pBoardInfo->dev_mem.phy_addr = (LSA_UINT32)((pBoardInfo->dev_mem.base_ptr - (LSA_UINT32)g_pEpsData->HwInfo.LocalMemPool.pBase) + g_pEpsData->HwInfo.LocalMemPool.uPhyAddr);
        // 2. the memory pool is divided into DEV and NRT memory pools
        eps_wpcapdrv_FillDevNrtMemParams(pBoardInfo);

		// prepare process image settings (local mem is used)
		// Note: we use the full size of KRAM, real size is calculated in PSI
		pBoardInfo->pi_mem.base_ptr = 0;
		pBoardInfo->pi_mem.phy_addr = 0;
		pBoardInfo->pi_mem.size     = 0;

		// setup HIF buffer
		pBoardInfo->hif_mem.base_ptr = 0;
		pBoardInfo->hif_mem.phy_addr = 0;
		pBoardInfo->hif_mem.size     = 0;

		// add the LL settings
		pBoardInfo->edds.is_valid          = LSA_TRUE;
		pBoardInfo->edds.ll_handle         = (LSA_VOID_PTR_TYPE)&pBoard->EDDS_LL.hLL;
		pBoardInfo->edds.ll_function_table = (LSA_VOID_PTR_TYPE)pLLTable;
		pBoardInfo->edds.ll_params         = (LSA_VOID*)pDeviceInfo->pName;

		pBoardInfo->edds.bFillActive        = LSA_FALSE;
        pBoardInfo->edds.bHsyncModeActive   = LSA_FALSE;
		pBoardInfo->edds.hTimer             = 0;
		pBoardInfo->edds.hMsgqHigh          = 0;
		pBoardInfo->edds.hMsgqLow           = 0;
        pBoardInfo->edds.hMsgqActive        = 0;
		pBoardInfo->edds.hThreadHigh        = 0;
		pBoardInfo->edds.hThreadLow         = 0;

		// fill other Params
		pBoard->sHw.asic_type               = EPS_PNDEV_ASIC_INTEL_XX;
		pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_INTEL_XX;
		pBoard->sHw.IrMode                  = EPS_PNDEV_IR_MODE_POLL;

		// initialization of MediaType
		eps_hw_init_board_port_media_type(pBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

        EPS_SYSTEM_TRACE_07(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_wpcapdrv_open succeeded. hd_nr = %d, mac = %02x:%02x:%02x:%02x:%02x:%02x ",
            pBoard->sysDev.hd_nr,
            pBoardInfo->if_mac.mac[0], pBoardInfo->if_mac.mac[1], pBoardInfo->if_mac.mac[2], 
            pBoardInfo->if_mac.mac[3], pBoardInfo->if_mac.mac[4], pBoardInfo->if_mac.mac[5]);
	}
	else
	{
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_wpcapdrv_open failed. Please check MAC Address. If you have disabled the board in device manager in the meantime, please do a reboot.");
		result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
	}

	if (result == EPS_PNDEV_RET_OK)
	{
		#ifdef EPS_WPCAPDRV_PRINTF_OPEN_CLOSE_INFO
		eps_wpcapdrv_printf_openclose_board(LSA_TRUE, pBoard);
		#endif

		*ppHwInstOut = &pBoard->sHw;
	}
	else
	{
		eps_wpcapdrv_free_board(pBoard);
	}

	eps_wpcapdrv_exit();

	return result;
}

/**
 * close device 
 * 
 * This function implements the eps_pndev_if function eps_pndev_if_close.
 * 
 * @see eps_pndev_if_close          - calls this function
 * 
 * @param	[in] pHwInstIn		    pointer to structure with hardware configuration
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_wpcapdrv_close (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
	EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard;
	EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;
    LSA_UINT16 retValDisableIsr;
    LSA_UINT16 retValFree;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_wpcapdrv_enter();

	pBoard = (EPS_WPCAPDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	if (pBoard->uCountIsrEnabled != 0)
	{
		if (pBoard->sIsrPnGathered.pCbf != LSA_NULL)
		{
			IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;
            retValDisableIsr = eps_wpcapdrv_disable_interrupt_intern(pHwInstIn, &IsrDesc);
            EPS_ASSERT(EPS_PNDEV_RET_OK == retValDisableIsr);
		}
	}

	EPS_ASSERT(pBoard->uCountIsrEnabled == 0);

	// free DEV and NRT memory pools (local mem is used): one memory pool was originally allocated and stored in DEV parameters
	EPS_ASSERT(pBoard->sHw.EpsBoardInfo.dev_mem.base_ptr != LSA_NULL);
    retValFree = eps_mem_free(pBoard->sHw.EpsBoardInfo.dev_mem.base_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_NRT_MEM);
    EPS_ASSERT(LSA_RET_OK == retValFree);

	eps_wpcapdrv_free_board(pBoard);

	#ifdef EPS_WPCAPDRV_PRINTF_OPEN_CLOSE_INFO
	eps_wpcapdrv_printf_openclose_board(LSA_FALSE, pBoard);
	#endif

	eps_wpcapdrv_exit();

	return EPS_PNDEV_RET_OK;
}

/**
 * enable interrupt for board
 * 
 * @param	[in] pHwInstIn		pointer to structure with hardware configuration
 * @param	[in] pInterrupt		pointer to enum with method type
 * @param	[in] pCbf			ptr to Callbackfunction
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_wpcapdrv_enable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf)
{
	LSA_UINT16 result;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_wpcapdrv_enter();

	result = eps_wpcapdrv_enable_interrupt_intern(pHwInstIn, pInterrupt, pCbf);

	eps_wpcapdrv_exit();

	return result;
}

/**
 * disable interrupt for board
 * 
 * @param	[in] pHwInstIn		pointer to structure with hardware configuration
 * @param	[in] pInterrupt		pointer to enum with method type
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_wpcapdrv_disable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt)
{
	LSA_UINT16 result;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_wpcapdrv_enter();

	result = eps_wpcapdrv_disable_interrupt_intern(pHwInstIn, pInterrupt);

	eps_wpcapdrv_exit();

	return result;
}

/**
 * Set gpio for a given board. Currently Not supported
 * 
 * @param	[in] pHwInstIn		        pointer to structure with hardware configuration
 * @param	[in] gpio			        enum with gpios
 * @return  EPS_PNDEV_RET_UNSUPPORTED   Board type is not supported.
 */
static LSA_UINT16 eps_wpcapdrv_set_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(gpio);
	/* Not Supported */
	return EPS_PNDEV_RET_OK;
}

/**
 * Clear gpio for a given board. Currently Not supported
 * 
 * @param	[in] pHwInstIn		        pointer to structure with hardware configuration
 * @param	[in] gpio			        enum with gpios
 * @return  EPS_PNDEV_RET_UNSUPPORTED   Board type is not supported.
 */
static LSA_UINT16 eps_wpcapdrv_clear_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(gpio);
	/* Not Supported */
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * start timer. Currently not supported.
 * 
 * @param [in] pHwInstIn 		pointer to structure with hardware configuration
 * @param [in] pCbf				ptr to Callbackfunction
 * @return 	EPS_PNDEV_RET_UNSUPPORTED	
 */
static LSA_UINT16 eps_wpcapdrv_timer_ctrl_start (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(pCbf);
	EPS_FATAL("function not supported"); /* Not Supported */
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * stop timer. Currently not supported.
 * 
 * @param [in] pHwInstIn 		pointer to structure with hardware configuration
 * @return 	EPS_PNDEV_RET_UNSUPPORTED	
 */
static LSA_UINT16 eps_wpcapdrv_timer_ctrl_stop (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
	LSA_UNUSED_ARG(pHwInstIn);
	EPS_FATAL("function not supported"); /* Not Supported */
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * read trace data. This function will never be supported by EPS Wpcap driver since we cannot download any firmware to the board.
 * 
 * @param [in]  pHwInstIn 			pointer to structure with hardware configuration
 * @param [in]  offset				offset where should be read from
 * @param [in]  ptr					destination pointer for the data
 * @param [in]  size				size of data
 * @return 	EPS_PNDEV_RET_UNSUPPORTED	
 */
static LSA_UINT16 eps_wpcapdrv_read_trace_data (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
	EPS_FATAL("function not supported"); /* Not Supported */
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * write trace data. This function will never be supported by EPS Wpcap driver since we cannot download any firmware to the board.
 * 
 * @param [in]  pHwInstIn 			pointer to structure with hardware configuration
 * @param [in]  ptr					source pointer for the data
 * @param [out] offset				offset where should be write to
 * @param [in]  size				size of data
 * @return 	EPS_PNDEV_RET_UNSUPPORTED	
 */
static LSA_UINT16 eps_wpcapdrv_write_trace_data (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
	EPS_FATAL("function not supported"); /* Not Supported */
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Reads the time of a lower instance and traces it. NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn - handle to lower instance. Not used
 * @param [in] lowerCpuId - handle to lower CPU. Used in trace macro.
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_wpcapdrv_write_sync_time_lower(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(lowerCpuId);
	EPS_FATAL("Writing lower sync time not supported");
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
	return EPS_PNDEV_RET_UNSUPPORTED;
}

static LSA_UINT16 eps_wpcapdrv_save_dump (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	EPS_FATAL("function not supported"); /* Not Supported */
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
	return EPS_PNDEV_RET_OK;
}

/**
 * Enable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_wpcapdrv_enable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Enable HW Interrupt in wpcap not supported in this driver."); 
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Disable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_wpcapdrv_disable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Disable HW Interrupt in wpcap not supported in this driver."); 
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Read HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_wpcapdrv_read_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(interrupts);
    EPS_FATAL("Read HW Interrupt in wpcap not supported in this driver."); 
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/*----------------------------------------------------------------------------*/
#endif // PSI_CFG_USE_EDDS

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

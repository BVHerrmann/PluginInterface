/*****************************************************************************/
/*  Copyright (C) 2017 Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  F i l e               &F: eps_lpcapdrv.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.01.00.09         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2017-12-19                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS LPCAP Interface Adaption                                             */
/*                                                                           */
/*****************************************************************************/
#if defined (PSI_EDDS_CFG_HW_LPCAP)


#define LTRC_ACT_MODUL_ID  20037
#define EPS_MODULE_ID      20037

/* - Includes ------------------------------------------------------------------------------------- */

#include <eps_sys.h>           /* Types / Prototypes / Funcs               */

#if ( PSI_CFG_USE_EDDS == 1 )
/*-------------------------------------------------------------------------*/
#include <eps_trc.h>           /* Tracing                                  */
#include <eps_locks.h>         /* EPS Locks                                */
#include <eps_cp_hw.h>         /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>     /* PN Device Driver Interface               */
#include <eps_plf.h>           /* EPS PCI Interface functions              */
#include <eps_timer.h>         /* EPS Timer                                */
#include <eps_mem.h>

#include "eps_pn_drv_if.h"

#include <edds_int.h>          /* EDDS Types                               */
#include <pcap/pcap.h>

#include "libpcap_inc.h"

#include <netinet/in.h>
#include <errno.h>

/*----------------------------------------------------------------------------*/
/*  Defines                                                                   */
/*----------------------------------------------------------------------------*/

// Partitioning of the DEV and NRT memory, all values in percent
#define CP_MEM_LPCAPDRV_PERCENT_DEV         10
#define CP_MEM_LPCAPDRV_PERCENT_NRT_TX      45
#define CP_MEM_LPCAPDRV_PERCENT_NRT_RX      45

#define CP_MEM_LPCAPDRV_PERCENT_TOTAL (CP_MEM_LPCAPDRV_PERCENT_DEV + CP_MEM_LPCAPDRV_PERCENT_NRT_TX + CP_MEM_LPCAPDRV_PERCENT_NRT_RX)

#if (CP_MEM_LPCAPDRV_PERCENT_TOTAL != 100)
#error "Sum has to be 100 percent!"
#endif

/* - Local Defines -------------------------------------------------------------------------------- */

#define EPS_CFG_LPCAPDRV_MAX_BOARDS             50

#define EPS_LPCAPDRV_CFG_TIMER_OVERSAMPLING_VALUE_MS     ((LSA_UINT32) 1)                   // 1 ms for systimer -> Oversampling factor 32
#define EPS_LPCAPDRV_CFG_TIMER_MIN_CBF_START_DELAY_NS    ((LSA_UINT64)  500000)             // 0.5ms // minimum delay between 2 Cbf executions
#define EPS_LPCAPDRV_CFG_TIMER_MAX_CBF_START_DELAY_NS    ((LSA_UINT64) 2500000)             // 2.5ms // maximum delay between 2 Cbf executions
#define EPS_LPCAPDRV_CFG_TIMER_MAX_CBF_DURATION_NS       ((LSA_UINT64) 1000000)             // 1ms // maximum duration Cbf should last

#ifndef EPS_SUPPRESS_PRINTF
#define EPS_LPCAPDRV_PRINTF_OPEN_CLOSE_INFO
#define EPS_LPCAPDRV_PRINTF_FOUND_BOARDS
#endif


/* - Function Forward Declaration ----------------------------------------------------------------- */

static LSA_UINT16  eps_lpcapdrv_enable_interrupt        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf);
static LSA_UINT16  eps_lpcapdrv_disable_interrupt       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);
static LSA_UINT16  eps_lpcapdrv_set_gpio                (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_lpcapdrv_clear_gpio              (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_lpcapdrv_timer_ctrl_start        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf);
static LSA_UINT16  eps_lpcapdrv_timer_ctrl_stop         (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_lpcapdrv_read_trace_data         (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
static LSA_UINT16  eps_lpcapdrv_write_trace_data        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size);
static LSA_UINT16  eps_lpcapdrv_save_dump               (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_lpcapdrv_enable_hw_interrupt     (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_lpcapdrv_disable_hw_interrupt    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_lpcapdrv_read_hw_interrupt       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts);
static LSA_UINT16  eps_lpcapdrv_write_sync_time_lower   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId);
static LSA_RESULT  eps_lpcapdrv_get_interface_mac       (EPS_MAC_TYPE *pMacArray);

static LSA_UINT16  eps_lpcapdrv_open                    (EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
static LSA_UINT16  eps_lpcapdrv_close                   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
LSA_VOID eps_lpcapdrv_uninstall(LSA_VOID);
/* - Typedefs ------------------------------------------------------------------------------------- */

struct eps_lpcapdrv_store_tag;

typedef LPCAP_LL_HANDLE_TYPE LPCAP_LL_HANDLE_TYPE;

typedef struct eps_stdmacdrv_device_info_tag
{
    LSA_CHAR* pName;
    LSA_CHAR Name[300];
    EPS_PNDEV_MAC_TYPE uMac;
} EPS_LPCAPDRV_DEVICE_INFO_TYPE, *EPS_LPCAPDRV_DEVICE_INFO_PTR_TYPE;

typedef struct eps_lpcapdrv_tmr_par
{
    LSA_UINT16 TmrID;
    EPS_PNDEV_TMR_TICK_PAR_TYPE TmrPar; // tmr tick parameters
} EPS_LPCAPDRV_TMR_PAR_TYPE, *EPS_LPCAPDRV_TMR_PAR_PTR_TYPE;

typedef struct eps_lpcapdrv_board_tag
{
    LSA_BOOL bUsed;                             /// flag: Board used
    EPS_PNDEV_HW_TYPE sHw;                      /// storage for EPS PN Dev if management info
    LSA_UINT32 uCountIsrEnabled;                /// Number of enabled isr / pollthreads. Value is checked at shutdown -> disable all ISRs / Pollthreads before shutdown!
    EPS_PNDEV_CALLBACK_TYPE sIsrPnGathered;     /// Callback function that triggers the poll function of EDDS (edds_scheduler)
    EPS_LPCAPDRV_TMR_PAR_TYPE TmrParPnGathered; /// Wrapper structure in order to use eps_pndev_if_timer_tick
    EPS_SYS_TYPE sysDev;                        /// Sys handle
    EPS_LPCAPDRV_DEVICE_INFO_TYPE sDeviceInfo;
    struct
    {
        LPCAP_LL_HANDLE_TYPE hLL;            /// Lower layer handle
        EDDS_LL_TABLE_TYPE tLLFuncs;            /// Lower layer table
    } EDDS_LL;
    struct eps_lpcapdrv_store_tag* pBackRef;
} EPS_LPCAPDRV_BOARD_TYPE, *EPS_LPCAPDRV_BOARD_PTR_TYPE;

typedef struct eps_lpcapdrv_store_tag
{
    LSA_BOOL bInit;
    LSA_UINT16 hEnterExit;
    EPS_LPCAPDRV_BOARD_TYPE board[EPS_CFG_LPCAPDRV_MAX_BOARDS];
    EPS_LPCAPDRV_DEVICE_INFO_TYPE foundDevices[EPS_CFG_LPCAPDRV_MAX_BOARDS];
    LSA_UINT32 uCntFoundDevices;
} EPS_LPCAPDRV_STORE_TYPE, *EPS_LPCAPDRV_STORE_PTR_TYPE;

/* - Global Data ---------------------------------------------------------------------------------- */
static EPS_LPCAPDRV_STORE_TYPE g_EpsLpcapDrv;
static EPS_LPCAPDRV_STORE_PTR_TYPE g_pEpsLpcapDrv = LSA_NULL;

/* - Source --------------------------------------------------------------------------------------- */

#ifdef EPS_LPCAPDRV_PRINTF_FOUND_BOARDS
static LSA_VOID eps_lpcapdrv_printf_found_boards(LSA_VOID)
{
    LSA_UINT32 i;
    EPS_LPCAPDRV_DEVICE_INFO_PTR_TYPE pDevice;

    printf("\r\nEPS LpcapDrv Found Network Boards:\r\n");
    printf("----------------------------------------");

    for (i=0; i<g_pEpsLpcapDrv->uCntFoundDevices; i++)
    {
        pDevice = &g_pEpsLpcapDrv->foundDevices[i];

        printf("\r\n%02x:%02x:%02x:%02x:%02x:%02x  -  %s",pDevice->uMac[0],pDevice->uMac[1],pDevice->uMac[2],pDevice->uMac[3],pDevice->uMac[4],pDevice->uMac[5],pDevice->pName);
    }

    printf("\r\n----------------------------------------\r\n");
    printf("\r\n");
}
#endif

#ifdef EPS_LPCAPDRV_PRINTF_OPEN_CLOSE_INFO
static LSA_VOID eps_lpcapdrv_printf_openclose_board(LSA_BOOL bOpen, EPS_LPCAPDRV_BOARD_PTR_TYPE pBoard)
{
    if (bOpen)
    {
        printf("\r\nEPS LpcapDrv Board Opened: %s", pBoard->sDeviceInfo.pName );
    }
    else
    {
        printf("\r\nEPS LpcapDrv Board Closed: %s", pBoard->sDeviceInfo.pName );
    }
    printf("\r\n");
}
#endif

/**
* Lock implementation
*/
static LSA_VOID eps_lpcapdrv_undo_init_critical_section(LSA_VOID)
{
    LSA_RESPONSE_TYPE retVal;
    retVal = eps_free_critical_section(g_pEpsLpcapDrv->hEnterExit);
    EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
* Lock implementation
*/
static LSA_VOID eps_lpcapdrv_init_critical_section(LSA_VOID)
{
    LSA_RESPONSE_TYPE retVal;
    retVal = eps_alloc_critical_section(&g_pEpsLpcapDrv->hEnterExit, LSA_FALSE);
    EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
* Lock implementation
*/
static LSA_VOID eps_lpcapdrv_enter(LSA_VOID)
{
    eps_enter_critical_section(g_pEpsLpcapDrv->hEnterExit);
}

/**
* Lock implementation
*/
static LSA_VOID eps_lpcapdrv_exit(LSA_VOID)
{
    eps_exit_critical_section(g_pEpsLpcapDrv->hEnterExit);
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
static EPS_LPCAPDRV_BOARD_PTR_TYPE eps_lpcapdrv_alloc_board(LSA_VOID)
{
    LSA_UINT32 ctr;

    for (ctr=0; ctr<EPS_CFG_LPCAPDRV_MAX_BOARDS; ctr++)
    {
        EPS_LPCAPDRV_BOARD_PTR_TYPE const pBoard = &g_pEpsLpcapDrv->board[ctr];

        if //free board entry available?
           (!pBoard->bUsed)
        {
            pBoard->sHw.hDevice             = (LSA_VOID*)pBoard;
            pBoard->sHw.EnableIsr           = (EPS_PNDEV_ENABLE_ISR_FCT) eps_lpcapdrv_enable_interrupt;
            pBoard->sHw.DisableIsr          = eps_lpcapdrv_disable_interrupt;
            pBoard->sHw.EnableHwIr          = eps_lpcapdrv_enable_hw_interrupt;
            pBoard->sHw.DisableHwIr         = eps_lpcapdrv_disable_hw_interrupt;
            pBoard->sHw.ReadHwIr            = eps_lpcapdrv_read_hw_interrupt;
            pBoard->sHw.SetGpio             = eps_lpcapdrv_set_gpio;
            pBoard->sHw.ClearGpio           = eps_lpcapdrv_clear_gpio;
            pBoard->sHw.TimerCtrlStart      = (EPS_PNDEV_TIMER_CTRL_START) eps_lpcapdrv_timer_ctrl_start;
            pBoard->sHw.TimerCtrlStop       = eps_lpcapdrv_timer_ctrl_stop;
            pBoard->sHw.ReadTraceData       = eps_lpcapdrv_read_trace_data;
            pBoard->sHw.WriteTraceData      = eps_lpcapdrv_write_trace_data;
            pBoard->sHw.WriteSyncTimeLower  = eps_lpcapdrv_write_sync_time_lower;
            pBoard->sHw.SaveDump            = eps_lpcapdrv_save_dump;
            pBoard->pBackRef                = g_pEpsLpcapDrv;
            pBoard->bUsed                   = LSA_TRUE;

            return pBoard;
        }
    }

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_lpcapdrv_alloc_board: no free board entry available");

    return LSA_NULL;
}

/**
* Stores all available lpcap drivers into a management structure.
*/
static LSA_VOID eps_lpcapdrv_register_device_infos(LSA_VOID)
{
    EPS_PNDEV_IF_DEV_INFO_TYPE  sPnDevInfo;                 /* Datastore for eps_pndev_if_register_device() */
    LSA_INT32                   result;
    pcap_if_t                   *alldevs;
    pcap_if_t                   *dev;
    const LSA_UINT8             *mac;
    char                        errbuf[PCAP_ERRBUF_SIZE];
    LSA_BOOL                    noRealDevice = LSA_FALSE;   /* Set when a non real device is found by pcap e.g. "any" */
    struct ifreq  ifr;
    int sockfd;

    /* Get socket for ioctl-operation */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_lpcapdrv_register_device_infos: Can't get a socket");
        EPS_FATAL("eps_lpcapdrv_register_device_infos - couldn't get a socket!");
    }

    /* pcap function to find all network devices */
    result = pcap_findalldevs(&alldevs, errbuf);
    if(result < 0)
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_lpcapdrv_register_device_infos: pcap_findalldevs failed with \"%s\"!", errbuf);
        EPS_FATAL("eps_lpcapdrv_register_device_infos: pcap_findalldevs failed with error-text:" errbuf);
    }

    /* Get all network devices */
    for(dev = alldevs; dev != LSA_NULL; dev = dev->next)
    {
        noRealDevice = LSA_FALSE;

        /* Name of the network-card we want to take a look at */
        strcpy(ifr.ifr_name, dev->name);
        ifr.ifr_addr.sa_family = AF_INET;

        /* Get hardware-address (MAC) of network card */
        result = ioctl(sockfd, SIOCGIFHWADDR, &ifr);
        if(result < 0)
        {
            switch(errno)
            {
            case ENODEV: /* happens for devices like "any" or "usbmon" */
                noRealDevice = LSA_TRUE;
                break;
            default:
                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_lpcapdrv_register_device_infos:ioctl failed with errno %d!", errno);
                EPS_FATAL("ioctl failed with errno " errno);
                break;
            }
        }

        /* If it isn't a real device skip setting it in registered devices */
        if(noRealDevice == LSA_TRUE || !strcmp(dev->name, "lo"))
        {
            continue;
        }

        mac = (unsigned char*)ifr.ifr_hwaddr.sa_data;

        g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[0] = mac[0];
        g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[1] = mac[1];
        g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[2] = mac[2];
        g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[3] = mac[3];
        g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[4] = mac[4];
        g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[5] = mac[5];

        eps_strcpy(&g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].Name[0], dev->name);
        g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].pName = &g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].Name[0];

        sPnDevInfo.uPciAddrValid = 0;
        sPnDevInfo.uMacAddrValid = 1;

        sPnDevInfo.uMacAddr[0] = g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[0];
        sPnDevInfo.uMacAddr[1] = g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[1];
        sPnDevInfo.uMacAddr[2] = g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[2];
        sPnDevInfo.uMacAddr[3] = g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[3];
        sPnDevInfo.uMacAddr[4] = g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[4];
        sPnDevInfo.uMacAddr[5] = g_pEpsLpcapDrv->foundDevices[g_pEpsLpcapDrv->uCntFoundDevices].uMac[5];

        sPnDevInfo.uEddType      = LSA_COMP_ID_EDDS;

        eps_pndev_if_register_device(&sPnDevInfo);

        g_pEpsLpcapDrv->uCntFoundDevices++;
    }

    close(sockfd);

    pcap_freealldevs(alldevs);

}

/**
 * free board entry
 *
 * @param [in] pBoard   ptr to current board structure
 * @return LSA_VOID
 */
static LSA_VOID eps_lpcapdrv_free_board(EPS_LPCAPDRV_BOARD_PTR_TYPE pBoard)
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
static LSA_VOID eps_lpcapdrv_timer_cbf (union sigval args)
{
    EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE pTmrPar = (EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE) args.sival_ptr;
    LSA_UINT64  uStartTimeNs;
    LSA_UINT64  uDiffTimeNs;

    // read current StartTime
    uStartTimeNs = eps_get_ticks_100ns();

    // calc diff to last call to see if we need to call eps_pndev_if_timer_tick or skip this tick
    uDiffTimeNs = (uStartTimeNs - pTmrPar->uTickCbfLastCallTimeNs) * 100;

    if  (uDiffTimeNs > pTmrPar->uTickUnderrunMarkNs)
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
 * open pndevdrv timer for EPS_LPCAPDRV_CFG_TIMER_OVERSAMPLING_VALUE_MS (e.g. 1 ms) eps_lpcapdrv_timer_cbf callbacks

 * Important note: We start the systimer with a shorter cycle, e.g. 1 ms, but we call the eps_pndev_if_timer_tick only if the uTickUnderrunMarkNs is reached (e.g. 32 ms) -> Oversampling.
 * That way, we have up to 31 useless calls if the systimer is able to call this function every millisecond.
 * The experience showed, that
 * 1. The Windows OS is not able to achieve a 1 ms timer. Oversampling with 1 ms is better than a 32 ms timer that comes too late
 * 2. If the system is in stress and the multimedia timer is not able to call the cbf in time, windows calls the cbf two or more times in a row to make up for the missed call.
 *    The EDDS function that is triggered by this function shall _never_ be called too soon
 * @see eps_lpcapdrv_timer_cbf - implements oversampling
 *
 * @param   [in] pBoard             ptr to corresponding board structure
 * @return  EPS_PNDEV_RET_OK,       upon successful execution
 */
static LSA_UINT16 eps_lpcapdrv_start_pn_gathered_timer(EPS_LPCAPDRV_BOARD_PTR_TYPE pBoard)
{
    EPS_LPCAPDRV_TMR_PAR_PTR_TYPE pTmrPar = &pBoard->TmrParPnGathered;

    eps_memset(pTmrPar, 0, sizeof(EPS_LPCAPDRV_TMR_PAR_TYPE));

    pTmrPar->TmrPar.pTickCbf                = &pBoard->sIsrPnGathered;
    pTmrPar->TmrPar.uTickCbfOverrunMarkNs   = EPS_LPCAPDRV_CFG_TIMER_MAX_CBF_DURATION_NS;
    pTmrPar->TmrPar.uTickUnderrunMarkNs     = EPS_LPCAPDRV_CFG_TIMER_MIN_CBF_START_DELAY_NS;
    pTmrPar->TmrPar.uTickOverrunMarkNs      = EPS_LPCAPDRV_CFG_TIMER_MAX_CBF_START_DELAY_NS;
    // save ref time when timer was started
    pTmrPar->TmrPar.uTickCbfLastCallTimeNs  = eps_get_ticks_100ns();

    /// Allocate a sytimer with 1 ms. Note: This will do a oversampling
    return eps_timer_allocate_sys_timer(&pTmrPar->TmrID, "EPS_LPCAPGPISR", EPS_LPCAPDRV_CFG_TIMER_OVERSAMPLING_VALUE_MS, eps_lpcapdrv_timer_cbf, &pTmrPar->TmrPar);
}

/**
 * close systimer timer for EPS_LPCAPDRV_CFG_TIMER_OVERSAMPLING_VALUE_MS eps_lpcapdrv_timer_cbf callbacks
 *
 * @param   [in] pBoard             ptr to corresponding board structure
 * @return  EPS_PNDEV_RET_OK,       upon successful execution
 */
static LSA_UINT16 eps_lpcapdrv_shutdown_pn_gathered_timer(EPS_LPCAPDRV_BOARD_PTR_TYPE pBoard)
{
    LSA_UINT16 result;
    EPS_LPCAPDRV_TMR_PAR_PTR_TYPE pTmrPar = &pBoard->TmrParPnGathered;

    result = eps_timer_free_sys_timer(pTmrPar->TmrID);

    if (result == EPS_PNDEV_RET_OK)
    {
        // show statistics
        EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_lpcapdrv_shutdown_pn_gathered_timer: Ticks: %u, TickOverruns: %u, TickUnderruns: %u, CbfOverruns: %u",
                            pTmrPar->TmrPar.uTickCount, pTmrPar->TmrPar.uTickOverrunCount, pTmrPar->TmrPar.uTickUnderrunCount,pTmrPar->TmrPar.uTickCbfOverrunCount);

        eps_memset(pTmrPar, 0, sizeof(EPS_LPCAPDRV_TMR_PAR_TYPE));
    }

    return result;
}

/**
 * enable interrupt in lpcap driver. Currently, we pnly support polling mode for lpcap variants.
 *
 * @param [in] pHwInstIn                - handle to board
 * @param [in] pInterrupt               - event mode (interrupt, polling)
 * @return EPS_PNDEV_RET_ERR            - something went wrong
 * @return EPS_PNDEV_RET_OK             - everything went better than expected
 * @return EPS_PNDEV_RET_UNSUPPORTED    - mode not supported
 */
static LSA_UINT16 eps_lpcapdrv_enable_interrupt_intern (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf)
{
    EPS_LPCAPDRV_BOARD_PTR_TYPE pBoard;
    LSA_UINT16 result = EPS_PNDEV_RET_OK;

    pBoard = (EPS_LPCAPDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

    EPS_ASSERT(pBoard->bUsed);

    switch (*pInterrupt)
    {
        case EPS_PNDEV_ISR_ISOCHRONOUS:
             EPS_FATAL("EPS_PNDEV_ISR_ISOCHRONOUS is not implemented in lpcap driver"); /* not supported */
            break;
        case EPS_PNDEV_ISR_PN_GATHERED:
            if (pBoard->sIsrPnGathered.pCbf != LSA_NULL)
            {
                result = EPS_PNDEV_RET_ERR;
                break;
            }

            pBoard->sIsrPnGathered = *pCbf;

            result = eps_lpcapdrv_start_pn_gathered_timer(pBoard);

            if (result != EPS_PNDEV_RET_OK)
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_lpcapdrv_enable_interrupt_intern: Error while enabling the polling mode!");
                pBoard->sIsrPnGathered.pCbf = LSA_NULL;
            }
            else
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_lpcapdrv_enable_interrupt_intern: Polling mode enabled");
            }
            break;
        case EPS_PNDEV_ISR_PN_NRT:
          EPS_FATAL("enabling NRT interrupts is not supported in lpcap driver"); /* not supported */
            break;
        case EPS_PNDEV_ISR_INTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_INTERRUPT is not implemented in lpcap driver");
            break;
        case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_POLLINTERRUPT is not implemented in lpcap driver");
            break;
        case EPS_PNDEV_ISR_CPU:
            EPS_FATAL("EPS_PNDEV_ISR_CPU is not implemented in lpcap driver");
            break;
        case EPS_PNDEV_ISR_IPC_RX:
            EPS_FATAL("EPS_PNDEV_ISR_IPC_RX is not implemented in lpcap driver");
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
 * disable interrupt in lpcap driver. Currently, we pnly support polling mode for lpcap variants.
 *
 * @param [in] pHwInstIn                - handle to board
 * @param [in] pInterrupt               - event mode (interrupt, polling)
 * @return EPS_PNDEV_RET_ERR            - something went wrong
 * @return EPS_PNDEV_RET_OK             - everything went better than expected
 * @return EPS_PNDEV_RET_UNSUPPORTED    - mode not supported
 */
static LSA_UINT16 eps_lpcapdrv_disable_interrupt_intern (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt)
{
    EPS_LPCAPDRV_BOARD_PTR_TYPE pBoard;
    LSA_UINT16 result = EPS_PNDEV_RET_OK;

    pBoard = (EPS_LPCAPDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

    EPS_ASSERT(pBoard->bUsed);
    EPS_ASSERT(pBoard->uCountIsrEnabled != 0);

    switch (*pInterrupt)
    {
        case EPS_PNDEV_ISR_ISOCHRONOUS:
            EPS_FATAL("EPS_PNDEV_ISR_ISOCHRONOUS is not implemented in lpcap driver");
            break;
        case EPS_PNDEV_ISR_PN_GATHERED:
            if (pBoard->sIsrPnGathered.pCbf == LSA_NULL)
            {
                result = EPS_PNDEV_RET_ERR;
                break;
            }

            result = eps_lpcapdrv_shutdown_pn_gathered_timer(pBoard);

            if (result == EPS_PNDEV_RET_OK)
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_lpcapdrv_enable_interrupt_intern: Error while disabling the polling mode!");
                pBoard->sIsrPnGathered.pCbf = LSA_NULL;
            }
            else
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_lpcapdrv_disable_interrupt_intern: Polling mode disabled");
            }
            break;
        case EPS_PNDEV_ISR_PN_NRT:
          EPS_FATAL("disabling NRT interrupts is not supported in lpcap driver"); /* not supported */
            break;
        case EPS_PNDEV_ISR_INTERRUPT:
            EPS_FATAL("disabling EPS_PNDEV_ISR_INTERRUPT is not implemented in lpcap driver");
            break;
        case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("disabling EPS_PNDEV_ISR_POLLINTERRUPT is not implemented in lpcap driver");
            break;
        case EPS_PNDEV_ISR_CPU:
            EPS_FATAL("disabling EPS_PNDEV_ISR_CPU is not implemented in lpcap driver");
            break;
        case EPS_PNDEV_ISR_IPC_RX:
            EPS_FATAL("disabling EPS_PNDEV_ISR_IPC_RX is not implemented in lpcap driver");
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
static LSA_VOID eps_lpcapdrv_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid)
{
    EPS_BOARD_INFO_TYPE * const pboardInfo           = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;
    LSA_UINT32            const dev_nrt_mem_size_all = pboardInfo->dev_mem.size;

    // setup DEV and NRT memory pools: divide the already allocated DEV memory pool into DEV and NRT memory pools

    //pboardInfo->dev_mem.base_ptr  = already valid
    //pboardInfo->dev_mem.phy_addr  = already valid
    pboardInfo->dev_mem.size        = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_LPCAPDRV_PERCENT_DEV);

    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->dev_mem.base_ptr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->dev_mem.phy_addr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_LPCAPDRV_PERCENT_NRT_TX);

    pboardInfo->nrt_rx_mem.base_ptr = pboardInfo->nrt_tx_mem.base_ptr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.phy_addr = pboardInfo->nrt_tx_mem.phy_addr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_LPCAPDRV_PERCENT_NRT_RX);

    EPS_ASSERT(pboardInfo->dev_mem.base_ptr != LSA_NULL);
}


/**
 * Installing EPS WinPCap driver for windows OS. The EPS LPcap PN Dev Driver implements the eps_pndev_if.
 *
 * The EPS WinPCap driver requires Lpcap installed in the system.
 *
 * This function links the functions required by the EPS PN Dev IF to internal functions
 *  eps_pndev_if_open      -> eps_lpcapdrv_open
 *  eps_pndev_if_close     -> eps_lpcapdrv_close
 *  eps_pndev_if_uninstall -> eps_lpcapdrv_uninstall
 *
 * @see eps_pndev_if_register - this function is called to register the PN Dev implementation into the EPS PN Dev IF.
 *
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_lpcapdrv_install(LSA_VOID)
{
    EPS_PNDEV_IF_TYPE sPnLpcapDrvIf;

    eps_memset(&g_EpsLpcapDrv, 0, sizeof(g_EpsLpcapDrv));
    g_pEpsLpcapDrv = &g_EpsLpcapDrv;

    g_pEpsLpcapDrv->bInit = LSA_TRUE;

    eps_lpcapdrv_init_critical_section();

    eps_lpcapdrv_register_device_infos();

    #ifdef EPS_LPCAPDRV_PRINTF_FOUND_BOARDS
    eps_lpcapdrv_printf_found_boards();
    #endif

    //Init Interface
    sPnLpcapDrvIf.open      = (EPS_PNDEV_OPEN_FCT)eps_lpcapdrv_open;
    sPnLpcapDrvIf.close     = eps_lpcapdrv_close;
    sPnLpcapDrvIf.uninstall = eps_lpcapdrv_uninstall;

    eps_pndev_if_register(&sPnLpcapDrvIf);
}

/**
 * uninstall EPS LPcap driver. Deletes management structures.
 *
 * @param   LSA_VOID
 * @return  LSA_VOID
 */
LSA_VOID eps_lpcapdrv_uninstall(LSA_VOID)
{
    EPS_ASSERT(g_pEpsLpcapDrv->bInit);

    eps_lpcapdrv_undo_init_critical_section();

    g_pEpsLpcapDrv->bInit = LSA_FALSE;

    g_pEpsLpcapDrv = LSA_NULL;
}

/**
 * Open a PN board supported by LPCal for the given MAC Address
 *
 * This function implements the eps_pndev_if function eps_pndev_if_open.
 * The function may only be called once since there is only one HD.
 * The lower layer table from the edds is linked into the lower layer table here.
 *
 * @see eps_pndev_if_open             - calls this function
 *
 * @param   [in] pLocation                  ptr including MAC Address
 * @param   [in] pOption                    ptr to options (not used)
 * @param   [out] ppHwInstOut               pointer to ptr to structure with hardware configuration
 * @param   [in] hd_id                      index of hd
 * @return  EPS_PNDEV_RET_OK                everything went better then expected.
 * @return  EPS_PNDEV_RET_DEV_NOT_FOUND     device was not found (wrong MAC address / did you disabled the board in device manager without reboot?)
 */
static LSA_UINT16 eps_lpcapdrv_open(EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id)
{
    EPS_LPCAPDRV_BOARD_PTR_TYPE       pBoard;
    EPS_LPCAPDRV_DEVICE_INFO_PTR_TYPE pDeviceInfo = LSA_NULL;
    EDDS_LL_TABLE_TYPE *              pLLTable;
    EPS_BOARD_INFO_PTR_TYPE           pBoardInfo;
    LSA_UINT16                        result = EPS_PNDEV_RET_OK;

    LSA_UNUSED_ARG(pOption);

    EPS_ASSERT(g_pEpsLpcapDrv->bInit);
    EPS_ASSERT(pLocation   != LSA_NULL);
    EPS_ASSERT(ppHwInstOut != LSA_NULL);

    eps_lpcapdrv_enter();

    pBoard = eps_lpcapdrv_alloc_board();
    EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */

    /* Connect Board HW / Map Memory */

    /* NOTE: EPS_APP_GET_MAC_ADDR is not called within eps_lpcapdriver
     * The board is selected by MAC-Address. This MAC-Address is used.
     */
    if (pLocation->eLocationSelector == EPS_PNDEV_LOCATION_MAC)
    {
        for (LSA_UINT16 i=0; i<g_pEpsLpcapDrv->uCntFoundDevices; i++)
        {

            pDeviceInfo = &g_pEpsLpcapDrv->foundDevices[i];

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
        pLLTable->open                      = LPCAP_LL_OPEN;
        pLLTable->setup                     = LPCAP_LL_SETUP;
        pLLTable->shutdown                  = LPCAP_LL_SHUTDOWN;
        pLLTable->close                     = LPCAP_LL_CLOSE;

        /* Transmit buffer handling functions */
        pLLTable->enqueueSendBuffer         = LPCAP_LL_SEND;
        pLLTable->getNextFinishedSendBuffer = LPCAP_LL_SEND_STS;
        pLLTable->triggerSend               = LPCAP_LL_SEND_TRIGGER;

        /* Receive buffer handling functions */
        pLLTable->getNextReceivedBuffer     = LPCAP_LL_RECV;
        pLLTable->provideReceiveBuffer      = LPCAP_LL_RECV_PROVIDE;
        pLLTable->triggerReceive            = LPCAP_LL_RECV_TRIGGER;

        /* Information functions */
        pLLTable->getLinkState              = LPCAP_LL_GET_LINK_STATE;
        pLLTable->getStatistics             = LPCAP_LL_GET_STATS;

        /* MAC address management functions */
        pLLTable->enableMC                  = LPCAP_LL_MC_ENABLE;
        pLLTable->disableMC                 = LPCAP_LL_MC_DISABLE;

        /* Scheduler function */
        pLLTable->recurringTask             = LPCAP_LL_RECURRING_TASK;

        /* Control functions */
        pLLTable->setLinkState              = LPCAP_LL_SET_LINK_STATE;
        pLLTable->backupLocationLEDs        = LPCAP_LL_LED_BACKUP_MODE;
        pLLTable->restoreLocationLEDs       = LPCAP_LL_LED_RESTORE_MODE;
        pLLTable->setLocationLEDs           = LPCAP_LL_LED_SET_MODE;

        /* Switch functions */
        pLLTable->setSwitchPortState        = LSA_NULL;
        pLLTable->controlSwitchMulticastFwd = LSA_NULL;
        pLLTable->flushSwitchFilteringDB    = LSA_NULL;

        pLLTable->changePort                = LPCAP_LL_SWITCH_CHANGE_PORT;

        // EDDS set arp filter function
        #ifdef LLIF_CFG_USE_LL_ARP_FILTER
        pLLTable->setArpFilter              = LSA_NULL;
        #endif

        // fill Device Info

        pBoardInfo = &pBoard->sHw.EpsBoardInfo;
        eps_memset(pBoardInfo, 0, sizeof(*pBoardInfo));

        pBoard->sysDev.hd_nr        = hd_id;
        pBoard->sysDev.pnio_if_nr   = 0;    // don't care
        pBoard->sysDev.edd_comp_id  = LSA_COMP_ID_EDDS;

        // Common HD settings
        pBoardInfo->edd_type        = LSA_COMP_ID_EDDS;
        pBoardInfo->hd_sys_handle   = &pBoard->sysDev;

        // init User to HW port mapping (used PSI GET HD PARAMS)
        eps_hw_init_board_port_param( pBoardInfo );

        // Port specific setup
        pBoardInfo->nr_of_ports = 1;

        // Interface Mac
        eps_lpcapdrv_get_interface_mac(&pBoardInfo->if_mac);

        for (LSA_UINT16 i = 0; i < pBoardInfo->nr_of_ports; i++ )
        {
            //MAC Port & Interface
            pBoardInfo->port_mac[i+1].mac[0] = pBoard->sDeviceInfo.uMac[0];
            pBoardInfo->port_mac[i+1].mac[1] = pBoard->sDeviceInfo.uMac[1];
            pBoardInfo->port_mac[i+1].mac[2] = pBoard->sDeviceInfo.uMac[2];
            pBoardInfo->port_mac[i+1].mac[3] = pBoard->sDeviceInfo.uMac[3];
            pBoardInfo->port_mac[i+1].mac[4] = pBoard->sDeviceInfo.uMac[4];
            pBoardInfo->port_mac[i+1].mac[5] = pBoard->sDeviceInfo.uMac[5];

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
        eps_lpcapdrv_FillDevNrtMemParams(pBoardInfo);

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
        //pBoardInfo->edds.bIOConfigured      = LSA_FALSE;
        pBoardInfo->edds.hTimer             = 0;
        pBoardInfo->edds.hMsgqHigh          = 0;
        pBoardInfo->edds.hMsgqLow           = 0;
        pBoardInfo->edds.hThreadHigh        = 0;
        pBoardInfo->edds.hThreadLow         = 0;

        pBoard->sHw.asic_type               = EPS_PNDEV_ASIC_INTEL_XX;
        pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_INTEL_XX;
        pBoard->sHw.IrMode                  = EPS_PNDEV_IR_MODE_POLL;

        // initialization of MediaType
        eps_hw_init_board_port_media_type(pBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

        EPS_SYSTEM_TRACE_07(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_lpcapdrv_open succeeded. hd_nr = %d, mac = %02x:%02x:%02x:%02x:%02x:%02x ",
            pBoard->sysDev.hd_nr,
            pBoardInfo->if_mac.mac[0], pBoardInfo->if_mac.mac[1], pBoardInfo->if_mac.mac[2],
            pBoardInfo->if_mac.mac[3], pBoardInfo->if_mac.mac[4], pBoardInfo->if_mac.mac[5]);
    }
    else
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_lpcapdrv_open failed. Please check MAC Address. If you have disabled the board in device manager in the meantime, please do a reboot.");
        result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
    }

    if (result == EPS_PNDEV_RET_OK)
    {
        #ifdef EPS_LPCAPDRV_PRINTF_OPEN_CLOSE_INFO
        eps_lpcapdrv_printf_openclose_board(LSA_TRUE, pBoard);
        #endif

        *ppHwInstOut = &pBoard->sHw;
    }
    else
    {
        eps_lpcapdrv_free_board(pBoard);
    }

    eps_lpcapdrv_exit();

    return result;
}

/**
 * close device
 *
 * This function implements the eps_pndev_if function eps_pndev_if_close.
 *
 * @see eps_pndev_if_close          - calls this function
 *
 * @param   [in] pHwInstIn          pointer to structure with hardware configuration
 * @return  EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_lpcapdrv_close (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_LPCAPDRV_BOARD_PTR_TYPE pBoard;
    EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;
    LSA_UINT16 retValDisableIsr;
    LSA_UINT16 retValFree;

    EPS_ASSERT(pHwInstIn != LSA_NULL);
    EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

    eps_lpcapdrv_enter();

    pBoard = (EPS_LPCAPDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

    EPS_ASSERT(pBoard->bUsed);

    if (pBoard->uCountIsrEnabled != 0)
    {
        if (pBoard->sIsrPnGathered.pCbf != LSA_NULL)
        {
            IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;
            retValDisableIsr = eps_lpcapdrv_disable_interrupt_intern(pHwInstIn, &IsrDesc);
            EPS_ASSERT(EPS_PNDEV_RET_OK == retValDisableIsr);
        }
    }

    EPS_ASSERT(pBoard->uCountIsrEnabled == 0);

    // free DEV and NRT memory pools (local mem is used): one memory pool was originally allocated and stored in DEV parameters
    EPS_ASSERT(pBoard->sHw.EpsBoardInfo.dev_mem.base_ptr != LSA_NULL);
    retValFree = eps_mem_free(pBoard->sHw.EpsBoardInfo.dev_mem.base_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_NRT_MEM);
    EPS_ASSERT(LSA_RET_OK == retValFree);

    eps_lpcapdrv_free_board(pBoard);

    #ifdef EPS_LPCAPDRV_PRINTF_OPEN_CLOSE_INFO
    eps_lpcapdrv_printf_openclose_board(LSA_FALSE, pBoard);
    #endif

    eps_lpcapdrv_exit();

    return EPS_PNDEV_RET_OK;
}

/**
 * enable interrupt for board
 *
 * @param   [in] pHwInstIn      pointer to structure with hardware configuration
 * @param   [in] pInterrupt     pointer to enum with method type
 * @param   [in] pCbf           ptr to Callbackfunction
 * @return  EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_lpcapdrv_enable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf)
{
    LSA_UINT16 result;

    EPS_ASSERT(pHwInstIn != LSA_NULL);
    EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

    eps_lpcapdrv_enter();

    result = eps_lpcapdrv_enable_interrupt_intern(pHwInstIn, pInterrupt, pCbf);

    eps_lpcapdrv_exit();

    return result;
}

/**
 * disable interrupt for board
 *
 * @param   [in] pHwInstIn      pointer to structure with hardware configuration
 * @param   [in] pInterrupt     pointer to enum with method type
 * @return  EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_lpcapdrv_disable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt)
{
    LSA_UINT16 result;

    EPS_ASSERT(pHwInstIn != LSA_NULL);
    EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

    eps_lpcapdrv_enter();

    result = eps_lpcapdrv_disable_interrupt_intern(pHwInstIn, pInterrupt);

    eps_lpcapdrv_exit();

    return result;
}

/**
 * Set gpio for a given board. Currently Not supported
 *
 * @param   [in] pHwInstIn              pointer to structure with hardware configuration
 * @param   [in] gpio                   enum with gpios
 * @return  EPS_PNDEV_RET_UNSUPPORTED   Board type is not supported.
 */
static LSA_UINT16 eps_lpcapdrv_set_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(gpio);
    /* Not Supported */
    return EPS_PNDEV_RET_OK;
}

/**
 * Clear gpio for a given board. Currently Not supported
 *
 * @param   [in] pHwInstIn              pointer to structure with hardware configuration
 * @param   [in] gpio                   enum with gpios
 * @return  EPS_PNDEV_RET_UNSUPPORTED   Board type is not supported.
 */
static LSA_UINT16 eps_lpcapdrv_clear_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(gpio);
    /* Not Supported */
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * start timer. Currently not supported.
 *
 * @param [in] pHwInstIn        pointer to structure with hardware configuration
 * @param [in] pCbf             ptr to Callbackfunction
 * @return  EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_lpcapdrv_timer_ctrl_start (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf)
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
 * @param [in] pHwInstIn        pointer to structure with hardware configuration
 * @return  EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_lpcapdrv_timer_ctrl_stop (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("function not supported"); /* Not Supported */
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * read trace data. This function will never be supported by EPS Lpcap driver since we cannot download any firmware to the board.
 *
 * @param [in]  pHwInstIn           pointer to structure with hardware configuration
 * @param [in]  offset              offset where should be read from
 * @param [in]  ptr                 destination pointer for the data
 * @param [in]  size                size of data
 * @return  EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_lpcapdrv_read_trace_data (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size)
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
 * write trace data. This function will never be supported by EPS Lpcap driver since we cannot download any firmware to the board.
 *
 * @param [in]  pHwInstIn           pointer to structure with hardware configuration
 * @param [in]  ptr                 source pointer for the data
 * @param [out] offset              offset where should be write to
 * @param [in]  size                size of data
 * @return  EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_lpcapdrv_write_trace_data (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size)
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
static LSA_UINT16 eps_lpcapdrv_write_sync_time_lower(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(lowerCpuId);
    EPS_FATAL("Writing lower sync time not supported");
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

static LSA_UINT16 eps_lpcapdrv_save_dump (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
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
static LSA_UINT16  eps_lpcapdrv_enable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Enable HW Interrupt in lpcap not supported in this driver.");
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Disable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 *
 * @param [in] pHwInstIn       - pointer to hardware instance
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_lpcapdrv_disable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Disable HW Interrupt in lpcap not supported in this driver.");
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Read HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 *
 * @param [in] pHwInstIn       - pointer to hardware instance
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_lpcapdrv_read_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(interrupts);
    EPS_FATAL("Read HW Interrupt in lpcap not supported in this driver.");
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Read all MAC's that are found in eps_pndev_if_get_registered_devices()
 * Choose the lowest and decrement -> That should be the Inteface-MAC
 *
 * @param [in] pMacArray       - pointer to hardware instance
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_RESULT eps_lpcapdrv_get_interface_mac(EPS_MAC_TYPE *pMacArray) /* Z003D3BW ToDo Interface MAC */
{
    LSA_RESULT                      result          = LSA_RET_ERR_PARAM;
    EPS_PNDEV_IF_DEV_INFO_PTR_TYPE  pDev;
    LSA_UINT16                      uMaxDev         = 50;
    LSA_UINT16                      cntDevices      = 0;
    EPS_MAC_TYPE                    buf;

    if(pMacArray == NULL)
    {
        result = LSA_RET_ERR_PARAM;
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_lpcapdrv_get_interface_mac - pMacArray (parameter) is NULL!");
        EPS_FATAL("eps_lpcapdrv_get_interface_mac - pMacArray (parameter) is NULL!");
        return result;
    }

    /* Get Interface MAC */

    pDev = (EPS_PNDEV_IF_DEV_INFO_TYPE *)malloc(sizeof(EPS_PNDEV_IF_DEV_INFO_TYPE) * uMaxDev);

    eps_pndev_if_get_registered_devices(pDev, uMaxDev, &cntDevices);

    // Get the lowest MAC
    if(cntDevices <= 0)
    {
        result = LSA_RET_ERR_PARAM;
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_lpcapdrv_get_interface_mac - No devices registered!");
        EPS_FATAL("eps_lpcapdrv_get_interface_mac - No devices registered!");
        return result;
    }

    /* Initial value */
    memcpy(buf.mac, pDev[0].uMacAddr, 6);

    /* Find lowest MAC-Address */
    for(LSA_UINT8 i = 1; i < cntDevices; cntDevices++)
    {
        for(LSA_UINT8 j = 5; j > -1; j--)
        {
            if( buf.mac[j] > pDev[i].uMacAddr[j] )
            {
                memcpy(buf.mac, pDev[i].uMacAddr, 6);
                break;
            }
        }
    }

    free(pDev);

    /* Get the lowest MAC-Address and decrement it */
    if(buf.mac[5] == 0)
    {
        buf.mac[5] = 0xFE; /* LSB must be 0 for individual address */
        if(buf.mac[4] == 0)
        {
            buf.mac[4] = 0xFF;
            if(buf.mac[3] == 0)
            {
                result = LSA_RET_ERR_PARAM;
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_lpcapdrv_get_interface_mac - Can't build a properly interface MAC!");
                EPS_FATAL("eps_lpcapdrv_get_interface_mac - Can't build a properly interface MAC!");
                return result;
            }
        }
    }
    else
    {
        buf.mac[5]--;
    }

    *pMacArray = buf;

    result = LSA_OK;

    return result;

}

/*----------------------------------------------------------------------------*/
#endif // PSI_CFG_USE_EDDS
#endif // PSI_EDDS_CFG_HW_LPCAP

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
/*  F i l e               &F: eps_ipc_generic_drv.c                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Inter Processor Communication generic driver                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20012
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>                  /* Types / Prototypes / Fucns */
#include <eps_trc.h>                  /* Tracing                    */
#include <eps_locks.h>                /* EPS Locks                  */

#include <eps_tasks.h>                /* EPS Tasks Api              */
#include <eps_rtos.h>                 /* Driver Interface           */
#include <eps_cp_hw.h>                /* EPS PSI HW adaption layer  */
#include <eps_pn_drv_if.h>            /* PN-Driver Interface        */
#include <eps_register.h>             /* Register access macros     */

#include <eps_ipc_drv_if.h>
#include <eps_adonis_ipc_drv.h>

/* Generic drv admin structure  */
static EPS_IPC_DRV_TYPE g_EpsIpcGenericDrv;
static EPS_IPC_DRV_PTR_TYPE g_pEpsIpcGenericDrv = LSA_NULL;


/* Basic initialization         */

static LSA_VOID   eps_ipc_generic_drv_uninstall    (LSA_VOID);

static LSA_VOID    eps_ipc_generic_drv_open        (LSA_VOID);
static LSA_VOID    eps_ipc_generic_drv_close       (LSA_VOID);

/* Local interrupt */
static LSA_UINT16 eps_ipc_generic_drv_rx_alloc     (LSA_UINT32* hRxHandle, LSA_UINT32* pPhysAddress, LSA_UINT32* pVal, LSA_UINT16 hd_id, LSA_UINT32 uType);
static LSA_VOID   eps_ipc_generic_drv_rx_free      (LSA_UINT32  hRxHandle);
static LSA_VOID   eps_ipc_generic_drv_rx_enable    (LSA_UINT32  hRxHandle, EPS_IPC_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs);
static LSA_VOID   eps_ipc_generic_drv_rx_disable   (LSA_UINT32  hRxHandle);
static LSA_VOID   eps_ipc_generic_drv_rx_set_shm   (LSA_UINT32  hRxHandle, LSA_UINT8* pBaseLocal, LSA_UINT8* pBaseRemote, LSA_UINT32 uSize);


/* Remote Interrupt */
static LSA_UINT16 eps_ipc_generic_drv_tx_alloc     (LSA_UINT32* hTxHandle, LSA_UINT32 uPhysAddress, LSA_UINT32 uMsgVal, LSA_UINT16 hd_id, LSA_UINT32 uType);
static LSA_UINT16 eps_ipc_generic_drv_tx_alloc_from_local_shm(LSA_UINT32* hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize, LSA_UINT16 hd_id, LSA_UINT32 uType);
static LSA_VOID   eps_ipc_generic_drv_tx_free      (LSA_UINT32  hTxHandle);
static LSA_VOID   eps_ipc_generic_drv_tx_send      (LSA_UINT32  hTxHandle);
static LSA_VOID   eps_ipc_generic_drv_tx_set_shm   (LSA_UINT32  hTxHandle, LSA_UINT8* pBaseLocal, LSA_UINT8* pBaseRemote, LSA_UINT32 uSize);



/**
* Disables ipc interrupt and unregisters it in adonis.
*
* @param pInst pointer to ipc receive instance containing interrupt information
*/
static LSA_VOID eps_ipc_generic_drv_disable_local_ipc_interrupt(EPS_IPC_RX_INST_PTR_TYPE pInst)
{
    LSA_INT retVal;

    EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsIpcGenericDrv->fdIpcDriver != -1);

    retVal = ioctl(g_pEpsIpcGenericDrv->fdIpcDriver, EPS_ADONIS_IPC_DRV_LOCAL_INT_DISABLE, (LSA_VOID*)&pInst->sLocalInterruptInfo);
    EPS_ASSERT(retVal == 0);
}


/**
* Destroys locks for eps critical sections.
*/
static LSA_VOID eps_ipc_generic_drv_undo_init_critical_section(LSA_VOID)
{
    eps_free_critical_section(g_pEpsIpcGenericDrv->hEnterExit);
}

/**
* Inits locks for eps critical sections.
*/
static LSA_VOID eps_ipc_generic_drv_init_critical_section(LSA_VOID)
{
    eps_alloc_critical_section(&g_pEpsIpcGenericDrv->hEnterExit, LSA_FALSE);
}


/**
* Enter function for critical sections in eps ipc
*
* Protects from concurrent access in following critical section.
*/
static LSA_VOID eps_ipc_generic_drv_enter(LSA_VOID)
{
    eps_enter_critical_section(g_pEpsIpcGenericDrv->hEnterExit);
}

/**
* Exit function for critical sections in eps ipc
*
* Marks the end of critical section in eps ipc.
*/
static LSA_VOID eps_ipc_generic_drv_exit(LSA_VOID)
{
    eps_exit_critical_section(g_pEpsIpcGenericDrv->hEnterExit);
}


/**
* Send SW interrupt when target is ready to receive new data.
* Wait for target if previous receive hasn't been acknowledged yet.
*
* @param pInst pointer to ipc transceive instance containing interrupt information
*/
static LSA_VOID eps_ipc_generic_drv_send_isr(EPS_IPC_TX_INST_PTR_TYPE pInst)
{
    if (pInst->pRemoteShm != LSA_NULL)
    {
#if !((EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NONE) || ((EPS_PLF == EPS_PLF_PNIP_A53) && (EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NO_HERA)))
        // Do not Sync IPC IRQ if (EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NONE) or when running on hera and (EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NO_HERA)
        if (pInst->pLocalShm != LSA_NULL)
        {
            if (pInst->pLocalShm->uRxCnt != pInst->uCntIsrTrigger)
            {
                EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_generic_drv_send_isr(): Send IPC Isr h(0x%x) but previous wasn't processed by target processor => sleeping, current datareivedelay=0x%x", pInst->hOwnHandle, pInst->uCntDataReceiveDelay);
                do
                {
                    /* The previous receive hasn't been acked so far => we need to wait until done (ping/pong mechanism) */
                    eps_tasks_usleep(0);

                    pInst->uCntDataReceiveDelay++;
                } while (pInst->pLocalShm->uRxCnt != pInst->uCntIsrTrigger);
                EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_generic_drv_send_isr(): Send IPC Isr h(0x%x) datareivedelay=0x%x after sleeping", pInst->hOwnHandle, pInst->uCntDataReceiveDelay);
            }
        }
#endif // #if !((EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NONE) || ((EPS_PLF == EPS_PLF_PNIP_A53) && (EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NO_HERA)))

        pInst->uCntIsrTrigger++;
        pInst->pRemoteShm->uTxCnt = pInst->uCntIsrTrigger;
    }
    else
    {
        pInst->uCntIsrTrigger++;
    }

    EPS_REG32_WRITE(pInst->sRemoteNotifyInfo.uAddress, pInst->sRemoteNotifyInfo.uValue); /* Write value to target processor SW interrupt */
}

/**
* Searches for a free ipc driver notification info instance and allocates it.
*
* Requests an interrupt vector from pool.
* Stores handle to interrupt vector in notification info.
* Stores 32 bit local address of interrupt vector in notification info
* Stores 32 bit translated pci address in notification info.
* Stores trigger value of interrupt vector in notification info.
*
* @param pInst pointer to ipc receive instance
*/
static LSA_VOID eps_ipc_generic_drv_alloc_local_notify_info(EPS_IPC_RX_INST_PTR_TYPE pInst)
{
    LSA_INT retVal;

    EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);

    EPS_ASSERT(g_pEpsIpcGenericDrv->fdIpcDriver != -1);

    retVal = ioctl(g_pEpsIpcGenericDrv->fdIpcDriver, EPS_ADONIS_IPC_DRV_ALLOC_LOCAL_NOTIFY_INFO, &pInst->sLocalNotifyInfo);
    EPS_ASSERT(retVal == 0);
    
	if (pInst->uType == eEPS_IPC_DRV_SUPPORT_CP1625)
	{
		// override address from irq_vector with local physical address, it is checked in eps_ipc_pndevdrv_drv
		pInst->sLocalNotifyInfo.uPhyAddress = (EPS_IPC_SOC_GICU_PHY_ADDR + EPS_IPC_SOC_GICU_OFFSET_IRQ_SWIRREG4);
	}
}


/**
* Free allocated free ipc driver notification info instance and interrupt vector.
*
* @param pInst pointer to ipc receive instance containing ipc notification info
*/
static LSA_VOID eps_ipc_generic_drv_free_local_notify_info(EPS_IPC_RX_INST_PTR_TYPE pInst)
{
    LSA_INT retVal;

    EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsIpcGenericDrv->fdIpcDriver != -1);

    retVal = ioctl(g_pEpsIpcGenericDrv->fdIpcDriver, EPS_ADONIS_IPC_DRV_FREE_LOCAL_NOTIFY_INFO, &pInst->sLocalNotifyInfo);
    EPS_ASSERT(retVal == 0);
}

/**
* Free allocated free ipc driver notification info instance and interrupt vector.
*
* @param pInst pointer to ipc receive instance containing ipc notification info
*/
static LSA_VOID eps_ipc_generic_drv_free_remote_notify_info(EPS_IPC_TX_INST_PTR_TYPE pInst)
{
    LSA_INT retVal;

    EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsIpcGenericDrv->fdIpcDriver != -1);

	if (pInst->uType == eEPS_IPC_DRV_SUPPORT_CP1625)
	{
		// remote notify was not allocated/translated by driver, just reset structure
		eps_memset(&pInst->sRemoteNotifyInfo, 0, sizeof(pInst->sRemoteNotifyInfo));
	}
	else
	{
		retVal = ioctl(g_pEpsIpcGenericDrv->fdIpcDriver, EPS_ADONIS_IPC_DRV_FREE_REMOTE_NOTIFY_INFO, &pInst->sRemoteNotifyInfo);
		EPS_ASSERT(retVal == 0);
	}
}



/**
* Registers ipc interrupt in adonis and enables it.
*
* @param pInst pointer to ipc receive instance containing interrupt information
*/
static LSA_VOID eps_ipc_generic_drv_enable_local_ipc_interrupt(EPS_IPC_RX_INST_PTR_TYPE pInst)
{
    LSA_INT retVal;

    EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsIpcGenericDrv->fdIpcDriver != -1);

    retVal = ioctl(g_pEpsIpcGenericDrv->fdIpcDriver, EPS_ADONIS_IPC_DRV_LOCAL_INT_ENABLE, (LSA_VOID*)&pInst->sLocalInterruptInfo);
    EPS_ASSERT(retVal == 0);
}

/**
* Adds logical address from uPciAddress in remote notification info to adonis memory mapping
* and stores virtual address into uAddress in remote notification info.
*
* @param pInst pointer to ipc receive instance containing remote notification info
*/
static LSA_VOID eps_ipc_generic_drv_translate_remote_notify_info(EPS_IPC_TX_INST_PTR_TYPE pInst)
{
    LSA_INT retVal;

    EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsIpcGenericDrv->fdIpcDriver != -1);
    
	if (pInst->uType == eEPS_IPC_DRV_SUPPORT_CP1625)
	{
		// no irq_vector but fix PCI ICU Interrupt is used -> just set address of SOC PCI ICU Register SWIRREG_A here -> see eps_ipc_pndevdrv_drv
		EPS_ASSERT(pInst->sRemoteNotifyInfo.uPhyAddress == (EPS_IPC_SOC_PICU_PHY_ADDR + EPS_IPC_SOC_PICU_OFFSET_IRQ_SWIRREG_A));
		pInst->sRemoteNotifyInfo.uAddress = pInst->sRemoteNotifyInfo.uPhyAddress;
	}
	else
	{
		// translate irq_vector
		retVal = ioctl(g_pEpsIpcGenericDrv->fdIpcDriver, EPS_ADONIS_IPC_DRV_TRANSLATE_NOTIFY_INFO, &pInst->sRemoteNotifyInfo);
		EPS_ASSERT(retVal == 0);
	}
}

/**
* Searches for a free ipc receive instance, allocates and inits it.
*
* @return pointer to allocated instance or LSA_NULL if no free instance was found
*/
static EPS_IPC_RX_INST_PTR_TYPE eps_ipc_generic_drv_alloc_rx_instance(LSA_VOID)
{
    LSA_UINT32 i;
    EPS_IPC_RX_INST_PTR_TYPE pInst = LSA_NULL;

    for (i = 0; i<EPS_IPC_MAX_INSTANCES; i++)
    {
        pInst = &g_pEpsIpcGenericDrv->InstRx[i];

        if (pInst->bUsed == LSA_FALSE)
            // found free instance
        {
            eps_memset(pInst, 0, sizeof(*pInst));
            pInst->hOwnHandle = i;
            pInst->bUsed = LSA_TRUE;
            break;
        }
        else
            // instance already used
        {
            pInst = LSA_NULL;
        }
    }

    return pInst;
}

/**
* Searches for a free ipc transceive instance, allocates and inits it.
*
* @return pointer to allocated instance or LSA_NULL if no free instance was found
*/
static EPS_IPC_TX_INST_PTR_TYPE eps_ipc_generic_drv_alloc_tx_instance(LSA_VOID)
{
    LSA_UINT32 i;
    EPS_IPC_TX_INST_PTR_TYPE pInst = LSA_NULL;

    for (i = 0; i<EPS_IPC_MAX_INSTANCES; i++)
    {
        pInst = &g_pEpsIpcGenericDrv->InstTx[i];

        if (pInst->bUsed == LSA_FALSE)
            // found free instance
        {
            eps_memset(pInst, 0, sizeof(*pInst));
            pInst->hOwnHandle = i;
            pInst->bUsed = LSA_TRUE;
            break;
        }
        else
            // instance already used
        {
            pInst = LSA_NULL;
        }
    }

    return pInst;
}

/**
* Marks used ipc receive instance as free.
*/
static LSA_VOID eps_ipc_generic_drv_free_rx_instance(EPS_IPC_RX_INST_PTR_TYPE pInst)
{
    EPS_ASSERT(pInst != LSA_NULL);

    EPS_ASSERT(pInst->bUsed == LSA_TRUE);
    pInst->uCntRcvIsrs = 0;
    pInst->uCntRcvTx = 0;
    pInst->uCntDataTransmitDelay = 0;
    pInst->bUsed = LSA_FALSE;
}

/**
* Marks used ipc transceive instance as free.
*/
static LSA_VOID eps_ipc_generic_drv_free_tx_instance(EPS_IPC_TX_INST_PTR_TYPE pInst)
{
    EPS_ASSERT(pInst != LSA_NULL);

    EPS_ASSERT(pInst->bUsed == LSA_TRUE);
    pInst->uCntIsrTrigger = 0;
    pInst->uCntDataReceiveDelay = 0;
    pInst->bUsed = LSA_FALSE;
}

/**
* Returns pointer to ipc receive instance for given ipc receive handle.
*
* @param hHandle of ipc receive instance
*
* @return pointer to ipce receive instance
*/
static EPS_IPC_RX_INST_PTR_TYPE eps_ipc_generic_drv_get_rx_inst(LSA_UINT32 hHandle)
{
    EPS_ASSERT(hHandle <= EPS_IPC_MAX_INSTANCES);
    EPS_ASSERT(g_pEpsIpcGenericDrv->InstRx[hHandle].bUsed == LSA_TRUE);
    return &g_pEpsIpcGenericDrv->InstRx[hHandle];
}

/**
* Returns pointer to ipc transceive instance for given ipc transceive handle.
*
* @param hHandle of ipc transceive instance
*
* @return pointer to ipce transceive instance
*/
static EPS_IPC_TX_INST_PTR_TYPE eps_ipc_generic_drv_get_tx_inst(LSA_UINT32 hHandle)
{
    EPS_ASSERT(hHandle <= EPS_IPC_MAX_INSTANCES);
    EPS_ASSERT(g_pEpsIpcGenericDrv->InstTx[hHandle].bUsed == LSA_TRUE);
    return &g_pEpsIpcGenericDrv->InstTx[hHandle];
}

/**
* Installation function of eps inter processor communication driver
* 
* Driver uses local interrupt controller for interrupt driven shm communication 
* Driver is used in combination with hif ipcsiedma driver in following cases
* 
* Advanced: Adonis x86 with Soc1 (hd upper)
* Advanced: Adonis MIPS/A53 with Soc1 and Hera (hd lower)
* Basic:    Windows x86 with Hera (hd/ld lower)  
* 
* Inits global eps ipc structure "g_EpsIpcGenericDrv" and stores it address in "g_pEpsIpcGenericDrv".
* Inits locks for eps critical sections.
* Installs eps ipc driver and opens it.
* Upon successful installation file descriptor to ipc driver is stored in global eps ipc structure.
*/
LSA_VOID eps_ipc_generic_drv_install(LSA_VOID)
{
    EPS_IPC_DRV_IF_TYPE ipcDrvIf;

    eps_memset(&g_EpsIpcGenericDrv, 0, sizeof(g_EpsIpcGenericDrv));
    g_pEpsIpcGenericDrv = &g_EpsIpcGenericDrv;

    eps_ipc_generic_drv_init_critical_section();
    
    eps_memset(&ipcDrvIf, 0, sizeof(ipcDrvIf));

    ipcDrvIf.uninstall =            eps_ipc_generic_drv_uninstall;

    ipcDrvIf.open =                 eps_ipc_generic_drv_open;
    ipcDrvIf.close =                eps_ipc_generic_drv_close;
    
    ipcDrvIf.rxAlloc =              eps_ipc_generic_drv_rx_alloc;
    ipcDrvIf.rxEnable =             eps_ipc_generic_drv_rx_enable;
    ipcDrvIf.rxDisable =            eps_ipc_generic_drv_rx_disable;
    ipcDrvIf.rxSetShm =             eps_ipc_generic_drv_rx_set_shm;
    ipcDrvIf.rxFree =               eps_ipc_generic_drv_rx_free;

    ipcDrvIf.txAlloc =              eps_ipc_generic_drv_tx_alloc;
    ipcDrvIf.txAllocFromLocalShm =  eps_ipc_generic_drv_tx_alloc_from_local_shm;
    ipcDrvIf.txFree =               eps_ipc_generic_drv_tx_free;
    ipcDrvIf.txSetShm =             eps_ipc_generic_drv_tx_set_shm;
    ipcDrvIf.txSend =               eps_ipc_generic_drv_tx_send;
    
    ipcDrvIf.uType =                eEPS_IPC_DRV_SUPPORT_GENERIC;

    eps_ipc_drv_if_register(&ipcDrvIf);
    
    g_pEpsIpcGenericDrv->bInit = LSA_TRUE;
}

/**
* Deinstallation function of eps inter processor communication
*
* Destroys locks for eps critical sections.
* Sets pointer to global eps ipc structure "g_pEpsIpcGenericDrv" to LSA_NULL.
*/
static LSA_VOID eps_ipc_generic_drv_uninstall(LSA_VOID)
{
    g_pEpsIpcGenericDrv->bInit = LSA_FALSE;
    
    eps_ipc_generic_drv_undo_init_critical_section();
    
    g_pEpsIpcGenericDrv = LSA_NULL;
}


/**
* Open function of eps inter processor communication
*
* Installs eps adonis ipc driver and opens it.
*/
static LSA_VOID eps_ipc_generic_drv_open(LSA_VOID)
{
	LSA_INT retVal;

    retVal = eps_adonis_ipc_drv_install();
	EPS_ASSERT(retVal == 0);

	g_pEpsIpcGenericDrv->fdIpcDriver = open(EPS_ADONIS_IPC_DRV_NAME, O_RDWR);
	EPS_ASSERT(g_pEpsIpcGenericDrv->fdIpcDriver != -1);
	
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Using EPS Ipc generic driver");
}



/**
* Deinstallation function of eps inter processor communication
*
* Closes eps adonis ipc driver and deinstalls it.
*/
static LSA_VOID eps_ipc_generic_drv_close(LSA_VOID)
{
	LSA_INT retVal;

	EPS_ASSERT(g_pEpsIpcGenericDrv->bInit == LSA_TRUE);

	close(g_pEpsIpcGenericDrv->fdIpcDriver);

	retVal = eps_adonis_ipc_drv_uninstall();
	EPS_ASSERT(retVal == 0);
}


/**
 * Eps ips interrupt service routine receive thread
 * 
 * Makes sure data has arrived and acknowledges the receive.
 * Executes given thread procedure after ipc receive.
 * 
 * @param uParam unused
 * @param pArgs pointer to ipc receive instance
 */
static LSA_VOID eps_ipc_generic_drv_isr_rx_rcv_thread(LSA_UINT32 uParam, LSA_VOID* pArgs)
{    
	EPS_IPC_RX_INST_PTR_TYPE pInst = (EPS_IPC_RX_INST_PTR_TYPE)pArgs;
	
	LSA_UNUSED_ARG(uParam);

	EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	EPS_ASSERT(pInst->bIsrEnabled == LSA_TRUE);

	if(pInst->pLocalShm != LSA_NULL)
	{
#if !((EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NONE) || ((EPS_PLF == EPS_PLF_PNIP_A53) && (EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NO_HERA)))
    // Do not Sync IPC IRQ if (EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NONE) or when running on hera and (EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NO_HERA)
        /* This algorithm checks if data that was transmitted has already arrived, that's needed
           because the data takes a different way then the interrupt 
        */
	    if(pInst->pLocalShm->uTxCnt == pInst->uCntRcvTx)
	    {
            EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_WARN, "eps_ipc_generic_drv_isr_rx_rcv_thread(): Received IPC Isr h(0x%x) but data hasn't arrived now => sleeping, current datatransmitdelay=0x%x", pInst->hOwnHandle, pInst->uCntDataTransmitDelay);
            while(pInst->pLocalShm->uTxCnt == pInst->uCntRcvTx) /* If there is new data the uTxCnt must be greater than the local uCntRcvTx */
            {
                pInst->uCntDataTransmitDelay++; /* If it's equal, data hasn't arrived => we need to wait for the data */
                eps_tasks_usleep(2);
            }
            EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_WARN, "eps_ipc_generic_drv_isr_rx_rcv_thread(): Received IPC Isr h(0x%x) datatransmitdelay=0x%x after sleeping", pInst->hOwnHandle, pInst->uCntDataTransmitDelay);
	    }
        /* So far this method works as long there is no interrupt lost. If one is lost the counters will be always different and we are losing incomplete data protection!
           Setting uCntRcvTx = uTxCnt sounds better but doesn't work as well, as a couple of interrupts get buffered by the apic causing uCntRcvTx and uTxCnt to get the same value
           which would stuck the system.

           The solution is to implement a ping/pong mechanism. Further tx interrupts on the source processor are not allowed until rx finished.
        */
#endif // #if !((EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NONE) || ((EPS_PLF == EPS_PLF_PNIP_A53) && (EPS_IPC_ACK_MODE == EPS_IPC_ACK_MODE_NO_HERA)))

		pInst->uCntRcvTx++; /* Increase our local uCntRcvtx => when IPC Irq is synchronized it should be equal to uTxCnt again */

		if(pInst->pRemoteShm != LSA_NULL)
		{
			pInst->pRemoteShm->uRxCnt = pInst->uCntRcvTx; /* If there is a feedback path in the other direction we set the actual counter there to ack the receive. */
		}
	}

	pInst->uCntRcvIsrs++;
	
	pInst->Cbf.pCbf(pInst->Cbf.uParam, pInst->Cbf.pArgs);
}



/**
* Enable ipc receive
* Creates and starts ipc event thread (@see eps_tasks_start_event_thread) and enables ipc interrupt.
*
* Ipc event thread waits for signal that is send when an interrupt has arrived.
* After signal for interrupt has arrived ipc isr receive thread routine (@see eps_ipc_generic_drv_isr_rx_rcv_thread) is executed.
* Where the given thread procedure "pCbf" is executed after receive is confirmed and acknowledged.
* After starting ipc event thread, an ipc interrupt is registered in adonis and enabled.
*
* @param hRxHandle handle of allocated ipc receive instance
* @param pCbf pointer to thread procedure to be executed after receive
* @param uParam parameter of thread procedure
* @param pArgs pointer to arguments of thread procedure
*/
static LSA_VOID eps_ipc_generic_drv_rx_enable(LSA_UINT32 hRxHandle, EPS_IPC_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs)
{
    EPS_IPC_RX_INST_PTR_TYPE pInst = eps_ipc_generic_drv_get_rx_inst(hRxHandle);

    EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsIpcGenericDrv->bInit == LSA_TRUE);

    eps_ipc_generic_drv_enter();

    EPS_ASSERT(pInst->bIsrEnabled == LSA_FALSE);

    pInst->Cbf.pCbf = pCbf;
    pInst->Cbf.uParam = uParam;
    pInst->Cbf.pArgs = pArgs;
    pInst->bIsrEnabled = LSA_TRUE;

    pInst->sLocalInterruptInfo.hThread = eps_tasks_start_event_thread("EPS_IPC_ISR", EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, eps_ipc_generic_drv_isr_rx_rcv_thread, 0, pInst, eRUN_ON_1ST_CORE);
    pInst->sLocalInterruptInfo.hIntHandle = pInst->sLocalNotifyInfo.hIntHandle;

    eps_ipc_generic_drv_enable_local_ipc_interrupt(pInst);

    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_generic_drv_rx_enable(): h(0x%x), hThread(0x%x), hIntHandle(0x%x)", pInst->hOwnHandle, pInst->sLocalInterruptInfo.hThread, pInst->sLocalInterruptInfo.hIntHandle);

    eps_ipc_generic_drv_exit();
}


/**
 * Allocates an ipc receive instance and ipc notification info instance. 
 * Requests an interrupt vector from pool and gets and stores ipc notification info.
 * 
 * @param hRxHandle is set to handle of allocated ipc receive instance
 * @param pPhysAddress is set to 32 bit pcie address (@see eps_ipc_generic_drv_alloc_local_notify_info())
 * @param pVal is set to ipc interrupt trigger value (@see eps_ipc_generic_drv_alloc_local_notify_info())
 * @param hd_id 
 * 
 * @return EPS_IPC_RET_OK
 */
static LSA_UINT16 eps_ipc_generic_drv_rx_alloc (LSA_UINT32* hRxHandle, LSA_UINT32* pPhysAddress, LSA_UINT32* pVal, LSA_UINT16 hd_id, LSA_UINT32 uType)
{
	EPS_IPC_RX_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
	EPS_ASSERT(pPhysAddress != LSA_NULL);
	EPS_ASSERT(pVal != LSA_NULL);
	EPS_ASSERT(hRxHandle != LSA_NULL);

	eps_ipc_generic_drv_enter();

	pInst = eps_ipc_generic_drv_alloc_rx_instance();
	EPS_ASSERT(pInst != LSA_NULL);
	
	pInst->hd_id = hd_id;
	pInst->uType = uType;

	eps_ipc_generic_drv_alloc_local_notify_info(pInst);

	*pVal = pInst->sLocalNotifyInfo.uValue;
	*pPhysAddress = pInst->sLocalNotifyInfo.uPhyAddress;

	*hRxHandle = pInst->hOwnHandle;

	EPS_SYSTEM_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_generic_drv_rx_alloc(): h(0x%x), hInt(0x%x), TrigAddr(0x%8x), TrigPhyAddr(0x%8x), Value(0x%8x), hd_id(0x%8x)", pInst->hOwnHandle, pInst->sLocalNotifyInfo.hIntHandle, pInst->sLocalNotifyInfo.uAddress, pInst->sLocalNotifyInfo.uPhyAddress, pInst->sLocalNotifyInfo.uValue, hd_id );

	eps_ipc_generic_drv_exit();

	return EPS_IPC_RET_OK;
}

/**
 * Free allocated ipc receive instance, ipc notification info instance
 * and used interrupt vector.
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 */
static LSA_VOID eps_ipc_generic_drv_rx_free (LSA_UINT32 hRxHandle)
{
    EPS_IPC_RX_INST_PTR_TYPE pInst = eps_ipc_generic_drv_get_rx_inst(hRxHandle);
    
	EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsIpcGenericDrv->bInit == LSA_TRUE);	

	eps_ipc_generic_drv_enter();

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	EPS_ASSERT(pInst->bIsrEnabled == LSA_FALSE);

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_generic_drv_rx_free(): h(0x%x)", pInst->hOwnHandle );

	eps_ipc_generic_drv_free_local_notify_info(pInst);

	eps_ipc_generic_drv_free_rx_instance(pInst);

	eps_ipc_generic_drv_exit();
}

/**
 * Disables ipc receive for given ipc receive handle.
 * 
 * Disables ipc receive interrupt and unregisters it in adonis.
 * Sends termination signal to ipc event thread (@see eps_tasks_stop_event_thread),
 * since receive interrupts are disabled ipc event thread will terminate upon termination signal.
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 */
static LSA_VOID eps_ipc_generic_drv_rx_disable (LSA_UINT32 hRxHandle)
{
    EPS_IPC_RX_INST_PTR_TYPE pInst = eps_ipc_generic_drv_get_rx_inst(hRxHandle);
    
	EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsIpcGenericDrv->bInit == LSA_TRUE);	

	eps_ipc_generic_drv_enter();

	EPS_ASSERT(pInst->bIsrEnabled == LSA_TRUE);

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_generic_drv_rx_disable(): h(0x%x)", pInst->hOwnHandle );

	eps_ipc_generic_drv_disable_local_ipc_interrupt(pInst);

	eps_tasks_stop_event_thread(pInst->sLocalInterruptInfo.hThread);
	
	pInst->bIsrEnabled = LSA_FALSE;

	eps_ipc_generic_drv_exit();
}

/**
 * Stores remote shared memory address and size in ipc receive instance.
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 * @param pBaseLocal base address of local shared memory
 * @param pBaseRemote base address of remote shared memory
 * @param uSize size of shared memory
 */
static LSA_VOID eps_ipc_generic_drv_rx_set_shm (LSA_UINT32 hRxHandle, LSA_UINT8* pBaseLocal, LSA_UINT8* pBaseRemote, LSA_UINT32 uSize)
{
    EPS_IPC_RX_INST_PTR_TYPE pInst = eps_ipc_generic_drv_get_rx_inst(hRxHandle);
    
	EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsIpcGenericDrv->bInit == LSA_TRUE);	

	eps_ipc_generic_drv_enter();

	EPS_ASSERT(pBaseLocal != LSA_NULL);
	EPS_ASSERT(pBaseRemote != LSA_NULL);
	EPS_ASSERT(uSize <= sizeof(EPS_IPC_SHM_TYPE));

	pInst->pLocalShm = (EPS_IPC_SHM_PTR_TYPE) pBaseLocal;
	
	pInst->pRemoteShm = (EPS_IPC_SHM_PTR_TYPE) pBaseRemote;

	pInst->pRemoteShm->uMagic = EPS_IPC_MAGIC_NUMBER;
	pInst->pRemoteShm->uTriggerBaseAddr = pInst->sLocalNotifyInfo.uPhyAddress;
	pInst->pRemoteShm->uTriggerMsg = pInst->sLocalNotifyInfo.uValue;
	pInst->pRemoteShm->uRxCnt = 0;

	eps_ipc_generic_drv_exit();
}

/**
 * Allocates an ipc transceive instance.
 * Stores target pci address, the virtual address from memory mapping (@see eps_ipc_translate_remote_notify_info())
 * and interrupt trigger value in remote notification info.
 * 
 * @param hTxHandle is set to handle of allocated ipc transceive instance
 * @param uPhysAddress target pci address
 * @param uMsgVal target interrupt trigger value
 * @param hd_id 
 * 
 * @return EPS_IPC_RET_OK
 */
static LSA_UINT16 eps_ipc_generic_drv_tx_alloc (LSA_UINT32* hTxHandle, LSA_UINT32 uPhysAddress, LSA_UINT32 uMsgVal, LSA_UINT16 hd_id, LSA_UINT32 uType)
{
	EPS_IPC_TX_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
	EPS_ASSERT(hTxHandle != LSA_NULL);

	eps_ipc_generic_drv_enter();

	pInst = eps_ipc_generic_drv_alloc_tx_instance();
	EPS_ASSERT(pInst != LSA_NULL);

	pInst->hd_id = hd_id;
	pInst->uType = uType;
	pInst->sRemoteNotifyInfo.uPhyAddress = uPhysAddress;
	pInst->sRemoteNotifyInfo.uValue = uMsgVal;

	eps_ipc_generic_drv_translate_remote_notify_info(pInst);

	EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_generic_drv_tx_alloc(): h(0x%x), TrigAddr(0x%8x), TrigPhyAddr(0x%8x), Value(0x%8x), hd_id(0x%8x)", pInst->hOwnHandle, pInst->sRemoteNotifyInfo.uAddress, pInst->sRemoteNotifyInfo.uPhyAddress, pInst->sRemoteNotifyInfo.uValue, hd_id );

	*hTxHandle = pInst->hOwnHandle;

	eps_ipc_generic_drv_exit();

	return EPS_IPC_RET_OK;
}


/**
 * Allocates an ipc transceive instance out of information stored in shared memory.
 * 
 * @param hTxHandle is set to handle of allocated ipc transceive instance
 * @param pBase base address of shared memory
 * @param uSize size of shared memory
 * @param hd_id 
 * 
 * @return EPS_IPC_RET_OK
 */
static LSA_UINT16 eps_ipc_generic_drv_tx_alloc_from_local_shm (LSA_UINT32* hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize, LSA_UINT16 hd_id, LSA_UINT32 uType)
{
	EPS_IPC_SHM_PTR_TYPE pShmLocalHeader = (EPS_IPC_SHM_PTR_TYPE) pBase;

	EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
	EPS_ASSERT(hTxHandle != LSA_NULL);
	EPS_ASSERT(pBase != LSA_NULL);
	EPS_ASSERT(uSize <= sizeof(EPS_IPC_SHM_TYPE));

	return eps_ipc_generic_drv_tx_alloc(hTxHandle, pShmLocalHeader->uTriggerBaseAddr, pShmLocalHeader->uTriggerMsg, hd_id, uType);
}

/**
 * Free allocated ipc transceive instance instance.
 * 
 * @param hTxHandle handle of allocated ipc transceive instance
 */
static LSA_VOID eps_ipc_generic_drv_tx_free (LSA_UINT32 hTxHandle)
{
    EPS_IPC_TX_INST_PTR_TYPE pInst = eps_ipc_generic_drv_get_tx_inst(hTxHandle);
    
	EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsIpcGenericDrv->bInit == LSA_TRUE);	

	eps_ipc_generic_drv_enter();

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);

	eps_ipc_generic_drv_free_remote_notify_info(pInst);

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_generic_drv_tx_free(): h(0x%x)", pInst->hOwnHandle );

	eps_ipc_generic_drv_free_tx_instance(pInst);

	eps_ipc_generic_drv_exit();
}

/**
 * Stores remote shared memory address and size in ipc transceive instance.
 * 
 * @param hTxHandle handle of allocated ipc transceive instance
 * @param pBaseLocal base address of local shared memory
 * @param pBaseRemote base address of remote shared memory
 * @param uSize size of shared memory
 */
static LSA_VOID eps_ipc_generic_drv_tx_set_shm (LSA_UINT32 hTxHandle, LSA_UINT8* pBaseLocal, LSA_UINT8* pBaseRemote, LSA_UINT32 uSize)
{
    EPS_IPC_TX_INST_PTR_TYPE pInst = eps_ipc_generic_drv_get_tx_inst(hTxHandle);
    
	EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsIpcGenericDrv->bInit == LSA_TRUE);	

	eps_ipc_generic_drv_enter();

	EPS_ASSERT(pBaseLocal != LSA_NULL);
	EPS_ASSERT(pBaseRemote != LSA_NULL);
	EPS_ASSERT(uSize <= sizeof(EPS_IPC_SHM_TYPE));

	pInst->pLocalShm = (EPS_IPC_SHM_PTR_TYPE) pBaseLocal;
	pInst->pRemoteShm = (EPS_IPC_SHM_PTR_TYPE) pBaseRemote;

	eps_ipc_generic_drv_exit();
}

/**
 * Send ipc SW interrupt to target.
 * 
 * @param hTxHandle handle of allocated ipc transceive instance
 */
static LSA_VOID eps_ipc_generic_drv_tx_send (LSA_UINT32 hTxHandle)
{
    EPS_IPC_TX_INST_PTR_TYPE pInst = eps_ipc_generic_drv_get_tx_inst(hTxHandle);
    
	EPS_ASSERT(g_pEpsIpcGenericDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsIpcGenericDrv->bInit == LSA_TRUE);	

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);

	eps_ipc_generic_drv_send_isr(pInst);
}

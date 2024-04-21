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
/*  F i l e               &F: eps_ipc_drv_if.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Inter Processor Communication Driver Interface                       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20010
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>                  /* Types / Prototypes / Fucns */
#include <eps_trc.h>                  /* Tracing                    */
#include <eps_ipc_drv_if.h>

#define EPS_IPC_DRV_IF_MAX		             2
#define EPS_CFG_MAX_DEVICES                 (PSI_CFG_MAX_IF_CNT) 

typedef struct eps_ipc_drv_if_store_tag
{
	struct
	{
		LSA_BOOL				bUsed;          // a driver is installed for this interface
		LSA_BOOL                bIsOpen;        // driver was opened before, important for closing
		EPS_IPC_DRV_IF_TYPE		ipcDrv;         // driver function pointers
		LSA_UINT32				uType;			// ipc type
	} sInterface[EPS_IPC_DRV_IF_MAX];

	LSA_UINT32                  ipcDrvMapping[EPS_CFG_MAX_DEVICES+1];       // map the hd_id to the proper ipc driver (ipc drivers are distinguished by uType)
} EPS_IPC_DRV_IF_STORE_TYPE;

//----------------------------------------------------------------------------------------------------

/* Global definition */
static EPS_IPC_DRV_IF_STORE_TYPE g_EpsIpcDrvIfStore;

static LSA_UINT32 eps_ipc_drv_get_driver(LSA_UINT16 hd_id)
{
    LSA_UINT32 driver;
    
    if (hd_id > (EPS_CFG_MAX_DEVICES))
    {
        EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "eps_ipc_drv: invalid hd number(%u)", hd_id);

        EPS_FATAL(0);
    }
    driver = g_EpsIpcDrvIfStore.ipcDrvMapping[hd_id];
    if(driver == 0xFFFFFFFF)
    {
        EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "eps_ipc_drv: no driver opened for this hd (%u)", hd_id);

        EPS_FATAL(0);
    }
    return driver;
}

/**
* Inits the IPC driver storage
* 
*/
LSA_VOID   eps_ipc_drv_if_init(LSA_VOID)
{
    LSA_UINT32 i;
    
    EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_drv_if_init: Init ipc driver");
	eps_memset(&g_EpsIpcDrvIfStore, 0, sizeof(g_EpsIpcDrvIfStore));
	
    for (i = 0; i <= EPS_CFG_MAX_DEVICES; i++)
    {
         g_EpsIpcDrvIfStore.ipcDrvMapping[i] = 0xFFFFFFFF;
    }
}


/**
* calls the uninstall function for all Interfaces.
*
* if all boards are closed, the Interfaces are uninstalled and marked as unused.
* g_EpsIpcDrvIfStore is set to 0 after uninstalling.
*
*/
LSA_VOID   eps_ipc_drv_if_undo_init(LSA_VOID)
{
	LSA_UINT32 i;

	for (i = 0; i < EPS_IPC_DRV_IF_MAX; i++)
	{
		if (g_EpsIpcDrvIfStore.sInterface[i].bUsed == LSA_TRUE)
		{
			EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[i].ipcDrv.uninstall != LSA_NULL);
			
			EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_drv_if_undo_init: Uninstall driver [%d]; Type: %d", i, g_EpsIpcDrvIfStore.sInterface[i].ipcDrv.uType);
			
			g_EpsIpcDrvIfStore.sInterface[i].ipcDrv.uninstall();
			g_EpsIpcDrvIfStore.sInterface[i].bUsed = LSA_FALSE;
		}
	}

	eps_memset(&g_EpsIpcDrvIfStore, 0, sizeof(g_EpsIpcDrvIfStore));
}


/**
* Registers a new interface
*
* if none function of pIpcDrvIf is LSA_NULL, the config is stored and it is marked as registered.
*
* @param [in] pIpcDrvIf - structure containing the driver controlling functions
*/
LSA_VOID   eps_ipc_drv_if_register(EPS_IPC_DRV_IF_CONST_PTR_TYPE pIpcDrvIf)
{
	LSA_UINT32 i;
   
	EPS_ASSERT(pIpcDrvIf->uninstall != LSA_NULL);

    EPS_ASSERT(pIpcDrvIf->open != LSA_NULL);
    EPS_ASSERT(pIpcDrvIf->close != LSA_NULL);
    
	EPS_ASSERT(pIpcDrvIf->rxEnable != LSA_NULL);
	EPS_ASSERT(pIpcDrvIf->rxDisable != LSA_NULL);
    EPS_ASSERT(pIpcDrvIf->rxAlloc != LSA_NULL);
    EPS_ASSERT(pIpcDrvIf->rxFree != LSA_NULL);
    EPS_ASSERT(pIpcDrvIf->rxSetShm != LSA_NULL);
    
    EPS_ASSERT(pIpcDrvIf->txAlloc != LSA_NULL);
    EPS_ASSERT(pIpcDrvIf->txAllocFromLocalShm != LSA_NULL);
    EPS_ASSERT(pIpcDrvIf->txFree != LSA_NULL);
    EPS_ASSERT(pIpcDrvIf->txSend != LSA_NULL);
    EPS_ASSERT(pIpcDrvIf->txSetShm != LSA_NULL);

	for (i = 0; i < EPS_IPC_DRV_IF_MAX; i++)
	{
		if (g_EpsIpcDrvIfStore.sInterface[i].bUsed == LSA_FALSE)
		{
		    EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_drv_if_register: Register driver [%d]; Type: %d", i, pIpcDrvIf->uType);
		    
		    g_EpsIpcDrvIfStore.sInterface[i].ipcDrv = *pIpcDrvIf;
		    g_EpsIpcDrvIfStore.sInterface[i].bUsed = LSA_TRUE;
			return;
		}
	}

	EPS_FATAL("eps_ipc_drv_if_register: no free slot available"); /* If Overflow */
}


/*********************/
/* Mapping functions */
/*********************/

/**
* Open one of the registered ipc driver depending on uType
* 
* An IPC driver can support one or more specific ipc types eEPS_IPC_DRV_SUPPORT_HERA/eEPS_IPC_DRV_SUPPORT_CP1625
* Or it is a generic ipc driver type eEPS_IPC_DRV_SUPPORT_GENERIC
* When no specific IPC Driver is found wich supports input parameter "uType", generic IPC Driver is used.
* Generic IPC Driver has to be installed after all specific IPC Drivers!
* 
* @see eps_ipc_generic_drv_open()
* @see eps_ipc_pndevdrv_drv_open()
* @see eps_ipc_linux_soc_drv_open()
*
* @param [in] hd_id - The hd for which the driver is installed
* @param [in] uType - Select which type of driver shall be used
*/
LSA_VOID   eps_ipc_drv_if_open(LSA_UINT16 hd_id, LSA_UINT32 uType)
{
	LSA_UINT32 i;

    if (hd_id > (EPS_CFG_MAX_DEVICES))
    {
        EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "eps_ipc_drv_if_open: invalid hd number(%u)", hd_id);

        EPS_FATAL(0);
    }
    
    EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_ipc_drv_if_open: Register driver for hd_id [%d]; Type support: %d", hd_id, uType);
    
	for (i = 0; i < EPS_IPC_DRV_IF_MAX; i++)
	{
		if (g_EpsIpcDrvIfStore.sInterface[i].bUsed == LSA_TRUE)
		{
		    // Select proper driver
			if (((g_EpsIpcDrvIfStore.sInterface[i].ipcDrv.uType & uType) != 0) || // use specific ipc driver
			    (g_EpsIpcDrvIfStore.sInterface[i].ipcDrv.uType == eEPS_IPC_DRV_SUPPORT_GENERIC)) // use generic ipc driver if no specific ipc driver was found
			{
			    // set mapping
			    g_EpsIpcDrvIfStore.ipcDrvMapping[hd_id] = i;

				// Store ipc type and call driver open function
				g_EpsIpcDrvIfStore.sInterface[i].ipcDrv.uType = uType;
			    g_EpsIpcDrvIfStore.sInterface[i].ipcDrv.open();
			    g_EpsIpcDrvIfStore.sInterface[i].bIsOpen = LSA_TRUE;
			    
			    EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_drv_if_open: Registration done for hd_id [%d] in slot [%d]; Type support: %d", hd_id, i, uType);
			    
				return;
			}
		}
	}

    EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL, "eps_ipc_drv_if_open: No proper driver installed");
    EPS_FATAL(0);
}

/**
* Close the registered ipc driver
*
* @see eps_ipc_generic_drv_close()
* @see eps_ipc_pndevdrv_drv_close()
* @see eps_ipc_linux_soc_drv_close()
*/
LSA_VOID   eps_ipc_drv_if_close_all(LSA_VOID)
{
    LSA_UINT32 i;

    EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_drv_if_close_all");
    
    for (i = 0; i < EPS_IPC_DRV_IF_MAX; i++)
    {
        if (g_EpsIpcDrvIfStore.sInterface[i].bUsed == LSA_TRUE)
        {
            if(g_EpsIpcDrvIfStore.sInterface[i].bIsOpen == LSA_TRUE)
            {
                // check if a closing function was registered. If so execute it
                if (g_EpsIpcDrvIfStore.sInterface[i].ipcDrv.close != LSA_NULL)
                {
                    g_EpsIpcDrvIfStore.sInterface[i].ipcDrv.close();
                }
                g_EpsIpcDrvIfStore.sInterface[i].bIsOpen = LSA_FALSE;
            }
        }
    }
}


/**
 * Allocates an ipc receive instance and ipc notification info instance. 
 * 
 * @see eps_ipc_generic_drv_rx_alloc()
 * @see eps_ipc_pndevdrv_drv_rx_alloc()
 * @see eps_ipc_linux_soc_drv_rx_alloc()
 * 
 * @param hRxHandle is set to handle of allocated ipc receive instance
 * @param pPhysAddress is set to 32 bit pcie address 
 * @param pVal is set to ipc interrupt trigger value
 * @param hd_id 
 *
 * @return EPS_IPC_RET_OK
 */
LSA_UINT16 eps_ipc_drv_if_rx_alloc(LSA_UINT32* hRxHandle, LSA_UINT32* pPhysAddress, LSA_UINT32* pVal, LSA_UINT16 hd_id)
{
	LSA_UINT32 driver = eps_ipc_drv_get_driver(hd_id);
    
    EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.rxAlloc != LSA_NULL);

    return g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.rxAlloc(hRxHandle, pPhysAddress, pVal, hd_id, g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.uType);
}

/**
 * Free allocated ipc receive instance, ipc notification info instance
 * and used interrupt vector.
 * 
 * @see eps_ipc_generic_drv_rx_free()
 * @see eps_ipc_pndevdrv_drv_rx_free()
 * @see eps_ipc_linux_soc_drv_rx_free()
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 */
LSA_VOID   eps_ipc_drv_if_rx_free(LSA_UINT32 hRxHandle, LSA_UINT16 hd_id)
{ 
	LSA_UINT32 driver = eps_ipc_drv_get_driver(hd_id);
    
    EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.rxFree != LSA_NULL);

    g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.rxFree(hRxHandle);
}

/**
 * Enable ipc receive
 * 
 * In Adonis/Linux:
 * Creates and starts ipc event thread and enables ipc interrupt.
 *
 * Ipc event thread waits for signal/event that is send when an interrupt has arrived.
 * After signal/event for interrupt has arrived ipc isr receive thread routine is executed.
 * Where the given thread procedure "pCbf" is executed after receive is confirmed and acknowledged.
 * After starting ipc event thread, an ipc interrupt is registered in adonis and enabled.
 *
 * In Windows:
 * Enable ipc interrupt in pndevdrv. The callbackfunction is called directly from pndevdrv.
 * 
 * @see eps_ipc_generic_drv_rx_enable()
 * @see eps_ipc_pndevdrv_drv_rx_enable()
 * @see eps_ipc_linux_soc_drv_rx_enable()
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 * @param pCbf pointer to thread procedure to be executed after receive
 * @param uParam parameter of thread procedure
 * @param pArgs pointer to arguments of thread procedure
 */
LSA_VOID   eps_ipc_drv_if_rx_enable(LSA_UINT32 hRxHandle, LSA_UINT16 hd_id, EPS_IPC_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs)
{
	LSA_UINT32 driver = eps_ipc_drv_get_driver(hd_id);
    
	EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.rxEnable != LSA_NULL);

	g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.rxEnable(hRxHandle, pCbf, uParam, pArgs);
}

/**
 * Disables ipc receive for given ipc receive handle.
 * 
 * Disables ipc receive interrupt and unregisters it.
 * 
 * @see eps_ipc_generic_drv_rx_disable()
 * @see eps_ipc_pndevdrv_drv_rx_disable()
 * @see eps_ipc_linux_soc_drv_rx_disable()
 * 
 *  @param hRxHandle handle of allocated ipc receive instance
 */
LSA_VOID   eps_ipc_drv_if_rx_disable(LSA_UINT32 hRxHandle, LSA_UINT16 hd_id)
{
	LSA_UINT32 driver = eps_ipc_drv_get_driver(hd_id);
    
	EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.rxDisable != LSA_NULL);

	g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.rxDisable(hRxHandle);
}

/**
 * Stores local/remote shared memory address and size in ipc receive instance.
 * 
 * @see eps_ipc_generic_drv_rx_set_shm()
 * @see eps_ipc_pndevdrv_drv_rx_set_shm()
 * @see eps_ipc_linux_soc_drv_rx_set_shm()
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 * @param pBaseLocal base address of local shared memory
 * @param pBaseRemote base address of remote shared memory
 * @param uSize size of shared memory
 */
LSA_VOID   eps_ipc_drv_if_rx_set_shm(LSA_UINT32  hRxHandle, LSA_UINT16 hd_id, LSA_UINT8* pBaseLocal, LSA_UINT8* pBaseRemote, LSA_UINT32 uSize)
{
	LSA_UINT32 driver = eps_ipc_drv_get_driver(hd_id);
    
	EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.rxSetShm != LSA_NULL);

	g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.rxSetShm(hRxHandle, pBaseLocal, pBaseRemote, uSize);
}


/**
 * Allocates an ipc transceive instance.
 * Stores target pci address, the virtual address from memory mapping
 * and interrupt trigger value in remote notification info.
 * 
 * @see eps_ipc_generic_drv_tx_alloc()
 * @see eps_ipc_pndevdrv_drv_tx_alloc()
 * @see eps_ipc_linux_soc_drv_tx_alloc()
 * 
 * @param hTxHandle is set to handle of allocated ipc transceive instance
 * @param uPhysAddress target pci address
 * @param uMsgVal target interrupt trigger value
 * @param hd_id 
 * 
 * @return EPS_IPC_RET_OK
 */
LSA_UINT16 eps_ipc_drv_if_tx_alloc(LSA_UINT32* hTxHandle, LSA_UINT32 uPhysAddress, LSA_UINT32 uMsgVal, LSA_UINT16 hd_id)
{
	LSA_UINT32 driver = eps_ipc_drv_get_driver(hd_id);
    
	EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.txAlloc != LSA_NULL);

	return g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.txAlloc(hTxHandle, uPhysAddress, uMsgVal, hd_id, g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.uType);
}

/**
 * Free allocated ipc transceive instance instance.
 * 
 * @see eps_ipc_generic_drv_tx_free()
 * @see eps_ipc_pndevdrv_drv_tx_free()
 * @see eps_ipc_linux_soc_drv_tx_free()
 * 
 * @param hTxHandle handle of allocated ipc transceive instance
 */
LSA_VOID   eps_ipc_drv_if_tx_free(LSA_UINT32  hTxHandle, LSA_UINT16 hd_id)
{
	LSA_UINT32 driver = eps_ipc_drv_get_driver(hd_id);
    
    EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.txFree != LSA_NULL);

    g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.txFree(hTxHandle);
}


/**
 * Allocates an ipc transceive instance out of information stored in shared memory.
 * 
 * @see eps_ipc_generic_drv_tx_alloc_from_local_shm()
 * @see eps_ipc_pndevdrv_drv_tx_alloc_from_local_shm()
 * @see eps_ipc_linux_soc_drv_tx_alloc_from_local_shm()
 * 
 * @param hTxHandle is set to handle of allocated ipc transceive instance
 * @param pBase base address of shared memory
 * @param uSize size of shared memory
 * @param hd_id 
 * 
 * @return EPS_IPC_RET_OK
 */
LSA_UINT16 eps_ipc_drv_if_tx_alloc_from_local_shm(LSA_UINT32* hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize, LSA_UINT16 hd_id)
{
	LSA_UINT32 driver = eps_ipc_drv_get_driver(hd_id);
    
	EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.txAllocFromLocalShm != LSA_NULL);

	return g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.txAllocFromLocalShm(hTxHandle, pBase, uSize, hd_id, g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.uType);
}


/**
 * Stores local/remote shared memory address and size in ipc transceive instance.
 * 
 * @see eps_ipc_generic_drv_tx_set_shm()
 * @see eps_ipc_pndevdrv_drv_tx_set_shm()
 * @see eps_ipc_linux_soc_drv_tx_set_shm()
 * 
 * @param hTxHandle handle of allocated ipc transceive instance
 * @param pBaseLocal base address of local shared memory
 * @param pBaseRemote base address of remote shared memory
 * @param uSize size of shared memory
 */
LSA_VOID   eps_ipc_drv_if_tx_set_shm(LSA_UINT32  hTxHandle, LSA_UINT16 hd_id, LSA_UINT8* pBaseLocal, LSA_UINT8* pBaseRemote, LSA_UINT32 uSize)
{
	LSA_UINT32 driver = eps_ipc_drv_get_driver(hd_id);
    
	EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.txSetShm != LSA_NULL);

	g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.txSetShm(hTxHandle, pBaseLocal, pBaseRemote, uSize);
}

/**
 * Send ipc SW interrupt to target.
 * 
 * @see eps_ipc_generic_drv_tx_send()
 * @see eps_ipc_pndevdrv_drv_tx_send()
 * @see eps_ipc_linux_soc_drv_tx_send()
 * 
 * @param hTxHandle handle of allocated ipc transceive instance
 */
LSA_VOID   eps_ipc_drv_if_tx_send(LSA_UINT32  hTxHandle, LSA_UINT16 hd_id)
{
	LSA_UINT32 driver = eps_ipc_drv_get_driver(hd_id);
    
	EPS_ASSERT(g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.txSend != LSA_NULL);

	g_EpsIpcDrvIfStore.sInterface[driver].ipcDrv.txSend(hTxHandle);
}


/**
* Returns size of eps ipc shared memory data structure.
*
* @return sizeof(EPS_IPC_SHM_TYPE)
*/
LSA_UINT32 eps_ipc_drv_if_sizeof_shm_data(LSA_VOID)
{
	return sizeof(EPS_IPC_SHM_TYPE);
}

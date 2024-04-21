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
/*  F i l e               &F: eps_hw_edds.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS EDDS Lower Layer Adaption                                            */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20060
#define EPS_MODULE_ID      20060

/* - Includes ------------------------------------------------------------------------------------- */
#include <eps_sys.h>            /* Types / Prototypes / Funcs               */
#include <eps_trc.h>            /* Tracing                                  */
#include <eps_cp_hw.h>          /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>      /* PN Device Driver Interface               */
#include <eps_plf.h>            /* EPS PCI Interface functions              */
#include <eps_app.h>            /* EPS Application Api                      */
#include <eps_hw_edds.h>        /* Own header                               */

/*----------------------------------------------------------------------------*/
/*  Defines                                                                   */
/*----------------------------------------------------------------------------*/

//Intel boards
#define EPS_HW_EDDS_INTEL_COUNT_MAC_IF      1
#define EPS_HW_EDDS_INTEL_COUNT_MAC_PORTS   1

//Intel IOT2000 boards
#ifdef PSI_EDDS_CFG_HW_IOT2000
#define EPS_HW_EDDS_IOT2000_COUNT_MAC_IF    1
#define EPS_HW_EDDS_IOT2000_COUNT_MAC_PORTS 1
#endif 

//Micrel boards
#define EPS_HW_EDDS_KSZ884X_COUNT_MAC_IF    1
#define EPS_HW_EDDS_KSZ8841_COUNT_MAC_PORTS 1
#define EPS_HW_EDDS_KSZ8842_COUNT_MAC_PORTS 2

//TI boards
#define EPS_HW_EDDS_TI_COUNT_MAC_IF         1
#define EPS_HW_EDDS_TI_COUNT_MAC_PORTS      2
//according to ti_reg.h
#define TI_MDIO_USERACCESS0                 0x1080
#define TI_MDIO_USERACCESS1                 0x1088
//according to LLIF_TI.h
/* Defines for tx/rx delay values for GMAC Gbit*/
#define TI_RX_TX_DELAY_VALUE_1900PS         38
#define TI_RX_TX_DELAY_VALUE_0PS            0

// Partitioning of the DEV and NRT memory for I210, all values in percent. Values are the same for i210 and i210IS
#define CP_MEM_I210_PERCENT_DEV             10
#define CP_MEM_I210_PERCENT_NRT_TX          35
#define CP_MEM_I210_PERCENT_NRT_RX          55

#define CP_MEM_I210_PERCENT_TOTAL (CP_MEM_I210_PERCENT_DEV + CP_MEM_I210_PERCENT_NRT_TX + CP_MEM_I210_PERCENT_NRT_RX)

#if (CP_MEM_I210_PERCENT_TOTAL != 100)
#error "Sum has to be 100 percent!"
#endif

// Partitioning of the DEV and NRT memory for I82574, all values in percent
#define CP_MEM_I82574_PERCENT_DEV           10
#define CP_MEM_I82574_PERCENT_NRT_TX        35
#define CP_MEM_I82574_PERCENT_NRT_RX        55

#define CP_MEM_I82574_PERCENT_TOTAL (CP_MEM_I82574_PERCENT_DEV + CP_MEM_I82574_PERCENT_NRT_TX + CP_MEM_I82574_PERCENT_NRT_RX)

#if (CP_MEM_I82574_PERCENT_TOTAL != 100)
#error "Sum has to be 100 percent!"
#endif

// Partitioning of the DEV and NRT memory for KSZ8841, all values in percent
#define CP_MEM_KSZ8841_PERCENT_DEV          10
#define CP_MEM_KSZ8841_PERCENT_NRT_TX       45
#define CP_MEM_KSZ8841_PERCENT_NRT_RX       45

#define CP_MEM_KSZ8841_PERCENT_TOTAL (CP_MEM_KSZ8841_PERCENT_DEV + CP_MEM_KSZ8841_PERCENT_NRT_TX + CP_MEM_KSZ8841_PERCENT_NRT_RX)

#if (CP_MEM_KSZ8841_PERCENT_TOTAL != 100)
#error "Sum has to be 100 percent!"
#endif

// Partitioning of the DEV and NRT memory for KSZ8842, all values in percent
#define CP_MEM_KSZ8842_PERCENT_DEV          10
#define CP_MEM_KSZ8842_PERCENT_NRT_TX       45
#define CP_MEM_KSZ8842_PERCENT_NRT_RX       45

#define CP_MEM_KSZ8842_PERCENT_TOTAL (CP_MEM_KSZ8842_PERCENT_DEV + CP_MEM_KSZ8842_PERCENT_NRT_TX + CP_MEM_KSZ8842_PERCENT_NRT_RX)

#if (CP_MEM_KSZ8842_PERCENT_TOTAL != 100)
#error "Sum has to be 100 percent!"
#endif

// Partitioning of the DEV and NRT memory for AM5728 GMAC, all values in percent
#define CP_MEM_AM5728_GMAC_PERCENT_DEV          10
#define CP_MEM_AM5728_GMAC_PERCENT_NRT_TX       45
#define CP_MEM_AM5728_GMAC_PERCENT_NRT_RX       45

#define CP_MEM_AM5728_GMAC_PERCENT_TOTAL (CP_MEM_AM5728_GMAC_PERCENT_DEV + CP_MEM_AM5728_GMAC_PERCENT_NRT_TX + CP_MEM_AM5728_GMAC_PERCENT_NRT_RX)

#if (CP_MEM_AM5728_GMAC_PERCENT_TOTAL != 100)
#error "Sum has to be 100 percent!"
#endif

/*----------------------------------------------------------------------------*/

#if (PSI_CFG_USE_EDDS == 1)

#if defined (PSI_EDDS_CFG_HW_INTEL)

/* - Prototypes ----------------------------------------------------------------------------------- */
static LSA_VOID EPS_INTEL_LL_LED_BACKUP_MODE(LSA_VOID_PTR_TYPE pLLManagement);
static LSA_VOID EPS_INTEL_LL_LED_RESTORE_MODE(LSA_VOID_PTR_TYPE pLLManagement);
static LSA_VOID EPS_INTEL_LL_LED_SET_MODE(LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL LEDOn);

/**
 * Get the MAC address for an Intel board
 * 
 * @param [in] uVendorId		 Vendor ID, e.g. Intel
 * @param [in] pBase			 Basepointer to seek for a MAC address
 * @param [out] pMac			 ptr to sys device in
 * @return LSA_TRUE              a MAC was read from the board
 * @return LSA_FALSE             Not supported
 */
static LSA_BOOL eps_intelLL_get_mac_addr(LSA_UINT16         uVendorId,
                                         const LSA_UINT8  * pBase,
                                         LSA_UINT8        * pMac)
{
	LSA_UINT32 tmp;

	EPS_ASSERT(pMac != LSA_NULL);
	EPS_ASSERT(pBase != LSA_NULL);
	EPS_ASSERT(uVendorId != 0);

	pMac[0] = 0;
	pMac[1] = 0;
	pMac[2] = 0;
	pMac[3] = 0;
	pMac[4] = 0;
	pMac[5] = 0;

	if (uVendorId == EPS_INTEL_PCI_VENDOR_ID)
	{
		tmp = eps_edds_basereg_reg32(pBase, INTEL_RAH_0_15(0));
		pMac[4] = (tmp /*>> 0*/ ) & 0xFF; // >> 0 commented out - avoid lint warning
		pMac[5] = (tmp >> 8     ) & 0xFF;
		tmp = eps_edds_basereg_reg32(pBase, INTEL_RAL_0_15(0));
		pMac[0] = (tmp /*>> 0*/ ) & 0xFF; // >> 0 commented out - avoid lint warning
		pMac[1] = (tmp >> 8     ) & 0xFF;
		pMac[2] = (tmp >> 16    ) & 0xFF;
		pMac[3] = (tmp >> 24    ) & 0xFF;

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/**
 * Intel LL open device for EDDS
 * 
 * @param pEddsIn 				pointer to edds parameter structure
 * @param pHwIn					pointer to structure with hardware configuration
 * @param pSysDevIn				ptr to sys device in
 * @param pRegBaseIn			ptr to BaseRegister
 * @param hd_idIn				index of hd
 * @param uVendorIdIn
 * @param uDeviceIdIn
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_intelLL_open(EPS_EDDS_INTEL_LL_PTR_TYPE   pEddsIn,
							EPS_PNDEV_HW_PTR_TYPE	     pHwIn,
							EPS_SYS_PTR_TYPE		     pSysDevIn,							
							LSA_UINT8                  * pRegBaseIn,
							LSA_UINT16 				     hd_idIn,
							LSA_UINT16				     uVendorIdIn,
							LSA_UINT16				     uDeviceIdIn)
{
	EDDS_LL_TABLE_TYPE      * pLLTable;
	LSA_UINT16				  uPortMacEnd;
	LSA_UINT16				  i;
	LSA_UINT16				  result = EPS_PNDEV_RET_OK;
	EPS_BOARD_INFO_PTR_TYPE   pBoardInfo;
	
    EPS_APP_MAC_ADDR_ARR      MacAddresses;
    LSA_UINT16                loopCnt = 0;
    EPS_MAC_PTR_TYPE          pMac = LSA_NULL;

	EPS_ASSERT(pEddsIn != LSA_NULL); 
	EPS_ASSERT(pHwIn != LSA_NULL); 
	EPS_ASSERT(pSysDevIn != LSA_NULL); 

	if (pRegBaseIn != LSA_NULL)
	{
		/* Fill EDDS LL */

		pLLTable = &pEddsIn->tLLFuncs;

		/* EDDS init/setup/shutdown/close functions */
		pLLTable->open                      = INTEL_LL_OPEN;
		pLLTable->setup                     = INTEL_LL_SETUP;
		pLLTable->shutdown                  = INTEL_LL_SHUTDOWN;
		pLLTable->close                     = INTEL_LL_CLOSE;

		/* Transmit buffer handling functions */
		pLLTable->enqueueSendBuffer         = INTEL_LL_SEND;
		pLLTable->getNextFinishedSendBuffer = INTEL_LL_SEND_STS;
		pLLTable->triggerSend               = INTEL_LL_SEND_TRIGGER;

		/* Receive buffer handling functions */
		pLLTable->getNextReceivedBuffer     = INTEL_LL_RECV;
		pLLTable->provideReceiveBuffer      = INTEL_LL_RECV_PROVIDE;
		pLLTable->triggerReceive            = INTEL_LL_RECV_TRIGGER;

		/* Information functions */
		pLLTable->getLinkState              = INTEL_LL_GET_LINK_STATE;
		pLLTable->getStatistics             = INTEL_LL_GET_STATS;

		/* MAC address management functions */
		pLLTable->enableMC                  = INTEL_LL_MC_ENABLE;
		pLLTable->disableMC                 = INTEL_LL_MC_DISABLE;

		/* Scheduler function */
        pLLTable->recurringTask             = INTEL_LL_RECURRING_TASK;

		/* Control functions */
		pLLTable->setLinkState              = INTEL_LL_SET_LINK_STATE;
		pLLTable->backupLocationLEDs        = EPS_INTEL_LL_LED_BACKUP_MODE;
		pLLTable->restoreLocationLEDs       = EPS_INTEL_LL_LED_RESTORE_MODE;
		pLLTable->setLocationLEDs           = EPS_INTEL_LL_LED_SET_MODE;

		/* Switch functions */
        // Note: All current Intel boards that are supported by EPS have one port. We can set these functions to LSA_NULL.
		pLLTable->setSwitchPortState        = LSA_NULL;
		pLLTable->controlSwitchMulticastFwd = LSA_NULL;
		pLLTable->flushSwitchFilteringDB    = LSA_NULL;

		pLLTable->changePort				= INTEL_LL_SWITCH_CHANGE_PORT;

	    // EDDS set arp filter function
		#ifdef LLIF_CFG_USE_LL_ARP_FILTER
		pLLTable->setArpFilter              = LSA_NULL;
		#endif

		// Fill Device Info
		pBoardInfo = &pHwIn->EpsBoardInfo;
		eps_memset(pBoardInfo,0, sizeof(*pBoardInfo));

		pSysDevIn->hd_nr       = hd_idIn;
		pSysDevIn->pnio_if_nr  = 0;	// don't care
		pSysDevIn->edd_comp_id = LSA_COMP_ID_EDDS;

		// Common HD settings
		pBoardInfo->edd_type      = LSA_COMP_ID_EDDS;
		pBoardInfo->hd_sys_handle = pSysDevIn;

        // -- MAC address handling START ----------------------------------

        // prepare the MAC-array
        eps_memset(&MacAddresses, 0, sizeof(MacAddresses));
        
        // Interface part
        // First we get the MAC address from the hardware - for intelLL it is only one
        eps_intelLL_get_mac_addr(uVendorIdIn, pRegBaseIn, pBoardInfo->if_mac.mac);
        // fill in the interface MAC to the array
        MacAddresses.lArrayMacAdr[0][0] = pBoardInfo->if_mac.mac[0];
        MacAddresses.lArrayMacAdr[0][1] = pBoardInfo->if_mac.mac[1];
        MacAddresses.lArrayMacAdr[0][2] = pBoardInfo->if_mac.mac[2];
        MacAddresses.lArrayMacAdr[0][3] = pBoardInfo->if_mac.mac[3];
        MacAddresses.lArrayMacAdr[0][4] = pBoardInfo->if_mac.mac[4];
        MacAddresses.lArrayMacAdr[0][5] = pBoardInfo->if_mac.mac[5];        

        // Port part
        // Init User to HW port mapping (used PSI GET HD PARAMS)
        eps_hw_init_board_port_param(pBoardInfo);

        // Port specific setup
        pBoardInfo->nr_of_ports = EPS_HW_EDDS_INTEL_COUNT_MAC_PORTS;

        // preset Port MACs (Ersatz Mac Adressen laut SPH PDEV bei Standard Hw)
        for (i = 0; i < pBoardInfo->nr_of_ports; i++)
        {
            uPortMacEnd = 0x3840 + i;

            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][0] = 0x08;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][1] = 0x00;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][2] = 0x06;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][3] = 0x9D;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][4] = (LSA_UINT8)(uPortMacEnd>>8);
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][5] = (LSA_UINT8)(uPortMacEnd&0xff);
        }

        // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
        EPS_APP_GET_MAC_ADDR(&MacAddresses, pSysDevIn->hd_nr, (EPS_HW_EDDS_INTEL_COUNT_MAC_IF + EPS_HW_EDDS_INTEL_COUNT_MAC_PORTS) );
        
        // we got the array with MAC addresses back and trace it
        for(loopCnt = 0; loopCnt < (EPS_HW_EDDS_INTEL_COUNT_MAC_IF + EPS_HW_EDDS_INTEL_COUNT_MAC_PORTS); loopCnt++)
        {
            EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                                    "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x", 
                                    pSysDevIn->hd_nr, loopCnt,
                                    MacAddresses.lArrayMacAdr[loopCnt][0],
                                    MacAddresses.lArrayMacAdr[loopCnt][1],
                                    MacAddresses.lArrayMacAdr[loopCnt][2],
                                    MacAddresses.lArrayMacAdr[loopCnt][3],
                                    MacAddresses.lArrayMacAdr[loopCnt][4],
                                    MacAddresses.lArrayMacAdr[loopCnt][5]);
        }
        
        // we now check the array with MAC addresses if any of the needed addresses is empty
        for(loopCnt = 0; loopCnt < (EPS_HW_EDDS_INTEL_COUNT_MAC_IF + EPS_HW_EDDS_INTEL_COUNT_MAC_PORTS); loopCnt++)
        {
            if(   MacAddresses.lArrayMacAdr[loopCnt][0] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][1] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][2] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][3] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][4] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][5] == 0x00
              )
            {
                EPS_FATAL("Empty MAC-Address detected");
            }
        }

        // set the interface MAC with the value from the array returned
        pMac = (EPS_MAC_PTR_TYPE)&MacAddresses.lArrayMacAdr[0];        
        pBoardInfo->if_mac = *pMac;

        // set the Port MAC(s) with the value(s) from the array returned
        for (i = 0; i < pBoardInfo->nr_of_ports; i++)
        {
            pMac = (EPS_MAC_PTR_TYPE)&MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF];        
            pBoardInfo->port_mac[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF] = *pMac;
            
            // setup portmapping (=1:1)
            pBoardInfo->port_map[i+1].hw_phy_nr  = i;
            pBoardInfo->port_map[i+1].hw_port_id = i+1;
        }

        // -- MAC address handling DONE ----------------------------------

		// Prepare process image settings
		pBoardInfo->pi_mem.base_ptr = 0;
		pBoardInfo->pi_mem.phy_addr = 0;
		pBoardInfo->pi_mem.size     = 0;

		// setup HIF buffer
		pBoardInfo->hif_mem.base_ptr = 0;
		pBoardInfo->hif_mem.phy_addr = 0;
		pBoardInfo->hif_mem.size     = 0;

		// add the LL settings
		pBoardInfo->edds.is_valid          = LSA_TRUE;
		pBoardInfo->edds.ll_handle         = (LSA_VOID_PTR_TYPE)&pEddsIn->hLL;
		pBoardInfo->edds.ll_function_table = (LSA_VOID_PTR_TYPE)&pEddsIn->tLLFuncs;

		pBoardInfo->edds.bFillActive        = LSA_FALSE;
        pBoardInfo->edds.bHsyncModeActive   = LSA_FALSE;
        pBoardInfo->edds.hTimer             = 0;
        pBoardInfo->edds.hMsgqHigh          = 0;
        pBoardInfo->edds.hMsgqLow           = 0;
        pBoardInfo->edds.hMsgqActive        = 0;
        pBoardInfo->edds.hThreadHigh        = 0;
        pBoardInfo->edds.hThreadLow         = 0;

		// fill LL Params
		pEddsIn->sIntelLLParameter.pRegBaseAddr           = (LSA_VOID_PTR_TYPE)pRegBaseIn; /* Register Base Address on Intel LL */
		pEddsIn->sIntelLLParameter.DeviceID               = (LSA_UINT16)uDeviceIdIn;
		pEddsIn->sIntelLLParameter.Disable1000MBitSupport = LSA_TRUE;
		pBoardInfo->edds.ll_params                        = (LSA_VOID*)(&pEddsIn->sIntelLLParameter);

		// fill other Params
		pHwIn->asic_type               = EPS_PNDEV_ASIC_INTEL_XX;
		pHwIn->EpsBoardInfo.board_type = EPS_PNDEV_BOARD_INTEL_XX;

		// initialization of MediaType
		eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pHwIn->EpsBoardInfo, pHwIn->asic_type, pHwIn->EpsBoardInfo.board_type);

        #if (EPS_PLF == EPS_PLF_ADONIS_X86)
		pHwIn->IrMode = EPS_PNDEV_IR_MODE_CPU; // TODO-MSI: = EPS_PNDEV_IR_MODE_POLLINTERRUPT;
        #else
		pHwIn->IrMode = EPS_PNDEV_IR_MODE_POLL; // TODO-MSI: = EPS_PNDEV_IR_MODE_POLLINTERRUPT;
        #endif
	}
	else
	{
		result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
	}

	return result;
}

LSA_VOID eps_i210_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid)
{
	EPS_BOARD_INFO_TYPE	* const pboardInfo           = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;
    LSA_UINT32            const dev_nrt_mem_size_all = pboardInfo->edds.sdram_NRT.size;

    // setup DEV and NRT memory pools

    pboardInfo->dev_mem.base_ptr    = pboardInfo->edds.sdram_NRT.base_ptr;
    pboardInfo->dev_mem.phy_addr    = pboardInfo->edds.sdram_NRT.phy_addr;
    pboardInfo->dev_mem.size        = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_I210_PERCENT_DEV);

    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->dev_mem.base_ptr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->dev_mem.phy_addr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_I210_PERCENT_NRT_TX);

    pboardInfo->nrt_rx_mem.base_ptr = pboardInfo->nrt_tx_mem.base_ptr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.phy_addr = pboardInfo->nrt_tx_mem.phy_addr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_I210_PERCENT_NRT_RX);

	EPS_ASSERT(pboardInfo->dev_mem.base_ptr != LSA_NULL);
}

LSA_VOID eps_i82574_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid)
{
	EPS_BOARD_INFO_TYPE	* const pboardInfo           = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;
    LSA_UINT32            const dev_nrt_mem_size_all = pboardInfo->edds.sdram_NRT.size;

    // setup DEV and NRT memory pools

    pboardInfo->dev_mem.base_ptr    = pboardInfo->edds.sdram_NRT.base_ptr;
    pboardInfo->dev_mem.phy_addr    = pboardInfo->edds.sdram_NRT.phy_addr;
    pboardInfo->dev_mem.size        = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_I82574_PERCENT_DEV);

    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->dev_mem.base_ptr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->dev_mem.phy_addr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_I82574_PERCENT_NRT_TX);

    pboardInfo->nrt_rx_mem.base_ptr = pboardInfo->nrt_tx_mem.base_ptr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.phy_addr = pboardInfo->nrt_tx_mem.phy_addr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_I82574_PERCENT_NRT_RX);

	EPS_ASSERT(pboardInfo->dev_mem.base_ptr != LSA_NULL);
}

/**
* LED Backup Mode for EDDS Intel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_INTEL_LL_LED_BACKUP_MODE(
    LSA_VOID_PTR_TYPE pLLManagement)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_INTEL_LL_LED_BACKUP_MODE(): call EPS_APP_LL_LED_BACKUP_MODE");
    EPS_APP_LL_LED_BACKUP_MODE(pBoard, pLLManagement);
#else
    INTEL_LL_LED_BACKUP_MODE(pLLManagement);
#endif
}

/**
* LED Restore Mode for EDDS Intel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_INTEL_LL_LED_RESTORE_MODE(
    LSA_VOID_PTR_TYPE pLLManagement)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_INTEL_LL_LED_RESTORE_MODE(): call EPS_APP_LL_LED_RESTORE_MODE");
    EPS_APP_LL_LED_RESTORE_MODE(pBoard, pLLManagement);
#else
    INTEL_LL_LED_RESTORE_MODE(pLLManagement);
#endif
}

/**
* LED Set Mode for EDDS Intel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_INTEL_LL_LED_SET_MODE(
    LSA_VOID_PTR_TYPE   pLLManagement,
    LSA_BOOL            LEDOn)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_INTEL_LL_LED_SET_MODE(): call EPS_APP_LL_LED_SET_MODE");
    EPS_APP_LL_LED_SET_MODE(pBoard, pLLManagement, LEDOn);
#else
    INTEL_LL_LED_SET_MODE(pLLManagement, LEDOn);
#endif
}
#endif // PSI_EDDS_CFG_HW_INTEL

#if defined (PSI_EDDS_CFG_HW_KSZ88XX)
/* - Prototypes ----------------------------------------------------------------------------------- */
static LSA_VOID EPS_KSZ88XX_LL_LED_BACKUP_MODE(LSA_VOID_PTR_TYPE pLLManagement);
static LSA_VOID EPS_KSZ88XX_LL_LED_RESTORE_MODE(LSA_VOID_PTR_TYPE pLLManagement);
static LSA_VOID EPS_KSZ88XX_LL_LED_SET_MODE(LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL LEDOn);

/**
 * Get the MAC address for a Micrel board
 * 
 * @param [in] uVendorId		 Vendor ID, e.g. Micrel
 * @param [in] pBase			 Basepointer to seek for a MAC address
 * @param [out] pMac			 ptr to sys device in
 * @return LSA_TRUE              a MAC was read from the board
 * @return LSA_FALSE             Not supported
 */
static LSA_BOOL eps_ksz88xx_get_mac_addr(LSA_UINT16         uVendorId,
								         const LSA_UINT8  * pBase,
								         LSA_UINT8        * pMac)
{
	EPS_ASSERT(pMac != LSA_NULL);
	EPS_ASSERT(pBase != LSA_NULL);
	EPS_ASSERT(uVendorId != 0);

	pMac[0] = 0;
	pMac[1] = 0;
	pMac[2] = 0;
	pMac[3] = 0;
	pMac[4] = 0;
	pMac[5] = 0;

	if (uVendorId == EPS_KSZ88XX_PCI_VENDOR_ID) 
	{
		LSA_UINT16 tmp;	
		tmp = eps_edds_basereg_reg16(pBase, KSZ88XX_MARL_0_15(0));
		pMac[4] = (tmp>>8)&0xFF; 
		pMac[5] = tmp & 0xFF;
		tmp = eps_edds_basereg_reg16(pBase, KSZ88XX_MARM_0_15(0));
		pMac[2] = (tmp>>8)&0xFF; 
		pMac[3] = tmp & 0xFF;
		tmp = eps_edds_basereg_reg16(pBase, KSZ88XX_MARH_0_15(0));
		pMac[0] = (tmp>>8)&0xFF; 
		pMac[1] = tmp & 0xFF;

		return LSA_TRUE;
	}

	return LSA_FALSE;
}											

/**
 * KSZ88XX LL open device for edds
 * 
 * @param pEddsIn 				pointer to edds parameter structure
 * @param pHwIn					pointer to structure with hardware configuration
 * @param pSysDevIn				ptr to sys device in
 * @param pRegBaseIn			ptr to BaseRegister
 * @param hd_idIn				index of hd
 * @param uVendorIdIn
 * @param uDeviceIdIn
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_ksz88xx_open(EPS_EDDS_KSZ88XX_LL_PTR_TYPE   pEddsIn,
							EPS_PNDEV_HW_PTR_TYPE		   pHwIn,
							EPS_SYS_PTR_TYPE			   pSysDevIn,
							LSA_UINT8                    * pRegBaseIn,
							LSA_UINT16 					   hd_idIn,
							LSA_UINT16					   uVendorIdIn,
							LSA_UINT16					   uDeviceIdIn)
{
	EDDS_LL_TABLE_TYPE      * pLLTable;
	LSA_UINT16                uPortMacEnd;
	LSA_UINT16                i;
	LSA_UINT16                result = EPS_PNDEV_RET_OK;
	EPS_BOARD_INFO_PTR_TYPE   pBoardInfo;

    EPS_APP_MAC_ADDR_ARR      MacAddresses;
    LSA_UINT16                loopCnt;
    EPS_MAC_PTR_TYPE          pMac;

	EPS_ASSERT(pEddsIn != LSA_NULL); 
	EPS_ASSERT(pHwIn != LSA_NULL); 
	EPS_ASSERT(pSysDevIn != LSA_NULL); 

	if (pRegBaseIn != LSA_NULL)
	{
		/* Fill EDDS LL */
		pLLTable = &pEddsIn->tLLFuncs;

		/* EDDS init/setup/shutdown/close functions */
		pLLTable->open                      = KSZ88XX_LL_OPEN;
		pLLTable->setup                     = KSZ88XX_LL_SETUP;
		pLLTable->shutdown                  = KSZ88XX_LL_SHUTDOWN;
		pLLTable->close                     = KSZ88XX_LL_CLOSE;

		/* Transmit buffer handling functions */
		pLLTable->enqueueSendBuffer         = KSZ88XX_LL_SEND;
		pLLTable->getNextFinishedSendBuffer = KSZ88XX_LL_SEND_STS;
		pLLTable->triggerSend               = KSZ88XX_LL_SEND_TRIGGER;

		/* Receive buffer handling functions */
		pLLTable->getNextReceivedBuffer     = KSZ88XX_LL_RECV;
		pLLTable->provideReceiveBuffer      = KSZ88XX_LL_RECV_PROVIDE;
		pLLTable->triggerReceive            = KSZ88XX_LL_RECV_TRIGGER;
    
		/* Information functions */
		pLLTable->getLinkState              = KSZ88XX_LL_GET_LINK_STATE;
		pLLTable->getStatistics             = KSZ88XX_LL_GET_STATS;
		
		/* MAC address management functions */
		pLLTable->enableMC                  = KSZ88XX_LL_MC_ENABLE;
		pLLTable->disableMC                 = KSZ88XX_LL_MC_DISABLE;

		/* Scheduler function */
        pLLTable->recurringTask             = KSZ88XX_LL_RECURRING_TASK;

		/* Control functions */
		pLLTable->setLinkState              = KSZ88XX_LL_SET_LINK_STATE;
		pLLTable->backupLocationLEDs        = EPS_KSZ88XX_LL_LED_BACKUP_MODE;
		pLLTable->restoreLocationLEDs       = EPS_KSZ88XX_LL_LED_RESTORE_MODE;
		pLLTable->setLocationLEDs           = EPS_KSZ88XX_LL_LED_SET_MODE;

		/* Switch functions */
		// Note: No differentiation for setSwitchPortState, controlSwitchMulticastFwd and flushSwitchFilteringDB for ksz8841 and ksz8842
        // We have two Micrel variants:
        // - ksz8841 does not need these functions, they could be NULL
        // - ksz8842 does need these functions if MRP is used
        // To make it simple, we provide the function for both boards.
		pLLTable->setSwitchPortState        = KSZ88XX_LL_SWITCH_SET_PORT_STATE;
		pLLTable->controlSwitchMulticastFwd = KSZ88XX_LL_SWITCH_MULTICAST_FWD_CTRL;
		pLLTable->flushSwitchFilteringDB    = KSZ88XX_LL_SWITCH_FLUSH_FILTERING_DB;

        pLLTable->changePort				= KSZ88XX_LL_SWITCH_CHANGE_PORT;

		/* EDDS set arp filter function */
		#ifdef LLIF_CFG_USE_LL_ARP_FILTER
		pLLTable->setArpFilter 				= LSA_NULL; 
		#endif

		/* Fill Device Info */
		pBoardInfo = &pHwIn->EpsBoardInfo;
		eps_memset(pBoardInfo,0, sizeof(*pBoardInfo));

		pSysDevIn->hd_nr       = hd_idIn;
		pSysDevIn->pnio_if_nr  = 0;	// don't care
		pSysDevIn->edd_comp_id = LSA_COMP_ID_EDDS;

		// Common HD settings
		pBoardInfo->edd_type      = LSA_COMP_ID_EDDS;
		pBoardInfo->hd_sys_handle = pSysDevIn;

        // -- MAC address handling START ----------------------------------

        // prepare the MAC-array
        eps_memset(&MacAddresses, 0, sizeof(MacAddresses));
                
        // Interface part
        // First we get the MAC address from the hardware - for ksz88xx it is only one
        eps_ksz88xx_get_mac_addr(uVendorIdIn, pRegBaseIn, pBoardInfo->if_mac.mac);
        // fill in the interface MAC to the array
        MacAddresses.lArrayMacAdr[0][0] = pBoardInfo->if_mac.mac[0];
        MacAddresses.lArrayMacAdr[0][1] = pBoardInfo->if_mac.mac[1];
        MacAddresses.lArrayMacAdr[0][2] = pBoardInfo->if_mac.mac[2];
        MacAddresses.lArrayMacAdr[0][3] = pBoardInfo->if_mac.mac[3];
        MacAddresses.lArrayMacAdr[0][4] = pBoardInfo->if_mac.mac[4];
        MacAddresses.lArrayMacAdr[0][5] = pBoardInfo->if_mac.mac[5];   

        // Port part
        // Init User to HW port mapping (used PSI GET HD PARAMS)
        eps_hw_init_board_port_param(pBoardInfo);

        // Port specific setup
        if (EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8841 == uDeviceIdIn)
        {
            pBoardInfo->nr_of_ports = EPS_HW_EDDS_KSZ8841_COUNT_MAC_PORTS;
        }
        else if (EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8842 == uDeviceIdIn)
        {
            pBoardInfo->nr_of_ports = EPS_HW_EDDS_KSZ8842_COUNT_MAC_PORTS;
        }
            
        // preset Port MACs (Ersatz Mac Adressen laut SPH PDEV bei Standard Hw)
        for (i = 0; i < pBoardInfo->nr_of_ports; i++)
        {
            uPortMacEnd = 0x3840 + i;

            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][0] = 0x08;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][1] = 0x00;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][2] = 0x06;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][3] = 0x9D;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][4] = (LSA_UINT8)(uPortMacEnd>>8);
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_INTEL_COUNT_MAC_IF][5] = (LSA_UINT8)(uPortMacEnd&0xff);
        }

        // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
        if (EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8841 == uDeviceIdIn)
        {
            EPS_APP_GET_MAC_ADDR(&MacAddresses, pSysDevIn->hd_nr, (EPS_HW_EDDS_KSZ884X_COUNT_MAC_IF + EPS_HW_EDDS_KSZ8841_COUNT_MAC_PORTS) );

        }
        else if (EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8842 == uDeviceIdIn)
        {
            EPS_APP_GET_MAC_ADDR(&MacAddresses, pSysDevIn->hd_nr, (EPS_HW_EDDS_KSZ884X_COUNT_MAC_IF + EPS_HW_EDDS_KSZ8842_COUNT_MAC_PORTS) );
        }
            
        // we got the array with MAC addresses back and trace it
        for(loopCnt = 0; loopCnt < (EPS_HW_EDDS_KSZ884X_COUNT_MAC_IF + pBoardInfo->nr_of_ports); loopCnt++)
        {
                EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                                        "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x", 
                                        pSysDevIn->hd_nr, loopCnt,
                                        MacAddresses.lArrayMacAdr[loopCnt][0],
                                        MacAddresses.lArrayMacAdr[loopCnt][1],
                                        MacAddresses.lArrayMacAdr[loopCnt][2],
                                        MacAddresses.lArrayMacAdr[loopCnt][3],
                                        MacAddresses.lArrayMacAdr[loopCnt][4],
                                        MacAddresses.lArrayMacAdr[loopCnt][5]);
        }

        // we now check the array with MAC addresses if any of the needed addresses is empty
        for(loopCnt = 0; loopCnt < (EPS_HW_EDDS_KSZ884X_COUNT_MAC_IF + pBoardInfo->nr_of_ports); loopCnt++)
        {
            if(   MacAddresses.lArrayMacAdr[loopCnt][0] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][1] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][2] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][3] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][4] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][5] == 0x00
              )
            {
                EPS_FATAL("Empty MAC-Address detected");
            }
        }
        
        // set the interface MAC with the value from the array returned
        pMac = (EPS_MAC_PTR_TYPE)&MacAddresses.lArrayMacAdr[0];        
        pBoardInfo->if_mac = *pMac;

        // set the Port MAC(s) with the value(s) from the array returned
        for (i = 0; i < pBoardInfo->nr_of_ports; i++)
        {
            pMac = (EPS_MAC_PTR_TYPE)&MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_KSZ884X_COUNT_MAC_IF];
            pBoardInfo->port_mac[i + EPS_HW_EDDS_KSZ884X_COUNT_MAC_IF] = *pMac;

            // setup portmapping (=1:1)
            pBoardInfo->port_map[i+1].hw_phy_nr  = i;
            pBoardInfo->port_map[i+1].hw_port_id = i+1;
        }

        // -- MAC address handling DONE ----------------------------------

		// Prepare process image settings
		pBoardInfo->pi_mem.base_ptr = 0;
		pBoardInfo->pi_mem.phy_addr = 0;
		pBoardInfo->pi_mem.size     = 0;

		// setup HIF buffer
		pBoardInfo->hif_mem.base_ptr = 0;
		pBoardInfo->hif_mem.phy_addr = 0;
		pBoardInfo->hif_mem.size     = 0;

		// add the LL settings
		pBoardInfo->edds.is_valid          = LSA_TRUE;
		pBoardInfo->edds.ll_handle         = (LSA_VOID_PTR_TYPE)&pEddsIn->hLL;
		pBoardInfo->edds.ll_function_table = (LSA_VOID_PTR_TYPE)&pEddsIn->tLLFuncs;

		pBoardInfo->edds.bFillActive        = LSA_FALSE;
        pBoardInfo->edds.bHsyncModeActive   = LSA_FALSE;
        pBoardInfo->edds.hTimer             = 0;
        pBoardInfo->edds.hMsgqHigh          = 0;
        pBoardInfo->edds.hMsgqLow           = 0;
        pBoardInfo->edds.hMsgqActive        = 0;
        pBoardInfo->edds.hThreadHigh        = 0;
        pBoardInfo->edds.hThreadLow         = 0;

        // fill LL Params
        pEddsIn->sMicrelLLParameter.pBaseAddr       = (LSA_VOID_PTR_TYPE)pRegBaseIn; /* Register Base Address on Micrel LL */
        pEddsIn->sMicrelLLParameter.ConfiguredPorts = pBoardInfo->nr_of_ports;
        pBoardInfo->edds.ll_params                  = (LSA_VOID_PTR_TYPE*)(&pEddsIn->sMicrelLLParameter);

		// fill other Params
		pHwIn->asic_type               = EPS_PNDEV_ASIC_KSZ88XX;
		pHwIn->EpsBoardInfo.board_type = EPS_PNDEV_BOARD_KSZ88XX;
		
		// initialization of MediaType
		eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pHwIn->EpsBoardInfo, pHwIn->asic_type, pHwIn->EpsBoardInfo.board_type);

        pHwIn->IrMode = EPS_PNDEV_IR_MODE_POLL; // TODO-MSI: = EPS_PNDEV_IR_MODE_POLLINTERRUPT;
	}
	else
	{
		result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
	}

	return result;
}

LSA_VOID eps_ksz8841_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid)
{
	EPS_BOARD_INFO_TYPE	* const pboardInfo           = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;
    LSA_UINT32            const dev_nrt_mem_size_all = pboardInfo->edds.sdram_NRT.size;

    // setup DEV and NRT memory pools

    pboardInfo->dev_mem.base_ptr    = pboardInfo->edds.sdram_NRT.base_ptr;
    pboardInfo->dev_mem.phy_addr    = pboardInfo->edds.sdram_NRT.phy_addr;
    pboardInfo->dev_mem.size        = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_KSZ8841_PERCENT_DEV);

    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->dev_mem.base_ptr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->dev_mem.phy_addr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_KSZ8841_PERCENT_NRT_TX);

    pboardInfo->nrt_rx_mem.base_ptr = pboardInfo->nrt_tx_mem.base_ptr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.phy_addr = pboardInfo->nrt_tx_mem.phy_addr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_KSZ8841_PERCENT_NRT_RX);

	EPS_ASSERT(pboardInfo->dev_mem.base_ptr != LSA_NULL);
}

LSA_VOID eps_ksz8842_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid)
{
	EPS_BOARD_INFO_TYPE	* const pboardInfo           = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;
    LSA_UINT32            const dev_nrt_mem_size_all = pboardInfo->edds.sdram_NRT.size;

    // setup DEV and NRT memory pools

    pboardInfo->dev_mem.base_ptr    = pboardInfo->edds.sdram_NRT.base_ptr;
    pboardInfo->dev_mem.phy_addr    = pboardInfo->edds.sdram_NRT.phy_addr;
    pboardInfo->dev_mem.size        = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_KSZ8842_PERCENT_DEV);

    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->dev_mem.base_ptr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->dev_mem.phy_addr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_KSZ8842_PERCENT_NRT_TX);

    pboardInfo->nrt_rx_mem.base_ptr = pboardInfo->nrt_tx_mem.base_ptr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.phy_addr = pboardInfo->nrt_tx_mem.phy_addr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_KSZ8842_PERCENT_NRT_RX);

	EPS_ASSERT(pboardInfo->dev_mem.base_ptr != LSA_NULL);
}

/**
* LED Backup Mode for EDDS Micrel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_KSZ88XX_LL_LED_BACKUP_MODE(
    LSA_VOID_PTR_TYPE pLLManagement)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    KSZ88XX_LL_HANDLE_TYPE* const handle = (KSZ88XX_LL_HANDLE_TYPE*)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_KSZ88XX_LL_LED_BACKUP_MODE(): call EPS_APP_LL_LED_BACKUP_MODE");
    EPS_APP_LL_LED_BACKUP_MODE(pBoard, pLLManagement);
#else
    KSZ88XX_LL_LED_BACKUP_MODE(pLLManagement);
#endif
}

/**
* LED Restore Mode for EDDS Micrel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_KSZ88XX_LL_LED_RESTORE_MODE(
    LSA_VOID_PTR_TYPE pLLManagement)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    KSZ88XX_LL_HANDLE_TYPE* const handle = (KSZ88XX_LL_HANDLE_TYPE*)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_KSZ88XX_LL_LED_RESTORE_MODE(): call EPS_APP_LL_LED_RESTORE_MODE");
    EPS_APP_LL_LED_RESTORE_MODE(pBoard, pLLManagement);
#else
    KSZ88XX_LL_LED_RESTORE_MODE(pLLManagement);
#endif
}

/**
* LED Set Mode for EDDS Micrel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_KSZ88XX_LL_LED_SET_MODE(
    LSA_VOID_PTR_TYPE pLLManagement,
    LSA_BOOL LEDOn)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    KSZ88XX_LL_HANDLE_TYPE* const handle = (KSZ88XX_LL_HANDLE_TYPE*)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_KSZ88XX_LL_LED_SET_MODE(): call EPS_APP_LL_LED_SET_MODE");
    EPS_APP_LL_LED_SET_MODE(pBoard, pLLManagement, LEDOn);
#else
    KSZ88XX_LL_LED_SET_MODE(pLLManagement, LEDOn);
#endif
}
#endif // PSI_EDDS_CFG_HW_KSZ88XX

#if defined (PSI_EDDS_CFG_HW_TI)
/* - Prototypes ----------------------------------------------------------------------------------- */
static LSA_VOID EPS_TI_LL_LED_BACKUP_MODE(LSA_VOID_PTR_TYPE pLLManagement);
static LSA_VOID EPS_TI_LL_LED_RESTORE_MODE(LSA_VOID_PTR_TYPE pLLManagement);
static LSA_VOID EPS_TI_LL_LED_SET_MODE(LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL LEDOn);

/**
 * TI LL open device for EDDS
 *
 * @param pEddsIn 				pointer to edds parameter structure
 * @param pHwIn					pointer to structure with hardware configuration
 * @param pSysDevIn				ptr to sys device in
 * @param pRegBaseIn			ptr to BaseRegister
 * @param hd_idIn				index of hd
 * @param uVendorIdIn
 * @param uDeviceIdIn
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_tiLL_open(EPS_EDDS_TI_LL_PTR_TYPE   const pEddsIn,
					     EPS_PNDEV_HW_PTR_TYPE	   const pHwIn,
						 EPS_SYS_PTR_TYPE		   const pSysDevIn,
						 LSA_UINT8               * const pRegBaseIn,
						 LSA_UINT16 			   const hd_idIn,
						 LSA_UINT16				   const uVendorIdIn,
						 LSA_UINT16				   const uDeviceIdIn,
						 EPS_MAC_PTR_TYPE          const pPndevdrvArrayMacAdr)
{
	EDDS_LL_TABLE_TYPE      * pLLTable;
	LSA_UINT16				  result = EPS_PNDEV_RET_OK;
	EPS_BOARD_INFO_PTR_TYPE   pBoardInfo;
	EPS_MAC_PTR_TYPE          pMac;
	LSA_UINT32                portIdx;
	LSA_UINT16                i;

    EPS_APP_MAC_ADDR_ARR      MacAddresses;
    LSA_UINT16                loopCnt;

	LSA_UNUSED_ARG(uVendorIdIn);
	LSA_UNUSED_ARG(pPndevdrvArrayMacAdr);

	EPS_ASSERT(pEddsIn != LSA_NULL);
	EPS_ASSERT(pHwIn != LSA_NULL);
	EPS_ASSERT(pSysDevIn != LSA_NULL);

    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_tiLL_open(): pRegBaseAddrGMAC=0x%x, hd_id=%u", pRegBaseIn, hd_idIn);

    if (pRegBaseIn != LSA_NULL)
	{
		/* Fill EDDS LL */

		pLLTable = &pEddsIn->tLLFuncs;

		/* EDDS init/setup/shutdown/close functions */
		pLLTable->open                      = TI_LL_OPEN;
		pLLTable->setup                     = TI_LL_SETUP;
		pLLTable->shutdown                  = TI_LL_SHUTDOWN;
		pLLTable->close                     = TI_LL_CLOSE;

		/* Transmit buffer handling functions */
		pLLTable->enqueueSendBuffer         = TI_LL_SEND;
		pLLTable->getNextFinishedSendBuffer = TI_LL_SEND_STS;
		pLLTable->triggerSend               = TI_LL_SEND_TRIGGER;

		/* Receive buffer handling functions */
		pLLTable->getNextReceivedBuffer     = TI_LL_RECV;
		pLLTable->provideReceiveBuffer      = TI_LL_RECV_PROVIDE;
		pLLTable->triggerReceive            = TI_LL_RECV_TRIGGER;

		/* Information functions */
		pLLTable->getLinkState              = TI_LL_GET_LINK_STATE;
		pLLTable->getStatistics             = TI_LL_GET_STATS;

		/* MAC address management functions */
		pLLTable->enableMC                  = TI_LL_MC_ENABLE;
		pLLTable->disableMC                 = TI_LL_MC_DISABLE;

		/* Scheduler function */
        pLLTable->recurringTask             = TI_LL_RECURRING_TASK;

		/* Control functions */
		pLLTable->setLinkState              = TI_LL_SET_LINK_STATE;
        pLLTable->backupLocationLEDs        = EPS_TI_LL_LED_BACKUP_MODE;
		pLLTable->restoreLocationLEDs       = EPS_TI_LL_LED_RESTORE_MODE; 
		pLLTable->setLocationLEDs           = EPS_TI_LL_LED_SET_MODE;

		/* Switch functions */
		pLLTable->setSwitchPortState        = LSA_NULL;
		pLLTable->controlSwitchMulticastFwd = LSA_NULL;
		pLLTable->flushSwitchFilteringDB    = LSA_NULL;

		pLLTable->changePort				= TI_LL_SWITCH_CHANGE_PORT_STATE;

	    // EDDS set arp filter function
		#ifdef LLIF_CFG_USE_LL_ARP_FILTER
		pLLTable->setArpFilter              = LSA_NULL;
		#endif

		// Fill Device Info
		pBoardInfo = &pHwIn->EpsBoardInfo;
		eps_memset(pBoardInfo,0, sizeof(*pBoardInfo));

		pSysDevIn->hd_nr       = hd_idIn;
		pSysDevIn->pnio_if_nr  = 0;	// don't care
		pSysDevIn->edd_comp_id = LSA_COMP_ID_EDDS;

		// Common HD settings
		pBoardInfo->edd_type      = LSA_COMP_ID_EDDS;
		pBoardInfo->hd_sys_handle = pSysDevIn;

        // -- MAC address handling START ----------------------------------

        // prepare the MAC-array
        eps_memset(&MacAddresses, 0, sizeof(MacAddresses));
        
        // Interface part
        // First we get the MAC address from the hardware
        // fill in the MACs to the array
        for(loopCnt = 0; loopCnt < (EPS_HW_EDDS_TI_COUNT_MAC_IF + EPS_HW_EDDS_TI_COUNT_MAC_PORTS); loopCnt++)
        {
            pMac = pPndevdrvArrayMacAdr + loopCnt;
            
            MacAddresses.lArrayMacAdr[loopCnt][0] = pMac->mac[0];
            MacAddresses.lArrayMacAdr[loopCnt][1] = pMac->mac[1];
            MacAddresses.lArrayMacAdr[loopCnt][2] = pMac->mac[2];
            MacAddresses.lArrayMacAdr[loopCnt][3] = pMac->mac[3];
            MacAddresses.lArrayMacAdr[loopCnt][4] = pMac->mac[4];
            MacAddresses.lArrayMacAdr[loopCnt][5] = pMac->mac[5];
        }

        // Port part
        // Init User to HW port mapping (used PSI GET HD PARAMS)
        eps_hw_init_board_port_param(pBoardInfo);

        // Port specific setup
        pBoardInfo->nr_of_ports = EPS_HW_EDDS_TI_COUNT_MAC_PORTS;

        // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
        EPS_APP_GET_MAC_ADDR(&MacAddresses, pSysDevIn->hd_nr, (EPS_HW_EDDS_TI_COUNT_MAC_IF + EPS_HW_EDDS_TI_COUNT_MAC_PORTS) );
        
        // we got the array with MAC addresses back and trace it
        for(loopCnt = 0; loopCnt < (EPS_HW_EDDS_TI_COUNT_MAC_IF + EPS_HW_EDDS_TI_COUNT_MAC_PORTS); loopCnt++)
        {
            EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                                    "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x", 
                                    pSysDevIn->hd_nr, loopCnt,
                                    MacAddresses.lArrayMacAdr[loopCnt][0],
                                    MacAddresses.lArrayMacAdr[loopCnt][1],
                                    MacAddresses.lArrayMacAdr[loopCnt][2],
                                    MacAddresses.lArrayMacAdr[loopCnt][3],
                                    MacAddresses.lArrayMacAdr[loopCnt][4],
                                    MacAddresses.lArrayMacAdr[loopCnt][5]);
        }
        
        // we now check the array with MAC addresses if any of the needed addresses is empty
        for(loopCnt = 0; loopCnt < (EPS_HW_EDDS_TI_COUNT_MAC_IF + EPS_HW_EDDS_TI_COUNT_MAC_PORTS); loopCnt++)
        {
            if(   MacAddresses.lArrayMacAdr[loopCnt][0] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][1] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][2] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][3] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][4] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][5] == 0x00
              )
            {
                EPS_FATAL("Empty MAC-Address detected");
            }
        }

        // set the interface MAC with the value from the array returned
        pMac = (EPS_MAC_PTR_TYPE)&MacAddresses.lArrayMacAdr[0];        
        pBoardInfo->if_mac = *pMac;

        // set the Port MAC(s) with the value(s) from the array returned
        for (i = 0; i < pBoardInfo->nr_of_ports; i++)
        {
            pMac = (EPS_MAC_PTR_TYPE)&MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_TI_COUNT_MAC_IF];        
            pBoardInfo->port_mac[i + EPS_HW_EDDS_TI_COUNT_MAC_IF] = *pMac;
            
            // setup portmapping (=1:1)
            pBoardInfo->port_map[i+1].hw_phy_nr  = i;
            pBoardInfo->port_map[i+1].hw_port_id = i+1;
        }

        // -- MAC address handling DONE ----------------------------------

		// Prepare process image settings
		pBoardInfo->pi_mem.base_ptr = 0;
		pBoardInfo->pi_mem.phy_addr = 0;
		pBoardInfo->pi_mem.size     = 0;

		// setup HIF buffer
		pBoardInfo->hif_mem.base_ptr = 0;
		pBoardInfo->hif_mem.phy_addr = 0;
		pBoardInfo->hif_mem.size     = 0;

		// add the LL settings
		pBoardInfo->edds.is_valid          = LSA_TRUE;
		pBoardInfo->edds.ll_handle         = (LSA_VOID_PTR_TYPE)&pEddsIn->hLL;
		pBoardInfo->edds.ll_function_table = (LSA_VOID_PTR_TYPE)&pEddsIn->tLLFuncs;

		pBoardInfo->edds.bFillActive        = LSA_FALSE;
        pBoardInfo->edds.bHsyncModeActive   = LSA_FALSE;
        pBoardInfo->edds.hTimer             = 0;
        pBoardInfo->edds.hMsgqHigh          = 0;
        pBoardInfo->edds.hMsgqLow           = 0;
        pBoardInfo->edds.hMsgqActive        = 0;
        pBoardInfo->edds.hThreadHigh        = 0;
        pBoardInfo->edds.hThreadLow         = 0;

		// fill LL Params
		pEddsIn->sTiLLParameter.pRegBaseAddrGMAC                        = (LSA_VOID_PTR_TYPE)pRegBaseIn; /* Register Base Address on TI LL */
		pEddsIn->sTiLLParameter.DeviceID                                = (LSA_UINT16)uDeviceIdIn;
        pEddsIn->sTiLLParameter.ConfiguredPorts                         = pBoardInfo->nr_of_ports;
		pEddsIn->sTiLLParameter.Disable1000MBitSupport[0]               = LSA_FALSE;
		pEddsIn->sTiLLParameter.Disable1000MBitSupport[1]               = LSA_FALSE;
        // port mapping
        pEddsIn->sTiLLParameter.PortMap[0].PortID_x_to_HardwarePort_y    = 1;		
        pEddsIn->sTiLLParameter.PortMap[0].PhyIndex_x_to_HardwarePort_y  = 2;
        pEddsIn->sTiLLParameter.PortMap[0].PortID_x_to_MDIO_USERACCESS   = TI_MDIO_USERACCESS0; 
        pEddsIn->sTiLLParameter.PortMap[0].TXCDelay_50ps                 = TI_RX_TX_DELAY_VALUE_0PS;
        pEddsIn->sTiLLParameter.PortMap[0].RXCDelay_50ps                 = TI_RX_TX_DELAY_VALUE_1900PS;
        pEddsIn->sTiLLParameter.PortMap[1].PortID_x_to_HardwarePort_y    = 2;
        pEddsIn->sTiLLParameter.PortMap[1].PhyIndex_x_to_HardwarePort_y  = 3;
        pEddsIn->sTiLLParameter.PortMap[1].PortID_x_to_MDIO_USERACCESS   = TI_MDIO_USERACCESS1;
        pEddsIn->sTiLLParameter.PortMap[1].TXCDelay_50ps                 = TI_RX_TX_DELAY_VALUE_0PS;
        pEddsIn->sTiLLParameter.PortMap[1].RXCDelay_50ps                 = TI_RX_TX_DELAY_VALUE_1900PS;

        // preset rest of port mapping and GBit supp
        for (portIdx = 2; portIdx < EDDS_MAX_PORT_CNT; portIdx++)
        {
            pEddsIn->sTiLLParameter.PortMap[portIdx].PortID_x_to_HardwarePort_y   = TI_GMAC_PORT_NOT_CONNECTED;
            pEddsIn->sTiLLParameter.PortMap[portIdx].PhyIndex_x_to_HardwarePort_y = TI_GMAC_PORT_NOT_CONNECTED; 
            pEddsIn->sTiLLParameter.PortMap[portIdx].PortID_x_to_MDIO_USERACCESS  = 0;
            pEddsIn->sTiLLParameter.PortMap[portIdx].TXCDelay_50ps                = TI_RX_TX_DELAY_VALUE_0PS;
            pEddsIn->sTiLLParameter.PortMap[portIdx].RXCDelay_50ps                = TI_RX_TX_DELAY_VALUE_0PS;
            pEddsIn->sTiLLParameter.Disable1000MBitSupport[portIdx]               = LSA_FALSE;
        }
	    
	    // preset PHY-Functions
	    pEddsIn->sTiLLParameter.FuncPtrs.EDDS_PHY_Init                  = EDDS_PHY_BCM5461S_Init;
        pEddsIn->sTiLLParameter.FuncPtrs.EDDS_PHY_SetPowerDown          = EDDS_PHY_BCM5461S_SetPowerDown;
        pEddsIn->sTiLLParameter.FuncPtrs.EDDS_PHY_LED_SetMode           = EDDS_PHY_BCM5461S_LED_SetMode;
        pEddsIn->sTiLLParameter.FuncPtrs.EDDS_PHY_LED_BackupMode        = EDDS_PHY_BCM5461S_LED_BackupMode;
        pEddsIn->sTiLLParameter.FuncPtrs.EDDS_PHY_RecurringTask         = EDDS_PHY_BCM5461S_RecurringTask;
	    
		pBoardInfo->edds.ll_params                                      = (LSA_VOID*)(&pEddsIn->sTiLLParameter);

		// fill other Params
		pHwIn->asic_type               = EPS_PNDEV_ASIC_TI_XX;
		pHwIn->EpsBoardInfo.board_type = EPS_PNDEV_BOARD_TI_XX;

		// initialization of MediaType
		eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pHwIn->EpsBoardInfo, pHwIn->asic_type, pHwIn->EpsBoardInfo.board_type);

		pHwIn->IrMode = EPS_PNDEV_IR_MODE_POLL; // TODO-MSI: = EPS_PNDEV_IR_MODE_POLLINTERRUPT;
	}
	else
	{
		result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
	}

	return result;
}

LSA_VOID eps_am5728_gmac_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid)
{
	EPS_BOARD_INFO_TYPE	* const pboardInfo           = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;
    LSA_UINT32            const dev_nrt_mem_size_all = pboardInfo->edds.sdram_NRT.size;

    // setup DEV and NRT memory pools

    pboardInfo->dev_mem.base_ptr    = pboardInfo->edds.sdram_NRT.base_ptr;
    pboardInfo->dev_mem.phy_addr    = pboardInfo->edds.sdram_NRT.phy_addr;
    pboardInfo->dev_mem.size        = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_AM5728_GMAC_PERCENT_DEV);

    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->dev_mem.base_ptr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->dev_mem.phy_addr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_AM5728_GMAC_PERCENT_NRT_TX);

    pboardInfo->nrt_rx_mem.base_ptr = pboardInfo->nrt_tx_mem.base_ptr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.phy_addr = pboardInfo->nrt_tx_mem.phy_addr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_AM5728_GMAC_PERCENT_NRT_RX);

	EPS_ASSERT(pboardInfo->dev_mem.base_ptr != LSA_NULL);
}

/**
* LED Backup Mode for EDDS Micrel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_TI_LL_LED_BACKUP_MODE(
    LSA_VOID_PTR_TYPE pLLManagement)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    TI_LL_HANDLE_PTR_TYPE const handle = (TI_LL_HANDLE_PTR_TYPE)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_TI_LL_LED_BACKUP_MODE(): call EPS_APP_LL_LED_BACKUP_MODE");
    EPS_APP_LL_LED_BACKUP_MODE(pBoard, pLLManagement);
#else
    TI_LL_LED_BACKUP_MODE(pLLManagement);
#endif
}

/**
* LED Restore Mode for EDDS Micrel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_TI_LL_LED_RESTORE_MODE(
    LSA_VOID_PTR_TYPE pLLManagement)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    TI_LL_HANDLE_PTR_TYPE const handle = (TI_LL_HANDLE_PTR_TYPE)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_TI_LL_LED_RESTORE_MODE(): call EPS_APP_LL_LED_RESTORE_MODE");
    EPS_APP_LL_LED_RESTORE_MODE(pBoard, pLLManagement);
#else
    TI_LL_LED_RESTORE_MODE(pLLManagement);
#endif
}

/**
* LED Set Mode for EDDS Micrel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_TI_LL_LED_SET_MODE(
    LSA_VOID_PTR_TYPE pLLManagement,
    LSA_BOOL LEDOn)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    TI_LL_HANDLE_PTR_TYPE const handle = (TI_LL_HANDLE_PTR_TYPE)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_TI_LL_LED_SET_MODE(): call EPS_APP_LL_LED_SET_MODE");
    EPS_APP_LL_LED_SET_MODE(pBoard, pLLManagement, LEDOn);
#else
    TI_LL_LED_SET_MODE(pLLManagement, LEDOn);
#endif
}
#endif // PSI_EDDS_CFG_HW_TI
#if defined (PSI_EDDS_CFG_HW_IOT2000)

// Partitioning of the DEV and NRT memory for I210, all values in percent. Values are the same for i210 and i210IS
#define CP_MEM_IOT2000_PERCENT_DEV             10
#define CP_MEM_IOT2000_PERCENT_NRT_TX          35
#define CP_MEM_IOT2000_PERCENT_NRT_RX          55

#define CP_MEM_IOT2000_PERCENT_TOTAL (CP_MEM_IOT2000_PERCENT_DEV + CP_MEM_IOT2000_PERCENT_NRT_TX + CP_MEM_IOT2000_PERCENT_NRT_RX)

#if (CP_MEM_IOT2000_PERCENT_TOTAL != 100)
#error "Sum has to be 100 percent!"
#endif



/* - Prototypes ----------------------------------------------------------------------------------- */
static LSA_VOID EPS_IX1000_LL_LED_BACKUP_MODE(LSA_VOID_PTR_TYPE pLLManagement);
static LSA_VOID EPS_IX1000_LL_LED_RESTORE_MODE(LSA_VOID_PTR_TYPE pLLManagement);
static LSA_VOID EPS_IX1000_LL_LED_SET_MODE(LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL LEDOn);

LSA_VOID eps_iot2000_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid)
{
    EPS_BOARD_INFO_TYPE   * const pboardInfo = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;
    LSA_UINT32                const dev_nrt_mem_size_all = pboardInfo->edds.sdram_NRT.size;

    // setup DEV and NRT memory pools

    pboardInfo->dev_mem.base_ptr = pboardInfo->edds.sdram_NRT.base_ptr;
    pboardInfo->dev_mem.phy_addr = pboardInfo->edds.sdram_NRT.phy_addr;
    pboardInfo->dev_mem.size = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_IOT2000_PERCENT_DEV);

    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->dev_mem.base_ptr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->dev_mem.phy_addr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.size = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_IOT2000_PERCENT_NRT_TX);

    pboardInfo->nrt_rx_mem.base_ptr = pboardInfo->nrt_tx_mem.base_ptr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.phy_addr = pboardInfo->nrt_tx_mem.phy_addr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.size = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_IOT2000_PERCENT_NRT_RX);

    EPS_ASSERT(pboardInfo->dev_mem.base_ptr != LSA_NULL);
}


/**
* intel quark iot2000 open device for edds
*
* @param pEddsIn 				pointer to edds parameter structure
* @param pHwIn					pointer to strucute with hardware configuration
* @param pSysDevIn				ptr to sys device in
* @param pRegBaseIn			ptr to BaseRegister
* @param hd_idIn				index of hd
* @param uVendorIdIn
* @param uDeviceIdIn
* @return 	EPS_PNDEV_RET_OK
*/
LSA_UINT16 eps_iot2000LL_open(EPS_EDDS_IOT2000_LL_PTR_TYPE  pEddsIn,
                              EPS_PNDEV_HW_PTR_TYPE         pHwIn,
                              EPS_SYS_PTR_TYPE			    pSysDevIn,							
                              LSA_UINT8* 					pRegBaseIn,
                              LSA_UINT16 				    hd_idIn,
                              LSA_UINT16					uVendorIdIn,
                              LSA_UINT16					uDeviceIdIn,
                              EPS_MAC_PTR_TYPE              pPndevdrvArrayMacAdr)
{
    EDDS_LL_TABLE_TYPE        * pLLTable;
	LSA_UINT16				    result = EPS_PNDEV_RET_OK;
	EPS_BOARD_INFO_PTR_TYPE     pBoardInfo;
    LSA_UINT16                  uPortMacEnd;
    LSA_UINT16				    i;
    
    EPS_APP_MAC_ADDR_ARR        MacAddresses;
    LSA_UINT16                  loopCnt = 0;
    EPS_MAC_PTR_TYPE            pMac = LSA_NULL;

    LSA_UNUSED_ARG(uVendorIdIn);
    LSA_UNUSED_ARG(uDeviceIdIn);

    EPS_ASSERT(pEddsIn != LSA_NULL);
    EPS_ASSERT(pHwIn != LSA_NULL);
    EPS_ASSERT(pSysDevIn != LSA_NULL);

    if (pRegBaseIn != LSA_NULL)
    {
        /* Fill EDDS LL */
        pLLTable = &pEddsIn->tLLFuncs;

        /* EDDS init/setup/shutdown/close functions */
        pLLTable->open                      = IX1000_LL_OPEN;
        pLLTable->setup                     = IX1000_LL_SETUP;
        pLLTable->shutdown                  = IX1000_LL_SHUTDOWN;
        pLLTable->close                     = IX1000_LL_CLOSE;

        /* Transmit buffer handling functions */
        pLLTable->enqueueSendBuffer         = IX1000_LL_SEND;
        pLLTable->getNextFinishedSendBuffer = IX1000_LL_SEND_STS;
        pLLTable->triggerSend               = IX1000_LL_SEND_TRIGGER;

        /* Receive buffer handling functions */
        pLLTable->getNextReceivedBuffer     = IX1000_LL_RECV;
        pLLTable->provideReceiveBuffer      = IX1000_LL_RECV_PROVIDE;
        pLLTable->triggerReceive            = IX1000_LL_RECV_TRIGGER;

        /* Information functions */
        pLLTable->getLinkState              = IX1000_LL_GET_LINK_STATE;
        pLLTable->getStatistics             = IX1000_LL_GET_STATS;

        /* MAC address management functions */
        pLLTable->enableMC                  = IX1000_LL_MC_ENABLE;
        pLLTable->disableMC                 = IX1000_LL_MC_DISABLE;

        /* Scheduler function */
        pLLTable->recurringTask             = IX1000_LL_RECURRING_TASK;

        /* Control functions */
        pLLTable->setLinkState              = IX1000_LL_SET_LINK_STATE;
        pLLTable->backupLocationLEDs        = EPS_IX1000_LL_LED_BACKUP_MODE;
        pLLTable->restoreLocationLEDs       = EPS_IX1000_LL_LED_RESTORE_MODE;
        pLLTable->setLocationLEDs           = EPS_IX1000_LL_LED_SET_MODE;

        /* Switch functions */
        pLLTable->setSwitchPortState        = LSA_NULL;
        pLLTable->controlSwitchMulticastFwd = LSA_NULL;
        pLLTable->flushSwitchFilteringDB    = LSA_NULL;

        pLLTable->changePort                = IX1000_LL_SWITCH_CHANGE_PORT;

        // EDDS set arp filter function
        #ifdef LLIF_CFG_USE_LL_ARP_FILTER
        pLLTable->setArpFilter              = LSA_NULL;
        #endif

        /* Fill Device Info */
        pBoardInfo = &pHwIn->EpsBoardInfo;
        eps_memset(pBoardInfo,0, sizeof(*pBoardInfo));

        pSysDevIn->hd_nr       = hd_idIn;
        pSysDevIn->pnio_if_nr  = 0; // don't care
        pSysDevIn->edd_comp_id = LSA_COMP_ID_EDDS;

        // Common HD settings
        pBoardInfo->edd_type      = LSA_COMP_ID_EDDS;
        pBoardInfo->hd_sys_handle = pSysDevIn;

        // -- MAC address handling START ----------------------------------

        // prepare the MAC-array
        eps_memset(&MacAddresses, 0, sizeof(MacAddresses));

        // Interface part
        // We already got the MAC address from the PnDevDrv and we fill it in our array
        MacAddresses.lArrayMacAdr[0][0] = pPndevdrvArrayMacAdr->mac[0];
        MacAddresses.lArrayMacAdr[0][1] = pPndevdrvArrayMacAdr->mac[1];
        MacAddresses.lArrayMacAdr[0][2] = pPndevdrvArrayMacAdr->mac[2];
        MacAddresses.lArrayMacAdr[0][3] = pPndevdrvArrayMacAdr->mac[3];
        MacAddresses.lArrayMacAdr[0][4] = pPndevdrvArrayMacAdr->mac[4];
        MacAddresses.lArrayMacAdr[0][5] = pPndevdrvArrayMacAdr->mac[5];

        // Port part
        // Init User to HW port mapping (used PSI GET HD PARAMS)
        eps_hw_init_board_port_param(pBoardInfo);

        // Port specific setup
        pBoardInfo->nr_of_ports = EPS_HW_EDDS_IOT2000_COUNT_MAC_PORTS;

        // preset Port MACs (Ersatz Mac Adressen laut SPH PDEV bei Standard Hw)
        for (i = 0; i < pBoardInfo->nr_of_ports; i++)
        {
            uPortMacEnd = 0x3840 + i;

            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_IOT2000_COUNT_MAC_IF][0] = 0x08;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_IOT2000_COUNT_MAC_IF][1] = 0x00;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_IOT2000_COUNT_MAC_IF][2] = 0x06;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_IOT2000_COUNT_MAC_IF][3] = 0x9D;
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_IOT2000_COUNT_MAC_IF][4] = (LSA_UINT8)(uPortMacEnd>>8);
            MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_IOT2000_COUNT_MAC_IF][5] = (LSA_UINT8)(uPortMacEnd&0xff);
        }

        // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
        EPS_APP_GET_MAC_ADDR(&MacAddresses, pSysDevIn->hd_nr, (EPS_HW_EDDS_IOT2000_COUNT_MAC_IF + EPS_HW_EDDS_IOT2000_COUNT_MAC_PORTS) );

        // we got the array with MAC addresses back and trace it
        for(loopCnt = 0; loopCnt < (EPS_HW_EDDS_IOT2000_COUNT_MAC_IF + EPS_HW_EDDS_IOT2000_COUNT_MAC_PORTS); loopCnt++)
        {
            EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH,
                                    "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x",
                                    pSysDevIn->hd_nr, loopCnt,
                                    MacAddresses.lArrayMacAdr[loopCnt][0],
                                    MacAddresses.lArrayMacAdr[loopCnt][1],
                                    MacAddresses.lArrayMacAdr[loopCnt][2],
                                    MacAddresses.lArrayMacAdr[loopCnt][3],
                                    MacAddresses.lArrayMacAdr[loopCnt][4],
                                    MacAddresses.lArrayMacAdr[loopCnt][5]);
        }

        // we now check the array with MAC addresses if any of the needed addresses is empty
        for(loopCnt = 0; loopCnt < (EPS_HW_EDDS_IOT2000_COUNT_MAC_IF + EPS_HW_EDDS_IOT2000_COUNT_MAC_PORTS); loopCnt++)
        {
            if(   MacAddresses.lArrayMacAdr[loopCnt][0] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][1] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][2] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][3] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][4] == 0x00
               && MacAddresses.lArrayMacAdr[loopCnt][5] == 0x00
              )
            {
                EPS_FATAL("Empty MAC-Address detected");
            }
        }

        // set the interface MAC with the value from the array returned
        pMac = (EPS_MAC_PTR_TYPE)&MacAddresses.lArrayMacAdr[0];
        pBoardInfo->if_mac = *pMac;

        // set the Port MAC(s) with the value(s) from the array returned
        for (i = 0; i < pBoardInfo->nr_of_ports; i++)
        {
            pMac = (EPS_MAC_PTR_TYPE)&MacAddresses.lArrayMacAdr[i + EPS_HW_EDDS_IOT2000_COUNT_MAC_IF];
            pBoardInfo->port_mac[i + EPS_HW_EDDS_IOT2000_COUNT_MAC_IF] = *pMac;

            // setup portmapping (=1:1)
            pBoardInfo->port_map[i+1].hw_phy_nr  = i;
            pBoardInfo->port_map[i+1].hw_port_id = i+1;
        }

        // -- MAC address handling DONE ----------------------------------
        
        // Prepare process image settings (local mem is used)
        pBoardInfo->pi_mem.base_ptr = 0;
        pBoardInfo->pi_mem.phy_addr = 0;
        pBoardInfo->pi_mem.size     = 0;

        // setup HIF buffer
        pBoardInfo->hif_mem.base_ptr = 0;
        pBoardInfo->hif_mem.phy_addr = 0;
        pBoardInfo->hif_mem.size     = 0;

        // add the LL settings
        pBoardInfo->edds.is_valid          = LSA_TRUE;
        pBoardInfo->edds.ll_handle         = (LSA_VOID_PTR_TYPE)&pEddsIn->hLL;
        pBoardInfo->edds.ll_function_table = (LSA_VOID_PTR_TYPE)&pEddsIn->tLLFuncs;

        pBoardInfo->edds.bFillActive        = LSA_FALSE;
        pBoardInfo->edds.bHsyncModeActive   = LSA_FALSE;
        pBoardInfo->edds.hTimer             = 0;
        pBoardInfo->edds.hMsgqHigh          = 0;
        pBoardInfo->edds.hMsgqLow           = 0;
        pBoardInfo->edds.hMsgqActive        = 0;
        pBoardInfo->edds.hThreadHigh        = 0;
        pBoardInfo->edds.hThreadLow         = 0;

        pEddsIn->sIX1000LLParameter.pRegBaseAddr_BAR0   = (LSA_VOID*)pRegBaseIn; /* Register Base Address on IX1000 LL */
        //pEddsIn->sIX1000LLParameter.DeviceID            = (LSA_UINT16)uDeviceIdIn;
        //pEddsIn->sIX1000LLParameter.VendorID            = (LSA_UINT16)uVendorIdIn;
        pEddsIn->sIX1000LLParameter.Aligment            = 32;
        //pEddsIn->sIX1000LLParameter.Granularity         = 32;
        pBoardInfo->edds.ll_params                      = (LSA_VOID*)(&pEddsIn->sIX1000LLParameter);

        /* Fill other Params */
        pHwIn->asic_type               = EPS_PNDEV_ASIC_INTEL_XX;
        pHwIn->EpsBoardInfo.board_type = EPS_PNDEV_BOARD_INTEL_XX;

        pHwIn->IrMode = EPS_PNDEV_IR_MODE_POLL;
    }
    else
    {
        result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
    }

	return result;
}

/**
* LED Backup Mode for EDDS Intel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_IX1000_LL_LED_BACKUP_MODE(
    LSA_VOID_PTR_TYPE pLLManagement)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    IX1000_LL_HANDLE_TYPE* const handle = (IX1000_LL_HANDLE_TYPE*)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_IX1000_LL_LED_BACKUP_MODE(): call EPS_APP_LL_LED_BACKUP_MODE");
    EPS_APP_LL_LED_BACKUP_MODE(pBoard, pLLManagement);
#else
    IX1000_LL_LED_BACKUP_MODE(pLLManagement);
#endif
}

/**
* LED Restore Mode for EDDS Intel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_IX1000_LL_LED_RESTORE_MODE(
    LSA_VOID_PTR_TYPE pLLManagement)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    IX1000_LL_HANDLE_TYPE* const handle = (IX1000_LL_HANDLE_TYPE*)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_IX1000_LL_LED_RESTORE_MODE(): call EPS_APP_LL_LED_RESTORE_MODE");
    EPS_APP_LL_LED_RESTORE_MODE(pBoard, pLLManagement);
#else
    IX1000_LL_LED_RESTORE_MODE(pLLManagement);
#endif
}

/**
* LED Set Mode for EDDS Intel cards
*
* @param [in] pLLManagement
*/
static LSA_VOID EPS_IX1000_LL_LED_SET_MODE(
    LSA_VOID_PTR_TYPE pLLManagement,
    LSA_BOOL LEDOn)
{
#ifdef EPS_CFG_USE_EXTERNAL_LED
    IX1000_LL_HANDLE_TYPE* const handle = (IX1000_LL_HANDLE_TYPE*)pLLManagement;
    PSI_EDD_SYS_TYPE* const pSysPtr = (PSI_EDD_SYS_PTR_TYPE)handle->hSysDev;

    EPS_ASSERT(pSysPtr != LSA_NULL);
    EPS_ASSERT(pSysPtr->edd_comp_id == LSA_COMP_ID_EDDS);
    EPS_ASSERT((pSysPtr->hd_nr) && (pSysPtr->hd_nr <= PSI_CFG_MAX_IF_CNT));

    EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store(pSysPtr->hd_nr);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "EPS_IX1000_LL_LED_SET_MODE(): call EPS_APP_LL_LED_SET_MODE");
    EPS_APP_LL_LED_SET_MODE(pBoard, pLLManagement, LEDOn);
#else
    IX1000_LL_LED_SET_MODE(pLLManagement, LEDOn);
#endif
}
#endif // #if defined (PSI_EDDS_CFG_HW_IOT2000)

#endif // #if (PSI_CFG_USE_EDDS == 1)

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

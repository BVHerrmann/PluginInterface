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
/*  F i l e               &F: ix1000_edd.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Low Level functions for EDDS (IX1000)            */
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
/* trace module data                                                         */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID  22
#define EDDS_MODULE_ID     LTRC_ACT_MODUL_ID  /* Ids 16..31 reserved for LL */

/**
 * \addtogroup IX1000
 * \ingroup LLIF
 * @{
 */

/*===========================================================================*/
/* includes                                                                  */
/*===========================================================================*/
#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"           /* internal header  */
#include "ix1000_inc.h"
#include "ix1000_reg.h"

/*===========================================================================*/
/* helper functions                                                          */
/*===========================================================================*/

/** \brief      Setup MDI control for reading PHY register.
 *
 * \details     The MDI control register of MAC is programmed for PHY register read.
 *              After this functions is called, #ix1000_endReadPhyReg could be called.
 *
 * \param       handle      Handle of EDDS interface (hardware instance).
 * \param       PhyRegAddr  PHY register address for reading.
 */
static LSA_VOID ix1000_beginReadPhyReg(
        IX1000_LL_HANDLE_TYPE const * const pHandle,
        LSA_UINT8 PhyRegAddr
        )
{
    LSA_UINT32 gmac_reg_4;

    /* setup phy for read */
    gmac_reg_4 = IX1000_BITS(IX1000_GMAC_REG_4_PA_s, IX1000_GMAC_REG_4_PA_e, pHandle->phyID)/* phy address */
               | IX1000_BITS(IX1000_GMAC_REG_4_GR_s, IX1000_GMAC_REG_4_GR_e, PhyRegAddr)    /* phy register address */
               | IX1000_BITS(IX1000_GMAC_REG_4_CR_s, IX1000_GMAC_REG_4_CR_e, IX1000_CR)     /* phy clöock range */
               /*  | IX1000_BIT(IX1000_GMAC_REG_4_GW,0)                                        read operation */
               | IX1000_BIT(IX1000_GMAC_REG_4_GB,                            1);            /* set busy bit */

    /* write command */
    IX1000_reg32(IX1000_GMAC_REG_4) = gmac_reg_4;
}


/** \brief      Setup MDI control for writing PHY register.
 *
 * \details     The MDI control register of MAC is programmed for PHY register write.
 *              After this functions is called, #ix1000_WritePhyReg could be called.
 *
 * \param       handle      handle of EDDS interface (hardware instance).
 * \param       PhyRegAddr  PHY register address for reading
 * \param[in]   data        Data to hand over to the PHY register.
 */
static LSA_VOID ix1000_beginWritePhyReg(
        IX1000_LL_HANDLE_TYPE const * const pHandle,
        LSA_UINT8 PhyRegAddr,
        LSA_UINT16 data
        )
{
    LSA_UINT32 gmac_reg_4;

    /* setup phy data for write */
    IX1000_reg32(IX1000_GMAC_REG_5) = data;

    /* setup phy for write */
    gmac_reg_4 = IX1000_BITS(IX1000_GMAC_REG_4_PA_s, IX1000_GMAC_REG_4_PA_e, pHandle->phyID)/* phy address */
               | IX1000_BITS(IX1000_GMAC_REG_4_GR_s, IX1000_GMAC_REG_4_GR_e, PhyRegAddr)    /* phy register address */
               | IX1000_BITS(IX1000_GMAC_REG_4_CR_s, IX1000_GMAC_REG_4_CR_e, IX1000_CR)     /* phy clöock range */
               | IX1000_BIT(IX1000_GMAC_REG_4_GW,                            1)             /* write operation */
               | IX1000_BIT(IX1000_GMAC_REG_4_GB,                            1);            /* set busy bit */

    /* write command */
    IX1000_reg32(IX1000_GMAC_REG_4) = gmac_reg_4;
}



/** \brief      Finish read/write of data from/to PHY register.
 *
 * \details     This function waits until to read/write of data from/to PHY register is finished.
 *              The address of the PHY register was setup with #ix1000_beginReadPhyReg or #ix1000_beginWritePhyReg.
 *
 * \param       handle      handle of EDDS interface (hardware instance).
 * \param[out]  ref_data    reference to a data storage for read data. If a NULL reference (NULL-pointer)
 *                          is hand over, no data is transfered to caller.
 *
 * \return      LSA_TRUE, if transfer was successfully finished.
 *              ref_data has PHY data that was read, if a read was programmed with #ix1000_beginReadPhyReg.
 *              Otherwise, ref_data has no data.
 *              LSA_FALSE, if an error (e.g. timeout) occurred; in this case ref_data has no data.
 *
 * \note        To finish a read, #ix1000_endReadPhyReg should be called. To Finish a programmed write,
 *              #ix1000_endWritePhyReg should be called. These functions are using this generalized one.
 */
static LSA_BOOL ix1000_endPhyReg(
        IX1000_LL_HANDLE_TYPE const * const pHandle,
        LSA_UINT16* ref_data)
{
    LSA_UINT32 gmac_reg_4;
    LSA_BOOL transferOK;

    transferOK = LSA_TRUE;

    gmac_reg_4 = IX1000_reg32(IX1000_GMAC_REG_4);

    if(IX1000_GETBIT(IX1000_GMAC_REG_4_GB, gmac_reg_4))
    {
        EDDS_LL_TRACE_00(pHandle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
            "ix1000_endPhyReg: PHY is not ready yet.");
        transferOK = LSA_FALSE;
    }

    if(transferOK)
    {
        if(LSA_NULL != ref_data)
        {
            LSA_UINT32 gmac_reg_5 = IX1000_reg32(IX1000_GMAC_REG_5);
            /* write data bits to ref_data (only reasonable data if a read was programmed) */
            *ref_data = IX1000_GETBITS(0,15,gmac_reg_5);
        }
    }

    return transferOK;
}

/** \brief      Finish read of data from PHY register.
 *
 * \details     This function waits until to read of data from PHY register is finished.
 *              The address of the PHY register was setup with #ix1000_beginReadPhyReg.
 *
 * \param       handle      handle of EDDS interface (hardware instance).
 * \param[out]  ref_data    reference to a data storage for read data. If a NULL reference (NULL-pointer)
 *                          is hand over, no data is transfered to caller.
 *
 * \return      LSA_TRUE, if transfer was successfully finished.
 *              ref_data has PHY data that was read.
 *              LSA_FALSE, if an error (e.g. timeout) occurred; in this case ref_data has no data.
 */
static LSA_BOOL ix1000_endReadPhyReg(
        IX1000_LL_HANDLE_TYPE const * const pHandle,
        LSA_UINT16* ref_data)
{
    return ix1000_endPhyReg(pHandle,ref_data);
}


/** \brief      Finish write of data to PHY register.
 *
 * \details     This function waits until to write of data to PHY register is finished.
 *              The address of the PHY register was setup with #ix1000_beginWritePhyReg.
 *
 * \param       handle      handle of EDDS interface (hardware instance).
 *
 * \return      LSA_TRUE, if transfer was successfully finished.
 *              LSA_FALSE, if an error (e.g. timeout) occurred.
 */
static LSA_BOOL ix1000_endWritePhyReg(
        IX1000_LL_HANDLE_TYPE const * const pHandle)
{
    return ix1000_endPhyReg(pHandle,LSA_NULL);
}


/**
 * \brief           helper function that gets data from a given phy register (synchronous call)
 *
 * \param[in|out]   pHandle     lower layer handle
 * \param[in]       reg         given phy reg
 * \param[out]      data        read data
 *
 * \retval          EDD_STS_OK          data is valid
 * \retval          EDD_STS_ERR_HW      no access possible in time (IX1000_TIMEOUT_US), no valid data read
 */
static LSA_RESULT ix1000_ReadPhyReg(
        IX1000_LL_HANDLE_TYPE const * const pHandle,
        LSA_UINT8  const                    PhyRegAddr,
        LSA_UINT16 * const                  data)
{
    LSA_RESULT Status;

    Status = EDD_STS_OK;
    /* try to access phy for a given time */
    EDDS_WAIT_UNTIL_TIMEOUT(
                    (ix1000_endPhyReg(pHandle,LSA_NULL)),
                    IX1000_TIMEOUT_US,
                    (Status = EDD_STS_ERR_HW));
    if(EDD_STS_OK == Status)
    {
        ix1000_beginReadPhyReg(pHandle, PhyRegAddr);
        EDDS_WAIT_UNTIL_TIMEOUT(
                        ix1000_endReadPhyReg(pHandle, data),
                        IX1000_TIMEOUT_US,
                        (Status = EDD_STS_ERR_HW));
    }

    return Status;
}

/**
 * \brief           helper function that writes data to a given phy register (synchronous call)
 *
 * \param[in|out]   pHandle     lower layer handle
 * \param[in]       reg         given phy reg
 * \param[in]       data        data to be written
 *
 * \retval          EDD_STS_OK          good, data written.
 * \retval          EDD_STS_ERR_HW      no access possible in time (IX1000_TIMEOUT_US), data may not been written.
 */
static LSA_RESULT ix1000_WritePhyReg(
        IX1000_LL_HANDLE_TYPE const * const pHandle,
        LSA_UINT8  const                    reg,
        LSA_UINT16 const                    data)
{
    LSA_RESULT Status;

    Status = EDD_STS_OK;
    /* try to access phy for a given time */
    EDDS_WAIT_UNTIL_TIMEOUT(
                    (ix1000_endPhyReg(pHandle,LSA_NULL)),
                    IX1000_TIMEOUT_US,
                    (Status = EDD_STS_ERR_HW));
    if(EDD_STS_OK == Status)
    {
        /* write data if phy is accessible */
        ix1000_beginWritePhyReg(pHandle, reg, data);
        EDDS_WAIT_UNTIL_TIMEOUT(
                        (ix1000_endWritePhyReg(pHandle)),
                        IX1000_TIMEOUT_US,
                        (Status = EDD_STS_ERR_HW));
    }

    return Status;
}

/**
 * \brief       helper function that initializes descriptors (tx)
 *
 * \param[in]   pHandle     lower layer handle
 *
 * \retval      void
 */
static LSA_VOID ix1000_initDescr_Tx(
        IX1000_LL_HANDLE_PTR_TYPE pHandle)
{
    LSA_UINT32  index;
    LSA_UINT32  phyAddrTx;
    LSA_UINT32  offsetTx;
    LSA_UINT32  alignment;

    alignment   = pHandle->pParams->Aligment;

    /* allocate descriptor memory (tx) */
    EDDS_ALLOC_DEV_BUFFER_MEM(pHandle->hSysDev, (EDD_UPPER_MEM_PTR_TYPE*)&pHandle->pTxDescrAlloc,
            pHandle->TxFrameBufferCount*sizeof(IX1000_TX_DSCR) + alignment);

    /* read phy address (tx) from descriptor memory (tx) */
    EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev, pHandle->pTxDescrAlloc, &phyAddrTx);

    /* create offset to descriptor memory (tx) and link it to the lower layer descriptor structure */
    offsetTx = ((phyAddrTx + (alignment - 1)) & ~(alignment - 1)) - phyAddrTx;
    pHandle->pTxDescriptors = (EDD_UPPER_MEM_PTR_TYPE)(((LSA_UINT8*)pHandle->pTxDescrAlloc) + offsetTx);

    /* initialize descriptor structures (rx) */
    for(index = 0; index < pHandle->TxFrameBufferCount; ++index)
    {
        pHandle->pTxDescriptors[index].tdes0 =
              //IX1000_BIT(IX1000_TDES0_OWN, 0)    | /* software is owner */
                IX1000_BIT(IX1000_TDES0_FS, 1) | /* no segmentation - first segment */
                IX1000_BIT(IX1000_TDES0_LS, 1) | /* no segmentation - last segment */
                IX1000_BIT(IX1000_TDES0_TCH, 1); /* second address in the descriptor points to next descriptor */
        pHandle->pTxDescriptors[index].tdes1 = 0;
        pHandle->pTxDescriptors[index].tdes2 = 0; /* buffer gets linked in LL_SEND */
        if(pHandle->TxFrameBufferCount == (index + 1))
        {
            /* last element points to the first one */
            EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev, (LSA_VOID_PTR_TYPE)&pHandle->pTxDescriptors[0],
                    (LSA_UINT32*)&pHandle->pTxDescriptors[index].tdes3);
        }
        else
        {
            EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev, (LSA_VOID_PTR_TYPE)&pHandle->pTxDescriptors[index + 1],
                    (LSA_UINT32*)&pHandle->pTxDescriptors[index].tdes3);
        }
    }
}

/**
 * \brief       helper function that initializes descriptors (rx)
 *
 * \param[in]   pHandle     lower layer handle
 *
 * \retval      void
 */
static LSA_VOID ix1000_initDescr_Rx(
        IX1000_LL_HANDLE_PTR_TYPE pHandle)
{
    LSA_UINT32  index;
    LSA_UINT32  phyAddrRx;
    LSA_UINT32  offsetRx;
    LSA_UINT32  alignment;

    alignment   = pHandle->pParams->Aligment;

    /* allocate descriptor memory (rx) */
    EDDS_ALLOC_DEV_BUFFER_MEM(pHandle->hSysDev, (EDD_UPPER_MEM_PTR_TYPE*)&pHandle->pRxDescrAlloc,
            pHandle->RxFrameBufferCount*sizeof(IX1000_RX_DSCR) + alignment);

    /* read phy address (rx) from descriptor memory (rx) */
    EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev, pHandle->pRxDescrAlloc, &phyAddrRx);

    /* create offset to descriptor memory (rx) and link it to the lower layer descriptor structure */
    offsetRx = ((phyAddrRx + (alignment - 1)) & ~(alignment - 1)) - phyAddrRx;
    pHandle->pRxDescriptors = (EDD_UPPER_MEM_PTR_TYPE)(((LSA_UINT8*)pHandle->pRxDescrAlloc) + offsetRx);

    /* allocate frame buffer pointers (rx) */
    EDDS_ALLOC_LOCAL_MEM((LSA_VOID**)(&pHandle->RxFrameBufferPtr), sizeof(pHandle->RxFrameBufferPtr[0]) * pHandle->RxFrameBufferCount);

    /* allocate frame buffers (rx) */
    for(index = 0; index < pHandle->RxFrameBufferCount; ++index)
    {
        EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(pHandle->hSysDev, (LSA_VOID**)&pHandle->RxFrameBufferPtr[index], sizeof(IX1000_RX_FRAMEBUFFER));
    }

    /* initialize descriptor structures (tx) */
    for(index = 0; index < pHandle->RxFrameBufferCount; ++index)
    {
        pHandle->pRxDescriptors[index].rdes0 = IX1000_BIT(IX1000_RDES0_OWN,1) | /* hardware is owner */
                IX1000_BIT(IX1000_RDES0_FS, 1) | /* no segmentation - first segment */
                IX1000_BIT(IX1000_RDES0_LS, 1); /* no segmentation - last segment */
        pHandle->pRxDescriptors[index].rdes1 = IX1000_BIT(IX1000_RDES1_RCH,1) | /* second address in the descriptor points to next descriptor */
                IX1000_BITS(IX1000_RDES1_RBS1_s, IX1000_RDES1_RBS1_e, sizeof(IX1000_RX_FRAMEBUFFER)); /* length: max length */
        EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev, pHandle->RxFrameBufferPtr[index], (LSA_UINT32*)&pHandle->pRxDescriptors[index].rdes2);

        /* last element points to the first one */
        if(pHandle->RxFrameBufferCount == (index + 1))
        {
            EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev, (LSA_VOID_PTR_TYPE)&pHandle->pRxDescriptors[0],
                    (LSA_UINT32*)&pHandle->pRxDescriptors[index].rdes3);
        }
        else
        {
            EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev, (LSA_VOID_PTR_TYPE)&pHandle->pRxDescriptors[index + 1],
                    (LSA_UINT32*)&pHandle->pRxDescriptors[index].rdes3);
        }
    }
}

/**
 * \brief       wrapper function that initializes descriptors
 *
 * \param[in]   pHandle     lower layer handle
 *
 * \retval      void
 */
static LSA_VOID ix1000_initDescr(
        IX1000_LL_HANDLE_PTR_TYPE pHandle)
{
    ix1000_initDescr_Tx(pHandle);
    ix1000_initDescr_Rx(pHandle);
}

/**
 * \brief       wrapper function that initializes descriptors
 *
 * \param[in]   pHandle     lower layer handle
 *

 * \retval      EDD_STS_OK              good
 * \retval      *err*                   error from system adaption, most likely EDD_STS_ERR_RESOURCE
 */
static LSA_RESULT ix1000_freeDescr(
        IX1000_LL_HANDLE_PTR_TO_CONST_TYPE  pHandle)
{
    LSA_RESULT  Status;

    Status      = EDD_STS_OK;

    /* free allocated memory (tx) */
    if(0 != (EDD_UPPER_MEM_PTR_TYPE)pHandle->pTxDescrAlloc)
    {
        EDDS_FREE_DEV_BUFFER_MEM(pHandle->hSysDev, &Status,
                (EDD_UPPER_MEM_PTR_TYPE)pHandle->pTxDescrAlloc);
    }

    /* free allocated memory (rx) */
    if((EDD_STS_OK == Status)
            && (0 != (EDD_UPPER_MEM_PTR_TYPE)pHandle->pRxDescrAlloc))
    {
        EDDS_FREE_DEV_BUFFER_MEM(pHandle->hSysDev, &Status,
                (EDD_UPPER_MEM_PTR_TYPE)pHandle->pRxDescrAlloc);
    }

    return Status;
}

/**
 * \brief       helper function that frees allocated memory
 *
 * \param[in]   pHandle     lower layer handle
 *

 * \retval      EDD_STS_OK              good
 * \retval      *err*                   error from system adaption, most likely EDD_STS_ERR_RESOURCE
 */
static LSA_RESULT ix1000_freeMemory(
        IX1000_LL_HANDLE_PTR_TO_CONST_TYPE pHandle)
{
    LSA_RESULT              Status;
    IX1000_RX_FRAMEBUFFER*  ptr;
    LSA_UINT32              count;
    LSA_UINT16              retval;

    for(count = 0; count < pHandle->RxFrameBufferCount; ++count)
    {
        ptr = pHandle->RxFrameBufferPtr[count];
        if(0 != ptr)
        {
            EDDS_FREE_RX_TRANSFER_BUFFER_MEM(pHandle->hSysDev, &retval, ptr);
        }
    }

    EDDS_FREE_LOCAL_MEM(&retval, pHandle->RxFrameBufferPtr);

    Status = ix1000_freeDescr(pHandle);

    if(EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "ix1000_freeMemory: freeing descriptors failed - Status %u",
                Status);
    }

    return Status;
}

/**
 * \brief           helper function that initializes fundamental structures
 *
 * \param[in|out]   pHandle     lower layer handle
 *
 * \retval          void
 */
static LSA_VOID ix1000_initStructures(
        IX1000_LL_HANDLE_PTR_TYPE pHandle)
{
    EDD_MAC_ADR_TYPE n_mac = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};

    /* default LinkStat is autoneg */
    pHandle->LinkSpeedModeConfigured    = EDD_LINK_AUTONEG;
    pHandle->AutonegCapsAdv             = 0;

    pHandle->LinkSM_State               = IX1000_LINK_IDLE;
    pHandle->phyData_Shadow             = 0;
    pHandle->IsSetLink                  = 0;
    pHandle->LinkData.Autoneg           = LSA_FALSE;
    pHandle->LinkData.Mode              = EDD_LINK_UNKNOWN;
    pHandle->LinkData.Speed             = EDD_LINK_UNKNOWN;
    pHandle->LinkData.Status            = EDD_LINK_DOWN;
    pHandle->LinkData_old.Autoneg       = LSA_FALSE;
    pHandle->LinkData_old.Mode          = EDD_LINK_UNKNOWN;
    pHandle->LinkData_old.Speed         = EDD_LINK_UNKNOWN;
    pHandle->LinkData_old.Status        = EDD_LINK_DOWN;
    pHandle->newLinkStat                = pHandle->LinkSpeedModeConfigured;
    pHandle->newPhyPower                = EDD_LINK_DOWN;
    pHandle->savedLinkStat              = pHandle->newLinkStat;
    pHandle->savedPhyPower              = pHandle->newPhyPower;

    /* no multicast frame will be received after being enabled with LL_MC_ENABLE */
    for(int i = 0; i < EDDS_MAX_MC_MAC_CNT; ++i)
    {
        /* 0 cannot be used as hash value default since it is a viable value (range from 0 to 63) */
        pHandle->macHashTable[i].hashValue = IX1000_err;
        pHandle->macHashTable[i].mcMacAddr = n_mac;
    }

    /* setup supported mib2 counter */
    pHandle->LL_Stats.SupportedCounters     = IX1000_MIB2_SUPPORTED_COUNTERS_PORT;
    pHandle->LL_Stats.InOctets              = 0;
    pHandle->LL_Stats.InUcastPkts           = 0;
    pHandle->LL_Stats.InNUcastPkts          = 0;
    pHandle->LL_Stats.InDiscards            = 0;
    pHandle->LL_Stats.InErrors              = 0;
    pHandle->LL_Stats.InUnknownProtos       = 0; /* not supported */
    pHandle->LL_Stats.OutOctets             = 0;
    pHandle->LL_Stats.OutUcastPkts          = 0;
    pHandle->LL_Stats.OutNUcastPkts         = 0;
    pHandle->LL_Stats.OutDiscards           = 0;
    pHandle->LL_Stats.OutErrors             = 0;
    pHandle->LL_Stats.OutQLen               = 0; /* not supported */
    pHandle->LL_Stats.InMulticastPkts       = 0;
    pHandle->LL_Stats.InBroadcastPkts       = 0;
    pHandle->LL_Stats.OutMulticastPkts      = 0;
    pHandle->LL_Stats.OutBroadcastPkts      = 0;
    pHandle->LL_Stats.InHCOctets            = 0;
    pHandle->LL_Stats.InHCUcastPkts         = 0;
    pHandle->LL_Stats.InHCMulticastPkts     = 0;
    pHandle->LL_Stats.InHCBroadcastPkts     = 0;
    pHandle->LL_Stats.OutHCOctets           = 0;
    pHandle->LL_Stats.OutHCUcastPkts        = 0;
    pHandle->LL_Stats.OutHCMulticastPkts    = 0;
    pHandle->LL_Stats.OutHCBroadcastPkts    = 0;

    /* no manually set LED configurations after setup */
    pHandle->LedBackUp = 0;

    /* setup structures for readStatistics state machine */
    pHandle->OutErr_Shadow      = 0;
    pHandle->InErr_Shadow       = 0;
    pHandle->readStatsState     = IX1000_READ_IN_STATS_INDISC;

    /* port is present by default */
    pHandle->PortStatus = EDD_PORT_PRESENT;
}

/**
 * \brief           helper function that compares the current to the previous link stat
 *
 * \param[in|out]   pHandle     lower layer handle
 *
 * \retval          LSA_TRUE, if link changed, otherwise LSA_FALSE
 */
static LSA_BOOL ix1000_DetectLinkChange(
        IX1000_LL_HANDLE_PTR_TYPE pHandle)
{
    LSA_BOOL Status = LSA_FALSE;

    /* compare the last checked link state to the current link */
    if(pHandle->LinkData_old.Autoneg    != pHandle->LinkData.Autoneg
    || pHandle->LinkData_old.Mode       != pHandle->LinkData.Mode
    || pHandle->LinkData_old.Speed      != pHandle->LinkData.Speed
    || pHandle->LinkData_old.Status     != pHandle->LinkData.Status)
    {
        pHandle->LinkData_old = pHandle->LinkData;
        Status =  LSA_TRUE;
    }

    return Status;
}


/**
 * \brief           helper function that handles the link state machine
 *
 * \param[in|out]   pHandle     lower layer handle
 * \param[in]       hwTimeSlicing   do one or all states
 *
 * \retval          LSA_RESULT type.
 */
static LSA_RESULT ix1000_handle_link_sm(
        IX1000_LL_HANDLE_PTR_TYPE pHandle, LSA_BOOL hwTimeSlicing)
{
    LSA_RESULT Status;

    Status = EDD_STS_OK;

    switch(pHandle->LinkSM_State)
    {
        /* initial state */
        case IX1000_LINK_IDLE:
        {
            pHandle->LinkSM_State = IX1000_LINK_GET_DATA;
        }
        //lint -fallthrough

        /* if access granted: get link data from phy
         * :readability/maintainability */
        case IX1000_LINK_GET_DATA:
        {
            Status = ix1000_ReadPhyReg(pHandle, MII_BMCR, &pHandle->phyData_Shadow);
            if(EDD_STS_OK != Status)
            {
                /* can be either _PENDING or _ERR_HW */
                break;
            }
            else
            {
                if(pHandle->IsSetLink)
                {
                    pHandle->LinkSM_State = IX1000_LINK_SET_DATA;
                }
                else
                {
                    pHandle->LinkSM_State = IX1000_LINK_UPDATE;
                }
            }
        }
        if(hwTimeSlicing) break;
        //lint -fallthrough

        /* if triggered and access granted: write link data to phy
         * :readability/maintainability */
        case IX1000_LINK_SET_DATA:
        {
            if(pHandle->IsSetLink)
            {
                /* set new link */
                switch(pHandle->newPhyPower)
                {
                    /* clear or set power down bit */
                    case EDDS_PHY_POWER_ON:
                    {
                        IX1000_CLRBIT(pHandle->phyData_Shadow, MII_BMCR_PDOWN);
                        break;
                    }
                    case EDDS_PHY_POWER_OFF:
                    {
                        IX1000_SETBIT(pHandle->phyData_Shadow, MII_BMCR_PDOWN);
                        break;
                    }
                    case EDDS_PHY_POWER_UNCHANGED:
                    {
                        break;
                    }
                    default:
                    {
                        Status = EDD_STS_ERR_PARAM;
                        EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                "ix1000_handle_link_sm(set): PhyPower (%u) not supported",
                                pHandle->newPhyPower);
                        break;
                    }
                }

                switch(pHandle->newLinkStat)
                {
                    /* set autoneg, link speed and link duplexity bits */
                    case EDD_LINK_AUTONEG:
                    {
                        /* speed and duplexity are 'do not care' bits for autoneg settings */
                        IX1000_SETBIT(pHandle->phyData_Shadow, MII_BMCR_ANENABLE);
                        break;
                    }
                    case EDD_LINK_UNCHANGED:
                    {
                        break;
                    }
                    default:
                    {
                        Status = EDD_STS_ERR_PARAM;
                        EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                "ix1000_handle_link_sm(set): LinkStat(%u) not supported",
                                pHandle->newLinkStat);
                        break;
                    }
                }

                /* write the actual/changed value */
                if(EDD_STS_OK == Status)
                {
                    Status = ix1000_WritePhyReg(pHandle, MII_BMCR, pHandle->phyData_Shadow);

                    /* update shadow structure if not 'unchanged' */
                    if(pHandle->newLinkStat != EDD_LINK_UNCHANGED)
                    {
                        pHandle->savedLinkStat = pHandle->newLinkStat;
                    }
                    if(pHandle->newPhyPower != EDDS_PHY_POWER_UNCHANGED)
                    {
                        pHandle->savedPhyPower = pHandle->newPhyPower;
                    }

                    /* setting link successfully executed */
                    pHandle->IsSetLink = 0;

                    pHandle->LinkSM_State = IX1000_LINK_UPDATE;
                }
                if(hwTimeSlicing) break;
            }
            else
            {
                /* update to correct state
                 * necessary for fall through behavior */
                pHandle->LinkSM_State = IX1000_LINK_UPDATE;
            }
        }
        //lint -fallthrough

        /* update the link data
         * :readability/maintainability */
        case IX1000_LINK_UPDATE:
        {
            LSA_UINT16 phyLinkStatus;
            Status = ix1000_ReadPhyReg(pHandle, 0x10, &phyLinkStatus);

            if(EDD_STS_OK == Status)
            {
                if(0 == IX1000_GETBIT(0, phyLinkStatus)) /* link not valid means link is down */
                {
                    pHandle->LinkData.Status = EDD_LINK_DOWN;
                    pHandle->LinkData.Speed = EDD_LINK_UNKNOWN;
                    pHandle->LinkData.Mode = EDD_LINK_UNKNOWN;
                }
                else
                {
                    pHandle->LinkData.Status = EDD_LINK_UP;
                    if(0 == IX1000_GETBIT(1, phyLinkStatus))
                    {
                        pHandle->LinkData.Speed = EDD_LINK_SPEED_100;
                    }
                    else
                    {
                        pHandle->LinkData.Speed = EDD_LINK_SPEED_10;
                    }

                    if(1 == IX1000_GETBIT(2, phyLinkStatus))
                    {
                        pHandle->LinkData.Mode = EDD_LINK_MODE_FULL;
                    }
                    else
                    {
                        pHandle->LinkData.Mode = EDD_LINK_MODE_HALF;
                    }

                    if(1 == IX1000_GETBIT(MII_BMCR_ANENABLE, pHandle->phyData_Shadow))
                    {
                        if(0 == IX1000_GETBIT(4, phyLinkStatus))
                        {

                            Status = EDD_STS_OK_PENDING;
                            break;
                        }

                        pHandle->LinkData.Autoneg = LSA_TRUE;
                    }
                    else
                    {
                        pHandle->LinkData.Autoneg = LSA_FALSE;
                    }
                }
                pHandle->LinkSM_State = IX1000_LINK_IDLE;
            }
            if(hwTimeSlicing) break;

            break;  /*last state before default, always break*/
        }
        default:
        {
            Status = EDD_STS_ERR_PARAM;
        }
        break;
    }

    return Status;
}


/**
 * \brief           helper function that triggers the readStatistics state machine
 *
 * \param[in|out]   pHandle         lower layer handle
 * \param[in]       hwTimeSlicing   do one or all states
 *
 * \retval          void
 */
static LSA_VOID ix1000_read_stats_sm(
        IX1000_LL_HANDLE_PTR_TYPE pHandle,
        LSA_BOOL hwTimeSlicing)
{
    IX1000_READ_STATS_SM_STATES_TYPE CurrentState = pHandle->readStatsState;

    EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
                    "IN : ix1000_read_stats_sm(): CurrentState: %u",
                    CurrentState);

    switch(CurrentState)
    {
        case IX1000_READ_IN_STATS_INDISC:
        {
            pHandle->LL_Stats.InDiscards += IX1000_reg32(IX1000_RXFIFOOVERFLOW);
            pHandle->readStatsState = IX1000_READ_IN_STATS_INERR_CRC;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_IN_STATS_INERR_CRC:
        {
            /* do not update incomplete values */
            pHandle->InErr_Shadow = IX1000_reg32(IX1000_RXCRCERROR);
            pHandle->readStatsState = IX1000_READ_IN_STATS_INERR_ALIGN;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_IN_STATS_INERR_ALIGN:
        {
            /* do not update incomplete values */
            pHandle->InErr_Shadow += IX1000_reg32(IX1000_RXALIGNMENTERROR);
            pHandle->readStatsState = IX1000_READ_IN_STATS_INERR_RUNT;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_IN_STATS_INERR_RUNT:
        {
            /* do not update incomplete values */
            pHandle->InErr_Shadow += IX1000_reg32(IX1000_RXRUNTERROR);
            pHandle->readStatsState = IX1000_READ_IN_STATS_INERR_JABB;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_IN_STATS_INERR_JABB:
            {
            /* do not update incomplete values */
            pHandle->InErr_Shadow += IX1000_reg32(IX1000_RXJABBERERROR);
            pHandle->readStatsState = IX1000_READ_IN_STATS_INERR_LEN;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_IN_STATS_INERR_LEN:
        {
            /* do not update incomplete values */
            pHandle->InErr_Shadow += IX1000_reg32(IX1000_RXLENGTHERROR);
            pHandle->readStatsState = IX1000_READ_IN_STATS_INERR_OOR;
            if(hwTimeSlicing) break; /* else fall through */
        }
            //lint -fallthrough
        case IX1000_READ_IN_STATS_INERR_OOR:
        {
            /* do not update incomplete values */
            pHandle->InErr_Shadow += IX1000_reg32(IX1000_RXOUTOFRANGETYPE);
            pHandle->readStatsState = IX1000_READ_IN_STATS_INERR_WD;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_IN_STATS_INERR_WD:
        {
            pHandle->LL_Stats.InErrors += (pHandle->InErr_Shadow + IX1000_reg32(IX1000_RXWATCHDOGERROR));
            pHandle->readStatsState = IX1000_READ_OUT_STATS_ERR_UFL;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_OUT_STATS_ERR_UFL:
        {
            /* do not update incomplete values */
            pHandle->OutErr_Shadow = IX1000_reg32(IX1000_TXUNDERFLOWERROR);
            pHandle->readStatsState = IX1000_READ_OUT_STATS_ERR_LCOL;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_OUT_STATS_ERR_LCOL:
        {
            /* do not update incomplete values */
            pHandle->OutErr_Shadow += IX1000_reg32(IX1000_TXLATECOL);
            pHandle->readStatsState = IX1000_READ_OUT_STATS_ERR_XCOL;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_OUT_STATS_ERR_XCOL:
        {
            /* do not update incomplete values */
            pHandle->OutErr_Shadow += IX1000_reg32(IX1000_TXEXESSCOL);
            pHandle->readStatsState = IX1000_READ_OUT_STATS_ERR_CE;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_OUT_STATS_ERR_CE:
        {
            /* do not update incomplete values */
            pHandle->OutErr_Shadow += IX1000_reg32(IX1000_TXCARRIERERROR);
            pHandle->readStatsState = IX1000_READ_OUT_STATS_ERR_EXDEF;
            if(hwTimeSlicing) break; /* else fall through */
        }
        //lint -fallthrough
        case IX1000_READ_OUT_STATS_ERR_EXDEF:
        {
            pHandle->LL_Stats.OutErrors += (pHandle->OutErr_Shadow + IX1000_reg32(IX1000_TXEXCESSDEF));
            pHandle->readStatsState = IX1000_READ_IN_STATS_INDISC;
            break; /* last state: always break! */
        }
        default:
        {
            pHandle->readStatsState = IX1000_READ_IN_STATS_INDISC;
            EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                            "ix1000_read_stats_sm(): UNEXP -> unknown state of statistics state machine (%u)!",
                            CurrentState);
            break;
        }
    }
}

/**
 * \brief       helper function that gets the autoneg capabilities
 *
 * \param[in]   pHandle     lower layer handle
 *
 * \retval      EDD_STS_OK              good
 * \retval      EDD_STS_ERR_HW          timeout while reading  from phy
 */
static LSA_RESULT ix1000_getCaps(
        IX1000_LL_HANDLE_PTR_TYPE pHandle)
{
    LSA_UINT16  capsFromPhy;
    LSA_RESULT  Status;

    Status = ix1000_ReadPhyReg(pHandle, MII_BMSR, &capsFromPhy);

    if(EDD_STS_OK == Status)
    {
        if(IX1000_GETBIT(MII_BMSR_100BASE4, capsFromPhy))
        {
            pHandle->AutonegCapsAdv |= EDD_AUTONEG_CAP_100BASET4;
        }
        if(IX1000_GETBIT(MII_BMSR_100FULL, capsFromPhy))
        {
            pHandle->AutonegCapsAdv |= EDD_AUTONEG_CAP_100BASETXFD;
        }
        if(IX1000_GETBIT(MII_BMSR_100HALF, capsFromPhy))
        {
            pHandle->AutonegCapsAdv |= EDD_AUTONEG_CAP_100BASETX;
        }
        if(IX1000_GETBIT(MII_BMSR_10FULL, capsFromPhy))
        {
            pHandle->AutonegCapsAdv |= EDD_AUTONEG_CAP_10BASETFD;
        }
        if(IX1000_GETBIT(MII_BMSR_10HALF, capsFromPhy))
        {
            pHandle->AutonegCapsAdv |= EDD_AUTONEG_CAP_10BASET;
        }
    }

    return Status;
}

/**
 * \brief       helper function that compares to mac addresses
 *
 * \param[in]   mac1, mac2  mac addresses to be compared
 *
 * \retval      0           equal to the bool'ish false
 * \retval      1           equal to the bool'ish true
 */
static LSA_BOOL ix1000_compareMac(
        EDD_MAC_ADR_TYPE mac1,
        EDD_MAC_ADR_TYPE mac2)
{
    LSA_UINT8 result;

    result = LSA_FALSE;

    if( mac1.MacAdr[0] == mac2.MacAdr[0]
     && mac1.MacAdr[1] == mac2.MacAdr[1]
     && mac1.MacAdr[2] == mac2.MacAdr[2]
     && mac1.MacAdr[3] == mac2.MacAdr[3]
     && mac1.MacAdr[4] == mac2.MacAdr[4]
     && mac1.MacAdr[5] == mac2.MacAdr[5]
      )
    {
        result = LSA_TRUE;
    }

    return result;
}

/**
 * \brief       helper function that resets the hardware (controller/phy/statistic counter)
 *
 * \param[in]   pHandle     lower layer handle
 *
 * \retval      EDD_STS_OK              good
 * \retval      EDD_STS_ERR_HW          reset failed
 */
static LSA_RESULT ix1000_resetHardware(
        IX1000_LL_HANDLE_TYPE const * const pHandle)
{
    LSA_RESULT  Status;

    Status      = EDD_STS_OK;

    /* controller reset */
    IX1000_SETBIT(IX1000_reg32(IX1000_DMA_REG_0),IX1000_DMA_REG_0_SWR);
    EDDS_WAIT_UNTIL_TIMEOUT((0 == IX1000_GETBIT(IX1000_DMA_REG_0_SWR, IX1000_reg32(IX1000_DMA_REG_0))),
            IX1000_TIMEOUT_US, (Status = EDD_STS_ERR_HW))

    if(EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_00(pHandle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "ix1000_resetHardware: software reset failed");
    }
    else
    {
        /* phy reset */
        Status = ix1000_WritePhyReg(pHandle, MII_BMCR, MII_BMCR_RESET_CMD);
    }

    if(EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_00(pHandle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                        "ix1000_resetHardware: phy reset failed");
    }

    return Status;
}

/**
 * \brief       helper function that sets up interface and port mac address
 *
 * \param[in|out]   pHandle     lower layer handle
 * \param[in|out]   pDSB        device setup block
 * \param[in|out]   pHwParam    hw parameter
 *
 * \retval      0           equal to the bool'ish false
 * \retval      1           equal to the bool'ish true
 */
static LSA_VOID ix1000_setupMac(
        IX1000_LL_HANDLE_PTR_TYPE pHandle,
        EDDS_UPPER_DSB_PTR_TYPE pDSB,
        EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE pHwParam)
{
    /* setup mac address */
    /* if mac address is given: not every byte is 0x00 */
    if((0 != pDSB->MACAddress[0].MacAdr[0])
                    || (0 != pDSB->MACAddress[0].MacAdr[1])
                    || (0 != pDSB->MACAddress[0].MacAdr[2])
                    || (0 != pDSB->MACAddress[0].MacAdr[3])
                    || (0 != pDSB->MACAddress[0].MacAdr[4])
                    || (0 != pDSB->MACAddress[0].MacAdr[5])
                    )
    {
        /* take mac interface mac adress from pDSB and save it to pHwParam */
        pHandle->InterfaceMAC = pDSB->MACAddress[0];
    }
    else
    {
        /* if no mac is given: take hw mac and write it back to pDSB */
        LSA_UINT32 gmac_reg_16, gmac_reg_17;
        gmac_reg_16 = IX1000_reg32(IX1000_GMAC_REG_16);
        gmac_reg_17 = IX1000_reg32(IX1000_GMAC_REG_17);

        pHandle->InterfaceMAC.MacAdr[0] = (gmac_reg_17 & 0xFF);
        pHandle->InterfaceMAC.MacAdr[1] = (gmac_reg_17 & 0xFF00) >> 8;
        pHandle->InterfaceMAC.MacAdr[2] = (gmac_reg_17 & 0xFF0000) >> 16;
        pHandle->InterfaceMAC.MacAdr[3] = (gmac_reg_17 & 0xFF000000) >> 24;
        pHandle->InterfaceMAC.MacAdr[4] = (gmac_reg_16 & 0xFF);
        pHandle->InterfaceMAC.MacAdr[5] = (gmac_reg_16 & 0xFF00) >> 8;

        pDSB->MACAddress[0] = pHandle->InterfaceMAC;
        /* if mac equals port mac for one port systems */
        pDSB->MACAddress[1] = pHandle->InterfaceMAC;
    }

    /* setup hard param port mac address */
    pHwParam->MACAddress = pHandle->InterfaceMAC;

    /* port mac cannot be taken from hw! */
    pHwParam->MACAddressPort[0] = pDSB->MACAddress[1];
}

/**
 * \brief       helper function that starts the Ethernet controller (Tx/Rx enable)
 *
 * \param[in]   pHandle     lower layer handle
 *
 * \retval      void
 */
static LSA_VOID ix1000_startController(IX1000_LL_HANDLE_PTR_TO_CONST_TYPE pHandle)
{
    LSA_UINT32  gmac_reg_16;
    LSA_UINT32  gmac_reg_17;

    LSA_UINT32 dma_reg_0;
    LSA_UINT32 dma_reg_6;

    gmac_reg_16 = ((LSA_UINT32)pHandle->InterfaceMAC.MacAdr[5] << 8)
                | ((LSA_UINT32)pHandle->InterfaceMAC.MacAdr[4]);
    gmac_reg_17 = ((LSA_UINT32)pHandle->InterfaceMAC.MacAdr[3] << 24)
                | ((LSA_UINT32)pHandle->InterfaceMAC.MacAdr[2] << 16)
                | ((LSA_UINT32)pHandle->InterfaceMAC.MacAdr[1] << 8)
                | ((LSA_UINT32)pHandle->InterfaceMAC.MacAdr[0]);

    /* write interface mac address to hardware */
    IX1000_SETBIT(gmac_reg_16, IX1000_GMAC_REG_16_AE);

    IX1000_reg32(IX1000_GMAC_REG_16) = gmac_reg_16;
    IX1000_reg32(IX1000_GMAC_REG_17) = gmac_reg_17;

    dma_reg_0 = IX1000_reg32(IX1000_DMA_REG_0);
    dma_reg_0 |= IX1000_BIT(IX1000_DMA_REG_0_FB,1)
              |  IX1000_BITS(IX1000_DMA_REG_0_PR_s, IX1000_DMA_REG_0_PR_e, 3)
              | IX1000_BITS(IX1000_DMA_REG_0_PBL_s, IX1000_DMA_REG_0_PBL_e, 8);
    IX1000_reg32(IX1000_DMA_REG_0) = dma_reg_0;

    dma_reg_6 = IX1000_reg32(IX1000_DMA_REG_6);
    dma_reg_6 |= IX1000_BIT(IX1000_DMA_REG_6_TSF, 1)
              |  IX1000_BIT(IX1000_DMA_REG_6_RSF, 1)
              |  IX1000_BIT(IX1000_DMA_REG_6_DFF, 1)
              |  IX1000_BIT(IX1000_DMA_REG_6_FTF, 1);
    IX1000_reg32(IX1000_DMA_REG_6) = dma_reg_6;

    /* set RMII speed to 100Mbits and duplex mode */
    IX1000_SETBIT(IX1000_reg32(IX1000_GMAC_REG_0), IX1000_GMAC_REG_0_FES);
    IX1000_SETBIT(IX1000_reg32(IX1000_GMAC_REG_0), IX1000_GMAC_REG_0_DM);

    /* enable multicast hash table */
    IX1000_SETBIT(IX1000_reg32(IX1000_GMAC_REG_1), IX1000_GMAC_REG_1_HMC);

    /* setup descriptor structure within hardware */
    LSA_UINT32 tmpVar;
    EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev, (LSA_VOID_PTR_TYPE)&pHandle->pTxDescriptors[0],
            (LSA_UINT32*)&tmpVar);
    IX1000_reg32(IX1000_DMA_REG_4) = tmpVar;
    EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev, (LSA_VOID_PTR_TYPE)&pHandle->pRxDescriptors[0],
            (LSA_UINT32*)&tmpVar);
    IX1000_reg32(IX1000_DMA_REG_3) = tmpVar;

    /* start dma controller (rx) */
    IX1000_SETBIT(IX1000_reg32(IX1000_DMA_REG_6), IX1000_DMA_REG_6_SR);
    /* start dma controller (rx) */
    IX1000_SETBIT(IX1000_reg32(IX1000_DMA_REG_6), IX1000_DMA_REG_6_ST);

    /* enable transmitter */
    IX1000_SETBIT(IX1000_reg32(IX1000_GMAC_REG_0), IX1000_GMAC_REG_0_TE);
    /* enable receiver */
    IX1000_SETBIT(IX1000_reg32(IX1000_GMAC_REG_0), IX1000_GMAC_REG_0_RE);

    /* trigger receive */
    IX1000_reg32(IX1000_DMA_REG_2) = 1;
}

/**
 * \brief       helper function that stops the Ethernet controller (Tx/Rx enable)
 *
 * \param[in]   pHandle     lower layer handle
 *
 * \retval      void
 */
static LSA_VOID ix1000_stopController(IX1000_LL_HANDLE_PTR_TO_CONST_TYPE pHandle)
{
    /* stop dma controller (rx) */
    IX1000_CLRBIT(IX1000_reg32(IX1000_DMA_REG_6), IX1000_DMA_REG_6_SR);
    /* stop dma controller (rx) */
    IX1000_CLRBIT(IX1000_reg32(IX1000_DMA_REG_6), IX1000_DMA_REG_6_ST);

    /* stop transmitter */
    IX1000_CLRBIT(IX1000_reg32(IX1000_GMAC_REG_0), IX1000_GMAC_REG_0_TE);
    /* stop receiver */
    IX1000_CLRBIT(IX1000_reg32(IX1000_GMAC_REG_0), IX1000_GMAC_REG_0_RE);
}

/**
 * \brief       helper function to determine weather or not a mac address is broadcast (0xFF:0xFF:0xFF:0xFF:0xFF:0xFF)
 *
 * \param[in]   mac     MAC address to be check
 *
 * \return      bool'sch
 */
static LSA_BOOL ix1000_IsBcstMac(EDD_MAC_ADR_TYPE mac)
{
    LSA_UINT32 i;
    LSA_BOOL ret = LSA_TRUE;

    for(i=0;i<EDD_MAC_ADDR_SIZE;++i)
    {
        if(mac.MacAdr[i] != 0xFF)
        {
            ret = LSA_FALSE;
            break;
        }
    }

    return ret;
}

/**
 * \brief       lower layer api function that initializes the lower layer (interface)
 *
 * \param[in]   pLLManagement   lower layer handle
 * \param[in]   hDDB            edds handle (handle device description block)
 * \param[in]   pDPB            pointer to DPB (device parameter block)
 * \param[in]   TraceIdx        used Trace ID for this function/lower layer
 * \param[out]  pCaps           capabilities of the lower layer
 *
 * \note        see EDDS_LLIF.DOCX for more informations
 *
 * \retval      EDD_STS_OK              good
 * \retval      EDD_STS_ERR_HW          error with some reset...
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_OPEN(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        EDDS_HANDLE                         hDDB,
        EDDS_UPPER_DPB_PTR_TO_CONST_TYPE    pDPB,
        LSA_UINT32                          TraceIdx,
        EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE    pCaps)
{
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;
    LSA_RESULT                  Status;

    pHandle                     = (IX1000_LL_HANDLE_TYPE*)pLLManagement;
    Status                      = EDD_STS_OK;

    EDDS_LL_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "IN :IX1000_LL_OPEN(pLLM: 0x%X)",
            pHandle);

    /* setup handle */
    pHandle->pParams    = (IX1000_LL_PARAMETER_PTR_TYPE) pDPB->pLLParameter;
    pHandle->hDDB       = hDDB;
    pHandle->hSysDev    = pDPB->hSysDev;
    pHandle->TraceIdx   = TraceIdx;
    pHandle->phyID      = IX1000_PHY_ID;

    /* set capabilities */
    pCaps->HWFeatures   = EDDS_LL_CAPS_HWF_CRC_GENERATE | EDDS_LL_CAPS_HWF_PHY_POWER_OFF;
    pCaps->PortCnt      = 1;
    pCaps->MaxFrameSize = 1536;

    pHandle->RxFrameBufferCount     = pDPB->RxBufferCnt;
    pHandle->RXDescriptorIndex      = 0;
    pHandle->TxFrameBufferCount     = pDPB->TxBufferCnt;
    pHandle->TXDescriptorIndex      = 0;
    pHandle->TXDescriptorIndexEnd   = 0;
    pHandle->TxDescriptorsUsed      = 0;

    ix1000_initDescr(pHandle);
    ix1000_initStructures(pHandle);

    return Status;
}

/**
 * \brief           lower layer api function that configures the lower layer (interface) and starts the ethernet controller
 *
 * \param[in]       pLLManagement       lower layer handle
 * \param[in]       pDSB                pointer to DSB (device setup block)
 * \param[in|out]   pHwParam            pointer to lower layer relevant hardware data
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          EDD_STS_OK          good
 * \retval          EDD_STS_ERR_PARAM   any parameter (input) cause the error (see traces for details)
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SETUP(
        LSA_VOID_PTR_TYPE               pLLManagement,
        EDDS_UPPER_DSB_PTR_TYPE         pDSB,
        EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE pHwParam)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;

    pHandle = (IX1000_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "IN :IX1000_LL_SETUP(pLLM: 0x%X)",
            pHandle);

    /* setting up the mac address must take place BEFORE resetting the hw
     * otherwise the mac addresses (read by hw) will be 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
     */
    ix1000_setupMac(pHandle, pDSB, pHwParam);
    Status = ix1000_resetHardware(pHandle);

    if(EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_SETUP() - hardware reset failed");
    }
    else
    {
        LSA_UINT32 gmac_reg_64;
        /* disable all interrupts that can occur */
        IX1000_reg32(IX1000_DMA_REG_7) = 0x0;
        IX1000_reg32(IX1000_GMAC_REG_15) = 0x200;
        IX1000_reg32(IX1000_MMC_INTR_MASK_RX) = 0x3FFFFFF;
        IX1000_reg32(IX1000_MMC_INTR_MASK_TX) = 0x3FFFFFF;
        IX1000_reg32(IX1000_MMC_IPC_INTR_MASK_RX) = 0x3FFFFFFF;

        gmac_reg_64 = IX1000_reg32(IX1000_GMAC_REG_64);
        IX1000_SETBIT(gmac_reg_64, IX1000_GMAC_REG_64_RSTONRD); /* reset on read */
        IX1000_reg32(IX1000_GMAC_REG_64) = gmac_reg_64;

        Status = ix1000_getCaps(pHandle);

        if(EDD_STS_OK == Status)
        {
            /* autoneg + phy off + mode(FD) + speed(100MBit) */
            Status = ix1000_WritePhyReg(pHandle, MII_BMCR, IX1000_PHY_INIT);
        }
    }

    if(EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_SETUP() - phyRegister_set failed");
    }
    else
    {
        pHwParam->PHYPower[0]       = EDDS_PHY_POWER_OFF;
        pHwParam->LinkSpeedMode[0]  = EDD_LINK_AUTONEG;
        pHwParam->IsWireless[0]     = LSA_FALSE;

        ix1000_startController(pHandle);
    }

    return Status;
}

/**
 * \brief           lower layer api function that stops the ethernet controller
 *
 * \param[in]       pLLManagement           lower layer handle
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          EDD_STS_OK              good
 * \retval          EDD_STS_ERR_HW          reset error
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SHUTDOWN(
        LSA_VOID_PTR_TYPE   pLLManagement)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE) pLLManagement;
    Status                      = EDD_STS_OK;

    EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
            "IN :IX1000_LL_SHUTDOWN()");

    ix1000_stopController(pHandle);

    EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
            "OUT:IX1000_LL_SHUTDOWN()");

    return Status;
}

/**
 * \brief           lower layer api function that closes the lower layer (interface) and frees allocated memory
 *
 * \param[in]       pLLManagement           lower layer handle
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          EDD_STS_OK              good
 * \retval          EDD_STS_ERR_RESOURCE    any parameter (input) cause the error (see traces for details)
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_CLOSE(
        LSA_VOID_PTR_TYPE   pLLManagement)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE) pLLManagement;

    EDDS_LL_TRACE_00(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :IX1000_LL_CLOSE()");

    Status = ix1000_freeMemory(pHandle);

    EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
            "OUT:IX1000_LL_CLOSE()");

    return Status;
}

/**
 * \brief           lower layer that checks if a frame was received (and returns its data it so)
 *
 * \param[in]       pLLManagement           lower layer handle
 * \param[out]      pBufferAddr             address of the received frames buffer
 * \param[out]      pLength                 length of the received frame
 * \param[out]      pPortID                 port where the frame was received
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          EDD_STS_OK              good - frame received
 * \retval          EDD_STS_OK_NO_DATA      no frame received
 * \retval          EDD_STS_ERR_RX          erroneous frame received (handling equals EDD_STS_OK within EDDS
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_RECV(
        LSA_VOID_PTR_TYPE                               pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR*     pBufferAddr,
        EDDS_LOCAL_MEM_U32_PTR_TYPE                     pLength,
        EDDS_LOCAL_MEM_U32_PTR_TYPE                     pPortID)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;
    IX1000_RX_DSCR*             RxD;
    LSA_UINT8*                  pMAC_t;
    EDD_MAC_ADR_TYPE            dstMAC;
    LSA_UINT32                  i;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;
    Status                      = EDD_STS_OK_NO_DATA;

    EDDS_LL_TRACE_03(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "IN :IX1000_LL_RECV(pLLM: 0x%X, pBufferAddr: 0x%X, pLength: 0x%X)",
            pLLManagement,
            pBufferAddr,
            pLength);

    /* get the current descriptor (rx) */
    RxD = &(pHandle->pRxDescriptors[pHandle->RXDescriptorIndex]);

    /* check if the a frame was received */
    if( 0 == IX1000_GETBIT(IX1000_RDES0_OWN, RxD->rdes0) )
    {
        Status = EDD_STS_OK;

        /* frame length included the checksum: subtract it from length */
        *pLength = IX1000_GETBITS(IX1000_RDES0_FL_s, IX1000_RDES0_FL_e, RxD->rdes0) - IX1000_CRC_LENGTH;
        *pBufferAddr = pHandle->RxFrameBufferPtr[pHandle->RXDescriptorIndex];
        *pPortID = 1;

        /* clear the buffer - a new buffer will be provided in LL_RECV_PROVIDE */
        pHandle->RxFrameBufferPtr[pHandle->RXDescriptorIndex] = 0;

        /* update shadow strcutures */
        ++(pHandle->RXDescriptorIndex);
        if(pHandle->RxFrameBufferCount <= pHandle->RXDescriptorIndex)
        {
            pHandle->RXDescriptorIndex = 0;
        }

        pMAC_t = (LSA_UINT8*) *pBufferAddr;

        for(i=0;i<EDD_MAC_ADDR_SIZE;++i)
        {
            dstMAC.MacAdr[i] = *pMAC_t;
            ++pMAC_t;
        }

        /* first bit: uc or mc/bc */
        if(IX1000_GETBIT(0, dstMAC.MacAdr[0]))
        {
            ++pHandle->LL_Stats.InNUcastPkts;

            //bc?
            if(ix1000_IsBcstMac(dstMAC))
            {
                ++pHandle->LL_Stats.InBroadcastPkts;
                ++pHandle->LL_Stats.InHCBroadcastPkts;
            }
            else
            {
                ++pHandle->LL_Stats.InMulticastPkts;
                ++pHandle->LL_Stats.InHCMulticastPkts;
            }
        }
        else
        {
            ++pHandle->LL_Stats.InUcastPkts;
            ++pHandle->LL_Stats.InHCUcastPkts;
        }

        pHandle->LL_Stats.InOctets += IX1000_GETBITS(IX1000_RDES0_FL_s, IX1000_RDES0_FL_e, RxD->rdes0);
        pHandle->LL_Stats.InHCOctets += IX1000_GETBITS(IX1000_RDES0_FL_s, IX1000_RDES0_FL_e, RxD->rdes0);
    }

    EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "OUT:IX1000_LL_RECV(), Status: 0x%X",
            Status);

    return Status;
}

/**
 * \brief           lower layer that (re-)provides a (new) rx frame buffer
 *
 * \param[in]       pLLManagement           lower layer handle
 * \param[in]       pBufferAddr             address of the received frames buffer
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          EDD_STS_OK              good
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_RECV_PROVIDE(
        LSA_VOID_PTR_TYPE       pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE  pBufferAddr)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;
    IX1000_RX_DSCR*             RxD;
    LSA_UINT32                  index;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;
    Status                      = EDD_STS_OK;

    EDDS_LL_TRACE_02(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "IN :IX1000_LL_RECV_PROVIDE(pLLM: 0x%X, pBufferAddr: 0x%X)",
            pHandle,
            pBufferAddr);

    /* get index of last descriptor (rx) */
    if( 0 == pHandle->RXDescriptorIndex)
    {
        index = pHandle->RxFrameBufferCount - 1;
    }
    else
    {
        index = pHandle->RXDescriptorIndex - 1;
    }

    /* get last descriptor (rx) */
    RxD = &pHandle->pRxDescriptors[index];
    pHandle->RxFrameBufferPtr[index] = pBufferAddr;

    /* provide the new buffer */
    EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev,pBufferAddr, (LSA_UINT32*)&RxD->rdes2);

    /* re-set the owner bit: hardware is now owner of the provided descriptor (again) */
    RxD->rdes0 |= IX1000_BIT(IX1000_RDES0_OWN, 1);

    EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "OUT:IX1000_LL_RECV_PROVIDE(), Status: 0x%X",
            Status);

    return Status;
}

/**
 * \brief           lower layer that (re-)triggers the frame receive cylce
 *
 * \param[in]       pLLManagement           lower layer handle
 * \param[in]       pBufferAddr             address of the received frames buffer
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          LSA_VOID
 */
LSA_VOID EDDS_LOCAL_FCT_ATTR IX1000_LL_RECV_TRIGGER(
        LSA_VOID_PTR_TYPE   pLLManagement)
{
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;

    pHandle = (IX1000_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
            "IN :IX1000_LL_RECV_TRIGGER()");

    /* retrigger rx */
    IX1000_reg32(IX1000_DMA_REG_2) = 1;

    EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
            "OUT:IX1000_LL_RECV_TRIGGER()");
}

/**
 * \brief           lower layer that queues and ethernet frame (preparation for transmission)
 *
 * \param[in]       pLLManagement           lower layer handle
 * \param[in]       pBufferAddr             address of the send buffer (given from EDDS/application)
 * \param[in]       Length                  length of the frame
 * \param[in]       PortID                  port ID for transmission
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          EDD_STS_OK              good
 * \retval          EDD_STS_ERR_RESOURCE    any parameter (input) cause the error (see traces for details)
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SEND(
        LSA_VOID_PTR_TYPE       pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE  pBufferAddr,
        LSA_UINT32              Length,
        LSA_UINT32              PortID)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;
    IX1000_TX_DSCR*             TxD;
    LSA_UINT8*                  pMAC_t;
    LSA_UINT32                  i;
    EDD_MAC_ADR_TYPE            dstMAC;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;
    Status                      = EDD_STS_OK;

    EDDS_LL_TRACE_04(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "IN :IX1000_LL_SEND(pLLM: 0x%X, pBufferAddr: 0x%X, Length: 0x%X, PortID: %d)",
            pHandle,
            pBufferAddr,
            Length,
            PortID);

    /* validate frame length */
    if ( Length > EDD_IEEE_FRAME_LENGTH)
    {
        Status = EDD_STS_ERR_PARAM;
        EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_SEND: invalid frame length (%u)",
                Length);
    }
    else
    {
        EDDS_ASSERT(pHandle->TxDescriptorsUsed < pHandle->TxFrameBufferCount);

        /* get current descriptor (tx) */
        TxD = &(pHandle->pTxDescriptors[pHandle->TXDescriptorIndexEnd]);

        /* check ownership for descriptor (tx) */
        if( 0 == IX1000_GETBIT(IX1000_TDES0_OWN, TxD->tdes0))
        {
            /* setup descriptor for transmission */
            EDDS_PTR_TO_ADDR32_DEV(pHandle->hSysDev,pBufferAddr, (LSA_UINT32*) &TxD->tdes2);
            IX1000_SETBITS(TxD->tdes1,IX1000_TDES1_TBS1_s, IX1000_TDES1_TBS1_e, Length);
            IX1000_SETBIT(TxD->tdes0,IX1000_TDES0_OWN);

            /* adjust descriptor shadow structures (tx) */
            ++pHandle->TxDescriptorsUsed;
            ++(pHandle->TXDescriptorIndexEnd);
            if(pHandle->TxFrameBufferCount <= pHandle->TXDescriptorIndexEnd)
            {
                pHandle->TXDescriptorIndexEnd = 0;
            }

            pMAC_t = (LSA_UINT8*) pBufferAddr;

            for(i=0;i<EDD_MAC_ADDR_SIZE;++i)
            {
                dstMAC.MacAdr[i] = *pMAC_t;
                ++pMAC_t;
            }

            /* first bit: uc or mc/bc */
            if(IX1000_GETBIT(0, dstMAC.MacAdr[0]))
            {
                ++pHandle->LL_Stats.OutNUcastPkts;

                //bc?
                if(ix1000_IsBcstMac(dstMAC))
                {
                    ++pHandle->LL_Stats.OutBroadcastPkts;
                    ++pHandle->LL_Stats.OutHCBroadcastPkts;
                }
                else
                {
                    ++pHandle->LL_Stats.OutMulticastPkts;
                    ++pHandle->LL_Stats.OutHCMulticastPkts;
                }
            }
            else
            {
                ++pHandle->LL_Stats.OutUcastPkts;
                ++pHandle->LL_Stats.OutHCUcastPkts;
            }

            pHandle->LL_Stats.OutOctets += (Length + IX1000_CRC_LENGTH);
            pHandle->LL_Stats.OutHCOctets +=  ((LSA_UINT64) Length + (LSA_UINT64) IX1000_CRC_LENGTH);
        }
        else
        {
            Status = EDD_STS_ERR_RESOURCE;
            ++pHandle->LL_Stats.OutDiscards;
            EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                    "IX1000_LL_SEND: frame could not be send (current descriptor owned by hardware)");
        }
    }

    EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "OUT:IX1000_LL_SEND(), Status: 0x%X",
            Status);

    return Status;
}

/**
 * \brief           lower layer that triggers the transmission of queued ethernet frames
 *
 * \param[in]       pLLManagement           lower layer handle
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          LSA_VOID
 */
LSA_VOID EDDS_LOCAL_FCT_ATTR IX1000_LL_SEND_TRIGGER(
        LSA_VOID_PTR_TYPE pLLManagement)
{
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;

    EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "IN :IX1000_LL_SEND_TRIGGER(pLLM: 0x%X)",
            pLLManagement);

    /* trigger transmit poll */
    IX1000_reg32(IX1000_DMA_REG_1) = 1;

    EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "OUT:IX1000_LL_SEND_TRIGGER()");
}

/**
 * \brief           lower layer that determines the status of a previously transmitted frame
 *
 * \param[in]       pLLManagement           lower layer handle
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          EDD_STS_OK              good
 * \retval          EDD_STS_OK_NO_DATA      no frame sent
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SEND_STS(
        LSA_VOID_PTR_TYPE   pLLManagement)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;
    IX1000_TX_DSCR*             lastTD;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;
    Status                      = EDD_STS_OK;

    EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "IN :IX1000_LL_SEND_STS(pLLM: 0x%X)",
            pHandle);

    lastTD = &pHandle->pTxDescriptors[pHandle->TXDescriptorIndex];

    if( (1 == IX1000_GETBIT(IX1000_TDES0_OWN,lastTD->tdes0)) && (pHandle->TxDescriptorsUsed))
    {
        /* trigger transmit poll */
        IX1000_reg32(IX1000_DMA_REG_1) = 1;
        Status = EDD_STS_OK_NO_DATA;
    }
    else if(0 == IX1000_GETBIT(IX1000_TDES0_OWN,lastTD->tdes0))
    {
        if(0 == lastTD->tdes2)
        {
            Status = EDD_STS_OK_NO_DATA;
        }
        else
        {
            EDDS_ASSERT(pHandle->TxDescriptorsUsed);
            lastTD->tdes2 = 0;
            Status = EDD_STS_OK;
            ++(pHandle->TXDescriptorIndex);
            if(pHandle->TxFrameBufferCount <= pHandle->TXDescriptorIndex)
            {
                pHandle->TXDescriptorIndex = 0;
            }
            --pHandle->TxDescriptorsUsed;
        }

    }
    else
    {
        Status = EDD_STS_OK_NO_DATA;
    }

    EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "OUT:IX1000_LL_SEND_STS(), Status: 0x%X",
            Status);

    return Status;
}

/**
 * \brief           lower layer that return the current link state
 *
 * \param[in]       pLLManagement               lower layer handle
 * \param[in]       PortID                      port id from where the statistics are retrieved
 * \param[out]      pStats                      current statistics
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 * \note            this function does not access registers; these are read in LL_RECURRING_TASK
 *
 * \retval          EDD_STS_OK
 * \retval          EDD_STS_ERR_PARAM
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_GET_STATS(
        LSA_VOID_PTR_TYPE               pLLManagement,
        LSA_UINT32                      PortID,
        EDDS_LOCAL_STATISTICS_PTR_TYPE  pStats)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;

    pHandle                     = (IX1000_LL_HANDLE_TYPE*)pLLManagement;
    Status                      = EDD_STS_OK;

    EDDS_LL_TRACE_02(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :IX1000_LL_GET_STATS(PortID: 0x%X, pStats: 0x%X)",
            PortID,
            pStats);

    if ((1 < PortID) || (0 == pStats))
    {
        Status = EDD_STS_ERR_PARAM;
        EDDS_LL_TRACE_02(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
                "IX1000_LL_GET_STATS port id wrong (%u) or stats pointer zero (%u)",
                PortID,
                pStats);
    }
    else
    {
        *pStats = pHandle->LL_Stats;
        if (PortID==0)
        {
            pStats->SupportedCounters     = IX1000_MIB2_SUPPORTED_COUNTERS_IF;
            pStats->InDiscards            = 0; /* not supported on Interface */
            pStats->InErrors              = 0; /* not supported on Interface */
            pStats->OutDiscards           = 0; /* not supported on Interface */
            pStats->OutErrors             = 0; /* not supported on Interface */
        }
    }

    EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:IX1000_LL_GET_STATS(), Status: 0x%X",
            Status);

    return Status;
}

/**
 * \brief           lower layer that return the current link state
 *
 * \param[in]       pLLManagement               lower layer handle
 * \param[in]       PortID                      port id from where the link state should be retrieved
 * \param[out]      pLinkStat                   current link status/mode/speed
 * \param[out]      pMauType                    current mau type (calculated)
 * \param[out]      pMediaType                  current media type
 * \param[out]      pIsPOF                      true/false (fiber optic)
 * \param[out]      pPortStatus                 port present or not
 * \param[out]      pAutonegCapAdvertised       supported link states for autoneg
 * \param[out}      pLinkSpeedModeConfigured    default configured link speed/mode
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 * \note            this function does not access (phy) registers; these are read in LL_RECURRING_TASK
 *
 * \retval          EDD_STS_OK                  good
 * \retval          EDD_STS_ERR_PARAM           wrong port id
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_GET_LINK_STATE(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        LSA_UINT32                          PortID,
        EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  pLinkStat,
        EDDS_LOCAL_MEM_U16_PTR_TYPE         pMAUType,
        EDDS_LOCAL_MEM_U8_PTR_TYPE          pMediaType,
        EDDS_LOCAL_MEM_U8_PTR_TYPE          pIsPOF,
        EDDS_LOCAL_MEM_U32_PTR_TYPE         pPortStatus,
        EDDS_LOCAL_MEM_U32_PTR_TYPE         pAutonegCapAdvertised,
        EDDS_LOCAL_MEM_U8_PTR_TYPE          pLinkSpeedModeConfigured)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;

    pHandle                     = (IX1000_LL_HANDLE_TYPE*)pLLManagement;
    Status                      = EDD_STS_OK;

    /* only one port on this board */
    if(1 == PortID)
    {
        pLinkStat->PortID = (LSA_UINT16) PortID;
        pLinkStat->Mode = pHandle->LinkData.Mode;
        pLinkStat->Speed = pHandle->LinkData.Speed;
        pLinkStat->Status = pHandle->LinkData.Status;

        *pMAUType = EDD_MAUTYPE_UNKNOWN;
        *pMediaType = EDD_MEDIATYPE_COPPER_CABLE;
        *pIsPOF = LSA_FALSE;
        *pPortStatus = pHandle->PortStatus;
        *pAutonegCapAdvertised = pHandle->AutonegCapsAdv;
        *pLinkSpeedModeConfigured = pHandle->LinkSpeedModeConfigured;

        EDDS_GET_PHY_PARAMS(pHandle->hSysDev,
                            pHandle->hDDB,
                            PortID,
                            pLinkStat->Speed,
                            pLinkStat->Mode,
                            pMAUType,
                            pMediaType,
                            pIsPOF);
    }
    else
    {
        Status = EDD_STS_ERR_PARAM;
        EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_GET_LINK_STATE: wrong input port id: %u",
                PortID);
    }

    EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "OUT:IX1000_LL_GET_LINK_STATE(), Status: 0x%X",
            Status);

    return Status;
}

/**
 * \brief           lower layer that triggers the setting of a new link state
 *
 * \param[in]       pLLManagement       lower layer handle
 * \param[in]       hwTimeSlicing       time slicing active or not
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 * \note            this function does not access (phy) registers; these are written in LL_RECURRING_TASK
 *
 * \retval          EDD_STS_OK          good
 * \retval          EDD_STS_ERR_PARAM   any parameter was wrong (e.g. phyPower/LinkStat/PortID)
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SET_LINK_STATE(
        LSA_VOID_PTR_TYPE   pLLManagement,
        LSA_UINT32          PortID,
        LSA_UINT8 * const   pLinkStat,
        LSA_UINT8 * const   pPHYPower)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;
    Status                      = EDD_STS_OK;

    EDDS_LL_TRACE_03(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :IX1000_LL_SET_LINK_STATE(PortID: %d, LinkStat: 0x%X, PhyPower: 0x%X)",
            PortID,
            *pLinkStat,
            *pPHYPower);

    /* parameter validation */
    if(1 != PortID)
    {
        Status = EDD_STS_ERR_PARAM;
    }
    else if( (*pLinkStat != EDD_LINK_AUTONEG)
          && (*pLinkStat != EDD_LINK_UNCHANGED) )
    {
        Status = EDD_STS_ERR_PARAM;
        EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_SET_LINK_STATE - wrong link stat: 0x%02x",
                *pLinkStat);
    }
    else if( (*pPHYPower != EDDS_PHY_POWER_UNCHANGED)
          && (*pPHYPower != EDDS_PHY_POWER_OFF)
          && (*pPHYPower != EDDS_PHY_POWER_ON) )
    {
        Status = EDD_STS_ERR_PARAM;
        EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_SET_LINK_STATE - wrong phy power: 0x%02x",
                *pPHYPower);
    }

    /* if an input is "unchanged" it needs to be set to the current value */
    if(EDD_LINK_UNCHANGED   == *pLinkStat
    && EDD_STS_OK           == Status)
    {
        *pLinkStat = pHandle->savedLinkStat;
    }

    if(EDDS_PHY_POWER_UNCHANGED == *pPHYPower
    && EDD_STS_OK               == Status)
    {
        *pPHYPower = pHandle->savedPhyPower;
    }

    if(EDD_STS_OK == Status)
    {
        pHandle->newPhyPower = *pPHYPower;
        pHandle->newLinkStat = *pLinkStat;

        /* set the trigger for LL_RECURRING_TASK - to be done last */
        pHandle->IsSetLink = 1;
    }

    EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,
            "OUT:IX1000_LL_SET_LINK_STATE(), Status: 0x%X",
            Status);

    return Status;
}

/**
 * \brief           lower layer that enabled a multicast mac address
 *
 * \param[in]       pLLManagement               lower layer handle
 * \param[in]       pMCAddr                     multicast address to be enabled
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 * \note            check if the address is really a multicast is missing, technically every mac address can be activated
 *
 * \retval          EDD_STS_OK
 * \retval          EDD_STS_ERR_PARAM
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_MC_ENABLE(
        LSA_VOID_PTR_TYPE                       pLLManagement,
        EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR*    pMCAddr)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;

    LSA_INT     bit;
    LSA_UINT32  val;
    LSA_UINT32  reg;
    LSA_UINT32  hash;
    LSA_UINT32  crc;
    LSA_UINT32  ind;
    LSA_UINT8*  mcMAC;
    LSA_UINT8   enabled;
    LSA_UINT8   length;

    pHandle     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;
    Status      = EDD_STS_OK;

    EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
        "IN :IX1000_LL_MC_ENABLE(pMCAddr: 0x%X)",
        pMCAddr);

    /* init default values */
    bit     = 0;
    val     = 0;
    reg     = 0;
    crc     = 0xFFFFFFFF;
    ind     = 0;
    enabled = 0;
    length  = 6;

    mcMAC = (LSA_UINT8*) pMCAddr;

    /* get hash value via algorithm */
    while (length != 0)
    {
        LSA_UINT8 data = *mcMAC++;

        for (bit = 0; bit < 8; ++bit, data >>= 1)
        {
            /* algorithm */
            crc = (crc << 1) ^ ( (((crc & 0x80000000) > 0) ? 1 : 0 ) ^ (data & 1) ? IX1000_CRC_32_IEE802_3 : 0);
        }
        --length;
    }

    /* inverse hash value and take only six bit! */
    hash = ((crc ^ ~0u)>>26) & 0x3F;

    /* set corresponding bit */
    if(hash & 0x20)
    {
        /* bit 5 cleared: high register selected */
        reg = IX1000_GMAC_REG_2;
    }
    else
    {
        /* bit 5 cleared: low register selected */
        reg = IX1000_GMAC_REG_3;
    }

    /* find lowest free index */
    while(ind < EDDS_MAX_MC_MAC_CNT)
    {
        if( 0 == pHandle->macHashTable[ind].mcMacAddr.MacAdr[0]
         && 0 == pHandle->macHashTable[ind].mcMacAddr.MacAdr[1]
         && 0 == pHandle->macHashTable[ind].mcMacAddr.MacAdr[2]
         && 0 == pHandle->macHashTable[ind].mcMacAddr.MacAdr[3]
         && 0 == pHandle->macHashTable[ind].mcMacAddr.MacAdr[4]
         && 0 == pHandle->macHashTable[ind].mcMacAddr.MacAdr[5]
          )
        {
            pHandle->macHashTable[ind].mcMacAddr = *pMCAddr;
            pHandle->macHashTable[ind].hashValue = hash;
            break;
        }
        ++ind;
    }

    if(ind >= EDDS_MAX_MC_MAC_CNT)
    {
        /* this should not occur: checked within EDDS */
        Status = EDD_STS_ERR_PARAM;
        EDDS_LL_TRACE_00(pHandle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_MC_ENABLE: no free multicast table entry: cannot active multicast");
    }

    if(Status == EDD_STS_OK)
    {
        LSA_UINT32  ind_2;
        ind_2       = 0;
        /* check if hash value is already enabled: only write registers if really needed! */
        while(ind_2 < EDDS_MAX_MC_MAC_CNT)
        {
            if(hash == pHandle->macHashTable[ind_2].hashValue
            && ind != ind_2)
            {
                enabled = 1;
                break;
            }
            ++ind_2;
        }

        if(!enabled)
        {
            val = IX1000_reg32(reg);
            IX1000_SETBIT(val, (hash & 0x1F));
            IX1000_reg32(reg) = val;
        }
    }

    EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:IX1000_LL_MC_ENABLE(), Status: 0x%X",
            Status);

    return Status;
}

/**
 * \brief           lower layer that disable a (or all) multicast mac address
 *
 * \param[in]       pLLManagement               lower layer handle
 * \param[in]       DisableAll                  selection bit - disable all or just one multicast address
 * \param[in]       pMCAddr                     multicast address to be enabled
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          EDD_STS_OK
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_MC_DISABLE(
        LSA_VOID_PTR_TYPE                       pLLManagement,
        LSA_BOOL                                DisableAll,
        EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR*    pMCAddr)
{
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;
    LSA_RESULT                  Status;

    LSA_UINT32  val;
    LSA_UINT32  reg;
    LSA_UINT32  ind;
    LSA_UINT32  hash_shadow;

    EDD_MAC_ADR_TYPE n_mac = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}} ;

    pHandle     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;

    EDDS_LL_TRACE_02(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
        "IN :IX1000_LL_MC_DISABLE(DisableAll: 0x%X, pMCAddr: 0x%X)",
        DisableAll,
        pMCAddr);

    Status  = EDD_STS_OK;
    reg     = 0;
    val     = 0;
    ind     = 0;

    hash_shadow = ~0u;

    if(DisableAll)
    {
        /* disable all multicast mac addresses */
        for(ind=0;ind<EDDS_MAX_MC_MAC_CNT;++ind)
        {
            pHandle->macHashTable[ind].mcMacAddr = n_mac;
            pHandle->macHashTable[ind].hashValue = ~0u;
        }

        /* zero hash table */
        IX1000_reg32(IX1000_GMAC_REG_2) = 0x0;
        IX1000_reg32(IX1000_GMAC_REG_3) = 0x0;
    }
    else
    {
        /* disable one multicast mac address */
        while(ind < EDDS_MAX_MC_MAC_CNT)
        {
            if(ix1000_compareMac(pHandle->macHashTable[ind].mcMacAddr, *pMCAddr))
            {
                hash_shadow = pHandle->macHashTable[ind].hashValue;
                pHandle->macHashTable[ind].mcMacAddr = n_mac;
                pHandle->macHashTable[ind].hashValue = ~0u;
                break;
            }
            ++ind;
        }

        /* check if hash value is used multiple time - if so: do not disable the corresponding bit in the hash table */
        ind = 0;

        while(ind < EDDS_MAX_MC_MAC_CNT)
        {
            if(pHandle->macHashTable[ind].hashValue == hash_shadow)
            {
                break;
            }
            ++ind;
        }

        /* no multi use of the hash value: disable the address by clearing the corresponding bit */
        if(ind >= EDDS_MAX_MC_MAC_CNT)
        {
            /* set corresponding bit */
            if(hash_shadow & 0x20)
            {
                /* bit 5 set -> high */
                reg = IX1000_GMAC_REG_2;
            }
            else
            {
                /* low */
                reg = IX1000_GMAC_REG_3;
            }

            val = IX1000_reg32(reg);
            IX1000_CLRBIT(val, (hash_shadow & 0x1F));
            IX1000_reg32(reg) = val;
        }
    }

    EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
        "OUT:IX1000_LL_MC_DISABLE(), Status: 0x%X",
        Status);

    return Status;
}

/**
 * \brief           lower layer that saves the led control register(s)
 *
 * \param[in]       pLLManagement       lower layer handle
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          LSA_VOID
 */
LSA_VOID EDDS_LOCAL_FCT_ATTR IX1000_LL_LED_BACKUP_MODE(
        LSA_VOID_PTR_TYPE   pLLManagement)
{
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;
    LSA_RESULT                  Status;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;

    EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
            "IN :IX1000_LL_LED_BACKUP_MODE()");

    Status = ix1000_ReadPhyReg(pHandle, MII_LEDCR, &pHandle->LedBackUp);

    if(EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_LED_BACKUP_MODE: phy access failed");
    }

    EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
            "OUT:IX1000_LL_LED_BACKUP_MODE()");
}

/**
 * \brief           lower layer that restores the led control register(s)
 *
 * \param[in]       pLLManagement       lower layer handle
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          LSA_VOID
 */
LSA_VOID EDDS_LOCAL_FCT_ATTR IX1000_LL_LED_RESTORE_MODE(
        LSA_VOID_PTR_TYPE pLLManagement)
{
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;
    LSA_RESULT                  Status;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;

    EDDS_LL_TRACE_00(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :IX1000_LL_LED_RESTORE_MODE()");

    Status = ix1000_WritePhyReg(pHandle, MII_LEDCR, pHandle->LedBackUp);

    if(EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_00(pHandle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_LED_RESTORE_MODE: phy access failed");
    }

    EDDS_LL_TRACE_00(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:IX1000_LL_LED_RESTORE_MODE()");
}

/**
 * \brief           lower layer that enables or disables the leds
 *
 * \param[in]       pLLManagement       lower layer handle
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          LSA_VOID
 */
LSA_VOID EDDS_LOCAL_FCT_ATTR IX1000_LL_LED_SET_MODE(
        LSA_VOID_PTR_TYPE   pLLManagement,
        LSA_BOOL            LEDOn)
{
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;
    LSA_RESULT                  Status = EDD_STS_OK;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;

    EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :IX1000_LL_LED_SET_MODE(LEDOn: 0x%X)",
            LEDOn);

    if(LEDOn)
    {
        Status = ix1000_WritePhyReg(pHandle, MII_LEDCR, MII_LEDCR_ON);
    }
    else
    {
        Status = ix1000_WritePhyReg(pHandle, MII_LEDCR, MII_LEDCR_OFF);
    }

    if(EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_01(pHandle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_LED_SET_MODE: phy access failed (led mode %u)",
                LEDOn);
    }

    EDDS_LL_TRACE_00(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:IX1000_LL_LED_SET_MODE()");
}

/**
 * \brief           lower layer that sets the port status (present or not)
 *
 * \param[in]       pLLManagement       lower layer handle
 * \param[in]       PortID              port id
 * \param[in]       isPulled            plugged or pulled
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 *
 * \retval          LSA_VOID
 */
LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SWITCH_CHANGE_PORT(
        LSA_VOID_PTR_TYPE   pLLManagement,
        LSA_UINT16          PortID,
        LSA_UINT8           isPulled)
{
    LSA_RESULT                  Status;
    IX1000_LL_HANDLE_PTR_TYPE   pHandle;

    pHandle                     = (IX1000_LL_HANDLE_PTR_TYPE) pLLManagement;
    Status                      = EDD_STS_OK;

    EDDS_LL_TRACE_02(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :IX1000_LL_SWITCH_CHANGE_PORT_STATE() isPulled(%d) for PortID(%d)",
            isPulled,
            PortID);

    if(1 != PortID)
    {
        Status = EDD_STS_ERR_PARAM;
        EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "IX1000_LL_SWITCH_CHANGE_PORT_STATE: wrong port id(%u)",
                PortID);
    }
    else
    {
        if(EDD_PORTMODULE_IS_PULLED == isPulled)
        {
            pHandle->PortStatus = EDD_PORT_NOT_PRESENT;
        }
        else if(EDD_PORTMODULE_IS_PLUGGED == isPulled)
        {
            pHandle->PortStatus = EDD_PORT_PRESENT;
        }
        else
        {
            Status = EDD_STS_ERR_PARAM;
            EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "IX1000_LL_SWITCH_CHANGE_PORT_STATE: wrong port status state (%u)",
                    isPulled);
        }
    }

    EDDS_LL_TRACE_00(pHandle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:TI_LL_SWITCH_CHANGE_PORT_STATE()");

    return (Status);
}

/**
 * \brief           lower layer that handle all tasks that need to by done cyclically within the lower layer
 *
 * \param[in]       pLLManagement               lower layer handle
 * \param[in]       hwTimeSlicing               time slicing active or not
 *
 * \note            see EDDS_LLIF.DOCX for more informations
 * \note            no IN/OUT trace in this functions (performance reasons!)
 * \note            hwTimeSlicing is not supported yet - writing a '1' to this bit will have no effect!
 *
 * \retval          EDDS_LL_RECURRING_TASK_     task that selects what the edds scheduler needs to do in this cycle
 *                                              possible bits: EDDS_LL_RECURRING_TASK_LINK_CHANGED and/or EDDS_LL_RECURRING_TASK_STATISTIC_COUNTER
 */
LSA_UINT32 EDDS_LOCAL_FCT_ATTR IX1000_LL_RECURRING_TASK(
        LSA_VOID_PTR_TYPE   pLLManagement,
        LSA_BOOL            hwTimeSlicing)
{
    LSA_RESULT Status;
    IX1000_LL_HANDLE_PTR_TYPE pHandle;
    LSA_UINT32 triggerMask;

    pHandle = (IX1000_LL_HANDLE_PTR_TYPE)pLLManagement;
    triggerMask = EDDS_LL_RECURRING_TASK_NOTHING;

    /* get and if necessary set link
     * if(pending): do not actualize like state! */
    Status = ix1000_handle_link_sm(pHandle,hwTimeSlicing);

    if(EDD_STS_OK == Status)
    {
        if(ix1000_DetectLinkChange(pHandle))
        {
            triggerMask |= EDDS_LL_RECURRING_TASK_LINK_CHANGED;
        }
    }

    if(EDD_STS_OK           != Status
    && EDD_STS_OK_PENDING   != Status)
    {
        EDDS_LL_TRACE_01(pHandle->TraceIdx, LSA_TRACE_LEVEL_FATAL,
                "IX1000_LL_RECURRING_TASK: phy access not possible - Status (0x%08x)",
                Status);
        EDDS_FatalError(EDDS_FATAL_ERR_LL,EDDS_MODULE_ID,__LINE__);
    }

    /* stats do not depend on link handling! */
    ix1000_read_stats_sm(pHandle, hwTimeSlicing);
    if(IX1000_READ_IN_STATS_INDISC == pHandle->readStatsState)
    {
        triggerMask |= EDDS_LL_RECURRING_TASK_STATISTIC_COUNTER;
    }

    return triggerMask;
}

/*****************************************************************************/
/* end of file ix1000_edd.c                                                  */
/*****************************************************************************/

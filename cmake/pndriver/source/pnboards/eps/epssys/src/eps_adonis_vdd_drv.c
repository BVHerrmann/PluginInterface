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
/*  F i l e               &F: eps_adonis_imcea.c                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Imcea Shared Mem Interface Adaption                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20045
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */

#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <adonis/driver.h>
#include <adonis.h>
#include <os_Env.h>
#include <eps_adonis_vdd_drv.h>       /* Own Header                        */
#include <eps_adonis_vdd.h>           /* Own Header                        */
#include <adonis/pci.h>
#include <adonis/boot_info.h>


struct
{
    uint32_t                          lCntInst;
    pthread_spinlock_t                sLock;
    uint32_t                          lPhyBaseAddr;
    uint32_t                          lSize;
}   g_EpsAdonisVdd;

static void eps_adonis_vdd_drv_get_phy_addr (void );

int32_t   eps_adonis_vdd_drv_install   (void)
{
    int ret_val = ADN_OK;
    
    memset(&g_EpsAdonisVdd,
           0,
           sizeof(g_EpsAdonisVdd));  

    
    ret_val = pthread_spin_init(&g_EpsAdonisVdd.sLock, 0);
    
    if(ret_val != ADN_OK)
    {
        return ADN_ERROR;
    }
    
    adn_drv_func_t eps_adonis_vdd_drv_func = {   eps_adonis_vdd_drv_open,
                                                 NULL,
                                                 NULL,
                                                 eps_adonis_vdd_drv_close,
                                                 eps_adonis_vdd_drv_ioctl,
                                                 eps_adonis_vdd_drv_mmap};
    
    ret_val = adn_drv_register(EPS_ADONIS_VDD_DRV_NAME, &eps_adonis_vdd_drv_func);
    
    if(ret_val != ADN_OK)
    {
        return ADN_ERROR;
    }
    
    return ADN_OK;
}

int32_t eps_adonis_vdd_drv_uninstall( void )
{
    int32_t ret_val = ADN_OK;

    ret_val = adn_drv_unregister(EPS_ADONIS_VDD_DRV_NAME);
        
    if(ret_val != ADN_OK)
    {
        return ADN_ERROR;
    }
    return ADN_OK;
}

int32_t eps_adonis_vdd_drv_open(adn_drv_info_t*  pDrvInfo, int oflag)
{
    g_EpsAdonisVdd.lCntInst++;
    
    pDrvInfo->state = ADN_DRV_STATE_BUSY;
    
    return ADN_OK;
}

int32_t eps_adonis_vdd_drv_close(adn_drv_info_t* pDrvInfo)
{
    if(--g_EpsAdonisVdd.lCntInst == 0);
    {
        pDrvInfo->state = ADN_DRV_STATE_FREE;
    }
    
    return ADN_OK;
}

int32_t eps_adonis_vdd_drv_ioctl(adn_drv_info_t* pDrvInfo, int cmd, void* arg)
{
    int ret_val = ADN_ERROR;
    
    switch(cmd)
    {
        case EPS_ADONIS_VDD_DRV_GET_PHY_ADDR:
        {
            eps_adonis_vdd_drv_get_phy_addr();
            
            ret_val = ADN_OK;
                    
            break;
        }
    }
    
    return ret_val;
}

void   *eps_adonis_vdd_drv_mmap(adn_drv_info_t* pDrvInfo, size_t len, off_t off)
{
    return ((void*)g_EpsAdonisVdd.lPhyBaseAddr + EPS_ADONIS_VDD_SHM_OFFSET);
}


static void eps_adonis_vdd_drv_get_phy_addr (void )
{
    uint8_t     lNoBus      = 0;
    uint16_t    lNoDevice   = 0;
    uint16_t    lNoFct      = 0;
    uint16_t    lIndex      = 0;
    
    uint32_t  lowPart     = 0;
    uint32_t  highPart    = 0;
    uint32_t  length      = 0;
    int32_t   lResultTmp  = ADN_ERROR;
    uint8_t   bResult     = TRUE;
    
    // search for the devices            
    while   (adn_pci_find_device(   (uint16_t) PCI_VENDOR_SIEMENS_AG,
                                    (uint16_t) PCI_DEVICE_ID_HYPERVISOR_VIRTDEV,
                                    lIndex,
                                    &lNoBus,
                                    &lNoDevice,
                                    &lNoFct) == ADN_OK)
    {
        break;
    }

    // get lowPart
    lResultTmp = adn_pci_read_config_uint32(lNoBus,
                                            lNoDevice,
                                            lNoFct,
                                            0x44,
                                            &lowPart);

    if  (lResultTmp != ADN_OK)
        // error
    {
        #if (ADN_BOOT_INFO_DEV != ADN_NULL)
        adn_boot_info_printf("eps_adonis_vdd_drv_get_phy_addr(): error reading lowPart\n");
        #endif
        
        bResult = FALSE;
    }
    
    if(bResult != FALSE)
    {
        // get highPart
        lResultTmp = adn_pci_read_config_uint32(lNoBus,
                                                lNoDevice,
                                                lNoFct,
                                                0x48,
                                                &highPart);

        if  (lResultTmp != ADN_OK)
            // error
        {
            #if (ADN_BOOT_INFO_DEV != ADN_NULL)
            adn_boot_info_printf("eps_adonis_vdd_drv_get_phy_addr(): error reading highPart\n");
            #endif
            
            bResult = FALSE;
        }        
    }   
    
    if(bResult != FALSE)
    {
        // get length
        lResultTmp = adn_pci_read_config_uint32(lNoBus,
                                                lNoDevice,
                                                lNoFct,
                                                0x4C,
                                                &length);

        if  (lResultTmp != ADN_OK)
            // error
        {
            #if (ADN_BOOT_INFO_DEV != ADN_NULL)
            adn_boot_info_printf("eps_adonis_vdd_drv_get_phy_addr(): error reading length\n");
            #endif
            
            bResult = FALSE;
        }        
    }  
    
    if(bResult != FALSE)
    {    
        g_EpsAdonisVdd.lPhyBaseAddr    = lowPart;
        g_EpsAdonisVdd.lSize           = length;
    }
}

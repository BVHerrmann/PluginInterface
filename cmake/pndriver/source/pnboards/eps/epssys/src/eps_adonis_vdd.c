#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include <adonis/driver.h>
#include <adonis.h>
#include <stropts.h>
#include <adonis/boot_info.h>


#include <stdlib.h>
#include <fcntl.h>
#include <os_Env.h>
#include "eps_adonis_vdd.h"
#include "eps_adonis_vdd_drv.h"
#include "adb_shared.h"

#include "pci.h"


adn_adb_shared_mem_t*           eps_adonis_vdd_adb_shared_mem = NULL;
int                             g_fdEpsAdonisVdd;
extern adn_adb_shared_mem_t*    adn_adb_shm_data;

INT32  eps_adonis_vdd_init( void ) // set define ADN_ADB_ACTIVATE_CALLBACK_FCT to use this function
{
    INT32                       Ret_Val = TRUE;
    
    if(eps_adonis_vdd_adb_shared_mem == NULL)
    {
        #if (ADN_BOOT_INFO_DEV != ADN_NULL)
        adn_boot_info_printf("\neps_adonis_vdd_init(): Start init eps_adonis_vdd_drv...\n");
        #endif
        
        if (eps_adonis_vdd_drv_install() != ADN_OK)
        {
            #if (ADN_BOOT_INFO_DEV != ADN_NULL)
            adn_boot_info_printf("eps_adonis_vdd_init(): error at installing eps_adonis_vdd_drv\n");
            #endif
        }
        else
        {
            g_fdEpsAdonisVdd = open(EPS_ADONIS_VDD_DRV_NAME, O_RDWR);
            
            if(g_fdEpsAdonisVdd == -1)
            {
                #if (ADN_BOOT_INFO_DEV != ADN_NULL)
                adn_boot_info_printf("eps_adonis_vdd_init(): error at opening eps_adonis_vdd_drv\n");
                #endif
            }
            else
            {
                // go to kernel-mode to get the physical address
                Ret_Val = ioctl( g_fdEpsAdonisVdd, 
                                 EPS_ADONIS_VDD_DRV_GET_PHY_ADDR, 
                                 NULL );   
                
                if(Ret_Val != ADN_OK)
                {
                    #if (ADN_BOOT_INFO_DEV != ADN_NULL)
                    adn_boot_info_printf("eps_adonis_vdd_init(): error at IOCTL\n");
                    #endif
                }
                else
                {    
                    //***************************************************************
                    // MAP
                    
                    uint8_t     *pMapRegBase;

                    pMapRegBase = (uint8_t*) mmap(   NULL, 
                                                     EPS_ADONIS_VDD_SHM_ADB_SIZE, 
                                                     PROT_READ | PROT_WRITE, 
                                                     MAP_SHARED, 
                                                     g_fdEpsAdonisVdd, 
                                                     0 );
                 
                    if (pMapRegBase == ((void*) -1))
                    {
                        #if (ADN_BOOT_INFO_DEV != ADN_NULL)
                        adn_boot_info_printf("eps_adonis_vdd_init(): error at mmap\n");
                        #endif
                    }
                    else
                    {
                        eps_adonis_vdd_adb_shared_mem = (adn_adb_shared_mem_t*)pMapRegBase;
                        adn_adb_shm_data = eps_adonis_vdd_adb_shared_mem;
                        
                        #if (ADN_BOOT_INFO_DEV != ADN_NULL)
                        adn_boot_info_printf("\neps_adonis_vdd_init(): eps_adonis_vdd_drv installed successfully!\n");
                        #endif
                        
                        Ret_Val = TRUE;
                    }
                }
            }
        }
    }
    
    return Ret_Val;
}

void eps_adonis_vdd_undo_init(void)
{
    int     ret_val = 0;
    
    // unmap
    if(eps_adonis_vdd_adb_shared_mem != NULL)    
        ret_val = munmap( (void*)eps_adonis_vdd_adb_shared_mem, EPS_ADONIS_VDD_SHM_ADB_SIZE );
    
    if (ret_val != 0)
    {
        #if (ADN_BOOT_INFO_DEV != ADN_NULL)
        adn_boot_info_printf("eps_adonis_vdd_undo_init(): error at munmap\n");
        #endif
    }
    else
    {    
        close(g_fdEpsAdonisVdd);
        
        if (eps_adonis_vdd_drv_uninstall() != ADN_OK)
        {
            #if (ADN_BOOT_INFO_DEV != ADN_NULL)
            adn_boot_info_printf("eps_adonis_vdd_undo_init(): error at uninstalling eps_adonis_vdd_drv\n");
            #endif
        }
    }
}

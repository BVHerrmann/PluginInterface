/*
 * eps_adb_cause_halt_drv.c
 *
 *  Created on: 02.12.2014
 *      Author: z00315mf
 */

/*
 * All this driver does is calling adn_adb_cause_halt() from Usercode
 */

/*********************************************************************
 Includes
 *********************************************************************/
#include <stdint.h>
#include <adonis/driver.h>
#include <eps_sys.h>
#include <eps_lib.h>
#include <adonis.h>


#include <eps_adb_cause_halt_drv.h>
/*********************************************************************
 File-local defines
 *********************************************************************/

/*********************************************************************
   Declarations
*********************************************************************/
/*********************************************************************
 Global Variables
 *********************************************************************/
///// Structure containing driver functions
adn_drv_func_t eps_adb_cause_halt_drv_func =
{
    eps_adb_cause_halt_drv_open,
    NULL,
    NULL,
    eps_adb_cause_halt_drv_close,
    eps_adb_cause_halt_drv_ioctl,
  NULL
};

typedef struct
{
  LSA_BOOL   bInit;
  LSA_UINT16 uCntInst;
  int fd_eps_adb_cause_halt_drv;
} EPS_ADB_CAUSE_HALT_DRV_TYPE, *EPS_ADB_CAUSE_HALT_DRV_PTR_TYPE;

EPS_ADB_CAUSE_HALT_DRV_TYPE g_EpsAdbCauseHaltDrv = {0};
EPS_ADB_CAUSE_HALT_DRV_PTR_TYPE g_pEpsAdbCauseHaltDrv = LSA_NULL;

/*********************************************************************
  Prototypes
 *********************************************************************/

/*********************************************************************
 Functions
 *********************************************************************/
extern void adn_adb_cause_halt(void); 

/**
 * Installs eps_adb_cause_halt_drv
 * 
 * @return ADN_OK    on success
 * @return ADN_ERROR on error
 */
int32_t eps_adb_cause_halt_drv_install()
{
  int32_t ret_val = ADN_OK;

  eps_memset(&g_EpsAdbCauseHaltDrv, 0, sizeof(g_EpsAdbCauseHaltDrv));

  g_pEpsAdbCauseHaltDrv = &g_EpsAdbCauseHaltDrv;

  /* Install Driver */
  ret_val = adn_drv_register(EPS_ADB_CAUSE_HALT_DRV_NAME, &eps_adb_cause_halt_drv_func);
  if(ret_val != ADN_OK)
  {
    return ADN_ERROR;
  }
  
  g_pEpsAdbCauseHaltDrv->bInit = LSA_TRUE; 
  
  return ADN_OK;
}

/**
 * Uninstalls eps_adb_cause_halt_drv
 * 
 * @return ADN_OK    on success
 * @return ADN_ERROR on error
 */
int32_t eps_adb_cause_halt_drv_uninstall()
{
  int32_t ret_val = ADN_OK;

  ret_val = adn_drv_unregister(EPS_ADB_CAUSE_HALT_DRV_NAME);
  
  g_pEpsAdbCauseHaltDrv->bInit = LSA_FALSE;
  g_pEpsAdbCauseHaltDrv = LSA_NULL;
  
  if(ret_val != ADN_OK)
  {
    return ADN_ERROR;
  }
  return ADN_OK;
}

/**
 * Opens the eps_adb_cause_halt_drv.
 * 
 * @param pDrvInfo Pointer to the driver info object, supplied by the IO system
 * @param oflag    Open flags (Unused since no read/write functionality in driver)
 *
 * @return ADN_OK 
 */
int32_t eps_adb_cause_halt_drv_open(adn_drv_info_t* pDrvInfo, int oflag)
{
  LSA_UNUSED_ARG(oflag);

  EPS_ASSERT(g_pEpsAdbCauseHaltDrv != LSA_NULL);
  EPS_ASSERT(g_pEpsAdbCauseHaltDrv->bInit == LSA_TRUE);
  
  g_pEpsAdbCauseHaltDrv->uCntInst++;

  pDrvInfo->state = ADN_DRV_STATE_BUSY;
  
  /* nothing to do */
  return ADN_OK;
}

/**
 * Closes the eps_adb_cause_halt_drv.
 *
 * @param pDrvInfo Pointer to the driver info object, supplied by the IO system
 *
 * @return ADN_OK
 */
int32_t eps_adb_cause_halt_drv_close(adn_drv_info_t* pDrvInfo)
{
   EPS_ASSERT(g_pEpsAdbCauseHaltDrv->uCntInst > 0);
   /// Last close?
   if (--g_pEpsAdbCauseHaltDrv->uCntInst == 0)
   {
     pDrvInfo->state = ADN_DRV_STATE_FREE;
   }
   
   /* nothing to do */
   return ADN_OK;
}

/**
 * Ioctl-Function for eps_adb_cause_halt_drv.
 * Causes Debug Exception
 *
 * @return ADN_OK    
 */
int32_t eps_adb_cause_halt_drv_ioctl(adn_drv_info_t* pDrvInfo, int cmd, void* arg)
{
   EPS_ASSERT(g_pEpsAdbCauseHaltDrv != LSA_NULL);
   EPS_ASSERT(g_pEpsAdbCauseHaltDrv->bInit == LSA_TRUE);
   EPS_ASSERT(cmd == EPS_ADB_CAUSE_HALT);
#if(ADN_ADB_USAGE == YES)
   adn_adb_cause_halt();//call this so that adb can work
   return ADN_OK;
#else
   return ADN_ERROR;
#endif
}


/*
 * eps_adb_cause_halt_drv.h
 *
 *  Created on: 02.12.2014
 *      Author: z00315mf
 */

#ifndef EPS_ADB_CAUSE_HALT_DRV_H_
#define EPS_ADB_CAUSE_HALT_DRV_H_

/*---------------------------------------------------------------------------*/
/* Defines                                                                   */
/*---------------------------------------------------------------------------*/
#define EPS_ADB_CAUSE_HALT_DRV_NAME     "/dev/eps_halt" /* Name of driver */

#define EPS_ADB_CAUSE_HALT    1

/*---------------------------------------------------------------------------*/
/* Prototypes                                                                */
/*---------------------------------------------------------------------------*/
int32_t eps_adb_cause_halt_drv_install(void);
int32_t eps_adb_cause_halt_drv_uninstall(void);

int32_t eps_adb_cause_halt_drv_open(adn_drv_info_t* pDrvInfo, int oflag);
int32_t eps_adb_cause_halt_drv_close(adn_drv_info_t* pDrvInfo);
int32_t eps_adb_cause_halt_drv_ioctl(adn_drv_info_t* pDrvInfo, int cmd, void* arg);

#endif /* EPS_ADB_CAUSE_HALT_DRV_H_ */

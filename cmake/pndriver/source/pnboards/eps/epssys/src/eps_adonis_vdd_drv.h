#ifndef EPS_ADONIS_VDD_DRV_H_
#define EPS_ADONIS_VDD_DRV_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/* Name of driver */
#define EPS_ADONIS_VDD_DRV_NAME     "/dev/adnvdd"

#define PCI_VENDOR_SIEMENS_AG               0x110A
#define PCI_DEVICE_ID_HYPERVISOR_VIRTDEV    0x4051
    
#define EPS_ADONIS_VDD_DRV_GET_PHY_ADDR     1
    
int32_t eps_adonis_vdd_drv_install(void);
int32_t eps_adonis_vdd_drv_uninstall(void);

int32_t eps_adonis_vdd_drv_open (adn_drv_info_t* pDrvInfo, int oflag);
int32_t eps_adonis_vdd_drv_close(adn_drv_info_t* pDrvInfo);
int32_t eps_adonis_vdd_drv_ioctl(adn_drv_info_t* pDrvInfo, int cmd, void* arg);
void    *eps_adonis_vdd_drv_mmap(adn_drv_info_t* pDrvInfo, size_t len, off_t off);


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif /* EPS_ADONIS_VDD_DRV_H_ */

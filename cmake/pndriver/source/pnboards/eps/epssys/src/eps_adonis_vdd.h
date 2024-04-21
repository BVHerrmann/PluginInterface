#ifndef EPS_ADONIS_VDD_H_
#define EPS_ADONIS_VDD_H_

#define EPS_ADONIS_VDD_SHM_SIZE       0x00600000
#define EPS_ADONIS_VDD_SHM_OFFSET     0x00500000
#define EPS_ADONIS_VDD_SHM_ADB_SIZE   0x00100000


int32_t     eps_adonis_vdd_init(void);
void        eps_adonis_vdd_undo_init(void);

#endif /* EPS_ADONIS_VDD_H_ */

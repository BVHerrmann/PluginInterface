#ifndef EPS_PNDEVDRV_BOARDS_H_      /* ----- reinclude-protection ----- */
#define EPS_PNDEVDRV_BOARDS_H_

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: eps_pndevdrv_boards.h                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PnDevDrv Driver                                                      */
/*                                                                           */
/*****************************************************************************/
//lint --e(537) Repeated include file - Defines are necessary, but other C files directly include psi_inc.h
#include "psi_inc.h" /*3*/
#include "eps_pndevdrv_types.h"

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
#define EPS_PNDEVDRV_ERTEC400_COUNT_MAC_IF      1
#define EPS_PNDEVDRV_ERTEC400_COUNT_MAC_PORTS   4
#define EPS_PNDEVDRV_ERTEC200_COUNT_MAC_IF      1
#define EPS_PNDEVDRV_ERTEC200_COUNT_MAC_PORTS   2
#define EPS_PNDEVDRV_SOC1_COUNT_MAC_IF          1
#define EPS_PNDEVDRV_SOC1_COUNT_MAC_PORTS       3
#define EPS_PNDEVDRV_EB200P_COUNT_MAC_IF        1
#define EPS_PNDEVDRV_EB200P_COUNT_MAC_PORTS     2
#define EPS_PNDEVDRV_HERA_COUNT_MAC_IF	        1
#define EPS_PNDEVDRV_HERA_COUNT_MAC_PORTS	    4
    
#if ( PSI_CFG_USE_EDDT == 1 )
#define EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_IF	    1
#define EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_PORTS    2
#endif


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
// Boards with IRTE-IP ASIC, driven by EDDI
#if ( PSI_CFG_USE_EDDI == 1 )
LSA_UINT16 eps_pndevdrv_open_ertec400(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 hd_id);
LSA_UINT16 eps_pndevdrv_open_ertec200(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 hd_id);
LSA_UINT16 eps_pndevdrv_open_soc1(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 hd_id);
#endif // ( PSI_CFG_USE_EDDI == 1 )

// Boards with PNIP ASIC, driven by EDDP
#if ( PSI_CFG_USE_EDDP == 1 )
LSA_UINT16 eps_pndevdrv_open_eb200p(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 hd_id);
LSA_UINT16 eps_pndevdrv_open_hera(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 hd_id);
#endif // ( PSI_CFG_USE_EDDP == 1 )

#if ( PSI_CFG_USE_EDDS == 1 )
// Intel Boards, driven by EDDS
#if defined (PSI_EDDS_CFG_HW_INTEL)
LSA_UINT16 eps_pndevdrv_open_i210(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id);
LSA_UINT16 eps_pndevdrv_open_i210IS(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id);
LSA_UINT16 eps_pndevdrv_open_i82574(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id);
#endif

// Micrel Boards, driven by EDDS
#if defined (PSI_EDDS_CFG_HW_KSZ88XX)
LSA_UINT16 eps_pndevdrv_open_ksz8841(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id);
LSA_UINT16 eps_pndevdrv_open_ksz8842(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id);
#endif

// TI Boards, driven by EDDS
#if defined (PSI_EDDS_CFG_HW_TI)
LSA_UINT16 eps_pndevdrv_open_am5728_gmac(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 const hd_id);
#endif

#if defined (PSI_EDDS_CFG_HW_IOT2000)
LSA_UINT16 eps_pndevdrv_open_iot2000(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id);
#endif
#endif // ( PSI_CFG_USE_EDDS == 1 )

#if ( PSI_CFG_USE_EDDT == 1 )
// TI Boards, driven by EDDT
LSA_UINT16 eps_pndevdrv_open_am5728_icss(EPS_PNDEVDRV_BOARD_PTR_TYPE const pBoard, EPS_PNDEV_OPEN_OPTION_PTR_TYPE const pOption, LSA_UINT16 const hd_id);
#endif // ( PSI_CFG_USE_EDDT == 1 )

// GPIO implementation, which is board specific
LSA_UINT16 eps_pndevdrv_clear_gpio (struct eps_pndev_hw_tag const* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
LSA_UINT16 eps_pndevdrv_set_gpio (struct eps_pndev_hw_tag const* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PNDEVDRV_BOARDS_H_ */

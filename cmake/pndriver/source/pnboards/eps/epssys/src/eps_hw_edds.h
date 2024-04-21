#ifndef EPS_HW_EDDS_H_              /* ----- reinclude-protection ----- */
#define EPS_HW_EDDS_H_

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
/*  F i l e               &F: eps_hw_edds.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS EDDS Interface Adaption                                              */
/*                                                                           */
/*****************************************************************************/

#include "eps_plf.h"
#include "eps_pn_drv_if.h"

#if (PSI_CFG_USE_EDDS == 1)
#include <edd_inc.h>            /* EDD Types */
#include <edds_int.h>           /* EDDS Types */

#define eps_edds_basereg_reg32(basereg, addr)	(((volatile LSA_UINT32*)(void*)basereg)[(addr)>>2])
#define eps_edds_basereg_reg16(basereg, addr)	(((volatile LSA_UINT16*)(void*)basereg)[(addr)>>1])

//Intel boards
#if defined (PSI_EDDS_CFG_HW_INTEL)
#include <intel_inc.h>          /* EDDS Lower Layer Intel Driver */

#define EPS_INTEL_PCI_VENDOR_ID     	    0x8086
#define	EPS_INTEL_PCI_DEVICE_ID_I210	    0x1533 // copper variant of the I210
#define EPS_INTEL_PCI_DEVICE_ID_I210IS      0x1536 // fiber optics variant of the I210
#define EPS_INTEL_PCI_DEVICE_ID_I82574      0x10D3

/* - Intel Register Access Defines ---------------------------------------------------------------- */
#define INTEL_RAL_0_15(number)		        (0x05400+(8*(number)))
#define INTEL_RAH_0_15(number)		        (0x05404+(8*(number)))
#endif //PSI_EDDS_CFG_HW_INTEL

//Micrel boards
#if defined (PSI_EDDS_CFG_HW_KSZ88XX)
#include <ksz88xx_inc.h>        /* EDDS Lower Layer KSZ88XX Driver */

#define EPS_KSZ88XX_PCI_VENDOR_ID     		0x16C6	
#define	EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8841	0x8841  
#define	EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8842	0x8842  

/* - KSZ88XX Register Access Defines ---------------------------------------------------------------- */
#define KSZ88XX_MARL_0_15(number)		    (0x0200+(8*(number)))
#define KSZ88XX_MARM_0_15(number)		    (0x0202+(8*(number)))
#define KSZ88XX_MARH_0_15(number)		    (0x0204+(8*(number)))
#endif //PSI_EDDS_CFG_HW_KSZ88XX

//TI boards
#if defined (PSI_EDDS_CFG_HW_TI)
#include <ti_inc.h>             /* EDDS Lower Layer TI Driver */
#include <../phy/bcm5461s/edds_phy_bcm5461s.h> /* EDDS PHY API for BCM5461S */

#define EPS_TI_PCI_VENDOR_ID     	        0x104C
#define	EPS_TI_PCI_DEVICE_ID_AM5728	        0x8888
#endif //PSI_EDDS_CFG_HW_TI

#if defined (PSI_EDDS_CFG_HW_IOT2000)
#include <ix1000_inc.h>     /* EDDS Lower Layer Intel Quark IOT2000 */

#define EPS_IX1000_PCI_VENDOR_ID     	    0x8086
#define EPS_IX1000_PCI_DEVICE_ID_IOT2000    0x0958
#endif

/*****************************************************************************/

//Intel boards
#if defined (PSI_EDDS_CFG_HW_INTEL)
typedef struct eps_edds_intel_ll_tag
{
	INTEL_LL_HANDLE_TYPE        hLL;
	EDDS_LL_TABLE_TYPE          tLLFuncs;
	INTEL_LL_PARAMETER_TYPE     sIntelLLParameter;

} EPS_EDDS_INTEL_LL_TYPE, *EPS_EDDS_INTEL_LL_PTR_TYPE;
#endif

//Micrel boards
#if defined (PSI_EDDS_CFG_HW_KSZ88XX)
typedef struct eps_edds_ksz88xx_ll_tag
{
	KSZ88XX_LL_HANDLE_TYPE 	    hLL;
	EDDS_LL_TABLE_TYPE   		tLLFuncs;
	KSZ88XX_LL_PARAMETER_TYPE   sMicrelLLParameter;

} EPS_EDDS_KSZ88XX_LL_TYPE, *EPS_EDDS_KSZ88XX_LL_PTR_TYPE;
#endif

//TI boards
#if defined (PSI_EDDS_CFG_HW_TI)
typedef struct eps_edds_ti_ll_tag
{
    TI_LL_HANDLE_TYPE 		    hLL;
    EDDS_LL_TABLE_TYPE   		tLLFuncs;
    TI_LL_PARAMETER_TYPE        sTiLLParameter;

} EPS_EDDS_TI_LL_TYPE, *EPS_EDDS_TI_LL_PTR_TYPE;
#endif

#if defined (PSI_EDDS_CFG_HW_IOT2000)
typedef struct eps_edds_ll_iot2000_tag
{
    IX1000_LL_HANDLE_TYPE       hLL;
    EDDS_LL_TABLE_TYPE          tLLFuncs;
    IX1000_LL_PARAMETER_TYPE    sIX1000LLParameter;
} EPS_EDDS_IOT2000_LL_TYPE, *EPS_EDDS_IOT2000_LL_PTR_TYPE;
#endif

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

#if defined (PSI_EDDS_CFG_HW_INTEL)
LSA_UINT16 eps_intelLL_open(EPS_EDDS_INTEL_LL_PTR_TYPE   pEddsIn,
							EPS_PNDEV_HW_PTR_TYPE	     pHwIn,
							EPS_SYS_PTR_TYPE		     pSysDevIn,
							LSA_UINT8                  * pRegBaseIn,
							LSA_UINT16 				     hd_idIn,
							LSA_UINT16				     uVendorIdIn,
							LSA_UINT16				     uDeviceIdIn);

LSA_VOID eps_i210_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid);

LSA_VOID eps_i82574_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid);
#endif

#if defined (PSI_EDDS_CFG_HW_KSZ88XX)
LSA_UINT16 eps_ksz88xx_open(EPS_EDDS_KSZ88XX_LL_PTR_TYPE   pEddsIn,
							EPS_PNDEV_HW_PTR_TYPE		   pHwIn,
							EPS_SYS_PTR_TYPE			   pSysDevIn,
							LSA_UINT8                    * pRegBaseIn,
							LSA_UINT16 					   hd_idIn,
							LSA_UINT16					   uVendorIdIn,
							LSA_UINT16					   uDeviceIdIn);

LSA_VOID eps_ksz8841_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid);

LSA_VOID eps_ksz8842_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid);
#endif

#if defined (PSI_EDDS_CFG_HW_TI)
LSA_UINT16 eps_tiLL_open(EPS_EDDS_TI_LL_PTR_TYPE   const pEddsIn,
					     EPS_PNDEV_HW_PTR_TYPE	   const pHwIn,
						 EPS_SYS_PTR_TYPE		   const pSysDevIn,
						 LSA_UINT8               * const pRegBaseIn,
						 LSA_UINT16 			   const hd_idIn,
						 LSA_UINT16				   const uVendorIdIn,
						 LSA_UINT16				   const uDeviceIdIn,
						 EPS_MAC_PTR_TYPE          const pPndevdrvArrayMacAdr);

LSA_VOID eps_am5728_gmac_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid);
#endif

#if defined (PSI_EDDS_CFG_HW_IOT2000)
LSA_VOID eps_iot2000_FillDevNrtMemParams(LSA_VOID * const pBoardInfoVoid);
LSA_UINT16 eps_iot2000LL_open(EPS_EDDS_IOT2000_LL_PTR_TYPE  pEddsIn,
                              EPS_PNDEV_HW_PTR_TYPE         pHwIn,
                              EPS_SYS_PTR_TYPE              pSysDevIn,
                              LSA_UINT8*                    pRegBaseIn,
                              LSA_UINT16                    hd_idIn,
                              LSA_UINT16                    uVendorIdIn,
                              LSA_UINT16                    uDeviceIdIn,
                              EPS_MAC_PTR_TYPE              pPndevdrvArrayMacAdr);
#endif

#endif //PSI_CFG_USE_EDDS

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_HW_EDDS_H_ */

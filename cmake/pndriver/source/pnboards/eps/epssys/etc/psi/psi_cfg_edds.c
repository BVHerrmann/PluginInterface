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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: psi_cfg_edds.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PSI EDDS HW system adaption                            */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      32008
#define LTRC_ACT_MODUL_ID  32008

#include <eps_sys.h>

#if (( PSI_CFG_USE_EDDS == 1 ) && (PSI_CFG_USE_HD_COMP == 1))

#include <eps_cp_hw.h>
#include <eps_isr.h>

/*=============================================================================*/
/*        Address translation                                                  */
/*=============================================================================*/

#ifndef PSI_EDDS_PTR_TO_ADDR_DEV_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR_DEV_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr)
{
	LSA_UINT32                    phy_addr;
	LSA_UINT8                   * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE       p_board;
	EPS_SYS_PTR_TYPE        const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT(hSysDev != LSA_NULL);
	EPS_ASSERT(pAddr != LSA_NULL);
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

	p_board = eps_hw_get_board_store(pSys->hd_nr);
	EPS_ASSERT(p_board != 0);
	EPS_ASSERT(p_board->edds.is_valid);

	base_ptr = (LSA_UINT8*)p_board->dev_mem.base_ptr;

	EPS_ASSERT((LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr);

	phy_addr = ((LSA_UINT32)Ptr) - ((LSA_UINT32)base_ptr);
	phy_addr += p_board->dev_mem.phy_addr;
	*pAddr    = phy_addr;
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_PTR_TO_ADDR64_DEV_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR64_DEV_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT64        * pAddr)
{
	LSA_UINT32                    phy_addr;
	LSA_UINT8                   * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE       p_board;
	EPS_SYS_PTR_TYPE        const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT(hSysDev != LSA_NULL);
	EPS_ASSERT(pAddr != LSA_NULL);
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

	p_board = eps_hw_get_board_store(pSys->hd_nr);
	EPS_ASSERT(p_board != 0);
	EPS_ASSERT(p_board->edds.is_valid);

	base_ptr = (LSA_UINT8*)p_board->dev_mem.base_ptr;

	EPS_ASSERT((LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr);

	phy_addr  = ((LSA_UINT32)Ptr) - ((LSA_UINT32)base_ptr);
	phy_addr += p_board->dev_mem.phy_addr;
	*pAddr    = (LSA_UINT64)phy_addr;
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_PTR_TO_ADDR_TX_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR_TX_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr)
{
	LSA_UINT32                    phy_addr;
	LSA_UINT8                   * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE       p_board;
	EPS_SYS_PTR_TYPE        const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT(hSysDev != LSA_NULL);
	EPS_ASSERT(pAddr != LSA_NULL);
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

	p_board = eps_hw_get_board_store(pSys->hd_nr);
	EPS_ASSERT(p_board != 0);
	EPS_ASSERT(p_board->edds.is_valid);

	base_ptr = (LSA_UINT8*)p_board->nrt_tx_mem.base_ptr;

	EPS_ASSERT((LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr);

	phy_addr = ((LSA_UINT32)Ptr) - ((LSA_UINT32)base_ptr);
	phy_addr += p_board->nrt_tx_mem.phy_addr;
	*pAddr    = phy_addr;
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_PTR_TO_ADDR64_TX_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR64_TX_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT64        * pAddr)
{
	LSA_UINT32                    phy_addr;
	LSA_UINT8                   * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE       p_board;
	EPS_SYS_PTR_TYPE        const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT(hSysDev != LSA_NULL);
	EPS_ASSERT(pAddr != LSA_NULL);
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

	p_board = eps_hw_get_board_store(pSys->hd_nr);
	EPS_ASSERT(p_board != 0);
	EPS_ASSERT(p_board->edds.is_valid);

	base_ptr = (LSA_UINT8*)p_board->nrt_tx_mem.base_ptr;

	EPS_ASSERT((LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr);

	phy_addr  = ((LSA_UINT32)Ptr) - ((LSA_UINT32)base_ptr);
	phy_addr += p_board->nrt_tx_mem.phy_addr;
	*pAddr    = (LSA_UINT64)phy_addr;
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_PTR_TO_ADDR_RX_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR_RX_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr)
{
	LSA_UINT32                    phy_addr;
	LSA_UINT8                   * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE       p_board;
	EPS_SYS_PTR_TYPE        const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT(hSysDev != LSA_NULL);
	EPS_ASSERT(pAddr != LSA_NULL);
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

	p_board = eps_hw_get_board_store(pSys->hd_nr);
	EPS_ASSERT(p_board != 0);
	EPS_ASSERT(p_board->edds.is_valid);

	base_ptr = (LSA_UINT8*)p_board->nrt_rx_mem.base_ptr;

	EPS_ASSERT((LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr);

	phy_addr = ((LSA_UINT32)Ptr) - ((LSA_UINT32)base_ptr);
	phy_addr += p_board->nrt_rx_mem.phy_addr;
	*pAddr    = phy_addr;
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_PTR_TO_ADDR64_RX_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR64_RX_UPPER(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT64        * pAddr)
{
	LSA_UINT32                    phy_addr;
	LSA_UINT8                   * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE       p_board;
	EPS_SYS_PTR_TYPE        const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT(hSysDev != LSA_NULL);
	EPS_ASSERT(pAddr != LSA_NULL);
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

	p_board = eps_hw_get_board_store(pSys->hd_nr);
	EPS_ASSERT(p_board != 0);
	EPS_ASSERT(p_board->edds.is_valid);

	base_ptr = (LSA_UINT8*)p_board->nrt_rx_mem.base_ptr;

	EPS_ASSERT((LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr);

	phy_addr  = ((LSA_UINT32)Ptr) - ((LSA_UINT32)base_ptr);
	phy_addr += p_board->nrt_rx_mem.phy_addr;
	*pAddr    = (LSA_UINT64)phy_addr;
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM
LSA_VOID PSI_EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length)
{
	EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

    PSI_NRT_ALLOC_RX_MEM(upper_mem_ptr_ptr, length, pSys->hd_nr, LSA_COMP_ID_EDDS);
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_FREE_RX_TRANSFER_BUFFER_MEM
LSA_VOID PSI_EDDS_FREE_RX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr)
{
	EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

    PSI_NRT_FREE_RX_MEM(ret_val_ptr, upper_mem_ptr, pSys->hd_nr, LSA_COMP_ID_EDDS);
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM
LSA_VOID PSI_EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length)
{
	EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

    PSI_NRT_ALLOC_TX_MEM(upper_mem_ptr_ptr, length, pSys->hd_nr, LSA_COMP_ID_EDDS);
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_FREE_TX_TRANSFER_BUFFER_MEM
LSA_VOID PSI_EDDS_FREE_TX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr)
{
	EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

    PSI_NRT_FREE_TX_MEM(ret_val_ptr, upper_mem_ptr, pSys->hd_nr, LSA_COMP_ID_EDDS);
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_ALLOC_DEV_BUFFER_MEM
LSA_VOID PSI_EDDS_ALLOC_DEV_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length)
{
	EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

	PSI_DEV_ALLOC_MEM(upper_mem_ptr_ptr, length, pSys->hd_nr, LSA_COMP_ID_EDDS);
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef PSI_EDDS_FREE_DEV_BUFFER_MEM
LSA_VOID PSI_EDDS_FREE_DEV_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr)
{
	EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

	PSI_DEV_FREE_MEM(ret_val_ptr, upper_mem_ptr, pSys->hd_nr, LSA_COMP_ID_EDDS);
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        NOTIFY-FILL-FUNCTION                                                 */
/*=============================================================================*/

#ifndef PSI_EDDS_NOTIFY_FILL_SETTINGS
LSA_BOOL PSI_EDDS_NOTIFY_FILL_SETTINGS(
    PSI_SYS_HANDLE const hSysDev,
    LSA_BOOL       const bFillActive,
    LSA_BOOL       const bIOConfigured,
    LSA_BOOL       const bHsyncModeActive)
{
    LSA_BOOL bRetVal;
    
	EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
    EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

    bRetVal = eps_edds_notify_fill_settings(hSysDev, bFillActive, bIOConfigured, bHsyncModeActive);

    return bRetVal;
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        NOTIFY-SCHEDULER-FUNCTION                                            */
/*=============================================================================*/

#ifndef PSI_EDDS_DO_NOTIFY_SCHEDULER
LSA_VOID PSI_EDDS_DO_NOTIFY_SCHEDULER(
    PSI_SYS_HANDLE const hSysDev)
{
    EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
    EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

	eps_edds_do_notify_scheduler(hSysDev);
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        RETRIGGER-SCHEDULER-FUNCTION                                         */
/*=============================================================================*/

#ifndef PSI_EDDS_DO_RETRIGGER_SCHEDULER
LSA_VOID PSI_EDDS_DO_RETRIGGER_SCHEDULER(
    PSI_SYS_HANDLE const hSysDev)
{
    EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
    EPS_ASSERT(pSys != LSA_NULL);

	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDS);

    eps_edds_do_retrigger_scheduler(hSysDev);
}
#else
#error "by design a function!"
#endif

#endif //PSI_CFG_USE_EDDS

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

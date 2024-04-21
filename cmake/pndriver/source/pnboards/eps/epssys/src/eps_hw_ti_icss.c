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
/*  F i l e               &F: eps_hw_ti_icss.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS TI ICSS adpation                                                     */
/*                                                                           */
/*****************************************************************************/

#define EPS_MODULE_ID      20029
#define LTRC_ACT_MODUL_ID  20029

#include <eps_sys.h>          /* Types / Prototypes / Funcs */

#if ( PSI_CFG_USE_EDDT == 1 ) 
/*---------------------------------------------------------------------------*/

#include <eps_trc.h>		/* Tracing						*/
#include <eps_cp_hw.h>		/* EPS CP PSI adaption			*/
#include <eps_hw_ti_icss.h>	/* EPS HW TI ICSS adaption	    */

/* NRT / CRT Ram Sizes */
#define SDRAM_NRT_SIZE_AM5728_ICSS_RECOMMENDED      0x003FF000   // 4MB-4KB
#define SDRAM_CRT_SIZE_AM5728_ICSS_RECOMMENDED      0x00001000   // 4KB
#define SDRAM_SIZE_AM5728_ICSS_RECOMMENDED (SDRAM_NRT_SIZE_AM5728_ICSS_RECOMMENDED + SDRAM_CRT_SIZE_AM5728_ICSS_RECOMMENDED)

/*----------------------------------------------------------------------------*/
/*  Defines                                                                   */
/*----------------------------------------------------------------------------*/

// Partitioning of the DEV and NRT memory for AM5728 ICSS, all values in percent
#define CP_MEM_AM5728_ICSS_PERCENT_DEV          10
#define CP_MEM_AM5728_ICSS_PERCENT_NRT_TX       45
#define CP_MEM_AM5728_ICSS_PERCENT_NRT_RX       45

#define CP_MEM_AM5728_ICSS_PERCENT_TOTAL (CP_MEM_AM5728_ICSS_PERCENT_DEV + CP_MEM_AM5728_ICSS_PERCENT_NRT_TX + CP_MEM_AM5728_ICSS_PERCENT_NRT_RX)

#if (CP_MEM_AM5728_ICSS_PERCENT_TOTAL != 100)
#error "Sum has to be 100 percent!"
#endif

/*---------------------------------------------------------------------------*/

LSA_VOID eps_hw_am5728_icss_calculateSizeInSDRAM( LSA_UINT32 Size, LSA_UINT32 * pSizeNRT, LSA_UINT32 * pSizeCRT )
{
	// calculate SDRAM mem ranges for CRT and NRT area based on avaliable SDRAM

	EPS_ASSERT( pSizeNRT != LSA_NULL );
	EPS_ASSERT( pSizeCRT != LSA_NULL );
	EPS_ASSERT( 0 == (Size % 4) );  // 4 byte alligned ?

	// check for required ram size
	if ( SDRAM_SIZE_AM5728_ICSS_RECOMMENDED <= Size ) // SDRAM size allows full range ?
	{
		*pSizeCRT = SDRAM_CRT_SIZE_AM5728_ICSS_RECOMMENDED;
		*pSizeNRT = SDRAM_NRT_SIZE_AM5728_ICSS_RECOMMENDED;
    }
	else
	{
		EPS_ASSERT( Size >= SDRAM_CRT_SIZE_AM5728_ICSS_RECOMMENDED );

		*pSizeCRT = SDRAM_CRT_SIZE_AM5728_ICSS_RECOMMENDED;
		*pSizeNRT = Size - SDRAM_CRT_SIZE_AM5728_ICSS_RECOMMENDED;
	}

	EPS_ASSERT( Size >= *pSizeNRT + *pSizeCRT );
}

LSA_VOID eps_am5728_icss_FillDevNrtMemParams( LSA_VOID * const pBoardInfoVoid )
{
	EPS_BOARD_INFO_TYPE	* const pboardInfo           = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;
    LSA_UINT32            const dev_nrt_mem_size_all = pboardInfo->edds.sdram_NRT.size;

    // setup DEV and NRT memory pools

    pboardInfo->dev_mem.base_ptr    = pboardInfo->edds.sdram_NRT.base_ptr;
    pboardInfo->dev_mem.phy_addr    = pboardInfo->edds.sdram_NRT.phy_addr;
    pboardInfo->dev_mem.size        = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_AM5728_ICSS_PERCENT_DEV);

    pboardInfo->nrt_tx_mem.base_ptr = pboardInfo->dev_mem.base_ptr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.phy_addr = pboardInfo->dev_mem.phy_addr + pboardInfo->dev_mem.size;
    pboardInfo->nrt_tx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_AM5728_ICSS_PERCENT_NRT_TX);

    pboardInfo->nrt_rx_mem.base_ptr = pboardInfo->nrt_tx_mem.base_ptr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.phy_addr = pboardInfo->nrt_tx_mem.phy_addr + pboardInfo->nrt_tx_mem.size;
    pboardInfo->nrt_rx_mem.size     = EPS_CALC_MEM_SIZE(dev_nrt_mem_size_all, CP_MEM_AM5728_ICSS_PERCENT_NRT_RX);

	EPS_ASSERT(pboardInfo->dev_mem.base_ptr != LSA_NULL);
}

/*----------------------------------------------------------------------------*/
#endif // PSI_CFG_USE_EDDT

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

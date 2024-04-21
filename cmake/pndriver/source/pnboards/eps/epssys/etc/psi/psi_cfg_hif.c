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
/*  F i l e               &F: psi_cfg_hif.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PSI HIF system adaption                                */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID     32005
#define LTRC_ACT_MODUL_ID 32005

#include "eps_sys.h"
#include "eps_plf.h"
#include "eps_app.h"           
#include "eps_hif_drv_if.h"
#include "eps_register.h" // EPS_CAST_TO_VOID_PTR

#if ( PSI_CFG_USE_HIF == 1 )
/*----------------------------------------------------------------------------*/

PSI_FILE_SYSTEM_EXTENSION( PSI_MODULE_ID )

/*----------------------------------------------------------------------------*/
/* HIF LD                                                                     */
/*----------------------------------------------------------------------------*/

#ifndef PSI_HIF_GET_LD_UPPER_HANDLE
LSA_VOID PSI_HIF_GET_LD_UPPER_HANDLE(
	LSA_UINT16  *hif_handle_ptr )
{
    LSA_UNUSED_ARG( hif_handle_ptr );
    *hif_handle_ptr = g_pEpsData->hUpperLD;
}
#else
# error "by design a function!"
#endif

#ifndef PSI_HIF_LD_U_GET_PARAMS
LSA_VOID PSI_HIF_LD_U_GET_PARAMS(
	LSA_UINT16           *ret_val_ptr,
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID_PTR_TYPE    pPars )
{
    PSI_HD_SYS_ID_PTR_TYPE pHdSys;
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
	LSA_BOOL bFWDownload = LSA_FALSE;

	PSI_ASSERT( pSys != 0 );

    pHdSys = &pSys->hd_sys_id;
    
    if (pSys->ldRunsOnType == PSI_LD_RUNS_ON_BASIC)
    {
        bFWDownload = LSA_TRUE;
    }
    
    eps_hw_connect(0/*this is the LD*/, bFWDownload, pHdSys);  
	eps_hif_drv_if_get_params(pSys->hd_nr, LSA_TRUE, pPars);

	*ret_val_ptr = LSA_RET_OK;
}
#else
# error "by design a function!"
#endif

#ifndef PSI_HIF_LD_L_GET_PARAMS
LSA_VOID PSI_HIF_LD_L_GET_PARAMS(
	LSA_UINT16           *ret_val_ptr,
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID_PTR_TYPE    pPars )
{
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
   	
    PSI_ASSERT( pSys != 0 );

	eps_hif_drv_if_get_params(pSys->hd_nr, LSA_FALSE, pPars);

	*ret_val_ptr = LSA_RET_OK;
}
#else
# error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_HIF_LD_ENABLE_ISR
LSA_VOID PSI_HIF_LD_ENABLE_ISR(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH )
{
    LSA_UINT16 retVal;
    PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	retVal = eps_hif_drv_if_enable_isr(pSys->hd_nr, pSys->is_upper, hH);
    EPS_ASSERT(EPS_HIF_DRV_RET_OK == retVal);
}
#else
# error "by design a function!"
#endif

#ifndef PSI_HIF_LD_DISABLE_ISR
LSA_VOID PSI_HIF_LD_DISABLE_ISR(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH )
{
    LSA_UINT16 retVal;
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );
	LSA_UNUSED_ARG(hH);

	retVal = eps_hif_drv_if_disable_isr(pSys->hd_nr, pSys->is_upper);
    EPS_ASSERT(EPS_HIF_DRV_RET_OK == retVal);
}
#else
# error "by design a function!"
#endif

#ifndef PSI_HIF_LD_SEND_IRQ
LSA_VOID PSI_HIF_LD_SEND_IRQ(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH )
{
    LSA_UINT16 retVal;
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );
	LSA_UNUSED_ARG(hH);

	retVal = eps_hif_drv_if_send_isr(pSys->hd_nr, pSys->is_upper);
    EPS_ASSERT(EPS_HIF_DRV_RET_OK == retVal);
}
#else
# error "by design a function!"
#endif

#ifndef  PSI_HIF_LD_ACK_IRQ
LSA_VOID PSI_HIF_LD_ACK_IRQ(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH )
{
    LSA_UINT16 retVal;
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );
	LSA_UNUSED_ARG(hH);

	retVal = eps_hif_drv_if_ack_isr(pSys->hd_nr, pSys->is_upper);
    EPS_ASSERT(EPS_HIF_DRV_RET_OK == retVal);
}
#else
# error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_HIF_LD_REQUEST_UPPER_DONE
LSA_VOID PSI_HIF_LD_REQUEST_UPPER_DONE(
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE  hif_request_upper_done_ptr,
	LSA_VOID_PTR_TYPE                ptr_upper_rqb,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	LSA_UNUSED_ARG( sys_ptr );

	// Call the central EPS CBF
	// Note: posting to  thread for context change is done in CBF
	(hif_request_upper_done_ptr)( ptr_upper_rqb );
}
#else
# error "by design a function!"
#endif

#ifndef PSI_HIF_LD_L_SYSTEM_REQUEST_DONE
LSA_VOID PSI_HIF_LD_L_SYSTEM_REQUEST_DONE(
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE  hif_system_request_done_ptr,
	LSA_VOID_PTR_TYPE                ptr_upper_rqb )
{
	// Call the central EPS CBF
	// Note: posting to  thread for context change is done in CBF
	(hif_system_request_done_ptr)( ptr_upper_rqb );
}
#else
# error "by design a function!"
#endif

#ifndef PSI_HIF_LD_U_SYSTEM_REQUEST_DONE
LSA_VOID PSI_HIF_LD_U_SYSTEM_REQUEST_DONE(
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE   hif_system_request_done_ptr,
	LSA_VOID_PTR_TYPE                 ptr_upper_rqb )
{
	// Call the central EPS CBF
	// Note: posting to  thread for context change is done in CBF
	(hif_system_request_done_ptr)( ptr_upper_rqb );
}
#else
# error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
/* HIF HD                                                                     */
/*----------------------------------------------------------------------------*/

#ifndef PSI_HIF_GET_HD_LOWER_HANDLE
LSA_VOID PSI_HIF_GET_HD_LOWER_HANDLE(
	LSA_UINT16      *hif_handle_ptr,
	LSA_UINT16      hd_id )
{
	LSA_UNUSED_ARG( hif_handle_ptr );
	LSA_UNUSED_ARG( hd_id );

	*hif_handle_ptr = g_pEpsData->hLowerHD;
}
#else
# error "by design a function!"
#endif

#ifndef PSI_HIF_HD_U_GET_PARAMS
LSA_VOID PSI_HIF_HD_U_GET_PARAMS(
	LSA_UINT16            *ret_val_ptr,
	PSI_SYS_HANDLE        hSysDev,
	LSA_VOID_PTR_TYPE     pPars )
{
	PSI_HD_SYS_ID_PTR_TYPE pHdSys;
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;

	PSI_ASSERT( pSys != 0 );

	pHdSys = &pSys->hd_sys_id;

	eps_hw_connect(pSys->hd_nr, LSA_TRUE, pHdSys);
	eps_hif_drv_if_get_params(pSys->hd_nr, LSA_TRUE, pPars);

	*ret_val_ptr = LSA_RET_OK;
}
#else
# error "by design a function!"
#endif

#ifndef PSI_HIF_HD_L_GET_PARAMS
LSA_VOID PSI_HIF_HD_L_GET_PARAMS(
	LSA_UINT16           *ret_val_ptr,
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID_PTR_TYPE    pPars )
{
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	eps_hif_drv_if_get_params(pSys->hd_nr, LSA_FALSE, pPars);

	*ret_val_ptr = LSA_RET_OK;
}
#else
# error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_HIF_HD_ENABLE_ISR
LSA_VOID PSI_HIF_HD_ENABLE_ISR(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH )
{
    LSA_UINT16 retVal;
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	retVal = eps_hif_drv_if_enable_isr(pSys->hd_nr, pSys->is_upper, hH);
    EPS_ASSERT(EPS_HIF_DRV_RET_OK == retVal);
}
#else
# error "by design a function!"
#endif

#ifndef PSI_HIF_HD_DISABLE_ISR
LSA_VOID PSI_HIF_HD_DISABLE_ISR(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH )
{
    LSA_UINT16 retVal;
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );
	LSA_UNUSED_ARG(hH);

	retVal = eps_hif_drv_if_disable_isr(pSys->hd_nr, pSys->is_upper);
    EPS_ASSERT(EPS_HIF_DRV_RET_OK == retVal);
}
#endif

#ifndef PSI_HIF_HD_SEND_IRQ
LSA_VOID PSI_HIF_HD_SEND_IRQ(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH )
{
    LSA_UINT16 retVal;
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );
	LSA_UNUSED_ARG(hH);

	retVal = eps_hif_drv_if_send_isr(pSys->hd_nr, pSys->is_upper);
    EPS_ASSERT(EPS_HIF_DRV_RET_OK == retVal);
}
#else
# error "by design a function!"
#endif

#ifndef  PSI_HIF_HD_ACK_IRQ
LSA_VOID PSI_HIF_HD_ACK_IRQ(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH )
{
    LSA_UINT16 retVal;
	PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );
	LSA_UNUSED_ARG(hH);

	retVal = eps_hif_drv_if_ack_isr(pSys->hd_nr, pSys->is_upper);
    EPS_ASSERT(EPS_HIF_DRV_RET_OK == retVal);
}
#else
# error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_HIF_HD_L_SYSTEM_REQUEST_DONE
LSA_VOID PSI_HIF_HD_L_SYSTEM_REQUEST_DONE(
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE  hif_system_request_done_ptr,
	LSA_VOID_PTR_TYPE                ptr_upper_rqb )
{
	// Note: in EPS system PSI LD opens HD lower (only in shortcut mode)
	psi_system_request_done( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)hif_system_request_done_ptr, ptr_upper_rqb );
}
#else
# error "by design a function!"
#endif

/****************************************************************************
*  Name:        PSI_HIF_MEMCPY_BUF_DMA()
*  Purpose:     - copies data from host to shared-mem without swapping 
*                 if the size is bigger than 0.
*               - If used device supports dma it is used for better performance 
*                 otherwise memcpy is used.
*               - writes the magic value HIF_DMA_FINISHED_FLAG after copy is finished.
*  Input:       src, destination address, size and Device handle
*  Return:      -
****************************************************************************/
#ifndef PSI_HIF_MEMCPY_BUF_DMA
LSA_VOID PSI_HIF_MEMCPY_BUF_DMA(
    LSA_VOID_PTR_TYPE dst,
    LSA_VOID const*   src,
    LSA_UINT32        size,
    PSI_SYS_HANDLE    sys_handle )
{
    PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)sys_handle;
    PSI_ASSERT( pSys );
    
	/* Skip the copy process if the size is 0. eps_hif_drv_if_dma_copy also writes finished info before/after copying the buffer. */
	if (size != 0)
	{
	    eps_hif_drv_if_dma_copy( pSys->hd_nr, pSys->is_upper, dst, (LSA_VOID_PTR_TYPE) src, size );
	}
}
#else
# error "by design a function!"
#endif

/****************************************************************************
*  Name:        PSI_HIF_MEMCPY_BUF()
*  Purpose:     - copies data from host to shared-mem without swapping 
*                 if the size is bigger than 0.
*               - writes the magic value HIF_DMA_FINISHED_FLAG after copy is finished.
*  Input:       src, destination address and size
*  Return:      -
****************************************************************************/
#ifndef PSI_HIF_MEMCPY_BUF
LSA_VOID PSI_HIF_MEMCPY_BUF(
    LSA_VOID_PTR_TYPE dst,
    LSA_VOID const*   src,
    LSA_UINT32        size)
{
    LSA_UINT32* pCopyFinsihedFlag = (LSA_UINT32*)dst;
    LSA_UINT32* dataDst = (LSA_UINT32*) EPS_CAST_TO_VOID_PTR(((LSA_UINT8*)(dst)) + 4);
    /* memcopy of length 0 is handled in eps_memcpy, but we also write infos before we copy the buffer (Magic key in pCopyFinsihedFlag). Skip the copy process if the size is 0 */
    if (size != 0)
    {
        // invalidate the copy status flag
        *pCopyFinsihedFlag = 0;
        // copy data
        eps_memcpy(dataDst, src, size);
        // set copy status flag
        *pCopyFinsihedFlag = HIF_DMA_FINISHED_FLAG;
    }
}
#else
# error "by design a function!"
#endif

/****************************************************************************
*  Name:        PSI_HIF_MEMCPY_BUF_WAIT_TX_FINISHED()
*  Purpose:     - wait until a buffer is completely transferred to the shm 
*			    - used before a buffer is read from shm
*  Input:       src
*  Return:      -
****************************************************************************/
#ifndef PSI_HIF_MEMCPY_BUF_WAIT_TX_FINISHED
LSA_VOID PSI_HIF_MEMCPY_BUF_WAIT_TX_FINISHED(
    LSA_VOID const* src)
{
    // wait for tx transfer to shm to be finished, 
    while (*(LSA_UINT32*)src != HIF_DMA_FINISHED_FLAG)
    {
        ; // active wait till transfer is finished
    }
}
#else
# error "by design a function!"
#endif


/****************************************************************************
*  Name:        PSI_HIF_BUF_DMA_PEND_FREE()
*  Purpose:     - Use dma to set the free dma buffer flag. 
*                 Buffer used by dma can only be freed after dma transfer 
*                 finished. HIF_DMA_FREE_BUF_FLAG will be set by dma after 
*                 all current transfers finished.
*  Input:       buffer -> address of buffer
*  Output:      pReadyForFree -> address where dma writes HIF_DMA_FREE_BUF_FLAG, 
*                                after buffer is no longer in use.
*  Return:      - LSA_FALSE when buffer is not in use by dma.
*               - LSA_TRUE when dma pended a free for buffer.
****************************************************************************/
#ifndef PSI_HIF_BUF_DMA_PEND_FREE
LSA_BOOL PSI_HIF_BUF_DMA_PEND_FREE(
    LSA_VOID_PTR_TYPE buffer,
    LSA_VOID_PTR_TYPE *pReadyForFree,
    PSI_SYS_HANDLE    sys_handle )
{
    PSI_HIF_SYS_PTR_TYPE pSys = (PSI_HIF_SYS_PTR_TYPE)sys_handle;
    PSI_ASSERT( pSys );
    
    return eps_hif_drv_if_dma_pend_free( pSys->hd_nr, pSys->is_upper, buffer, pReadyForFree );
}
#else
# error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
#endif /* PSI_CFG_USE_HIF */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

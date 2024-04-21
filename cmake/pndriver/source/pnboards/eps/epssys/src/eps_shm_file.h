#ifndef EPS_SHM_FILE_H_
#define EPS_SHM_FILE_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: eps_shm_file.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Shared memory file transfer module                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif
    
#include "eps_sys.h"
#include <eps_plf.h>   
#include <eps_app.h>   
#include "eps_rtos.h"
  
/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef struct eps_shm_file_transfer_tag {
	struct
	{
		LSA_UINT32   bReqStart;                 /**< Start request flag                     */
		LSA_UINT32   uReqPacketRead;            /**< Counter for read packets               */
		LSA_UINT32   isHD;                      /**< is running on hd                       */
		LSA_UINT32   pndevBoard;                /**< Board type @see EPS_PNDEV_BOARD_TYPE   */
		LSA_UINT32   pndevBoardDetail;          /**< Board type @see EPS_EDDP_BOARD_TYPE    */
        LSA_UINT32   FileType;                  /**< File type @see EPS_APP_FILE_TAG_ENUM   */
	} req;
	struct
	{
		LSA_UINT32 bRspDone;                    /**< Request was handled, response is available         */
		LSA_UINT32 bRspErr;                     /**< Error flag                                         */
		LSA_UINT32 uFileSize;                   /**< Size of file which is transfered                   */
		LSA_UINT32 uActPacket;                  /**< Actual packet count                                */
		LSA_UINT32 uActPacketSize;              /**< Size of actual transfered packet                   */
		LSA_UINT8  uData[EPS_APP_FILE_CHUNK_SIZE]; /**< Data which is transfered                        */
		LSA_UINT32 bTransferDone;               /**< Transfer finished                                  */
		LSA_UINT32 bTransferDoneCnf;	        /**< Transfer finsihed confirmation                     */
	} rsp;
} EPS_SHM_FILE_TRANSFER_TYPE, *EPS_SHM_FILE_TRANSFER_PTR_TYPE;


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID eps_shm_file_init              (LSA_UINT8* pBase);
LSA_VOID eps_shm_file_undo_init         (LSA_VOID);

LSA_VOID eps_shm_file_alloc             (EPS_APP_FILE_TAG_PTR_TYPE pFile);
LSA_VOID eps_shm_file_free              (EPS_APP_FILE_TAG_PTR_TYPE pFile);


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_SHM_IF_H_ */

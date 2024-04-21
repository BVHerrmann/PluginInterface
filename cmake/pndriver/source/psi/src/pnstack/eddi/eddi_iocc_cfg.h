#ifndef EDDI_IOCC_CFG_H          //reinclude-protection
#define EDDI_IOCC_CFG_H

#ifdef __cplusplus          //If C++ - compiler: Use C linkage
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
/*  C o m p o n e n t     &C: EDDI                                      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_iocc_cfg.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for EDDI IOCC                                              */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/
/** @file eddi_iocc_cfg.h "Configuration for EDDI IOCC"  **/
/**
@defgroup eddi_iocc_cfg EDDI IOCC configuration
@ingroup EDDI_IOCC

@{
**/
    #define EDDI_IOCC_CFG_MAX_INSTANCES    1
    /**< Maximum number of instances supported. Each instance consists of two IOCC channels. **/

    #define EDDI_IOCC_UPPER_HANDLE_TYPE LSA_VOID*
    /**< Type of the upper handle supplied by the application during @ref eddi_IOCC_Setup **/

    //#define EDDI_IOCC_CFG_DISABLE_LOCK_COMPLETE_TRANSFER
    /**< Normally, eddi_IOCC protects the complete data transfer against reentrance, i.e. the register access for setting up the transfer, and the transfer itself, including waiting for its end (synchronously). If this define is set, only the register access is protected, the transfer itself not. Before a transfer is started, eddi_IOCC checks wether the channel used is still in use and will return EDD_STS_OK_PENDING in this case. **/
    /** @todo how can the spec be referenced ? **/
    
    //#define EDDI_IOCC_CFG_DISABLE_USE_CHECK
    /**< If this define is set, checking wether an IOCC channel is still transfering data before a new transfer is started, is disabled. Setting this flag saves valuable run time as the necessary read access to IOCC is omitted. Only set if  implementation is stable and it is made sure that no new transfer is started before the previously has been finished. \n
        If EDDI_IOCC_CFG_DISABLE_USE_CHECK is set, EDDI_IOCC_CFG_DISABLE_LOCK_COMPLETE_TRANSFER is not allowed to be set! **/
    /** @todo how can the spec be referenced ? **/

    //#define EDDI_IOCC_CFG_NO_FATAL_FILE_INFO
    /**< If defined, no file info will be passed to EDDI_IOCC_Excp. **/

    //#define EDDI_IOCC_CFG_DISABLE_HANDLE_CHECKS
    /**< If this define is set, the IOCCHandle is not checked for validity in the working functions (speed). **/

/** @} **/ //end of group eddi_iocc_cfg 
#if defined __cplusplus
}
#endif

#endif //EDDI_IOCC_CFG_H

/*****************************************************************************/
/*  end of file eddi_iocc_cfg.h/txt                                          */
/*****************************************************************************/

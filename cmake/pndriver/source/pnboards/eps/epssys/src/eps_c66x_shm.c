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
/*  F i l e               &F: eps_c66x_shm.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS C66x Shared Memory                                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20092
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <eps_sys.h>        /* Types / Prototypes         */
#include <eps_trc.h>


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define C66_RELEASE_CMD_OFFSET      (shm_size - 8)
#define C66_READY_CMD_OFFSET        (shm_size - 4)

#define RELEASE_CMD         0xDEADBEEF
#define READY_CMD           0x00C0FFEE


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------
// from linker script
extern LSA_UINT32 __C66X_SHM_START__;
extern LSA_UINT32 __C66X_SHM_END__;

static LSA_UINT32 shm_start_addr;
static LSA_UINT32 shm_size;


/**
 * Initialize shared memory section for communication with dsp1.
 */
LSA_VOID eps_c66x_shm_init()
{
    LSA_UINT32 i;

    shm_start_addr = (LSA_UINT32)__C66X_SHM_START__;    
    shm_size       = (LSA_UINT32)__C66X_SHM_END__ - (LSA_UINT32)__C66X_SHM_START__;
    
    // Set shared memory section to zero
    for(i=0;i<shm_size;i++)
    {
        *((LSA_UINT8*)(shm_start_addr + i)) = 0;
    }
}


/**
 * Write word to shared memory at given offset.
 * 
 * @param offset [in] Address offset in byte to write to
 * @param val [in] Word which is written to memory
 */
LSA_VOID eps_c66x_shm_write(LSA_UINT32 offset, LSA_UINT32 val)
{
    if(offset < shm_size)
    {
        *((LSA_UINT32*)(shm_start_addr + offset)) = val;
    }
    else
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_c66x_shm_write(): Memory access beyond shm size");
        EPS_FATAL(0);
    }
}

/**
 * Read word from shared memory at specified offset.
 * 
 * @param offset [in] Address offset in byte to read from
 * @return Read word
 */
LSA_UINT32 eps_c66x_shm_read(LSA_UINT32 offset)
{
    LSA_UINT32 val = 0;
    
    if(offset < shm_size)
    {
        val = *((LSA_UINT32*)(shm_start_addr + offset));
    }
    else
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_c66x_shm_read(): Memory access beyond shm size");
        EPS_FATAL(0);
    }
    
    return val;
}

/**
 * Write release command to shared memory. DSP1 should wait before program execution
 * for this startup command.
 */
LSA_VOID eps_c66x_shm_write_c66xReleaseCmd()
{
    eps_c66x_shm_write(C66_RELEASE_CMD_OFFSET, RELEASE_CMD);
}


/**
 *  When program startup of dsp1 is finished, this command should be written
 *  to signal successfull startup to main core
 */
LSA_VOID eps_c66x_shm_wait_c66xReadyCmd()
{
    while(READY_CMD != eps_c66x_shm_read(C66_READY_CMD_OFFSET));
}


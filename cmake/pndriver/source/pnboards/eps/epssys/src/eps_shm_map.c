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
/*  F i l e               &F: eps_shm_map.h								:F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS shared memory map.													 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20071
#define EPS_MODULE_ID      20071

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/
#include <eps_sys.h>        /* Types / Prototypes         */
#include <eps_trc.h>
#include <eps_shm_map.h>


/*****************************************************************************/
/*  Functions                                                                */
/*****************************************************************************/

/**
 * write params in adress map
 *
 * @param shm_map [in] adress map
 * @param key     [in] key for buffer (defined in eps_shm_map.h)
 * @param offset  [in] buffer offset
 * @param size    [in] buffer size
 *
 * @return   LSA_RET_OK Section etry written
 * @return   LSA_RET_ERR_PARAM Wrong parameters.
 */
LSA_UINT32 eps_shm_map_write(EPS_SHM_ADR_MAP_PTR_TYPE shm_map, LSA_UINT32 key, LSA_UINT32 offset, LSA_UINT32 size)
{
    if(key < EPS_SHM_MAP_KEY_MAX)
    {
        shm_map->adr_map[key].key = key;
        shm_map->adr_map[key].offset = offset;
        shm_map->adr_map[key].size = size;
        
        return LSA_RET_OK;
    }
    
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_shm_map_write() - SHM map key not valid");
    
    return LSA_RET_ERR_PARAM;
}


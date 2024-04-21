#ifndef SRC_EPS_SHM_MAP_H_
#define SRC_EPS_SHM_MAP_H_

#ifdef __cplusplus              /* If C++ - compiler: Use C linkage */
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

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/


/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
// memory regions
#define EPS_SHM_MAP_KEY                 0x0000
#define EPS_SHM_MAP_KEY_HIF             0x0001
#define EPS_SHM_MAP_KEY_SRD_API_MEM     0x0002
#define EPS_SHM_MAP_KEY_GTEST           0x0003
#define EPS_SHM_MAP_KEY_FILE            0x0004 
#define EPS_SHM_MAP_KEY_RSMD            0x0005

#define EPS_SHM_MAP_KEY_MAX             0x0006


// memory region sizes  
// Overall size = 0x001FE000 last 8 kB are used for RSMD
// => 772 kB are left for future use
#define EPS_SHM_MAP_SIZE	            0x19000 // 100 kB
#define EPS_SHM_MAP_SIZE_HIF		    0xFF000 // 1 MB - 4 kB
#define EPS_SHM_MAP_SIZE_SRD_API_MEM    0x00800 // 2 kB -> EDDI_GSHAREDMEM_TYPE/EDDP_GSHAREDMEM_TYPE must fit in
#define EPS_SHM_MAP_SIZE_GTEST		    0x02800 // 10 kB
#define EPS_SHM_MAP_SIZE_FILE   	    0x20000 // 128 kB
#define EPS_SHM_MAP_SIZE_RSMD		    0x02000 // 8 kB currently not used by RSMD
                                            
    
/*****************************************************************************/
/*  Typedefs                                                                 */
/*****************************************************************************/
typedef struct
{
    LSA_UINT32 key;         /**< Shared memory section key              */
    LSA_UINT32 offset;      /**< Offset of a section in shared memory   */
    LSA_UINT32 size;        /**< Size of a section in shared memory     */
}EPS_SHM_ADR_MAP_ETRY_TYPE, *EPS_SHM_ADR_MAP_ENTRY_PTR_TYPE;

typedef struct
{
	EPS_SHM_ADR_MAP_ETRY_TYPE adr_map[EPS_SHM_MAP_KEY_MAX];     /**< Shared memory address map, containing all sections and their description   */
}EPS_SHM_ADR_MAP_TYPE, *EPS_SHM_ADR_MAP_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_UINT32 eps_shm_map_write(EPS_SHM_ADR_MAP_PTR_TYPE shm_map, LSA_UINT32 key, LSA_UINT32 offset, LSA_UINT32 size);


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif /* SRC_EPS_SHM_MAP_H_ */

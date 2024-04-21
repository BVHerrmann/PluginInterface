/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_platform_cal_pack_on.h
 *
 * @brief       C(ompiler) A(daption) L(ayer)
 * @details     include file for compiler specific structure / alignment definitions,
 *              switches on a given (via define OBSD_CAL_PACK_SIZE) structure packing / alignment
 *
 * @author      Hendrik Gerlach PD TI ATS TM4 2
 *
 * @version     V0.01
 * @date        created: 23.10.2014
 */
/* -------------------------------------------------------------------------------------------------- */

/* #ifndef __OBSD_PLATFORM_CAL_PACK_ON_H__          no include guards because this file has parameters*/
/* #define __OBSD_PLATFORM_CAL_PACK_ON_H__          and not definitions or declarations               */


/*---------------------------------------------------------------------------*/
/*      1.  SANITY CHECK                                                     */
/*---------------------------------------------------------------------------*/

#ifdef OBSD_PLATFORM_CAL_PACK_ON
    #error "double PACK_ON"
#endif

/*---------------------------------------------------------------------------*/
/*      2.  DEFINE/ UNDEF PACK DEFINES                                       */
/*---------------------------------------------------------------------------*/

#define OBSD_PLATFORM_CAL_PACK_ON
#undef  OBSD_PLATFORM_CAL_PACK_OFF


/*---------------------------------------------------------------------------*/
/*      3.  SET PACKING                                                      */
/*---------------------------------------------------------------------------*/

#include <obsd_pnio_toolchain.h>

#if       OBSD_CAL_PACK_SIZE == 1
#include "pnio_pck1_on.h"                           /* gh2289n: PNIO packing */
#else  /* OBSD_CAL_PACK_SIZE == 1 */
#error "unknown pack size"
#endif /* OBSD_CAL_PACK_SIZE == 1 */

/* #endif */ /* __OBSD_PLATFORM_CAL_PACK_ON_H__ */

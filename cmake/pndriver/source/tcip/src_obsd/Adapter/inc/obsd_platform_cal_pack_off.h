/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_platform_cal_pack_off.h
 *
 * @brief       C(ompiler) A(daption) L(ayer)
 * @details     include file for compiler specific structure / alignment definitions,
 *              switches off a special structure packing / alignment (switches back to default)
 *
 * @author      Hendrik Gerlach PD TI ATS TM4 2
 *
 * @version     V0.01
 * @date        created: 23.10.2014
 */
/* -------------------------------------------------------------------------------------------------- */

/* #ifndef __OBSD_PLATFORM_CAL_PACK_OFF_H__         no include guards because this file has parameters*/
/* #define __OBSD_PLATFORM_CAL_PACK_OFF_H__         and not definitions or declarations               */

/*---------------------------------------------------------------------------*/
/*      1.  SANITY CHECK                                                     */
/*---------------------------------------------------------------------------*/

#ifdef OBSD_PLATFORM_CAL_PACK_OFF
#error "double PACK_OFF"
#endif


/*---------------------------------------------------------------------------*/
/*      2.  UNDEF PACKED DEFINES                                             */
/*---------------------------------------------------------------------------*/

#define OBSD_PLATFORM_CAL_PACK_OFF
#undef  OBSD_PLATFORM_CAL_PACK_ON


/*---------------------------------------------------------------------------*/
/*      3.  RESTORE LAST PACK                                                */
/*---------------------------------------------------------------------------*/

#include <obsd_pnio_toolchain.h>

#include "pnio_pck_off.h"                           /* gh2289n: PNIO packing */

/* #endif */ /* __OBSD_PLATFORM_CAL_PACK_OFF_H__ */

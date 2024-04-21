#ifndef PSI_INT_H                   /* ----- reinclude-protection ----- */
#define PSI_INT_H

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_int.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile                                                      */
/*                                                                           */
/*****************************************************************************/

#include "psi_inc.h"
#include "psi_cfg.h"

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

#include "ltrc_if.h"

#include "psi_usr.h"
#include "psi_sys.h"
#include "psi_trc.h"        // Tracing
#include "psi_res_calc.h"   // Resource calulation
#include "psi_pnstack.h"    // PN-Stack adaption

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/* ========================================================================= */

/* PSI module IDs used in C-Files */

/* PSI-CORE: 1..10 (..\psi\src\src\) */
#define PSI_MODULE_ID_PSI_MBX               1
#define PSI_MODULE_ID_PSI_USR               2
#define PSI_MODULE_ID_PSI_SYS               3
#define PSI_MODULE_ID_PSI_LD                4
#define PSI_MODULE_ID_PSI_HD                5
#define PSI_MODULE_ID_PSI_RES_CALC          6

/* PSI-CORE-PNStack: 100..199 (..\psi\src\src\) */
#define PSI_MODULE_ID_PSI_ACP               100
#define PSI_MODULE_ID_PSI_CLRPC             101
#define PSI_MODULE_ID_PSI_CM                102
#define PSI_MODULE_ID_PSI_DCP               103
#define PSI_MODULE_ID_PSI_DNS               104
#define PSI_MODULE_ID_PSI_EDDI              105
#define PSI_MODULE_ID_PSI_EDDP              106
#define PSI_MODULE_ID_PSI_EDDS              107
#define PSI_MODULE_ID_PSI_GSY               108
#define PSI_MODULE_ID_PSI_HIF               109
#define PSI_MODULE_ID_PSI_IOH               110
#define PSI_MODULE_ID_PSI_LLDP              111
#define PSI_MODULE_ID_PSI_MRP               112
#define PSI_MODULE_ID_PSI_NARE              113
#define PSI_MODULE_ID_PSI_OHA               114
#define PSI_MODULE_ID_PSI_POF               115
#define PSI_MODULE_ID_PSI_SNMPX             116
#define PSI_MODULE_ID_PSI_SOCK              117
#define PSI_MODULE_ID_PSI_TCIP              118
#define PSI_MODULE_ID_PSI_HSA               119
#define PSI_MODULE_ID_PSI_EDDT              120
#define PSI_MODULE_ID_PSI_SOCKAPP           121

/* PSI-CFG-Integration-PNStack "ACP"        : 1000..1099 (..\psi\src\pnstack\acp\) */
#define PSI_MODULE_ID_ACP_CFG               1000
/* PSI-CFG-Integration-PNStack "CLRPC"      : 1100..1199 (..\psi\src\pnstack\clrpc\) */
#define PSI_MODULE_ID_CLRPC_CFG             1100
#define PSI_MODULE_ID_CLRPC_CFG_LIB         1101
/* PSI-CFG-Integration-PNStack "CM"         : 1200..1299 (..\psi\src\pnstack\cm\) */
#define PSI_MODULE_ID_CM_CFG                1200
#define PSI_MODULE_ID_CM_CFG_ACP            1201
#define PSI_MODULE_ID_CM_CFG_EDD            1202
#define PSI_MODULE_ID_CM_CFG_GSY            1203
#define PSI_MODULE_ID_CM_CFG_LIB            1204
#define PSI_MODULE_ID_CM_CFG_MRP            1205
#define PSI_MODULE_ID_CM_CFG_NARE           1206
#define PSI_MODULE_ID_CM_CFG_OHA            1207
#define PSI_MODULE_ID_CM_CFG_POF            1208
#define PSI_MODULE_ID_CM_CFG_RPC            1209
/* PSI-CFG-Integration-PNStack "DCP"        : 1300..1399 (..\psi\src\pnstack\dcp\) */
#define PSI_MODULE_ID_DCP_CFG               1300
/* PSI-CFG-Integration-PNStack "DNS"        : 1400..1499 (..\psi\src\pnstack\dns\) */
#define PSI_MODULE_ID_DNS_CFG               1400
/* PSI-CFG-Integration-PNStack "EDDI"       : 1500..1599 (..\psi\src\pnstack\eddi\) */
#define PSI_MODULE_ID_EDDI_CFG              1500
/* PSI-CFG-Integration-PNStack "EDDP"       : 1600..1699 (..\psi\src\pnstack\eddp\) */
#define PSI_MODULE_ID_EDDP_CFG              1600
#define PSI_MODULE_ID_HERA_IO_CFG           1601
#define PSI_MODULE_ID_PERIF_CFG             1602
/* PSI-CFG-Integration-PNStack "EDDS"       : 1700..1799 (..\psi\src\pnstack\edds\) */
#define PSI_MODULE_ID_EDDS_CFG              1700
/* PSI-CFG-Integration-PNStack "GSY"        : 1800..1899 (..\psi\src\pnstack\gsy\) */
#define PSI_MODULE_ID_GSY_CFG               1800
#define PSI_MODULE_ID_GSY_SYNCALGORITHM     1801
/* PSI-CFG-Integration-PNStack "HIF"        : 1900..1999 (..\psi\src\pnstack\hif\) */
#define PSI_MODULE_ID_HIF_CFG               1900
/* PSI-CFG-Integration-PNStack "IOH"        : 2000..2099 (..\psi\src\pnstack\ioh\) */
#define PSI_MODULE_ID_IOH_CFG               2000
/* PSI-CFG-Integration-PNStack "LLDP"       : 2100..2199 (..\psi\src\pnstack\lldp\) */
#define PSI_MODULE_ID_LLDP_CFG              2100
/* PSI-CFG-Integration-PNStack "MRP"        : 2200..2299 (..\psi\src\pnstack\mrp\) */
#define PSI_MODULE_ID_MRP_CFG               2200
/* PSI-CFG-Integration-PNStack "NARE"       : 2300..2399 (..\psi\src\pnstack\nare\) */
#define PSI_MODULE_ID_NARE_CFG              2300
/* PSI-CFG-Integration-PNStack "OHA"        : 2400..2499 (..\psi\src\pnstack\oha\) */
#define PSI_MODULE_ID_OHA_CFG               2400
/* PSI-CFG-Integration-PNStack "PNIO"       : 2500..2599 (..\psi\src\pnstack\pnio\) */
#define PSI_MODULE_ID_EDD_CFG               2500
/* PSI-CFG-Integration-PNStack "POF"        : 2600..2699 (..\psi\src\pnstack\pof\) */
#define PSI_MODULE_ID_POF_CFG               2600
/* PSI-CFG-Integration-PNStack "SNMPX"      : 2700..2799 (..\psi\src\pnstack\snmpx\) */
#define PSI_MODULE_ID_SNMPX_CFG             2700
/* PSI-CFG-Integration-PNStack "SOCK"       : 2800..2899 (..\psi\src\pnstack\sock\) */
#define PSI_MODULE_ID_SOCK_CFG              2800
#define PSI_MODULE_ID_SOCK_CFG_TCIP         2801
/* PSI-CFG-Integration-PNStack "TCIP"       : 2900..2999 (..\psi\src\pnstack\tcip\) */
#define PSI_MODULE_ID_TCIP_CFG              2900
/* PSI-CFG-Integration-PNStack "HSA"        : 3000..3099 (..\psi\src\pnstack\hsa\) */
#define PSI_MODULE_ID_HSA_CFG               3000
/* PSI-CFG-Integration-PNStack "EDDT"       : 3100..3199 (..\psi\src\pnstack\eddt\) */
#define PSI_MODULE_ID_EDDT_CFG              3100
/* PSI-CFG-Integration-PNStack "SOCKAPP"    : 3200..3299 (..\psi\src\pnstack\sockapp\) */
#define PSI_MODULE_ID_SOCKAPP_CFG           3200

/*===========================================================================*/
/*                            compiler switches                              */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Number of stored Channel-Handles per Component (used only for HD Channels)*/
/*---------------------------------------------------------------------------*/
#define PSI_MAX_COMPONENTS                  ((PSI_COMP_ID_LAST) + 1)
#define PSI_MAX_CHANNELS_PER_COMPONENT      (PSI_INVALID_HANDLE)
    /* 256 Channels per Bytelen of (LSA_HANDLE_TYPE): 0-255 */
    /* 1Byte:  265 Channels                                 */
    /* 2Bytes: 65536 Channels                               */


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* stringify                                                                 */
/*---------------------------------------------------------------------------*/

#define PSI_STRINGIFY_(n)  # n
#define PSI_STRINGIFY(n)   PSI_STRINGIFY_(n) /* make it a string */

/*---------------------------------------------------------------------------*/
/* pragma message                                                            */
/* intended usage: #pragma PSI_MESSAGE(...)                                  */
/*---------------------------------------------------------------------------*/

/*
#define PSI_MESSAGE(str_) message( __FILE__ "(" PSI_STRINGIFY(__LINE__) ") : " str_) //PSI_MESSAGE only for developers!
*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define PSI_FCT_PTR_NULL                (0)

#define psi_function_is_null(fct_ptr)   (fct_ptr == PSI_FCT_PTR_NULL)
#define psi_is_null(ptr)                (LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define psi_is_not_null(ptr)            (!LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define psi_are_equal(ptr1,ptr2)        (LSA_HOST_PTR_ARE_EQUAL((ptr1), (ptr2)))

/* using for adaption of all <comp>_ASSERT, if no own ASSERT is avaliable */
#define PSI_FATAL_COMP( comp_id, mod_id, error_code_0 )     psi_fatal_error (comp_id, mod_id, (LSA_UINT16)__LINE__, (LSA_UINT32)(error_code_0), 0, 0, 0, 0, LSA_NULL)

#define PSI_INIT_USER_ID_UNION(_union)          \
            _union.uvar8            = 0;        \
            _union.uvar16           = 0;        \
            _union.uvar32           = 0;        \
            _union.svar8            = 0;        \
            _union.svar16           = 0;        \
            _union.svar32           = 0;        \
            _union.uvar8_array[0]   = 0;        \
            _union.uvar8_array[1]   = 0;        \
            _union.uvar8_array[2]   = 0;        \
            _union.uvar8_array[3]   = 0;        \
            _union.uvar16_array[0]  = 0;        \
            _union.uvar16_array[1]  = 0;        \
            _union.uvar32_array[0]  = 0;        \
            _union.svar8_array[0]   = 0;        \
            _union.svar8_array[1]   = 0;        \
            _union.svar8_array[2]   = 0;        \
            _union.svar8_array[3]   = 0;        \
            _union.svar16_array[0]  = 0;        \
            _union.svar16_array[1]  = 0;        \
            _union.svar32_array[0]  = 0;        \
            _union.void_ptr         = LSA_NULL;

/*===========================================================================*/
/*    prototypes for common used functions                                   */
/*===========================================================================*/

LSA_VOID_PTR_TYPE psi_alloc_local_mem( LSA_UINT32 length );
LSA_VOID psi_free_local_mem( LSA_VOID_PTR_TYPE  mem_ptr );

LSA_VOID psi_pn_stack_init (LSA_VOID);
LSA_VOID psi_pn_stack_undo_init (LSA_VOID);
LSA_VOID psi_edd_init (LSA_VOID);
LSA_VOID psi_edd_undo_init (LSA_VOID);


#if (PSI_CFG_USE_LD_COMP == 1)
PSI_SOCK_INPUT_PTR_TYPE psi_ld_get_sock_details( LSA_VOID );
#endif

LSA_BOOL psi_get_mrp_default_rports( 
	PSI_HD_INPUT_PTR_TYPE   hd_input_ptr,
	LSA_UINT8 *             rport1_ptr,
	LSA_UINT8 *             rport2_ptr);

PSI_HD_PORT_INPUT_PTR_TYPE psi_get_hw_port_entry_for_user_port( 
	LSA_UINT16              usr_port_nr,
	PSI_HD_INPUT_PTR_TYPE   hd_input_ptr);

/*===========================================================================*/
/*    prototypes for conversion functions                                    */
/*===========================================================================*/

#ifndef PSI_SWAP_U16
LSA_VOID   PSI_SWAP_U16(
	LSA_UINT16  * short_ptr ); /* correct alignment! */
#endif

#ifndef   PSI_SWAP_U32
LSA_VOID PSI_SWAP_U32(
	LSA_UINT32  * long_ptr ); /* correct alignment! */
#endif

#ifndef   PSI_SWAP_U64
LSA_VOID PSI_SWAP_U64(
	LSA_UINT64  * long_long_ptr ); /* correct alignment! */
#endif

#ifndef PSI_NTOH16
	LSA_UINT16 PSI_NTOH16(LSA_UINT16 v);
#endif

#ifndef PSI_HTON16
	LSA_UINT16 PSI_HTON16(LSA_UINT16 v);
#endif

#ifndef PSI_NTOH32
	LSA_UINT32 PSI_NTOH32(LSA_UINT32 v);
#endif

#ifndef PSI_HTON32
	LSA_UINT32 PSI_HTON32(LSA_UINT32 v);
#endif

#ifndef PSI_NTOH64
	LSA_UINT64 PSI_NTOH64(LSA_UINT64 v);
#endif

#ifndef PSI_HTON64
	LSA_UINT64 PSI_HTON64(LSA_UINT64 v);
#endif

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif /* PSI_INT_H_ */

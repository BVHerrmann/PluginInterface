#ifndef EPS_SYS_H                   /* ----- reinclude-protection ----- */
#define EPS_SYS_H

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_sys.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface                                                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

//lint --e(537) Repeated include file - Defines are necessary, but other C files directly include psi_inc.h
#include "psi_inc.h"                /* PSI Interface                */
//lint --e(537) eps_lib.h may also be included in psi_inc.h. 
#include "eps_lib.h"                /* EPS Library Functions        */

#include "eps_internal_cfg.h"       /* EPS Internal Configurations  */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif


//---------------------------------------------------------------------------
// Compileschalter
//---------------------------------------------------------------------------

// Compile-Schalter "EPS_HERA_ENABLE_POLLING_MODE" zur Aktivierung des POLLING-Modes
//         = 1 (aktiviert)    --> POLLING-MODE 
//         = 0 (deaktiviert)  --> ISR-MODE

#define EPS_HERA_ENABLE_POLLING_MODE           0 // POLLING-Mode ist aktiviert

// enable tracing of statistics for ISO-TransferEnd-Interrupt
//#define EPS_STATISTICS_ISO_TRANSFEREND

/*****************************************************************************/
/*  Macros                                                                   */
/*****************************************************************************/

#define EPS_RQB_SET_REQ_FCT_PTR(rb_, val_)   {(rb_)->_user_func = (val_);}
    
/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef LSA_VOID LSA_FCT_PTR(,EPS_UPPER_CALLBACK_FCT_PTR_TYPE)(LSA_VOID* pEpsOpen, LSA_VOID* unused);

typedef LSA_VOID (*EPS_LOWER_DONE_FCT)(LSA_VOID_PTR_TYPE pInst, LSA_VOID_PTR_TYPE pRQB);

struct eps_header
{
	LSA_RQB_HEADER(struct eps_header*)
};

typedef struct eps_sys_tag
{
	LSA_UINT16 hd_nr;
	LSA_UINT16 pnio_if_nr;
	LSA_UINT16 edd_comp_id;
	LSA_BOOL   shm_if;
} EPS_SYS_TYPE, *EPS_SYS_PTR_TYPE;

typedef struct
{
	LSA_UINT8*  pBase;
	LSA_UINT32  uPhyAddr;
	LSA_UINT32  lSize;
} EPS_MEM_REGION_TYPE, *EPS_MEM_REGION_PTR_TYPE;

typedef struct
{	
	EPS_MEM_REGION_TYPE         LocalMemPool;
	EPS_MEM_REGION_TYPE         FastMemPool;
} EPS_HW_INFO_TYPE, *EPS_HW_INFO_PTR_TYPE;

typedef enum eps_runs_on_type_enum { 
	EPS_RUNS_ON_UNKNOWN      = 0x00,
    EPS_RUNS_ON_APPLICATION  = 0x01,  // Pciox App or light, basic and advanced
    EPS_RUNS_ON_ADVANCED     = 0x02,  // adonis_x32 fw
    EPS_RUNS_ON_BASIC        = 0x03,  // basic fw
	EPS_RUNS_ON_MAX
} EPS_RUNS_ON_TYPE;

typedef enum eps_mem_ram_type_enum {
    EPS_RAM_MEM_IRTE        = 0,
    EPS_RAM_MEM_PNIP        = 1,
    EPS_RAM_MEM_SDRAM       = 2,
    EPS_RAM_MEM_PERIF_RAM   = 3,
    EPS_RAM_MEM_HERA_K32_A  = 4,
    EPS_RAM_MEM_HERA_K32_B  = 5,
    EPS_RAM_MEM_HERA_DDR3   = 6,
    EPS_RAM_MEM_K32_TCM     = 7
} EPS_MEM_RAM_TYPE;

typedef struct eps_hd_out_tag
{
    EPS_MEM_REGION_TYPE io_mem;
    EPS_MEM_REGION_TYPE io_iocc;
    EPS_MEM_REGION_TYPE srd_api_mem;
    EPS_MEM_REGION_TYPE irte_base;
} EPS_HD_IO_OUTPUT_TYPE, *EPS_HD_IO_OUTPUT_PTR_TYPE;
typedef EPS_HD_IO_OUTPUT_TYPE * const EPS_HD_IO_OUTPUT_CONST_PTR_TYPE;

typedef struct eps_open_tag
{
    LSA_UINT32                      hd_count;
	PSI_SOCK_INPUT_TYPE             sock_args;                    //In: Sock configuration values for LD 
    PSI_IM_INPUT_TYPE               imInput;
	PSI_HD_INPUT_TYPE               hd_args[PSI_CFG_MAX_IF_CNT];    //In: configuration of all HD
	PSI_HD_OUTPUT_TYPE              hd_out[PSI_CFG_MAX_IF_CNT];     //out: calculated config of all HD
    LSA_BOOL                        ramtest_on[PSI_CFG_MAX_IF_CNT + 1]; // LD + n HD                   
    LSA_BOOL                        write_dumpfiles_on_fatal[PSI_CFG_MAX_IF_CNT + 1]; // LD + n HD  
    LSA_BOOL                        fw_debug_activated;
	LSA_BOOL                        shm_debug_activated;
	LSA_BOOL                        shm_halt_activated;
    LSA_VOID*                       pUsrHandle;
    LSA_UINT16                      eps_proceed_event;
    PSI_LD_RUNS_ON_TYPE             ldRunsOnType;
    PSI_LD_OUTPUT_TYPE              ld_out;
    LSA_RESULT                      retVal; ///< Out LSA_RSP_OK: EPS Open successfull, LSA_RSP_ERR_PARAM - Error in Input parameters. See trace
    EPS_HD_IO_OUTPUT_TYPE           io_param_out[PSI_CFG_MAX_IF_CNT];     //out: io addresses for io_data access for configured hds
} EPS_OPEN_TYPE, *EPS_OPEN_PTR_TYPE;
typedef EPS_OPEN_TYPE const * EPS_OPEN_CONST_PTR_TYPE;

typedef struct eps_close_tag
{
    LSA_VOID*                       pUsrHandle;
    LSA_UINT16                      eps_proceed_event;
} EPS_CLOSE_TYPE, *EPS_CLOSE_PTR_TYPE;
typedef EPS_CLOSE_TYPE const * EPS_CLOSE_CONST_PTR_TYPE;

typedef enum eps_state{
    EPS_STATE_RESET_DONE = 0,
    EPS_STATE_INIT_RUNNING,
    EPS_STATE_INIT_DONE,
    EPS_STATE_OPEN_CALLED,
    EPS_STATE_OPEN_DOING_HIF_LOWER_OPEN,
    EPS_STATE_OPEN_DOING_HIF_UPPER_OPEN,
    EPS_STATE_OPEN_DOING_PSI_OPEN,
    EPS_STATE_RUNNING,
    EPS_STATE_CLOSE_DOING_PSI_CLOSE,
    EPS_STATE_CLOSE_DOING_HIF_UPPER_CLOSE,
    EPS_STATE_CLOSE_DOING_HIF_LOWER_CLOSE,
    EPS_STATE_CLOSE_DOING_REMOVE_BOARDS,
    EPS_STATE_CLOSE_DONE,
    EPS_STATE_UNDO_INIT_RUNNING,
    EPS_STATE_UNDO_INIT_DONE
} EPS_STATE_TYPE;

typedef struct
{
	LSA_BOOL                    bEpsInFatal;
	LSA_BOOL                    bBlockingPermitted;
	LSA_BOOL                    bDebug;
	LSA_BOOL					bShmDebug;
	LSA_BOOL					bShmHalt;
	LSA_BOOL                    bShutdown;
	LSA_BOOL                    bRunning;
    LSA_BOOL                    bEpsShmIfInitialized;
    LSA_BOOL                    bEpsDevIfInitialized;

    EPS_HW_INFO_TYPE            HwInfo;
    LSA_VOID_PTR_TYPE           hSysUpper;
    PSI_LD_RUNS_ON_TYPE         ldRunsOnType;             // PSI configuration for distributed system 
    LSA_UINT16                  hdCount;                  // number of hds  

    #if ( EPS_CFG_USE_HIF == 1 )
    PSI_HIF_SYS_TYPE            SysLowerHD;
	PSI_HIF_SYS_TYPE            SysLowerLD;
    PSI_HIF_SYS_TYPE            SysUpperLD;               // Handle to upper LD IF       
	HIF_HANDLE                  hLowerLD;
	HIF_HANDLE                  hLowerHD;
    HIF_HANDLE                  hUpperLD;                 // HIF LD upper handle                      
    #endif                 

    PSI_HD_INPUT_TYPE           hdInput[PSI_CFG_MAX_IF_CNT]; //store for HD information               
    PSI_IM_INPUT_TYPE           imInput;                   // Storage for IM Data                     
    EPS_OPEN_TYPE               uEpsOpen;
    EPS_CLOSE_TYPE              uEpsClose;
    LSA_UINT32                  iOpenUserChannels;
    EPS_STATE_TYPE              eState;
} EPS_DATA_TYPE, *EPS_DATA_PTR_TYPE;
typedef EPS_DATA_TYPE const * EPS_DATA_CONST_PTR_TYPE;

#define EPS_ERROR_DATA_BUF_LENGTH   1000
typedef struct eps_shm_fatal_store_tag
{
	LSA_UINT32              bUsed;
	LSA_UINT32              uSize;
	LSA_UINT32              exit_code_org;
	LSA_UINT32              uLine;
	LSA_CHAR                sMsg[200];
	LSA_CHAR                sFile[200];
	LSA_CHAR                sComp[200];
	LSA_CHAR                sFunc[200];
	LSA_UINT32              uLsaFatalLen;
    LSA_FATAL_ERROR_TYPE    lsa_fatal;
	LSA_UINT8               lsa_fatal_error_data[EPS_ERROR_DATA_BUF_LENGTH];
} EPS_SHM_FATAL_STORE_TYPE, *EPS_SHM_FATAL_STORE_PTR_TYPE;

typedef struct eps_fatal_store_dump
{
    LSA_UINT8*              pDumpBuf;                /* base address of local buffer */
    LSA_UINT32              lDumpSize;               /* size of local Buffer */
    LSA_UINT32              lBoardType;
    LSA_UINT32              lAsicType;
    LSA_UINT32              lAsicTypeInfo;
    LSA_UINT32              lMemRamType;
} EPS_FATAL_STORE_DUMP_TYPE, *EPS_FATAL_STORE_DUMP_PTR_TYPE;

#ifdef EPS_LOCAL_PN_USER

#define EPS_RQB_SET_USER_INST_PTR(rb_, val_)  {(rb_)->_user_instance = (val_);}
#define EPS_RQB_GET_USER_INST_PTR(rb_)        ((rb_)->_user_instance)
#define EPS_RQB_SET_USER_CBF_PTR(rb_, val_)  {(rb_)->_user_func = (val_);}
#define EPS_RQB_GET_USER_CBF_PTR(rb_)        ((rb_)->_user_func)

//EPS RQB Header type
typedef struct eps_rqb_header 
{
	LSA_RQB_HEADER(struct eps_rqb_header*)
} EPS_RQB_TYPE;

#endif

/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/
#ifdef EPS_SYS
	EPS_DATA_TYPE                   g_EpsData;
	EPS_DATA_PTR_TYPE               g_pEpsData;
#else
	LSA_EXTERN EPS_DATA_TYPE        g_EpsData;
	LSA_EXTERN EPS_DATA_PTR_TYPE    g_pEpsData;
#endif

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

/* eps_sys.c       ----------------------------------------------------------*/
LSA_VOID eps_init( LSA_VOID_PTR_TYPE hSys );
LSA_VOID eps_undo_init( LSA_VOID );

LSA_VOID eps_open( EPS_OPEN_PTR_TYPE pEpsOpen, EPS_UPPER_CALLBACK_FCT_PTR_TYPE pCbf, LSA_UINT16 TimeOutSec ); 
LSA_VOID eps_close( EPS_CLOSE_PTR_TYPE pEpsClose, EPS_UPPER_CALLBACK_FCT_PTR_TYPE pCbf, LSA_UINT16 TimeOutSec );
LSA_VOID eps_ld_upper_open( EPS_OPEN_CONST_PTR_TYPE pEpsOpen, LSA_UINT16 * phDoneEvent );
LSA_VOID eps_ld_upper_close( EPS_CLOSE_CONST_PTR_TYPE pEpsClose, LSA_UINT16 * phDoneEvent );
#if ( EPS_CFG_USE_HIF == 1)
LSA_VOID eps_hif_ld_upper_open( EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16 * phDoneEvent );
LSA_VOID eps_hif_ld_upper_open_done( LSA_VOID * pArg, HIF_RQB_PTR_TYPE pRQB );
#endif

PSI_LD_RUNS_ON_TYPE eps_get_ld_runs_on();

LSA_VOID eps_remove_boards( LSA_VOID );
LSA_VOID eps_advanced_init_reboot( LSA_VOID );
LSA_UINT32 eps_advanced_reboot_is_active( LSA_VOID );

#ifdef EPS_LOCAL_PN_USER
LSA_VOID eps_user_system_request_lower( LSA_VOID_PTR_TYPE  rqb_ptr, LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT p_fct );
// open user channel for a PNIO comp
LSA_VOID eps_user_open_channel_lower( LSA_VOID_PTR_TYPE rqb_ptr, LSA_VOID_PTR_TYPE p_inst, EPS_LOWER_DONE_FCT p_fct );
// close user channel for a PNIO comp
LSA_VOID eps_user_close_channel_lower( LSA_VOID_PTR_TYPE rqb_ptr, LSA_VOID_PTR_TYPE p_inst, EPS_LOWER_DONE_FCT p_fct );
LSA_VOID eps_user_request_lower( LSA_VOID_PTR_TYPE rqb_ptr, LSA_VOID_PTR_TYPE p_inst, EPS_LOWER_DONE_FCT p_fct );
#endif

LSA_UINT32 eps_get_ticks_1ms(LSA_VOID);
LSA_UINT64 eps_get_ticks_100ns(LSA_VOID);

#if (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)
LSA_VOID eps_hif_hd_lower_close( EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16 * phDoneEvent );
#endif

LSA_INT eps_sprintf( LSA_UINT8 * buffer, const char * format, ... );

/*****************************************************************************/
/*  Fatal / Assert                                                           */
/*****************************************************************************/

/* eps_exit_code ------------------------------------------------------------*/
typedef LSA_UINT32 EPS_TYPE_EXIT_CODE;

//Config exit codes => Default action: close program. Range: 100...109
#define EPS_EXIT_CODE_APP_CONFIG_INVALID            100     //A PCIOX ini problem, config invalid. Default action: close program.
#define EPS_EXIT_CODE_APP_CONFIG_NOT_AVAIL          101     //PCIOX tries to access an INI file but cannot. Default action: close program.
#define EPS_EXIT_CODE_APP_SCRIPT_CONN_NOT_AVAIL     102     //PCIOX tries to connect to scripting server but failed. Default action: close program.

//EPS exit codes => Default action: close program. Range: 110...119
#define EPS_EXIT_CODE_APP_FW_NOT_AVAIL              110     //PCIOX cannot find the FW-File or the Bootloader-File of the board. Default action: close program.
#define EPS_EXIT_CODE_LSA_FATAL                     111     //PSI_FATAL_ERROR() was called. Default action: close program.
#define EPS_EXIT_CODE_EPS_FATAL                     112     //EPS_FATAL(), EPS_ASSERT(), PCIOX_ASSERT() or mem3_assert_fatal() was called. Default action: close program.
#define EPS_EXIT_CODE_DRIVER_NOT_FOUND              113     //Driver not installed or driver version mismatch. Default action: close program.
#define EPS_EXIT_CODE_DEVICE_NOT_FOUND              114     //Device not found (e.g. invalid PCI or MAC address). Default action: close program.

//Advanced exit codes => Special action: reset core 2. Range: 120...129
#define EPS_EXIT_CODE_ADVANCED_FATAL                120     //Special action: reset core 2.

//Board exit codes => Special action: reset board. Range: 130...139
#define EPS_EXIT_CODE_BOARD_FATAL                   130     //Special action: reset board.
#define EPS_EXIT_CODE_MASTER_APP_ALREADY_EXIST      131     //Master App already exist! Special action: reset board.
#define EPS_EXIT_CODE_EPS_EXCEPTION                 132     //Platform exception handler was called. Special action: reset board.

/*---------------------------------------------------------------------------*/

LSA_VOID eps_fatal_with_reason( EPS_TYPE_EXIT_CODE const exit_code_org, EPS_TYPE_EXIT_CODE const exit_code_signal, LSA_CHAR * comp, LSA_UINT16 length, LSA_FATAL_ERROR_TYPE * const lsa_fatal_ptr, LSA_CHAR * file, LSA_INT line, const LSA_CHAR * func, LSA_CHAR * str );

/*---------------------------------------------------------------------------*/

#ifndef EPS_FATAL_WITH_REASON
#define EPS_FATAL_WITH_REASON(exit_code_org, exit_code_signal, comp, length, lsa_fatal_ptr, file, line, func, str) \
    eps_fatal_with_reason(exit_code_org, exit_code_signal, comp, length, lsa_fatal_ptr, file, line, func, str)
#endif

#define EPS_FATAL(reason_) EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_EPS_FATAL, EPS_EXIT_CODE_EPS_FATAL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "fatal: " #reason_)

#define EPS_ASSERT(cond_) \
	if (!(cond_))   EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_EPS_FATAL, EPS_EXIT_CODE_EPS_FATAL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "assert: " #cond_)

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_SYS_H */

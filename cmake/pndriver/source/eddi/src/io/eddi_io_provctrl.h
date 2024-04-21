#ifndef EDDI_IO_PROVCTRL_H      //reinclude-protection
#define EDDI_IO_PROVCTRL_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_io_provctrl.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* User Interface                                                            */
/* CRT-                                                                      */
/* Defines constants, types, macros and prototyping for prefix.              */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date      Who   What                                                      */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                              Settings                                     */
/*===========================================================================*/
#define EDDI_IO_UNKNOWN_PROV_ID        0xFFFF            //unknown/free provider-id
#define EDDI_IO_UNDEFINED_INSTANCE_HANDLE 0xFFFFFFFFUL

#if defined (EDDI_CFG_REV5)
#define EDDI_IO_KRAM_SIZE  EDDI_KRAM_SIZE_ERTEC400
#elif defined (EDDI_CFG_REV6)
#define EDDI_IO_KRAM_SIZE  EDDI_KRAM_SIZE_ERTEC200
#elif defined (EDDI_CFG_REV7)
#define EDDI_IO_KRAM_SIZE  EDDI_KRAM_SIZE_SOC
#endif

#if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW) || defined (EDDI_CFG_REV5)
#define EDDI_IO_KRAM_TO_REGS_OFFSET    0x100000UL        //offset from kram to the irte-regs      
#endif 

#if !defined (EDDI_CFG_LITTLE_ENDIAN) && defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
#error "BIG-ENDIAN currently not supported for EDDI_IO!"
#endif

#define EDDI_INT_MAX_NR_PROVIDERS   0x7FFF

/*===========================================================================*/
/*                              Structs                                      */
/*===========================================================================*/

/*** CRT-CONSISTENCY-CONTROL-Structure ***/
#if defined (EDDI_CFG_REV5)
typedef enum _EDDI_IO_CONSST_STS_ENUM
{
    EDDI_IO_CONSST_STS_FREE, // describes the free Status
    EDDI_IO_CONSST_STS_INUSE // Consistency-Control is already occupied
} EDDI_IO_CONSST_STS_ENUM;

// describes the Status and saves the current params (if state== INUSE) of
// one Consistency-Control-Half (Read- or Write - interface)
typedef struct _EDDI_IO_CONSST_TYPE
{
    EDDI_IO_CONSST_STS_ENUM   Status;

} EDDI_IO_CONSST_TYPE;
#endif //(EDDI_CFG_REV5)


/*** 3BSW-CONTROL-Structure ***/
#if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
struct _EDDI_IO_PROVIDER_LIST_ENTRY_TYPE;
struct _EDDI_IO_CB_TYPE;
typedef LSA_RESULT ( EDDI_SYSTEM_IN_FCT_ATTR  *  EDDI_IO_XCHANGE_FCT) (
    struct _EDDI_IO_PROVIDER_LIST_ENTRY_TYPE *  const  pProvider,
    LSA_UINT32                               *  const  pOffsetDB,
    struct _EDDI_IO_CB_TYPE                  *  const  pIOCB,
    LSA_UINT32                                  const  InstanceHandle);
#endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)

/*** Providerhandling ***/
typedef struct _EDDI_IO_PROVIDER_LIST_ENTRY_TYPE
{
    LSA_UINT16                        provider_id;
    LSA_UINT8                       * pDataStatus;
    LSA_UINT8                         ProvType;
#if defined (EDDI_CFG_SYSRED_2PROC)
    LSA_UINT16                        OrderID;
#endif
#if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    LSA_UINT8                       * pDataStatus1;
    LSA_UINT8                       * pDataStatus2;
    LSA_UINT32                        offset_U_buffer;        //offset of the USER-buffer
    LSA_UINT32                        offset_D_buffer;        //offset of the DATA-buffer
    LSA_UINT32                        offset_F_buffer;        //offset of the FREE-buffer
    LSA_UINT32                        offset_U2D_buffer;      //offset of the USER2DATA-buffer
    LSA_UINT32                        offset_D2F_buffer;      //offset of the DATA2FREE-buffer
    EDDI_SER10_ACW_SND_TYPE         * pACWSnd;                //SendACW
    EDDI_SER10_FCW_SND_TYPE         * pFCWSnd;                //SendFCW
    LSA_UINT32                      * pTransport;
    LSA_UINT32                        clk_cnt_begin;
    EDDI_IO_XCHANGE_FCT               io_xchange_fct;
#endif //defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)

} EDDI_IO_PROVIDER_LIST_ENTRY_TYPE;

typedef struct _EDDI_IO_CB_TYPE
{
    LSA_UINT32                         InstanceHandle;
    LSA_UINT32                         TraceIdx;
    LSA_UINT32                         pKRAM;
#if !defined (EDDI_CFG_3BIF_2PROC)
    EDDI_GSHAREDMEM_TYPE             * pGSharedRAM;
#endif

#if defined EDDI_CFG_MAX_NR_PROVIDERS
    EDDI_IO_PROVIDER_LIST_ENTRY_TYPE   provider_list[EDDI_CFG_MAX_NR_PROVIDERS];
#else
    EDDI_IO_PROVIDER_LIST_ENTRY_TYPE * provider_list;
#endif
    LSA_UINT16                         MaxNrProviders;

    LSA_UINT32                         pIRTE;
#if defined (EDDI_CFG_REV5)
    LSA_UINT32                         offset_ProcessImageEnd;
    LSA_UINT32                         PollTimeOut_10ns;
    EDDI_IO_CONSST_TYPE                ConsistencyRead;       // Consistency for reading
    EDDI_IO_CONSST_TYPE                ConsistencyWrite;      // Consistency for writing
#endif //(EDDI_CFG_REV5)
} EDDI_IO_CB_TYPE;

extern  LSA_INT32   EDDI_Lock_Sema_IO[];

/*===========================================================================*/
/*                              Macros                                       */
/*===========================================================================*/

//-----------------------------------------
// Lock macros for use in eddi_io_provctr.c
//-----------------------------------------
#define EDDI_ENTER_IO_S(InstanceHandle_)  {                                                                       \
        EDDI_ENTER_IO();                                                                                          \
        if ( 0 == EDDI_Lock_Sema_IO[InstanceHandle_] )                                                            \
        {                                                                                                         \
            EDDI_Lock_Sema_IO[InstanceHandle_]++;                                                                 \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            /* no trace entries in macros possible (tracescanner) */                                              \
            EDDI_Excp("EDDI_ENTER_IO -> ", EDDI_FATAL_ERR_EXCP, EDDI_Lock_Sema_IO[InstanceHandle_], __LINE__);    \
        }                                                                                                         \
    }

#define EDDI_EXIT_IO_S(InstanceHandle_)  {                                                                        \
        EDDI_Lock_Sema_IO[InstanceHandle_]--;                                                                     \
        if ( 0 != EDDI_Lock_Sema_IO[InstanceHandle_] )                                                            \
        {                                                                                                         \
            /* no trace entries in macros possible (tracescanner) */                                              \
            EDDI_Excp("EDDI_EXIT_IO -> ", EDDI_FATAL_ERR_EXCP, EDDI_Lock_Sema_IO[InstanceHandle_], __LINE__);     \
        }                                                                                                         \
        EDDI_EXIT_IO();                                                                                           \
    }

//-----------------------------------------
// Lock macros for use in eddi_io_kram.c
//-----------------------------------------
#if defined (EDDI_CFG_REV5)
#define EDDI_ENTER_IO_KRAM_S(InstanceHandle_)  {                                                                  \
        EDDI_ENTER_IO_KRAM();                                                                                     \
        if ( 0 == EDDI_Lock_Sema_KRAM[InstanceHandle_] )                                                          \
        {                                                                                                         \
            EDDI_Lock_Sema_KRAM[InstanceHandle_]++;                                                               \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            /* no trace entries in macros possible (tracescanner) */                                              \
            EDDI_Excp("EDDI_ENTER_IO_KRAM -> ", EDDI_FATAL_ERR_EXCP, EDDI_Lock_Sema_KRAM[InstanceHandle_], __LINE__);   \
        }                                                                                                         \
    }

#define EDDI_EXIT_IO_KRAM_S(InstanceHandle_)  {                                                                     \
        EDDI_Lock_Sema_KRAM[InstanceHandle_]--;                                                                     \
        if ( 0 != EDDI_Lock_Sema_KRAM[InstanceHandle_] )                                                            \
        {                                                                                                           \
            /* no trace entries in macros possible (tracescanner) */                                                \
            EDDI_Excp("EDDI_EXIT_IO_KRAM -> ", EDDI_FATAL_ERR_EXCP, EDDI_Lock_Sema_KRAM[InstanceHandle_], __LINE__);\
        }                                                                                                           \
        EDDI_EXIT_IO_KRAM();                                                                                        \
    }
#endif //defined (EDDI_CFG_REV5)

//----------------------------------------
// Lock macros for use in EDDI LSA package
//----------------------------------------
#if defined (EDDI_CFG_3BIF_2PROC)
#define EDDI_ENTER_IO_S_INT(InstanceHandle_)
#define EDDI_EXIT_IO_S_INT(InstanceHandle_)
#else //defined (EDDI_CFG_3BIF_2PROC)
#define EDDI_ENTER_IO_S_INT(InstanceHandle_)  {                                                                   \
        EDDI_ENTER_IO();                                                                                          \
        if ( 0 == EDDI_Lock_Sema_IO[InstanceHandle_] )                                                            \
        {                                                                                                         \
            EDDI_Lock_Sema_IO[InstanceHandle_]++;                                                                 \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            /* no trace entries in macros possible (tracescanner) */                                              \
            EDDI_Excp("EDDI_ENTER_IO -> ", EDDI_FATAL_ERR_EXCP, EDDI_Lock_Sema_IO[InstanceHandle_], 0);           \
        }                                                                                                         \
    }

#define EDDI_EXIT_IO_S_INT(InstanceHandle_)  {                                                                    \
        EDDI_Lock_Sema_IO[InstanceHandle_]--;                                                                     \
        if ( 0 != EDDI_Lock_Sema_IO[InstanceHandle_] )                                                            \
        {                                                                                                         \
            /* no trace entries in macros possible (tracescanner) */                                              \
            EDDI_Excp("EDDI_EXIT_IO -> ", EDDI_FATAL_ERR_EXCP, EDDI_Lock_Sema_IO[InstanceHandle_], 0);            \
        }                                                                                                         \
        EDDI_EXIT_IO();                                                                                           \
    }
#endif //defined (EDDI_CFG_3BIF_2PROC)

//----------------------------------------
// IO access, bitfields
//----------------------------------------
#define EDDI_IO_IO_START (pIOCB->pIRTE)
#define EDDI_IO_x32(offset) (*(volatile EDDI_DEV_MEM_U32_PTR_TYPE)((EDDI_IO_IO_START) + (offset)))

#if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW) || defined (EDDI_CFG_REV5)
#if defined (EDDI_CFG_IO_LITTLE_ENDIAN)
#define EDDI_IO_SWAP_32(var)       (var)
#elif defined (EDDI_CFG_IO_BIG_ENDIAN)
#define EDDI_IO_SWAP_32(var)                \
/*lint --e(941) --e(572) --e(778)*/         \
(  ((((var)) & 0xFF000000UL) >> 24)         \
 + ((((var)) & 0x00FF0000UL) >>  8)         \
 + ((((var)) & 0x0000FF00UL) <<  8)         \
 + ((((var)) & 0x000000FFUL) << 24))
#else
#error "Neither EDDI_CFG_IO_LITTLE_ENDIAN nor EDDI_CFG_IO_BIG_ENDIAN defined in eddi_cfg.h!"
#endif

#define EDDI_IO_HOST2IRTE32(val_) EDDI_IO_SWAP_32(val_)

#define EDDI_IO_R32(offset) EDDI_IO_HOST2IRTE32((*(volatile EDDI_DEV_MEM_U32_PTR_TYPE)((EDDI_IO_IO_START) + (offset))))
//#define EDDI_IO_W32(offset, val) EDDI_IO_HOST2IRTE32(*(volatile EDDI_DEV_MEM_U32_PTR_TYPE)((EDDI_IO_IO_START) + (offset)), val)

#endif //(EDDI_INTCFG_PROV_BUFFER_IF_3BSW) || defined (EDDI_CFG_REV5)


/*===========================================================================*/
/*                              Protos                                       */
/*===========================================================================*/

#if defined (EDDI_CFG_REV5)
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CnsReset( LSA_UINT32 const InstanceHandle );
#endif //EDDI_CFG_REV5

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_IO_PROVCTRL_H


/*****************************************************************************/
/*  end of file eddi_io_provctrl.h                                           */
/*****************************************************************************/

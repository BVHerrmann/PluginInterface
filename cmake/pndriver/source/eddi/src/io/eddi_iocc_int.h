#ifndef EDDI_IOCC_INT_H           //reinclude-protection
#define EDDI_IOCC_INT_H

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
/*  F i l e               &F: eddi_iocc_int.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* Defines for IOCC support                                                  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date      Who   What                                                      */
/*                                                                           */
/*****************************************************************************/

#include "eddi_iocc_inc.h"
#include "eddi_m_id.h"
#include "eddi_iocc.h"
#include "eddi_iocc_out.h"
#if defined (EDDI_CFG_SOC)

/*===========================================================================*/
/*                              Settings                                     */
/*===========================================================================*/
#define EDDI_IOCC_INT_CFG_IOCC_CHANNELS     2       //2 channels per instance
#define EDDI_IOCC_MAX_EXCP_INFO             0x140
#define EDDI_IOCC_MAX_SINGLE_TRANSFER_SIZE  1512    //max nr of bytes for a single transfer
#define EDDI_IOCC_MAX_GROUP_TRANSFER_SIZE   1536    //max nr of bytes for a group transfer
#define EDDI_IOCC_MAX_NR_LINES              EDDI_IOCC_MAX_GROUP_TRANSFER_SIZE    //max nr of lines in 1 LL

/*===========================================================================*/
/*                              Defines                                      */
/*===========================================================================*/
/****************************/
/*   IOCC registers         */
/****************************/
#define EDDI_IOCC_BASE_AHB                                     0x1D600000UL
#if !defined (EDDI_IOCC_HOST_LL_inst_hwp0_ram_start)
#define EDDI_IOCC_HOST_LL_inst_hwp0_ram_start                  (0x000020)
//#define EDDI_IOCC_HOST_LL_inst_hwp0_ram_end                    (0x00061F)
#define EDDI_IOCC_HOST_LL_inst_hwp1_ram_start                  (0x000820)
//#define EDDI_IOCC_HOST_LL_inst_hwp1_ram_end                    (0x000E1F)
#define EDDI_IOCC_HOST_LL_inst_ll_ram_start                    (0x002000)
#define EDDI_IOCC_HOST_LL_inst_ll_ram_end                      (0x003FFF)
#define EDDI_IOCC_PA_EA_DIRECT_start                           (0x100000)
#define EDDI_IOCC_PA_EA_DIRECT_end                             (0x18FFFC)
//#define EDDI_IOCC_HOST_LL_inst_hwp0_mb_adr                     (0x000000)
//#define EDDI_IOCC_HOST_LL_inst_hwp0_mb_data                    (0x000004)
//#define EDDI_IOCC_HOST_LL_inst_hwp0_mb_access                  (0x000008)
//#define EDDI_IOCC_HOST_LL_inst_hwp0_mb_access_mb_type          (0x00000003)
#define EDDI_IOCC_HOST_LL_inst_hwp0_cntrl                      (0x00000C)
//#define EDDI_IOCC_HOST_LL_inst_hwp0_cntrl_mb_en                (0x00000001)
//#define EDDI_IOCC_HOST_LL_inst_hwp0_cntrl_wrtrig               (0x00000002)
#define EDDI_IOCC_HOST_LL_inst_hwp0_ll_sl                      (0x000010)
#define EDDI_IOCC_HOST_LL_inst_hwp0_ll_nl                      (0x000014)
#define EDDI_IOCC_HOST_LL_inst_hwp0_nmb_data                   (0x000018)
//#define EDDI_IOCC_HOST_LL_inst_hwp0_dummy                      (0x00001C)
#define EDDI_IOCC_HOST_LL_inst_hrp0_mb_adr                     (0x000620)
#define EDDI_IOCC_HOST_LL_inst_hrp0_mb_data                    (0x000624)
#define EDDI_IOCC_HOST_LL_inst_hrp0_mb_access                  (0x000628)
//#define EDDI_IOCC_HOST_LL_inst_hrp0_mb_access_mb_type          (0x00000003)
#define EDDI_IOCC_HOST_LL_inst_hrp0_cntrl                      (0x00062C)
#define EDDI_IOCC_HOST_LL_inst_hrp0_cntrl_mb_en                (0x00000001)
#define EDDI_IOCC_HOST_LL_inst_hrp0_cntrl_wrtrig               (0x00000002)
#define EDDI_IOCC_HOST_LL_inst_hrp0_ll_sl                      (0x000630)
#define EDDI_IOCC_HOST_LL_inst_hrp0_ll_nl                      (0x000634)
#define EDDI_IOCC_HOST_LL_inst_hrp0_dest_adr                   (0x000638)
//#define EDDI_IOCC_HOST_LL_inst_hrp0_dummy                      (0x00063C)
//#define EDDI_IOCC_HOST_LL_inst_hwp1_mb_adr                     (0x000800)
//#define EDDI_IOCC_HOST_LL_inst_hwp1_mb_data                    (0x000804)
//#define EDDI_IOCC_HOST_LL_inst_hwp1_mb_access                  (0x000808)
//#define EDDI_IOCC_HOST_LL_inst_hwp1_mb_access_mb_type          (0x00000003)
#define EDDI_IOCC_HOST_LL_inst_hwp1_cntrl                      (0x00080C)
//#define EDDI_IOCC_HOST_LL_inst_hwp1_cntrl_mb_en                (0x00000001)
//#define EDDI_IOCC_HOST_LL_inst_hwp1_cntrl_wrtrig               (0x00000002)
#define EDDI_IOCC_HOST_LL_inst_hwp1_ll_sl                      (0x000810)
#define EDDI_IOCC_HOST_LL_inst_hwp1_ll_nl                      (0x000814)
#define EDDI_IOCC_HOST_LL_inst_hwp1_nmb_data                   (0x000818)
//#define EDDI_IOCC_HOST_LL_inst_hwp1_dummy                      (0x00081C)
#define EDDI_IOCC_HOST_LL_inst_hrp1_mb_adr                     (0x000E20)
#define EDDI_IOCC_HOST_LL_inst_hrp1_mb_data                    (0x000E24)
#define EDDI_IOCC_HOST_LL_inst_hrp1_mb_access                  (0x000E28)
//#define EDDI_IOCC_HOST_LL_inst_hrp1_mb_access_mb_type          (0x00000003)
#define EDDI_IOCC_HOST_LL_inst_hrp1_cntrl                      (0x000E2C)
#define EDDI_IOCC_HOST_LL_inst_hrp1_cntrl_mb_en                (0x00000001)
#define EDDI_IOCC_HOST_LL_inst_hrp1_cntrl_wrtrig               (0x00000002)
#define EDDI_IOCC_HOST_LL_inst_hrp1_ll_sl                      (0x000E30)
#define EDDI_IOCC_HOST_LL_inst_hrp1_ll_nl                      (0x000E34)
#define EDDI_IOCC_HOST_LL_inst_hrp1_dest_adr                   (0x000E38)
//#define EDDI_IOCC_HOST_LL_inst_hrp1_dummy                      (0x000E3C)
#define EDDI_IOCC_HOST_LL_inst_status                          (0x000F00)
#define EDDI_IOCC_HOST_LL_inst_status_hwp0_used                (0x00000001)
#define EDDI_IOCC_HOST_LL_inst_status_hwp0_acc_run             (0x00000002)
#define EDDI_IOCC_HOST_LL_inst_status_hrp0_used                (0x00000004)
#define EDDI_IOCC_HOST_LL_inst_status_hrp0_acc_run             (0x00000008)
#define EDDI_IOCC_HOST_LL_inst_status_hwp1_used                (0x00000010)
#define EDDI_IOCC_HOST_LL_inst_status_hwp1_acc_run             (0x00000020)
#define EDDI_IOCC_HOST_LL_inst_status_hrp1_used                (0x00000040)
#define EDDI_IOCC_HOST_LL_inst_status_hrp1_acc_run             (0x00000080)
#define EDDI_IOCC_HOST_LL_inst_sync_reset                      (0x000F04)
#define EDDI_IOCC_HOST_LL_inst_sync_reset_hwp0                 (0x00000001)
#define EDDI_IOCC_HOST_LL_inst_sync_reset_hrp0                 (0x00000002)
#define EDDI_IOCC_HOST_LL_inst_sync_reset_hwp1                 (0x00000004)
#define EDDI_IOCC_HOST_LL_inst_sync_reset_hrp1                 (0x00000008)
//#define EDDI_IOCC_HOST_LL_inst_hwp0_dlength                    (0x000F08)
//#define EDDI_IOCC_HOST_LL_inst_hrp0_dlength                    (0x000F0C)
//#define EDDI_IOCC_HOST_LL_inst_hwp1_dlength                    (0x000F10)
//#define EDDI_IOCC_HOST_LL_inst_hrp1_dlength                    (0x000F14)
#define EDDI_IOCC_HOST_LL_inst_debug                           (0x000F18)
#define EDDI_IOCC_HOST_LL_inst_debug_param_err                 (0x00000001)
#define EDDI_IOCC_HOST_LL_inst_debug_dlength_err               (0x00000002)
#define EDDI_IOCC_HOST_LL_inst_debug_paea_err                  (0x00000004)
#define EDDI_IOCC_HOST_LL_inst_debug_size_err                  (0x00000008)
#define EDDI_IOCC_HOST_LL_inst_debug_addr_err                  (0x00000010)
#define EDDI_IOCC_HOST_LL_inst_ll_ram_adr_offset               (0x000F54)
#endif //!defined (EDDI_IOCC_HOST_LL_inst_hwp0_ram_start)

/*===========================================================================*/
/*                              Types                                        */
/*===========================================================================*/
typedef struct _EDDI_IOCC_INSTANCE_CB_TYPE  * EDDI_IOCC_INSTANCE_CB_PTR_TYPE;

/*===========================================================================*/
/*                              Structs                                      */
/*===========================================================================*/

//exception structure
typedef struct _EDDI_IOCC_EXCP_TYPE
{
    EDDI_IOCC_FATAL_ERROR_TYPE  Error;
    LSA_UINT32                  ModuleID;
    LSA_UINT32                  Line;
    LSA_UINT8                   sInfo[EDDI_IOCC_MAX_EXCP_INFO];
} EDDI_IOCC_EXCP_TYPE;

//channel structure. Each struct refers to 1 IOCC channel
typedef struct _EDDI_IOCC_CHANNEL_CB_TYPE
{
    EDDI_IOCC_LOWER_HANDLE_TYPE             IOCCHandle;     //Lower handle for usage check. NIL = free
    EDDI_IOCC_UPPER_HANDLE_TYPE             UpperHandle;    //UpperHandle. Supplied by application
    EDDI_IOCC_INSTANCE_CB_PTR_TYPE          pInstance;      //Backwards reference to instance
    LSA_VOID                              * pIOCC;          //Ptr to IOCC regs
    LSA_UINT8                             *	pDestBase;      //Ptr to destination base
    LSA_UINT32                              DestBasePhysAddr; //Phys addr of dest base
    LSA_UINT32                              MbxOffset;      //read mailbox offset 
    LSA_UINT8                             * pExtLinkListBase;           //Ptr to external linklist (virt)
    LSA_UINT32                              ExtLinkListBasePhysAddr;    //Ptr to external linklist (phys)
    LSA_UINT8                               LockCtr;        //lock semaphore counter
    LSA_UINT8                               AddRemoveCtr;               //Add remove semaphore counter
    LSA_UINT8                               ChannelIdx;     //channel index (0/1)
    LSA_BOOL                                bCopy2WB;       //TRUE: copy to WB internally. FALSE: Application copies to WB before calling eddi_iocc_single/multiple_write
    LSA_VOID                              * pPAEABase;      //Ptr to PAEARAM (virt)
    LSA_UINT32                            * pMbx;           //Ptr to mailbox (virt)
    LSA_UINT32                              MbxPhysAddr;    //Ptr to mailbox (phys)
    LSA_UINT32                              MbxVal;         //Next value for mailbox
    LSA_UINT32                            * pLLEntryRead;   //Ptr to Linklist-memory-entry for read-direction (virt)
    LSA_UINT32                              LLEntryReadPhysAddr;   //Ptr to Linklist-memory-entry for read-direction (phys)
    LSA_UINT32                            * pLLEntryWrite;  //Ptr to Linklist-memory-entry for write-direction (virt)
    LSA_UINT32                              LLEntryWritePhysAddr;  //Ptr to Linklist-memory-entry for write-direction (phys)
    LSA_UINT8                             * pWB;            //Start of write buffer
    EDDI_IOCC_ERROR_TYPE                  * pErrorInfo;
} EDDI_IOCC_CHANNEL_CB_TYPE;

//Instance structure. Each struct refers to 1 IOCC instance with 2 channels
typedef struct _EDDI_IOCC_INSTANCE_CB_TYPE
{
    LSA_BOOL                                bUsed;          //LSA_TRUE: Instance is used
    LSA_UINT32                              InstanceHandle; 
    LSA_UINT8                             * pIntLinkListMem;    //Start of internal LinkList memory
    EDDI_IOCC_CHANNEL_CB_TYPE               Channel[EDDI_IOCC_INT_CFG_IOCC_CHANNELS];
} EDDI_IOCC_INSTANCE_CB_TYPE;

//Info struct for exceptions
typedef struct _EDDI_IOCC_INFO_TYPE
{
    EDDI_IOCC_EXCP_TYPE                     Excp;
    EDDI_IOCC_INSTANCE_CB_TYPE              Instance[EDDI_IOCC_CFG_MAX_INSTANCES];
} EDDI_IOCC_INFO_TYPE;

//linklist element structure. Each struct refers to 1 linklist line
typedef struct _EDDI_IOCC_LINKLIST_ELEMENT_TYPE
{
    LSA_UINT32                            * pLLLine;        //Ptr to LL line (virt)
    LSA_VOID                              * UserIOHandle;   //UserIOHandle. Needed to search if deleting
    LSA_UINT8                             *	pSrc;           //WRITE: Ptr to src (virt.)
    LSA_UINT16                              DataLength;     //Data length (for remove)
} EDDI_IOCC_LINKLIST_ELEMENT_TYPE;

//linklist structure. Each struct refers to 1 linklist
typedef struct _EDDI_IOCC_LINKLIST_CB_TYPE
{
    EDDI_IOCC_LL_HANDLE_TYPE                LLHandle;       //LinkList handle passed to application
    EDDI_IOCC_CHANNEL_CB_TYPE             * pChannel;       //backwards ref to channel
    LSA_UINT8                               LinkListType;   //EDDI_IOCC_LINKLIST_TYPE_INTERNAL/EDDI_IOCC_LINKLIST_TYPE_EXTERNAL
    LSA_UINT8                               LinkListMode;   //EDDI_IOCC_LINKLIST_MODE_READ/EDDI_IOCC_LINKLIST_MODE_WRITE_SINGLE/EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP
    LSA_UINT16                              NrOfLines;      //Nr of lines alltogether
    LSA_UINT16                              NrOfUsedLines;  //Nr of used lines  
    LSA_UINT16                              DataLengthAll;  //DataLength all together. 1...1536 B  
    EDDI_IOCC_LINKLIST_ELEMENT_TYPE       * pLinkListElements; //Ptr to LinkList element array.
    LSA_UINT32                            * pLinkList;      //Ptr to LinkList
    LSA_UINT8                             *	pDest;          //READ: Ptr to destination (virt.)
    LSA_UINT32                              DestPhysAddr;   //READ: Ptr to destination (phys.)
    LSA_UINT32                              LinkListPhysAddr;
} EDDI_IOCC_LINKLIST_CB_TYPE;

/*===========================================================================*/
/*                              Protos                                       */
/*===========================================================================*/
static LSA_VOID EDDI_IOCC_FatalError( LSA_UINT32     const  Line,
                                      LSA_UINT8   *  const  sFile,
                                      LSA_UINT32     const  ModuleID,
                                      LSA_UINT8   *  const  sErr,
                                      LSA_UINT32     const  Error,
                                      LSA_UINT32     const  DW_0,
                                      LSA_UINT32     const  DW_1 );

/*===========================================================================*/
/*                              Macros                                       */
/*===========================================================================*/
#define EDDI_IOCC_REG32b(_Base, _Offset)    (*(LSA_UINT32 volatile *)(/*(LSA_UINT8 *)*/(_Base)+(_Offset)))

#define EDDI_IOCC_SET_LL_ENTRY(_pEntry, _PAEAOffset, _DataLength)   {*(_pEntry) = ((_PAEAOffset)<<14) + ((LSA_UINT32)_DataLength<<3);}

#define EDDI_IOCC_FILL_ERROR_INFO(_pErrorInfo, _Status, _DestAddr, _DestLength, _DebugReg) { \
(_pErrorInfo)->Line       = __LINE__;                                                        \
(_pErrorInfo)->ModuleID   = LTRC_ACT_MODUL_ID;                                               \
(_pErrorInfo)->Status     = (LSA_UINT32)(_Status);                                           \
(_pErrorInfo)->DestAddr   = (LSA_UINT32)(_DestAddr);                                         \
(_pErrorInfo)->DestLength = (LSA_UINT32)(_DestLength);                                       \
(_pErrorInfo)->DebugReg   = (LSA_UINT32)(_DebugReg);                                         \
}


#if defined (EDDI_IOCC_CFG_NO_FATAL_FILE_INFO)
#define EDDI_IOCC_Excp(sErr, Error, DW_0, DW_1) \
    /*lint --e(961) */   \
    EDDI_IOCC_FatalError((LSA_UINT32)__LINE__, (LSA_UINT8 *)(void *)EDDI_NULL_PTR, EDDI_IOCC_MODULE_ID, \
    (LSA_UINT8 *)(void *)(sErr), (LSA_UINT32)(Error), (LSA_UINT32)(DW_0), (LSA_UINT32)(DW_1))
#else
#define EDDI_IOCC_Excp(sErr, Error, DW_0, DW_1) \
    /*lint --e(961) */   \
    EDDI_IOCC_FatalError((LSA_UINT32)__LINE__, (LSA_UINT8 *)(void *)__FILE__, EDDI_IOCC_MODULE_ID, \
    (LSA_UINT8 *)(void *)(sErr), (LSA_UINT32)(Error), (LSA_UINT32)(DW_0), (LSA_UINT32)(DW_1))
#endif


#define EDDI_ENTER_IOCC_CH1_S(_pChannelCB)                                                  \
EDDI_ENTER_IOCC_CH1((_pChannelCB)->UpperHandle);                                            \
if (0 == (_pChannelCB)->LockCtr)                                                            \
{(_pChannelCB)->LockCtr++;}                                                                 \
else                                                                                        \
{/* no trace entries in macros possible (tracescanner) */                                   \
EDDI_IOCC_Excp("EDDI_ENTER_IOCC_CH1_S -> ", EDDI_IOCC_FATAL_ERR_EXCP, (_pChannelCB)->LockCtr, 0);\
}

#define EDDI_ENTER_IOCC_CH2_S(_pChannelCB)                                                  \
EDDI_ENTER_IOCC_CH2((_pChannelCB)->UpperHandle);                                            \
if (0 == (_pChannelCB)->LockCtr)                                                            \
{(_pChannelCB)->LockCtr++;}                                                                 \
else                                                                                        \
{/* no trace entries in macros possible (tracescanner) */                                   \
EDDI_IOCC_Excp("EDDI_ENTER_IOCC_CH2_S -> ", EDDI_IOCC_FATAL_ERR_EXCP, (_pChannelCB)->LockCtr, 0);\
}

#define EDDI_EXIT_IOCC_CH1_S(_pChannelCB)                                                   \
(_pChannelCB)->LockCtr--;                                                                   \
if (0 != (_pChannelCB)->LockCtr)                                                            \
{/* no trace entries in macros possible (tracescanner) */                                   \
EDDI_IOCC_Excp("EDDI_EXIT_IOCC_CH1_S -> ", EDDI_IOCC_FATAL_ERR_EXCP, (_pChannelCB)->LockCtr, 0); \
}                                                                                           \
EDDI_EXIT_IOCC_CH1((_pChannelCB)->UpperHandle);

#define EDDI_EXIT_IOCC_CH2_S(_pChannelCB)                                                   \
(_pChannelCB)->LockCtr--;                                                                   \
if (0 != (_pChannelCB)->LockCtr)                                                            \
{/* no trace entries in macros possible (tracescanner) */                                   \
EDDI_IOCC_Excp("EDDI_EXIT_IOCC_CH2_S -> ", EDDI_IOCC_FATAL_ERR_EXCP, (_pChannelCB)->LockCtr, 0); \
}                                                                                           \
EDDI_EXIT_IOCC_CH2((_pChannelCB)->UpperHandle);


#endif //(EDDI_CFG_SOC)

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif //EDDI_IOCC_INT_H
/*****************************************************************************/
/*  end of file eddi_iocc_int.h                                              */
/*****************************************************************************/

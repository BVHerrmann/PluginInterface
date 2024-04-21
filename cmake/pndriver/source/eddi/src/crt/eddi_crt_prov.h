#ifndef EDDI_CRT_PROV_H         //reinclude-protection
#define EDDI_CRT_PROV_H

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
/*  F i l e               &F: eddi_crt_prov.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
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
/*                                constants                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/

/* DataLen            :   DataLen of I/O-area of provider. Not Frame length! */
#define EDDI_IRT_DATALEN_MIN             40
#define EDDI_IRT_DATALEN_MAX             1490

// States for EDD-Provider
#define EDDI_CRT_PROV_STS_NOTUSED        0
#define EDDI_CRT_PROV_STS_RESERVED       1
#define EDDI_CRT_PROV_STS_INACTIVE       2
#define EDDI_CRT_PROV_STS_ACTIVE         3
#define EDDI_CRT_PROV_STS_AS_INACTIVE    4

// Maskbits for handling AUX/RTC3-Providers
#define EDDI_PROV_ACTIVE_NONE          0x0000
#define EDDI_PROV_ACTIVE_RTC3_MAIN     0x0001
#define EDDI_PROV_ACTIVE_AUX_MAIN      0x0002
#define EDDI_PROV_ACTIVE_CLASS12_MAIN  0x0100

// States IRTTop-SM
typedef enum _EDDI_PROV_IRTTOP_SM_STATE_TYPE
{
  EDDI_PROV_STATE_PASSIVE,
  EDDI_PROV_STATE_WF_CLASS3_TX,
  EDDI_PROV_STATE_RED_GREEN_ACTIVE,
  EDDI_PROV_STATE_RED_ACTIVE
} EDDI_PROV_IRTTOP_SM_STATE_TYPE;

// Trigger-events IRTTop-SM
typedef enum _EDDI_PROV_IRTTOP_SM_TRIGGER_TYPE
{
  EDDI_PROV_SM_TRIGGER_ACTIVATE,              //RTC3PSM (ANY->RUN) && IRDATA exist
  EDDI_PROV_SM_TRIGGER_ACTIVATE_AUX,          //Activate Aux (green)
  EDDI_PROV_SM_TRIGGER_PASSIVATE,             //passivating main provider
  EDDI_PROV_SM_TRIGGER_PASSIVATE_AUX         //passivating AUX provider (legacy-mode)
} EDDI_PROV_IRTTOP_SM_TRIGGER_TYPE;

/************************************************************************
 * Remote-Provider-Surveillance
 *
 ************************************************************************/
typedef enum _EDDI_RT_TYPE
{
    EDDI_RT_TYPE_NORMAL,
    EDDI_RT_TYPE_UDP

} EDDI_RT_TYPE;

/*---------------------------------------------------------------------------*/
/* CRT-PROVIDER-Structure                                                    */
/*---------------------------------------------------------------------------*/

// Contains all Param-Data for adding a Provider
typedef struct _EDDI_CRT_PROVIDER_PARAM_TYPE
{
    EDD_UPPER_MEM_U8_PTR_TYPE           pKRAMDataBuffer;  // Pointer of IN-Data in KRAM

    LSA_UINT16                          FrameId;
    EDD_MAC_ADR_TYPE                    DstMAC;
    LSA_UINT16                          CycleReductionRatio;
    LSA_UINT16                          CyclePhase;
    LSA_UINT32                          CyclePhaseSequence;

    LSA_UINT8                           ListType;     // ACW or FCW ?
    LSA_UINT8                           ProviderType;
    LSA_UINT8                           ImageMode;    // Defines OK ??

    LSA_UINT8                           SFPosition;   //PosT (DG only)
    LSA_UINT16                          DataLen;      // DataLen             (withOUT   APDU-Status !)
    LSA_UINT32                          DataOffset;   //!< Offset of DataBlock (INcluding APDU-Status In the case of 3B-Interface!) relative to Begin of KRAM/PAEA
    LSA_UINT32                          DataStatusOffset;   //!< Offset of DataStatus relative to Begin of KRAM  OR
                                                            // in case of "EDDI_CFG_SYSRED_2PROC" the provider_ID
    LSA_UINT32                          SFOffset;         //Offset of DG.C_SDU relative to the beginning of the packframe
    LSA_UINT32                          BufferProperties;

    EDDI_RT_TYPE                        RT_Type;
    EDDI_IP_TYPE                        UPD_DstIPAdr;

    LSA_UINT32                          SrtRawPortMask;

} EDDI_CRT_PROVIDER_PARAM_TYPE;

//typedef struct _EDDI_CRT_PROVIDER_PARAM_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_CRT_PROVIDER_PARAM_PTR_TYPE;

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
typedef struct _EDDI_CRT_UDP_PROV_BUF_TYPE
{
    EDDI_CRT_DATA_APDU_STATUS         *pAPDU;
    EDDI_DEV_MEM_U8_PTR_TYPE          pFrame;

    EDDI_DEV_MEM_U8_PTR_TYPE          pRTDataBuffer;
    EDDI_UDP_XRT_FRAME_PTR_TYPE       pIPHeader;

} EDDI_CRT_UDP_PROV_BUF_TYPE;

typedef struct _EDDI_CRT_UDP_PROV_TYPE
{

    EDDI_CRT_UDP_PROV_BUF_TYPE        Buf1;
    EDDI_CRT_UDP_PROV_BUF_TYPE        Buf2;
    EDDI_CRT_UDP_PROV_BUF_TYPE        Buf3;

    EDDI_CRT_UDP_PROV_BUF_TYPE      * pData;
    EDDI_CRT_UDP_PROV_BUF_TYPE      * pUser;
    EDDI_CRT_UDP_PROV_BUF_TYPE      * pNext;
    EDDI_CRT_UDP_PROV_BUF_TYPE      * pFree;

    LSA_UINT32                        FrameLength;

    EDD_RQB_TYPE                      RQB;
    EDD_RQB_NRT_SEND_TYPE             ParamSnd;

    EDDI_LOCAL_DDB_PTR_TYPE           pDDB;

} EDDI_CRT_UDP_PROV_TYPE;
#endif

typedef struct _EDDI_PROV_IRTTOP_TYPE
{
    EDDI_PROV_IRTTOP_SM_STATE_TYPE    ProvState;         
    struct _EDDI_CRT_PROVIDER_TYPE  * pAscProvider;
    LSA_UINT16                        ProvActiveMask;   
} EDDI_PROV_IRTTOP_TYPE;

typedef union _EDDI_PROV_LINK
{
    struct _EDDI_CRT_PROVIDER_TYPE * pNext;  // Chaining elements for phase tree: DO NOT MOVE!!!
    E_TREE_LIST                      TreeList;

} EDDI_PROV_LINK;

typedef struct _EDDI_CRT_PROVIDER_TYPE
{
    EDDI_PROV_LINK                    Link;
    
    LSA_UINT16                        ProviderId;          // Unique ident in a Provider-List, Needed to Control and to Remove a Provider
    LSA_UINT16                        GroupId;             // Needed for Producer-Set-State-Request
    LSA_UINT32                        UpperUserId;         // Needed in TimerScoreboard-Indications
    LSA_UINT32                        Status;              // Status for the EDD-Producer-StateMachine.
    #if defined (EDDI_CFG_SYSRED_2PROC)
    LSA_UINT32                        LatestDataStatusOrder_LE; // The last DataStatus set with eddi_SysRed_ProviderDataStatus_Set 
                                                                // in Little Endian format
    #endif

    LSA_UINT8                         LocalDataStatus;     // The last DataStatus set with ProviderControl-Service
    LSA_UINT16                        Properties;
    LSA_UINT16                        PendingIndEvent;     // Current pending Indication-Event for this Provider
    LSA_UINT16                        PendingCycleCnt;     // Frame CycleCounter of Indication-Event
    EDDI_PROV_IRTTOP_TYPE             IRTtopCtrl;  
    
    #if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_REV7)
    LSA_BOOL                          IsXChangingBuffer;   // True, if Provider is exchanging its Buffer
    #endif

    EDD_UPPER_MEM_U8_PTR_TYPE         pUserDataBuffer;     // Pointer to DataBuffer (can be in KRAM or User-RAM)

    EDDI_IRT_FRM_HANDLER_PTR_TYPE     pFrmHandler;         // Pointer to FrameHandler-Element

    LSA_BOOL                          bRedundantFrameID;   // TRUE if Redundant Provider is available
                                                        
    EDDI_TREE_ELEM_PTR_TYPE           pLowerCtrlACW;       // LowerLevel-Control-Structure for ACW-Entries
    EDDI_SER_CCW_PTR_TYPE             pCWStored;           // Stored ACW entrie due to passivate
    LSA_BOOL                          bXCW_DBInitialized;  // LSA_TRUE: The DB-ptr in the FCW/ACW is already initialized (IIIB_SW only)!

    EDDI_CRT_PROVIDER_PARAM_TYPE      LowerParams;         // Contains all Data for the lower structures

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) || defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    LSA_BOOL                          Locked;              // Describes if ConsumerBuffer is locked
    #endif
    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    EDD_UPPER_MEM_U8_PTR_TYPE         pLockedUserBuffer3B;
    LSA_BOOL                          bHadBeenActive;    // TRUE:had at least been activated once
    #endif
    #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    LSA_UINT32                      * pIOCW;          
    #endif
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDD_UPPER_RQB_PTR_TYPE            pRQB_PendingRemove;
    #endif

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    EDDI_CRT_UDP_PROV_TYPE            Udp;
    struct _EDDI_CRT_PROVIDER_TYPE *  pNext;
    #endif

    #if defined (EDDI_CFG_REV7)
    LSA_BOOL                          usingPAEA_Ram;      // !< If TRUE: using IOC storing the user data in the PAEARam; else 3-Buffer-Interface is used
    #endif

    struct _EDDI_CRT_PROVIDER_TYPE *  pNextAutoStopProv;
    LSA_UINT16                        AutoStopConsumerID;

    struct _EDDI_PRM_SF_DSCR_TYPE   * pSubFrmHandler;     // Pointer to SubFrameHandler-Element, in a PF: pointer to PF-Handler element
    #if defined (EDDI_CFG_DFP_ON)
    LSA_UINT8                         DGActiveCtr;        // Ctr for active DGs 
    LSA_UINT8                         DGPassiveCtr;       // Ctr for passive DGs
    LSA_BOOL                          bIsPFProvider;      // LSA_TRUE: Provider references a packframe
    LSA_UINT16                        PFProviderId;       // ProviderID of the dedicated packframe
    #endif
    
    LSA_BOOL                          bBufferParamsValid;
    LSA_BOOL                          bActivateAllowed;
    
} EDDI_CRT_PROVIDER_TYPE;

typedef       struct _EDDI_CRT_PROVIDER_TYPE EDDI_LOCAL_MEM_ATTR *       EDDI_CRT_PROVIDER_PTR_TYPE;
typedef const struct _EDDI_CRT_PROVIDER_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_CONST_CRT_PROVIDER_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* CRT-PROVIDER-List-Structure                                               */
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_CRT_PROVIDER_LIST_TYPE
{
    //LSA_UINT32                 Status;
    LSA_UINT32                   MaxEntries;       //number of available entries
    
    LSA_UINT32                   LastIndexRTC12;   // = index of currently highest valid entry
    LSA_UINT32                   LastIndexRTC3;    // = index of currently highest valid entry
    LSA_UINT32                   LastIndexDFP;     // = index of currently highest valid entry
    
    LSA_UINT32                   UsedEntries;      //number of currently valid entries
    LSA_UINT32                   UsedACWs;

    LSA_UINT32                   ActiveProvRTC12;  // number of active RTC12 prov., includes AUX prov.
    LSA_UINT32                   ActiveProvRTC3;   // number of active RTC3 prov., excludes DFP prov., includes packframes, redundant prov. count as 1

    EDDI_CRT_PROVIDER_PTR_TYPE   pEntry;           //Pointer to Array of Providers
    LSA_UINT16                   MaxGroups;
    LSA_UINT8                    GroupDataStatus[EDD_CFG_CSRT_MAX_PROVIDER_GROUP];
    LSA_UINT32                   MaxUsedReduction; //Maximum reduction used for a provider

    #if defined (EDDI_CFG_SYSRED_2PROC)
    LSA_UINT32                   LastSetDS_ChangeCount;      //
    EDDI_SHED_OBJ_TYPE           SysRedPoll;                 //
    LSA_UINT16                   MinSysRedProviderID;        //
    LSA_UINT16                   MaxSysRedProviderID;        //
    #endif
    //This information is needed to handle the CycleId-Ramp in SYNC-Modul

} EDDI_CRT_PROVIDER_LIST_TYPE;

typedef EDDI_CRT_PROVIDER_LIST_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_CRT_PROVIDER_LIST_PTR_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CRT_PROV_H


/*****************************************************************************/
/*  end of file eddi_crt_prov.h                                              */
/*****************************************************************************/

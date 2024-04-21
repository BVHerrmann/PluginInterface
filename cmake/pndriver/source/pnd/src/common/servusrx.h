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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: servusrx.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Local download user interface functions                                  */
/*                                                                           */
/*****************************************************************************/

#ifndef SERVUSER_H
#define SERVUSER_H

#include "pniobase.h"

#undef ATTR_PACKED
#if defined(_MSC_VER)
  #pragma pack( push, safe_old_packing, 4 )
  #define ATTR_PACKED
#elif defined(__GNUC__)
  #define ATTR_PACKED  __attribute__ ((aligned (4)))
#elif defined(BYTE_ATTR_PACKING)
  #include "pack.h"
  #define ATTR_PACKED PPC_BYTE_PACKED
#else
  #error please adapt pniousrx.h header for your compiler
#endif

/**
    \enum
    PNIO_CP_SELECT_TYPE The "PNIO_CP_SELECT_TYPE" data type selects the addressing mode for the network adapter.
    \param  PNIO_CP_SELECT_WITH_PCI_LOCATION Selection using PCI location
    \param  PNIO_CP_SELECT_WITH_MAC_ADDRESS Selection based on MAC address (relevant only with WinPcap)
 */
typedef enum
{
    PNIO_CP_SELECT_WITH_PCI_LOCATION  = 1,  ///< CP selected by PCI location 
    PNIO_CP_SELECT_WITH_MAC_ADDRESS   = 2   ///< CP selected by MAC address
} PNIO_CP_SELECT_TYPE;

/**
    \brief
    The "PNIO_MAC_ADDR_TYPE" data type shows an array for transferring the MAC address in conjunction with WinPcap.
*/
#define PNIO_MAC_ADDR_SIZE  6
typedef PNIO_UINT8 PNIO_MAC_ADDR_TYPE[PNIO_MAC_ADDR_SIZE];  

typedef struct pnio_sw_version_type_s 
{
    PNIO_UINT8 revision_prefix;          ///< revision prefix (i.E. T, P, ..)
    PNIO_UINT8 functional_enhancement;   ///< revision number for functional enhancement
    PNIO_UINT8 bug_fix;                  ///< revision number for bug fix
    PNIO_UINT8 internal_change;          ///< revision number for internal change
} PNIO_SW_VERSION_TYPE;

typedef struct pnio_system_description_s 
{
    PNIO_UINT8              Vendor[128];       
    PNIO_UINT8              ProductFamily[128];
    PNIO_UINT8              ProductSerialNr[128];
    PNIO_UINT8              IM_DeviceType[25+1];  //TO-DO: Make a desicion how to use PSI_MAX_SIZE_IM_DEVICE_TYPE here
    PNIO_UINT8              IM_OrderId[20+1];  //TO-DO: Make a desicion how to use PSI_MAX_SIZE_IM_ORDER_ID here    
    PNIO_UINT16             IM_HwRevision;
    PNIO_SW_VERSION_TYPE    IM_SwVersion; 
    PNIO_UINT16             IM_RevisionCounter;
    PNIO_UINT16             IM_Version;
    PNIO_UINT16             IM_Supported;
    PNIO_UINT16             IM_ProfileSpecificType;
    PNIO_UINT16             IM_ProfileId;
} PNIO_SYSTEM_DESCR;

/**
    \brief
    The "PNIO_PCI_LOCATION_TYPE" shows the parameter for transferring the PCI location of the network adapter.
    \param BusNr PCI bus number
    \param DeviceNr PCI device number
    \param FunctionNr PCI function number
*/
typedef struct pnio_pci_location_s
{
    PNIO_UINT16 BusNr;       ///< Bus number
    PNIO_UINT16 DeviceNr;    ///< Device number
    PNIO_UINT16 FunctionNr;  ///< Function number

} PNIO_PCI_LOCATION_TYPE, *PNIO_PCI_LOCATION_PTR_TYPE;

/**
    \brief
    The data type "PNIO_CP_ID_TYPE" contains information on a network adapter.
    \param CpSelection Address mode for selecting the network adapter. 
        With "WinPcap" you need to set the value to "PNIO_CP_SELECT_WITH_MAC_ADDRESS".
    \param CpMacAddr MAC address of the network adapter (WinPcap).
    \param CpPciLocation PCI location of the network adapter.
    \param Description[300]	Contains a description of the network adapter.
*/
typedef struct pnio_cp_id_s
{
    PNIO_CP_SELECT_TYPE        CpSelection;      ///< Selector for board selection
    PNIO_MAC_ADDR_TYPE         CpMacAddr;        ///< MAC address of CP
    PNIO_PCI_LOCATION_TYPE     CpPciLocation;    ///< PCI location of CP
    PNIO_UINT8                 Description[300]; ///< Wpcap: "Intel(R) Ethernet Connection I217-V" / "PnDevDriver: I210 board"
} PNIO_CP_ID_TYPE, *PNIO_CP_ID_PTR_TYPE;

/// enum of PNIO stack components (used for trace level setting)
enum pnio_stack_comp {
    PNIO_TRACE_COMP_ACP,
    PNIO_TRACE_COMP_CLRPC,
    PNIO_TRACE_COMP_CM,
    PNIO_TRACE_COMP_DCP,
    PNIO_TRACE_COMP_EDDI,
    PNIO_TRACE_COMP_EDDS,
    PNIO_TRACE_COMP_EPS,
    PNIO_TRACE_COMP_GSY,
    PNIO_TRACE_COMP_HIF,
	PNIO_TRACE_COMP_LLDP,
	PNIO_TRACE_COMP_NARE,    
    PNIO_TRACE_COMP_OHA,
    PNIO_TRACE_COMP_PNTRC,
    PNIO_TRACE_COMP_PSI,
    PNIO_TRACE_COMP_SNMPX,
    PNIO_TRACE_COMP_SOCK,
    PNIO_TRACE_COMP_TCIP,
    PNIO_TRACE_COMP_PND,

    PNIO_TRACE_COMP_NUM //has to be last element!
};

enum pnio_trace_level
{
    PNIO_TRACE_LEVEL_OFF,
    PNIO_TRACE_LEVEL_FATAL,
    PNIO_TRACE_LEVEL_ERROR,
    PNIO_TRACE_LEVEL_UNEXP,
    PNIO_TRACE_LEVEL_WARN,
    PNIO_TRACE_LEVEL_NOTE_HIGH,
    PNIO_TRACE_LEVEL_NOTE,
    PNIO_TRACE_LEVEL_NOTE_LOW,
    PNIO_TRACE_LEVEL_CHAT,

    PNIO_TRACE_LEVEL_NUM //has to be last element!
};

/**
    \brief
    This callback function is used to receive trace data. 
    This gives the user the option of saving the trace data in the file system or similar.
    \param pBuffer Pointer to the trace data buffer
    \param BufferSize Length of the memory area the "pBuffer" parameter references.	
*/
typedef void (*PNIO_PNTRC_BUFFER_FULL)
    ( PNIO_UINT8       * pBuffer,     
    PNIO_UINT32        BufferSize
);

/**
    \brief
    This callback function is used to write the trace buffers to file system
    \param bIsFatal FATAL occured in PN Stack
    */
typedef void(*PNIO_PNTRC_WRITE_TRACE_BUFFER)
(PNIO_UINT8       bIsFatal
    );

/**
\brief
    This structure contains the callback functions and the initial trace levels of the trace subsystems. 
    The user specifies this structure as a parameter for "SERV_CP_init". 
    If the parameter "CbfPntrcBufferFull" is set to "ZERO", the internal trace system is not used. 
    Otherwise, if the trace buffer overflows, this callback function is called and 
    the user can back up the transferred trace data.
    \param CbfPntrcBufferFull Is used to transfer trace data.
    \param TraceLevels[PNIO_TRACE_COMP_NUM] Array of all internal modules for setting the initial trace level.
*/
typedef struct pnio_debug_settings_s
{
    PNIO_PNTRC_BUFFER_FULL        CbfPntrcBufferFull;                 ///< CBF for receiving trace data; if NULL debug trace is turned off
    PNIO_PNTRC_WRITE_TRACE_BUFFER CbfPntrcWriteBuffer;                ///< CBF for writing trace data to file system
    PNIO_UINT8                    TraceLevels[PNIO_TRACE_COMP_NUM];   ///< set trace levels for each component over all CPs
} PNIO_DEBUG_SETTINGS_TYPE, *PNIO_DEBUG_SETTINGS_PTR_TYPE;



/**
    \brief
    This callback function is called as soon as the trace levels have been set. 
          This callback is registered with the SERV_CP_set_trace_level call.
 */
typedef void (*PNIO_PNTRC_SET_TRACE_LEVEL_DONE)(void);
#ifdef __cplusplus
extern "C" {
#endif

/**
    \brief
    This must be called as the last function when exiting the user program.
    After the return of the function, all internal threads have been exited and 
    the entire local memory has been released again.
    \note The "SERV_CP_shutdown" function may only be called after the "PNIO_controller_close( )" function.
    \return If successful, PNIO_OK is returned. If an error occurs, the following value is returned 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_SEQUENCE
 */
PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_shutdown ();

/**
    \brief
    This function starts all internal tasks and configures the PNIO stack according to the configuration.
    The following parameters are set by the user program:
    Selection of the Ethernet interfaces based on the MAC address when using the WinPcap driver or
    based on the PCI location with all other variants.PN Driver V1.0 only supports operation of a single interface.
    This means that "CpList" may only contain a single element and that "NrOfCp" must always be "1".
    Configuration data: The configuration which is prepared at Engineering phase.
    Retentive data: The data which is hold by Rema file. This file stores the data that user wants to keep for the next running session of the system.
    System description data: The data which holds system information like Vendor ID, Product Family, Product Serial Number etc.
    \note The "SERV_CP_startup" function must be called before the "PNIO_controller_open()" function.
    \param CpList  Array of the designation that selects the network adapter to be used.
    \param NrOfCp  Number of elements in "CpList". Must always be set to "1".
    \param pConfigData  Pointer to the memory containing the XML PROFINET configuration in the form of a character string. 
        The end of the string must be indicated by a "0" in the last byte (terminating NULL).
    \param ConfigDataLen  Length of "pConfigData" in bytes without the terminating NULL.
    \param pRemaData  Pointer to the retentive data, can be NULL.
    \param RemaDataLen  Length of the memory area the "pRemaData" parameter references.
    \param pSysDescr Pointer to the structure "PNIO_SYSTEM_DESCR".
    \return If successful, PNIO_OK is returned. 
        If an error occurs, the following values are possible 
        (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_SEQUENCE
        - PNIO_ERR_PRM_POINTER
        - PNIO_ERR_CREATE_INSTANCE
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_CORRUPTED_DATA
*/  
PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_startup (
    PNIO_CP_ID_PTR_TYPE           CpList,           //in
    PNIO_UINT32                   NrOfCp,           //in
    PNIO_UINT8                  * pConfigData,      //in
    PNIO_UINT32                   ConfigDataLen,    //in
    PNIO_UINT8                  * pRemaData,        //in
    PNIO_UINT32                   RemaDataLen,      //in
    PNIO_SYSTEM_DESCR           * pSysDescr
);

/**
    \brief
    With this function, the trace level of individual internal components can be changed during operation. 
        The function can be called at any time after the "SERV_CP_startup( )" function. 
        After being called, the function returns immediately and 
        acknowledges the successful change of the trace level with the "CbfSetTraceLevelDone" callback function.
    \param  Component  Selection of the PNIO stack component (enum pnio_stack_comp in servusrx.h).
    \param  TraceLevel  Selection of the PNIO stack trace level (enum pnio_trace_level in seruvsrx.h).
    \param  CbfSetTraceLevelDone  Callback function for acknowledging the change.
    \return If successful, PNIO_OK is returned. If an error occurs, 
        the following values are possible (for the meaning, refer to the comments in the header file "pnioerrx.h"):
        - PNIO_ERR_SEQUENCE
        - PNIO_ERR_INTERNAL
        - PNIO_ERR_PRM_INVALIDARG
 */
PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_set_trace_level ( 
    PNIO_UINT32                       Component,            //in
    PNIO_UINT32                       TraceLevel,           //in
    PNIO_PNTRC_SET_TRACE_LEVEL_DONE   CbfSetTraceLevelDone  //in
);

/**
    \brief
    SERV_CP_init stands for initialization of Communication Processor services.
    This function has several tasks and needs to be called in the start sequence of the PN Driver. 
    It performs the following tasks:
    Initialization of the trace subsystem (if enabled)
    Initialization of the PNIO stack
    Initialization of the mailbox subsystem
    Initialization of the threads
    Starting the threads
    \note The function "SERV_CP_init" must be called as the first function in the start sequence. 
    Only afterwards can "SERV_CP_startup" and "PNIO_controller_open" or "PNIO_interface_open" be called.
    \note The Trace function is useful for troubleshooting if there are problems in the configuration or system adaptation.
        Contact Siemens support to analyze the trace data and to obtain advice on setting the correct trace level. 
    \param DebugSetting This structure allows the user to make a callack function available that is called as soon as the trace buffer is full. 
        The trace levels can also be set. If the parameter has the value "NULL", the internal trace is not used.
    \return if successful, the function returns "PNIO_OK" otherwise "PNIO_ERR_SEQUENCE".
 */
PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_init (
    PNIO_DEBUG_SETTINGS_PTR_TYPE  DebugSetting
);

/**
    \brief
    This function returns a list of all detected network adapters.
    \note The function "SERV_CP_init" must have been called previously.
    \param CpList List of network adapters
    \param NrOfCp Number of list elements in CpList.
    If successful, the function returns "PNIO_OK" otherwise "PNIO_ERR_NO_ADAPTER_FOUND".
*/
PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_get_network_adapters( 
    PNIO_CP_ID_PTR_TYPE CpList, // output
    PNIO_UINT8 *NumberOfCps // output
);

/**
    \brief
    This function completes the shutdown sequence.
    \note The function "SERV_CP_undo_init" needs to be called as the last function in the shutdown sequence. 
    Prior to this, the functions "PNIO_controller_close" or "PNIO_interface_close" and 
    "SERV_CP_shutdown" need to be called.
    \param " - " This function has no parameters.
    \return This function has no return values.
 */
PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_undo_init ();
#ifdef __cplusplus
}
#endif



#if defined(_MSC_VER)
  #pragma pack( pop, safe_old_packing )
#elif defined(BYTE_ATTR_PACKING)
  #include "unpack.h"
#endif

#endif /* SERVUSER_H */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#ifndef EDDI_IOCC_H              //reinclude-protection
#define EDDI_IOCC_H

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
/*  F i l e               &F: eddi_iocc.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* User Interface for IOCC support                                           */
/* Defines constants, types, macros and prototyping for prefix.              */
/*                                                                           */
/*****************************************************************************/
/** @file eddi_iocc.h "EDDI IOCC User Interface for IOCC support"  **/
/**
@defgroup eddi_iocc_api EDDI IOCC Input functions
@ingroup eddi-iocc-group
@{
**/


/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/
#define EDDI_IOCC_FATAL_ERROR_TYPE      LSA_UINT32

#define EDDI_IOCC_FATAL_NO_ERROR        (EDDI_IOCC_FATAL_ERROR_TYPE)  0x00   
#define EDDI_IOCC_FATAL_ERR_EXCP        (EDDI_IOCC_FATAL_ERROR_TYPE)  0x0D

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/
#define EDDI_IOCC_LOWER_HANDLE_TYPE LSA_VOID * 
/**< Type of the lower handle returned to the application during @ref eddi_IOCC_Setup **/

#define EDDI_IOCC_LL_HANDLE_TYPE LSA_VOID * 
/**< Type of the LinkList handle returned to the application during @ref eddi_IOCC_CreateLinkList **/

#define EDDI_IOCC_FATAL_ERROR_PTR_TYPE struct lsa_fatal_error_tag *
/**< pointer to LSA_FATAL_ERROR **/

typedef struct _EDDI_IOCC_ERROR_TYPE
{
    LSA_UINT32  Status;     /**< IOCC status. See [/20/, ch. 4.3.6.](@ref eddi-ref-no-20)
                            @attention For some errors, *Status* can contain other information!

                            @imageSize{eddi_iocc_iocc_statusreg.png,width:700px;;,IOCC status register} **/
    LSA_UINT32  DestAddr;   /**< Physical destination address of a read transfer. 
                            @attention For some errors, *DestAddr* can contain other information!  **/
    LSA_UINT32  DestLength; /**< Lenght (in bytes) of a read transfer. **/
    LSA_UINT32  DebugReg;   /**< IOCC debug register. See [/20/, ch. 4.3.6.](@ref eddi-ref-no-20) \n
                            @attention For some errors, *DebugReg* can contain other information!

                            @imageSize{eddi_iocc_iocc_debugreg.png,width:700px;;,IOCC debug register} **/
    LSA_UINT32  Line;       /**< Code linenumber where the error occurred. **/
    LSA_UINT32  ModuleID;   /**< ModuleID where the error occurred. **/
} EDDI_IOCC_ERROR_TYPE; /**< The type used for error returns */

/*===========================================================================*/
/*                                  protos                                   */
/*===========================================================================*/
/**************************************************************************//**
@brief Initialisation of administrative data for all instances of eddi_IOCC.
@details This function is used to setup and allocate the administrative data for **all instances** of eddi_IOCC. 
@warning No care will be taken if IOCC transfers are still running.
@warning Administrative data and knowledge for **all instances** will be lost.
@return -
******************************************************************************/
LSA_VOID eddi_IOCC_Init(LSA_VOID);


/**************************************************************************//**
@brief Deinitialisation of administrative data for all instances of eddi_IOCC.
@details This function frees the resources claimed with eddi_IOCC_Init. 
@warning No care will be taken if IOCC transfers are still running.
@warning Administrative data and knowledge for **all instances** will be lost.
@return -
******************************************************************************/
LSA_VOID eddi_IOCC_Deinit(LSA_VOID);


/**************************************************************************//**
@brief Setup one of the two IOCC channels for one instance.
@details This function is used to claim and initialize one IOCC channel for one interface. As IOCC handling is independent of EDDI, and can run on a separate processor, all necessary information has to be supplied. \n
•	If the first IOCC channel of an instance is setup, the common IOCC instance parts get initialized. \n
•	The specified IOCC channel gets initialized and prepared for transfer. \n
@imageSize{eddi_iocc_6.2_pic1.png,width:700px;;,Transfers to PAEARAM (Provider)}
For explanation see @ref eddi-iocc-transfers-to-paearam

@imageSize{eddi_iocc_6.3_pic1.png,width:700px;;,Transfers from PAEARAM (Consumer)}
For explanation see @ref eddi-iocc-transfers-from-paearam

@warning Note to system design:\n
eddi_IOCC_Setup() is not protected by ENTER/EXIT macros. Thus the application has to make sure that it is not called from different thread levels!

@attention The area pointed to by pDestBase either has to be uncached, or cached on a system featuring cache coherency! \n
If none of these can be provided, it is advisable to use SOC TPA RAM as an intermediate memory and then copy from TPA RAM to the final destination.

## Return of the request
Result (Response)    | Meaning
---------------------|--------------------------------
EDD_STS_OK           | Setup successful.
EDD_STS_ERR_PARAM    | Parameter error.
EDD_STS_ERR_HW       | IOCC did not react as requested.
EDD_STS_ERR_RESOURCE | IOCC channel is already used.

@param [inout]  pIOCCHandle      Lower IOCC handle returned by this function uniquely identifying the IOCC channel on this instance.
@param [in]     UpperHandle      Upper IOCC handle supplied by the application. This handle is passed to output macros.
@param [in]     InstanceHandle   A 32bit number unambigously specifying this instance. eddi_IOCC uses this number to group the 2 IOCC channels that belong to each instance.
@param [in]     IOCCChannel      IOCC channel used. Allowed range:\n
1:	Use IOCC channel 1\n
2:	Use IOCC channel 2
@param [in]     pIOCC            Start of the IOCC-registers. This is a **virtual address** (as the user sees the IOCC-registers). By this pointer eddi_IOCC also calculates the virtual address of the WriteBuffer and of the beginning of PAEARAM.
@param [in]     pDestBase        Base address of the destination IO area (IOC: “Inputs” ). This is a **virtual address** (as the user sees this area). See picture in details. \n
This parameter is used for transfers from PAEARAM to application memory only.
@param [in]     DestBasePhysAddr Base   address of the destination IO area (IOC: “Inputs” ). This is the **physical address** as IOCC would see this area. See [/21/](@ref eddi-ref-no-21) for AHB addresses and picture in details. \n
This parameter is used for transfers from PAEARAM to application memory only.
@param [in]     pExtLinkListBase        Base of LinkList memory in external memory (only SOC1-DDR2 is allowed here), allocated with @ref EDDI_IOCC_ALLOC_LINKLIST_MEM. This is a **virtual address** (as the user sees this area).
                                        @note This parameter has to be supplied with a value !=0 even if the external linklists are not used.
@param [in]     ExtLinkListBasePhysAddr Base of LinkList memory in external memory. This is a **phsysical address** (as IOCC would see this area).
@param [inout]  ppIntLinkListMem        Returns the address of internal LinkList memory, for pool creation. Calculated using pIOCC.
@param [inout]  pIntLinkListMemSize     Returns the size of internal LinkList memory, for pool creation. Typically 8K - internally used LinkList lines
@param [inout]  ppWB                    Returns the address of the writebuffer for this channel. Calculated using pIOCC and IOCCChannel.

ppWB          | Result
--------------|----------------------
0             | @ref eddi_IOCC_SingleWrite and @ref eddi_IOCC_MultipleWrite with Mode = EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP will copy the data from pSrc with internal copy functions. No address returned.
valid pointer | @ref eddi_IOCC_SingleWrite and @ref eddi_IOCC_MultipleWrite with Mode = EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP will omit copying the data. @attention The user is completely responsible for copying the data to the writebuffer by itself. \n Take care to **exactly** copy as much bytes as stated in DataLength (@ref eddi_IOCC_SingleWrite) or as the sum of all linklist line DataLength. Otherwise the transfer will not start!

@param [in]     MbxOffset        Offset of a 32bit value from pDestBase needed as write-mailbox to check for an end of the transfer (internally). See picture in details. \n
This parameter is used for transfers from PAEARAM to application memory only.
@param [in]     pErrorInfo       Pointer to @ref EDDI_IOCC_ERROR_TYPE where eddi_IOCC stores error information if the read/write functions return with EDD_STS_ERR_HW.
@return EDD_STS_OK on success @return EDD_STS_ERR_PARAM/EDD_STS_ERR_HW/EDD_STS_ERR_RESOURCE on failure
******************************************************************************/
LSA_RESULT eddi_IOCC_Setup( EDDI_IOCC_LOWER_HANDLE_TYPE      * pIOCCHandle,
                            EDDI_IOCC_UPPER_HANDLE_TYPE        UpperHandle,
                            LSA_UINT32                 const   InstanceHandle,
                            LSA_UINT8                  const   IOCCChannel,
                            LSA_VOID                 * const   pIOCC,
                            LSA_UINT8                * const   pDestBase,
                            LSA_UINT32                 const   DestBasePhysAddr,
                            LSA_UINT8                * const   pExtLinkListBase,
                            LSA_UINT32                 const   ExtLinkListBasePhysAddr,
                            LSA_UINT8             *  * const   ppIntLinkListMem,
                            LSA_UINT32               * const   pIntLinkListMemSize,
                            LSA_UINT8             *  * const   ppWB,
                            LSA_UINT32                 const   MbxOffset,
                            EDDI_IOCC_ERROR_TYPE     * const   pErrorInfo);


/**************************************************************************//**
@brief Shuts down one IOCC channel.
@details This function is used to shutdown the interface for one IOCC channel for one interface. \n
@warning Note to system design:\n
eddi_IOCC_Shutdown() is not protected by ENTER/EXIT macros. Thus the application has to make sure that it is not called from different thread levels!

@param [in] IOCCHandle Handle returned by eddi_IOCC_Setup.
@return EDD_STS_OK on success @return EDD_STS_ERR_PARAM on failure
******************************************************************************/
LSA_RESULT eddi_IOCC_Shutdown (EDDI_IOCC_LOWER_HANDLE_TYPE const IOCCHandle);


/**************************************************************************//**
@brief Creates a LinkList.
@details This function is used to create a LinkList. Memory gets allocated using @ref EDDI_IOCC_ALLOC_LINKLIST_MEM.
@warning Note to system design:\n
eddi_IOCC_CreateLinkList() is not protected by ENTER/EXIT macros. Thus the application has to make sure that it is not called from different thread levels!

## Return of the request
Result (Response)    | Meaning
---------------------|--------------------------------
EDD_STS_OK           | Creation successful.
EDD_STS_ERR_PARAM    | Parameter error.
EDD_STS_ERR_RESOURCE | No more memory.

@param [in] IOCCHandle    Handle returned by eddi_IOCC_Setup.
@param [inout] pLLHandle  LinkList handle returned by this function unambigously identifying this LinkList.
@param [in] LinkListType  Memory type of LinkList:
|                                 |                               |
|---------------------------------|--------------------------------|
|EDDI_IOCC_LINKLIST_TYPE_INTERNAL | LinkList is created in IOCC internal LinkList memory
|EDDI_IOCC_LINKLIST_TYPE_EXTERNAL | LinkList is created in external memory (SOC1 DDR2).

@param [in] LinkListMode  Mode:
|                                     |                               |
|-------------------------------------|--------------------------------
|EDDI_IOCC_LINKLIST_MODE_READ         | Transfer from PAEARAM to application memory. \n All IO objects will be transferred to application memory together.
|EDDI_IOCC_LINKLIST_MODE_WRITE_SINGLE | Transfer application memory to PAEARAM . \n Each IO object will be copied to the WriteBuffer seperately.
|EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP  | Transfer application memory to PAEARAM . \n All IO objects will be copied to the WriteBuffer together.

@param [in] NrOfLines     Nr. of (possible) LinkList lines. **Range: 1..1536**
@note Only LinkList lines appended with @ref eddi_IOCC_AppendToLinkList are used!
@return EDD_STS_OK on success @return EDD_STS_ERR_PARAM/EDD_STS_ERR_RESOURCE on failure
******************************************************************************/
LSA_RESULT eddi_IOCC_CreateLinkList(EDDI_IOCC_LOWER_HANDLE_TYPE    const   IOCCHandle,
                                    EDDI_IOCC_LL_HANDLE_TYPE     *         pLLHandle,
                                    LSA_UINT8                      const   LinkListType, 
                                    LSA_UINT8                      const   LinkListMode, 
                                    LSA_UINT16                     const   NrOfLines);

#define EDDI_IOCC_LINKLIST_TYPE_INTERNAL        1
#define EDDI_IOCC_LINKLIST_TYPE_EXTERNAL        2
#define EDDI_IOCC_LINKLIST_MODE_READ            3
#define EDDI_IOCC_LINKLIST_MODE_WRITE_SINGLE    4
#define EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP     5

/**************************************************************************//**
@brief Deletes a LinkList.
@details This function is used to delete a LinkList. Memory gets freed using @ref EDDI_IOCC_FREE_LINKLIST_MEM.
@warning Note to system design:\n
eddi_IOCC_DeleteLinkList() is not protected by ENTER/EXIT macros. Thus the application has to make sure that it is not called from different thread levels!
@note Lines do not have to be removed (using @ref eddi_IOCC_RemoveFromLinkList) before deleting a LinkList.

## Return of the request
Result (Response)    | Meaning
---------------------|--------------------------------
EDD_STS_OK           | Deletion successful.
EDD_STS_ERR_PARAM    | Parameter error.
EDD_STS_ERR_RESOURCE | Mem free error.

@param [in] LLHandle      LinkList handle returned by @ref eddi_IOCC_CreateLinkList
@return EDD_STS_OK on success @return EDD_STS_ERR_PARAM/EDD_STS_ERR_RESOURCE on failure
******************************************************************************/
LSA_RESULT eddi_IOCC_DeleteLinkList(EDDI_IOCC_LL_HANDLE_TYPE const LLHandle);


/**************************************************************************//**
@brief Appends a LinkList line to the end of a LinkList.
@details 

## Return of the request
Result (Response)    | Meaning
---------------------|--------------------------------
EDD_STS_OK           | Operation successful.
EDD_STS_ERR_PARAM    | Parameter error.
EDD_STS_ERR_RESOURCE | LinkList is full.

@param [in] LLHandle      LinkList handle returned by @ref eddi_IOCC_CreateLinkList
@param [in] PAEARAMOffset **EDDI_IOCC_LINKLIST_MODE_READ**: Offset of the first **source** byte of this IO object in PAEARAM.
                       \n **EDDI_IOCC_LINKLIST_MODE_WRITE_xxx**: Offset of the first **destination** byte of this IO object in PAEARAM.
@param [in] DataLength    Nr. of bytes to be copied. **Range: 1...1512**
@param [in] pMem          **EDDI_IOCC_LINKLIST_MODE_READ**: Destination address of the first byte *of the very first* IO object of this LinkList.\n
                          Has to be in range started with pDestBase (@ref eddi_IOCC_Setup).\n
                          All pMem addresses have to be the same!
                       \n **EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP**: Src address of the first byte *of the very first* IO object of this LinkList.\n
                          Has to be in range started with pDestBase (@ref eddi_IOCC_Setup).\n
                          All pMem addresses have to be the same!
                       \n **EDDI_IOCC_LINKLIST_MODE_WRITE_SINGLE**: Src address of the first byte of *this* IO object.\n
                          Has to be in range started with pDestBase (@ref eddi_IOCC_Setup).\n
@param [in] UserIOHandle  Handle specified by the user identifying the IO object. @ref eddi_IOCC_RemoveFromLinkList needs this handle to find the related element. Fixed type (void *) to simplify comparisons in eddi_IOCC.
@return EDD_STS_OK on success @return EDD_STS_ERR_PARAM/EDD_STS_ERR_RESOURCE on failure
******************************************************************************/
LSA_RESULT eddi_IOCC_AppendToLinkList(EDDI_IOCC_LL_HANDLE_TYPE     const   LLHandle,
                                      LSA_UINT32                   const   PAEARAMOffset, 
                                      LSA_UINT16                   const   DataLength,
                                      LSA_UINT8                  * const   pMem,
                                      LSA_VOID                   * const   UserIOHandle);


/**************************************************************************//**
@brief Removes a LinkList line from a LinkList.
@details 
@warning Time consuming, because elements have to be copied!

## Return of the request
Result (Response)    | Meaning
---------------------|--------------------------------
EDD_STS_OK           | Operation successful.
EDD_STS_ERR_PARAM    | Parameter error

@param [in] LLHandle      LinkList handle returned by @ref eddi_IOCC_CreateLinkList
@param [in] UserIOHandle  Handle identifying the IO object, specified at @ref eddi_IOCC_AppendToLinkList
@return EDD_STS_OK on success @return EDD_STS_ERR_PARAM on failure
******************************************************************************/
LSA_RESULT eddi_IOCC_RemoveFromLinkList(EDDI_IOCC_LL_HANDLE_TYPE     const   LLHandle,
                                        LSA_VOID        const      * const   UserIOHandle);


/**************************************************************************//**
@brief Consistent read of a single IO object.
@details This function is used to consistent read a single IO object from PAEARAM to application memory.

## Return of the request
Result (Response)    | Meaning
---------------------|--------------------------------
EDD_STS_OK           | Read successful.
EDD_STS_OK_PENDING   | Channel is still in use.
EDD_STS_ERR_PARAM    | Parameter error.
EDD_STS_ERR_HW       | IOCC did not react as requested.

@param [in] IOCCHandle    Handle returned by eddi_IOCC_Setup.
@param [in] DataLength    Nr. of bytes to be copied. **Range: 1...1512**.
@param [in] PAEARAMOffset Offset of the first **source** byte in PAEARAM.
@param [in] pDest         Destination address. Has to be in range started with pDestBase (@ref eddi_IOCC_Setup).
@return EDD_STS_OK/EDD_STS_OK_PENDING on success @return EDD_STS_ERR_PARAM/EDD_STS_ERR_HW on failure
******************************************************************************/
LSA_RESULT eddi_IOCC_SingleRead (EDDI_IOCC_LOWER_HANDLE_TYPE    const   IOCCHandle,
                                 LSA_UINT32                     const   DataLength, 
                                 LSA_UINT32                     const   PAEARAMOffset,
                                 LSA_UINT8                    * const   pDest);


/**************************************************************************//**
@brief Consistent write of a single IO object.
@details This function is used to consistent write a single IO object to PAEARAM from application memory.

## Return of the request
Result (Response)    | Meaning
---------------------|--------------------------------
EDD_STS_OK           | Write successful.
EDD_STS_OK_PENDING   | Channel is still in use.
EDD_STS_ERR_PARAM    | Parameter error.
EDD_STS_ERR_HW       | IOCC did not react as requested.

@param [in] IOCCHandle    Handle returned by eddi_IOCC_Setup.
@param [in] DataLength    Nr. of bytes to be copied. **Range: 1...1512**.
@param [in] PAEARAMOffset Offset of the first **destination** byte in PAEARAM.
@param [in] pSrc          Source address. This is a **virtual address**.
@return EDD_STS_OK/EDD_STS_OK_PENDING on success @return EDD_STS_ERR_PARAM/EDD_STS_ERR_HW on failure
******************************************************************************/
LSA_RESULT eddi_IOCC_SingleWrite (EDDI_IOCC_LOWER_HANDLE_TYPE    const   IOCCHandle,
                                  LSA_UINT32                     const   DataLength, 
                                  LSA_UINT32                     const   PAEARAMOffset,
                                  LSA_UINT8                    * const   pSrc);


/**************************************************************************//**
@brief Consistent read of a complete set of IO objects
@details This function is used to consistently read a whole set of IO objects from PAEARAM to application memory. 
@warning 
- If @ref eddi_IOCC_RemoveFromLinkList should still be running for a LinkList in another thread, the transfer for this complete LinkList will be skipped and EDD_STS_OK_NO_DATA returned!
- LLHandles all have to address the same IOCCHandle!

## Return of the request
Result (Response)    | Meaning
---------------------|--------------------------------
EDD_STS_OK           | Read successful.
EDD_STS_OK_PENDING   | Channel is still in use.
EDD_STS_OK_NO_DATA   | LinkList is in use by @ref eddi_IOCC_RemoveFromLinkList or @ref eddi_IOCC_AppendToLinkList.
EDD_STS_ERR_PARAM    | Parameter error.
EDD_STS_ERR_HW       | IOCC did not react as requested.

@param [in] pLLHandle     Pointer to array of LinkList handles (returned by @ref eddi_IOCC_CreateLinkList)
@param [in] NrOfLinkLists Nr. of LinkLists in LinkList array.
@return EDD_STS_OK/EDD_STS_OK_PENDING/EDD_STS_OK_NO_DATA on success @return EDD_STS_ERR_PARAM/EDD_STS_ERR_HW on failure
******************************************************************************/
LSA_RESULT eddi_IOCC_MultipleRead (EDDI_IOCC_LL_HANDLE_TYPE             * pLLHandle,
                                   LSA_UINT32                     const   NrOfLinkLists);


/**************************************************************************//**
@brief Consistent write of a complete set of IO objects
@details This function is used to consistently write a whole set of IO objects from application memory to PAEARAM
@warning 
- If @ref eddi_IOCC_RemoveFromLinkList should still be running for a LinkList in another thread, the transfer for this complete LinkList will be skipped and EDD_STS_OK_NO_DATA returned!
- LLHandles all have to address the same IOCCHandle!

## Return of the request
Result (Response)    | Meaning
---------------------|--------------------------------
EDD_STS_OK           | Read successful.
EDD_STS_OK_PENDING   | Channel is still in use.
EDD_STS_OK_NO_DATA   | LinkList is in use by @ref eddi_IOCC_RemoveFromLinkList or @ref eddi_IOCC_AppendToLinkList.
EDD_STS_ERR_PARAM    | Parameter error.
EDD_STS_ERR_HW       | IOCC did not react as requested.

@param [in] pLLHandle     Pointer to array of LinkList handles (returned by @ref eddi_IOCC_CreateLinkList)
@param [in] NrOfLinkLists Nr. of LinkLists in LinkList array.
@return EDD_STS_OK/EDD_STS_OK_PENDING/EDD_STS_OK_NO_DATA on success @return EDD_STS_ERR_PARAM/EDD_STS_ERR_HW on failure
******************************************************************************/
LSA_RESULT eddi_IOCC_MultipleWrite (EDDI_IOCC_LL_HANDLE_TYPE             * pLLHandle,
                                    LSA_UINT32                     const   NrOfLinkLists);


/**************************************************************************//**
@brief Calculates a direct access address in PAEARAM.
@details This function calculates a direct access PAEARAM address using the formula in @todo how to add reference to spec.
@note Due to speed issues, no checks will be made!

@param [in] IOCCHandle    Handle returned by eddi_IOCC_Setup.
@param [in] pPAEARAM      The PAEARAM address (virtual adress) as it would be seen if PAEARAM was a linear memory.
@return The direct access address as a **virtual address**.
******************************************************************************/
LSA_VOID * eddi_IOCC_CalcDirectAccessAddress (EDDI_IOCC_LOWER_HANDLE_TYPE   const   IOCCHandle,
                                              LSA_VOID                    * const   pPAEARAM);

/** @} **/ //end of group eddi_iocc_api 

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif //EDDI_IOCC_H


/*****************************************************************************/
/*  end of file eddi_iocc.h                                                  */
/*****************************************************************************/

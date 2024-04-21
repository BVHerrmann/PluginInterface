#ifndef HIF_SYS_H                   /* ----- reinclude-protection ----- */
#define HIF_SYS_H

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
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_sys.h                                 :F&  */
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

#if (HIF_CFG_USE_HIF == 1)

// Number of ring memory pools is equal to the number of all HIF instances in this firmware
#define HIF_MEM_RING_POOL_MAX       (HIF_CFG_MAX_LD_INSTANCES + HIF_CFG_MAX_HD_INSTANCES) 

#if !defined(HIF_MEM_RING_POOL_MAX) || (HIF_MEM_RING_POOL_MAX == 0)
#error "No memory pools configured"
#endif

/*===========================================================================*/
/*                            HIF transfer modes                             */
/*===========================================================================*/
#define HIF_TM_SHARED_MEM           1
#define HIF_TM_SHORT                2
#define HIF_TM_MIXED                3
#define HIF_TM_SHARED_MEM_NO_SER    4

/*===========================================================================*/
/*                       Max number of pipes / device                        */
/*===========================================================================*/
#define HIF_MAX_PIPES_INST 128
    
/*===========================================================================*/
/*                       HIF_CFG_MEM_RING_CHECK_OVERWRITE                    */
/*===========================================================================*/
/** 
  * This switch can be activated to check HIF serialization/deserialization overwrite in Shared Memory.
  * Possible values are:
  *
  * HIF_MEM_RING_CHECK_START_STOP_1
  * Restrictions: currently only available when using HIF_MEM_RING
  * After every serialization the current SER writepointer will be checked against the RQB SHM start pointer + RQB serialization size.
  * Will generate a START sequenze (STAR (0x)) before the allocated SER buffer and a STOP sequenze (STOP (0x)) after the buffer. These 
  * 8 Bytes (2 * 4 Bytes) will be transparent for the "user". After every de-/serialization HIF will check if the START and STOP sequences
  * are okay and not overwritten.
  *
  * HIF_MEM_RING_CHECK_START_STOP_4
  * Restrictions: currently only available when using HIF_MEM_RING
  * Includes HIF_MEM_RING_CHECK_START_STOP_1 but with 4 START sequences at the beginning and 4 STOP sequences after the allocated shared memory
  **/
#define HIF_MEM_RING_CHECK_START_STOP_1          1
#define HIF_MEM_RING_CHECK_START_STOP_4          4

/*===========================================================================*/
/*                              HIF DMA flags                                */
/*===========================================================================*/
/**
 * FLAG has to be copied in HIF_SER_COPY_HTSHM_BUFFER after dmacopy/memcpy
 * of buffer is finished.
 */
#define HIF_DMA_FINISHED_FLAG        0xCAFEBABE
/**
 * FLAG to be written by DMA after buffer, can be freed.
 */
#define HIF_DMA_FREE_BUF_FLAG        0xDECAFBAD

/*===========================================================================*/
/*                             HIF Memory types                              */
/*===========================================================================*/
#define HIF_MEM_TYPE_RQB            1       // rqb memory
#define HIF_MEM_TYPE_BUFFER         2       // buffer mem in a rqb
#define HIF_MEM_TYPE_NRT_TX_BUFFER  3       // nrt tx buffer mem
#define HIF_MEM_TYPE_NRT_RX_BUFFER  4       // nrt rx buffer mem
#define HIF_MEM_TYPE_OTHER          5       // Non serialization/deserialization mem
    
/**
 * Only define HIF_FREE_LOCAL_MEMORY for
 *     memType==HIF_MEM_TYPE_BUFFER
 *     memType==HIF_MEM_TYPE_RQB
 *     memType==HIF_MEM_TYPE_OTHER
 *
 * other memType is invalid for HIF_FREE_LOCAL_MEMORY.
 * -> no define to HIF_FREE_LOCAL_MEM.
 * -> this will cause compliler warning: implicit declaration and linker error for other memType
 */
#define HIF_FREE_LOCAL_MEMORY(ret_val_ptr, ptr_mem, componentId, memType, ...) \
        HIF_FREE_LOCAL_MEMORY_ ## memType (ret_val_ptr, ptr_mem, componentId, memType, ##__VA_ARGS__)

/**
 * When HIF_FREE_LOCAL_MEMORY is called with the following memTypes, the memory can be freed immediate:
 *     memType==HIF_MEM_TYPE_RQB
 *     memType==HIF_MEM_TYPE_OTHER
 */
#define HIF_FREE_LOCAL_MEMORY_HIF_MEM_TYPE_RQB(ret_val_ptr, ptr_mem, componentId, memType, ...) \
        HIF_FREE_LOCAL_MEM(ret_val_ptr, ptr_mem, componentId, memType)
#define HIF_FREE_LOCAL_MEMORY_HIF_MEM_TYPE_OTHER(ret_val_ptr, ptr_mem, componentId, memType, ...) \
        HIF_FREE_LOCAL_MEM(ret_val_ptr, ptr_mem, componentId, memType)

/**
 * When a buffer is freed with HIF_FREE_LOCAL_MEMORY/HIF_FREE_NRT_MEMORY
 * it is given to hif internal buffer handler. 
 * If a buffer is freed with HIF_FREE_BUFFER_IMMEDIATE it is freed immediately.
 * HIF_FREE_BUFFER_IMMEDIATE can be called with the following memTypes:
 *     memType==HIF_MEM_TYPE_BUFFER
 *     memType==HIF_MEM_TYPE_NRT_TX_BUFFER
 *     memType==HIF_MEM_TYPE_NRT_RX_BUFFER
 *     
 * other memType is invalid for HIF_FREE_BUFFER_IMMEDIATE.
 */
#define HIF_FREE_BUFFER_IMMEDIATE(ret_val_ptr, ptr_mem, componentId, memType, hd_id) \
{ \
    if(memType == HIF_MEM_TYPE_BUFFER) \
    { \
        HIF_FREE_LOCAL_MEM(ret_val_ptr, ptr_mem, componentId, memType); \
    } \
    else if(memType == HIF_MEM_TYPE_NRT_TX_BUFFER) \
    { \
        HIF_NRT_FREE_TX_MEM(ret_val_ptr, ptr_mem, hd_id); \
    } \
    else if(memType == HIF_MEM_TYPE_NRT_RX_BUFFER) \
    { \
        HIF_NRT_FREE_RX_MEM(ret_val_ptr, ptr_mem, hd_id); \
    } \
    else \
    { \
        *ret_val_ptr = HIF_ERR_PARAM; \
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "HIF_FREE_BUFFER_IMMEDIATE() - memType not valid (0x%x)", memType); \
        HIF_FATAL(0); \
    } \
}

/*===========================================================================*/
/*                             HIF_CFG_USE_DMA                               */
/*===========================================================================*/
/**
 * This switch can be activated to use dma copy functions in hif.
 * Therefore HIF_SER_COPY_HTSHM_BUF_DMA and HIF_BUF_DMA_PEND_FREE have to be implemented using dma.
 *
 * Otherwise
 * - HIF_SER_COPY_HTSHM_BUFFER is mapped to HIF_SER_COPY_HTSHM_BUF
 * - HIF_FREE_LOCAL_MEMORY_HIF_MEM_TYPE_BUFFER is mapped to HIF_FREE_BUFFER_IMMEDIATE
 * - HIF_FREE_NRT_MEMORY is mapped to HIF_FREE_BUFFER_IMMEDIATE
 **/
#if (HIF_CFG_USE_DMA == 1)
/**
 * DMA is used, map buffer copy to dma copy macro.
 */
#define HIF_SER_COPY_HTSHM_BUFFER(dst, src, size, hSysDev) \
        HIF_SER_COPY_HTSHM_BUF_DMA(dst, src, size, hSysDev)
    
/**
 *  When HIF_FREE_LOCAL_MEMORY is called with memType == HIF_MEM_TYPE_BUFFER
 *  buffer can be still in use by dma. Pend freeing the buffer if buffer still in use.
 */
#define HIF_FREE_LOCAL_MEMORY_HIF_MEM_TYPE_BUFFER(ret_val_ptr, ptr_mem, componentId, memType, hSysDev) \
        HIF_FREE_BUFFER_DMA(ret_val_ptr, ptr_mem, componentId, memType, hSysDev, 0 /* not used for free */)

/**
 *  When HIF_FREE_NRT_MEMORY is called with memType == HIF_MEM_TYPE_NRT_TX_BUFFER/HIF_MEM_TYPE_NRT_RX_BUFFER
 *  buffer can be still in use by dma. Pend freeing the buffer if buffer still in use.
 */
#define HIF_FREE_NRT_MEMORY(ret_val_ptr, ptr_mem, hd_id, componentId, memType, hSysDev) \
        HIF_FREE_BUFFER_DMA(ret_val_ptr, ptr_mem, componentId, memType, hSysDev, hd_id)

/**
 * Free buffer after not longer in use by dma.
 * HIF_FREE_BUFFER_DMA is called in serialization code.
 * -> Buffer can be still in use by dma. Pend freeing the buffer if buffer still in use.
 */
#define HIF_FREE_BUFFER_DMA(ret_val_ptr, ptr_mem, componentId, memType, hSysDev, hd_id) \
{ \
    LSA_BOOL buffer_in_use = LSA_FALSE; \
    LSA_UINT32* pReadyForFree = LSA_NULL; \
    \
    /* 
     * When buffer is no longer in use by dma HIF_BUF_DMA_PEND_FREE() returns LSA_FALSE.
     * Otherwise it pends writing HIF_DMA_FREE_BUF_FLAG on pReadyForFree address when buffer can be freed.
     */ \
    buffer_in_use = HIF_BUF_DMA_PEND_FREE(ptr_mem, (LSA_VOID_PTR_TYPE *) &pReadyForFree, hSysDev); \
    /* if buffer is no longer in use by dma free it now. */ \
    if (!buffer_in_use) \
    { \
        HIF_FREE_BUFFER_IMMEDIATE(ret_val_ptr, ptr_mem, componentId, memType, hd_id); \
    } \
    else \
    { \
        /* 
         * Add buffer to hif buffer administration, since it can not be freed at the moment.
         * Buffer will be freed when the buffer free flag is written.
         */ \
        hif_buffer_admin_add(ptr_mem, componentId, memType, hd_id, HIF_DMA_FREE_BUF_FLAG, (LSA_UINT32 *)pReadyForFree); \
        *ret_val_ptr = HIF_OK; \
    }\
}
#else
/**
 * No DMA used, map macros.
 */
#define HIF_SER_COPY_HTSHM_BUFFER(dst, src, size, hSysDev) \
{ \
    LSA_UNUSED_ARG(hSysDev); \
    HIF_SER_COPY_HTSHM_BUF(dst, src, size) \
}

#define HIF_FREE_LOCAL_MEMORY_HIF_MEM_TYPE_BUFFER(ret_val_ptr, ptr_mem, componentId, memType, hSysDev) \
{ \
    LSA_UNUSED_ARG(hSysDev); \
    HIF_FREE_LOCAL_MEM(ret_val_ptr, ptr_mem, componentId, memType); \
}
#define HIF_FREE_NRT_MEMORY(ret_val_ptr, ptr_mem, hd_id, componentId, memType, hSysDev) \
{ \
    LSA_UNUSED_ARG(hSysDev); \
    HIF_FREE_BUFFER_IMMEDIATE(ret_val_ptr, ptr_mem, componentId, memType, hd_id); \
}
#endif // #if (HIF_CFG_USE_DMA == 1)
    
/*===========================================================================*/
/*                       constants for detail-pointer                        */
/*===========================================================================*/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#define HIF_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
    struct lsa_fatal_error_tag *

/*------------------------------------------------------------------------------
// Enum HIF_SEND_IRQ_MODE_TYPE (defines behavior before sending irq)
//----------------------------------------------------------------------------*/
typedef enum
{
    HIF_SEND_IRQ_MODE_INVALID    = 0,
    HIF_SEND_IRQ_AFTER_NO_WAIT   = 1, // default is no wait before sending irq!
    HIF_SEND_IRQ_AFTER_READ_BACK = 2, // read back before sending irq to make sure irq is deliverd after data!
    HIF_SEND_IRQ_AFTER_TIMEOUT   = 3  // for further performance optimation explicit timeout before sending irq can be used.
} HIF_SEND_IRQ_MODE_TYPE; 

/*------------------------------------------------------------------------------
// hif_get_par_tag
//----------------------------------------------------------------------------*/

typedef struct hif_get_par_tag
{
    HIF_HANDLE hH;                                    //Out: HIF Handle
    LSA_UINT16 Mode;                                  //In:  HIF Transfer Modes: Shared Mem Transfer / Pass Through / Mixed
    struct
    {
        struct
        {
			LSA_BOOL bUseTimerPoll;                   //In:  If true HIF starts an Timer for polling shared memory transfer activities (needed in case interrupts are not fully supported)
			HIF_SEND_IRQ_MODE_TYPE SendIrqMode;       //In:  mode for sending irq's (no_wait/read_back/timeout before send)
			LSA_UINT64 uNsWaitBeforeSendIrq;          //In:  ns timeout before sending irq (only used if send_irq_mode is set to timeout mode)     
			struct
			{
				LSA_UINT8* Base;                      //In:  Base Address of HIF Shared Mem Interface (Shared Mem Mode/Mixed Mode) (must be aligned correctly e.g. 32Bit)
				LSA_UINT32 Size;                      //In:  Size fo HIF Shared Mem in Bytes
			} Local;
			struct
			{
				LSA_UINT8* Base;                      //In:  Base Address of HIF Shared Mem Interface (Shared Mem Mode/Mixed Mode) (must be aligned correctly e.g. 32Bit)
				LSA_UINT32 Size;                      //In:  Size fo HIF Shared Mem in Bytes
			} Remote;
        } SharedMem;                                  //Shared Mem Transfer Mode / Mixed Mode
        HIF_HANDLE hif_lower_device;                  //In:  Lower HIF handle for Pass Through / Mixed Mode (Open Upper Device only)
        struct
        {
			LSA_UINT16 Mode;                          //In:  Shared Mem Mode / Pass Through Mode
        } Pipes[HIF_MAX_PIPES_INST];                  //Only used in Mixed Mode
    } if_pars;    
} HIF_GET_PAR_TYPE;

typedef struct hif_get_par_tag  *HIF_GET_PAR_PTR_TYPE;

/*------------------------------------------------------------------------------
// hif_detail_tag
//----------------------------------------------------------------------------*/

typedef struct hif_detail_tag
{
	HIF_HANDLE hH;
	HIF_HD_ID_TYPE hd_id;
	HIF_PIPE_ID_TYPE pipe_id;
} HIF_DETAIL_TYPE;

typedef struct hif_detail_tag   *HIF_DETAIL_PTR_TYPE;

/*--------------------------------------------------------------------------------------
// hif_component_support_std_tag / hif_compononet_support_sys_tag / serialization types
//--------------------------------------------------------------------------------------*/

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
typedef LSA_UINT16 (*HIF_SERIALIZE_CBF_TYPE)  (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
typedef LSA_UINT16 (*HIF_DESERIALIZE_CBF_TYPE)(HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

typedef struct hif_component_support_std_tag
{
    /**In: Callback which returns if target RQB is a close_channel opcode ofTarget component */
	LSA_BOOL        (*IsCloseChannelRqb)            (LSA_VOID* pRqb);
	/**In: Callback for replacing the lower handle with hHifChHandle in an open_channel RQB of target component */
	LSA_VOID        (*ReplaceOpenChannelLowerHandle)(HIF_RQB_PTR_TYPE pRQB, LSA_HANDLE_TYPE hHifChHandle);
	/**In: Callback for getting the lower handle of an open channel RQB */
	LSA_HANDLE_TYPE (*GetOpenChannelLowerHandle)    (HIF_RQB_PTR_TYPE pRQB);
	/**In: Callback for replacing an request_done callback an open channel RQB */
	LSA_UINT16      (*ReplaceOpenChannelCbf)        (HIF_RQB_PTR_TYPE pRQB, HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf);
	/**In: target Callback for getting the request_done() callback, upper handle and Path of an open channel RQB */
	LSA_UINT16      (*GetOpenChannelCompInfo)       (HIF_RQB_PTR_TYPE pRQB, HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,	LSA_SYS_PATH_TYPE *pPath, LSA_HANDLE_TYPE *hUpper);

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	/**In: Upper Serialization   */
	HIF_SERIALIZE_CBF_TYPE   SerializeUpper;
	/**In: Lower Deserialization */
	HIF_DESERIALIZE_CBF_TYPE DeserializeLower;
	/**In: Lower Serialization   */
	HIF_SERIALIZE_CBF_TYPE   SerializeLower;
	/**In: Upper Deserialization */
	HIF_DESERIALIZE_CBF_TYPE DeserializeUpper;
#endif
} HIF_COMPONENT_SUPPORT_STD_TYPE, *HIF_COMPONENT_SUPPORT_STD_PTR_TYPE;

typedef struct hif_compononet_support_sys_tag
{
    /**In: Get hif instance handle this request should be send to */
	LSA_VOID (*GetSysRequesthH)      (HIF_HANDLE* phH, HIF_RQB_PTR_TYPE pRQB);
	/**In: Callback for getting request_done callback function */
	LSA_VOID (*GetSysRequestCbf)     (HIF_SYS_HANDLE hSysDev, HIF_RQB_PTR_TYPE pRQB, HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE *ppCbf);
	/**In: Callback for replacing request_done callback function */
	LSA_VOID (*ReplaceSysRequestCbf) (HIF_SYS_HANDLE hSysDev, HIF_RQB_PTR_TYPE pRQB, HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE pCbf);

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	/**In: Upper Serialization   */
	HIF_SERIALIZE_CBF_TYPE   SerializeUpper;
	/**In: Lower Deserialization */
	HIF_DESERIALIZE_CBF_TYPE DeserializeLower;
	/**In: Lower Serialization   */
	HIF_SERIALIZE_CBF_TYPE   SerializeLower;
	/**In: Upper Deserialization */
	HIF_DESERIALIZE_CBF_TYPE DeserializeUpper;
#endif
} HIF_COMPONENT_SUPPORT_SYS_TYPE, *HIF_COMPONENT_SUPPORT_SYS_PTR_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define HIF_FATAL(error_code_0)	\
            hif_fatal_error (HIF_MODULE_ID, (LSA_UINT16)__LINE__,\
			(LSA_UINT32)(error_code_0), 0, 0, 0, 0, LSA_NULL)

// Cast a pointer to void*. 
// Usage: 
// { uint8_t * ptr; struct A* value = (struct A*) HIF_CAST_TO_VOID_PTR(ptr); }
// { uint8_t * ptr; uint16_t value = (struct A*) HIF_CAST_TO_VOID_PTR(ptr); }
// Note: The cast is done to avoid the lint warning 826: Suspicious pointer-to-pointer conversion (area too small). 
// A Clean solution would be to use void* to pass pointers. Due to API restrictions, this is sometimes not possible.
#define HIF_CAST_TO_VOID_PTR(Value) ((void*) (Value))

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/**
 * \brief initialize hif
 * 
 * This function must be called during startup.
 * 
 * @return LSA_RET_OK           - success 
 * @return LSA_RET_ERR_RESOURCE - unable to allocate ressources
 */
LSA_UINT16 hif_init(LSA_VOID);

/**
 * \brief undo the initialization of hif
 * 
 * @return LSA_RET_OK           - success
 * @return LSA_RET_ERR_SEQUENCE - undo called before init or during operation  
 */
LSA_UINT16 hif_undo_init(LSA_VOID);

/**
 * \brief register a component support structure
 * 
 * With this function, system adaption is able to register an LSA component at HIF which is not natively supported (for example PROFIBUS components). The registration needs to take place after hif_init().
 * @see HIF_COMPONENT_SUPPORT_STD_PTR_TYPE.
 * 
 * @param [in] CompId           - Component Id of the component registered
 * @param [in] pCompFuncs       - Structure containing the required callback functions to support the component in HIF
 * 
 * @return LSA_RET_OK           - success
 * @return LSA_RET_ERR_PARAM    - Input params wrong/incomplete
 * @return LSA_RET_ERR_SEQUENCE - Sequence error
 */
LSA_UINT16 hif_register_standard_component(
	LSA_COMP_ID_TYPE                   const CompId,
	HIF_COMPONENT_SUPPORT_STD_PTR_TYPE const pCompFuncs
);

/**
 * \brief register a component support structure
 * 
 * With this function, system adaption is able to register a sys component at HIF which is not natively supported (for example configuration components). 
 * The registration needs to take place after hif_init(). The difference to an LSA component is that sys requests are working without a channel. 
 * The HIF internal administration pipechannel is used to transfer the sys requests. 
 * @see HIF_COMPONENT_SUPPORT_SYS_PTR_TYPE.
 * 
 * @param [in] CompId       - Component Id of the component registered
 * @param [in] pCompFuncs   - Structure containing the required callback functions to support the component in HIF
 * 
 * @return LSA_RET_OK           - success
 * @return LSA_RET_ERR_PARAM    - Input params wrong/incomplete
 * @return LSA_RET_ERR_SEQUENCE - Sequence error
 * 
 */
LSA_UINT16 hif_register_sys_component(
	LSA_COMP_ID_TYPE                   const CompId,
	HIF_COMPONENT_SUPPORT_SYS_PTR_TYPE const pCompFuncs
);

/**
 * \brief expiration of the running time of a timer
 * 
 * This function is called on timer timeout.
 * 
 * @param [in] timer_id - Specified during timer start
 * @param [in] user_id  - Specified during timer start
 */
LSA_VOID hif_timeout(
    LSA_UINT16          timer_id,
    LSA_USER_ID_TYPE    user_id
);

/**
 * \brief hif poll service routine
 * 
 * The system adaptation may call this function to poll a HIF instance. This may be useful to reduce the delay between two LSA Timer ticks.
 * hif_poll sends internal RQBs to the HIF task. If there are still internal RQBs on the way, this call does nothing.
 */
LSA_VOID hif_poll(LSA_VOID);

/**
 * \brief hif interrupt service routine
 * This function needs to be called if a target HIF device interrupt occurred. HIF then checks if there is a new request on the shared memory.
 * @param [in] hH - The HIF handle  which was delivered by HIF_ENABLE_ISR().
 */
LSA_VOID hif_interrupt(
	HIF_HANDLE hH
);

/*====  out functions  =====*/

/*===========================================================================*/
/*                                 common                                    */
/*===========================================================================*/

#ifndef HIF_ALLOC_TIMER
/**
 * \brief Function allocates a new LSA timer.
 * 
 * @param [out] ret_val_ptr     - Pointer to address for response
 * @param [out] timer_id_ptr    - Pointer to the address for the Timer ID
 * @param [in] timer_type       - One shot timer / cyclic timer
 * @param [in] time_base        - LSA_TIME_BASE_1MS, LSA_TIME_BASE_10MS, LSA_TIME_BASE_100MS, LSA_TIME_BASE_1S, LSA_TIME_BASE_10S, LSA_TIME_BASE_100S
 * 
 * @return 
 */
LSA_VOID HIF_ALLOC_TIMER(
    LSA_UINT16  *ret_val_ptr,
    LSA_UINT16  *timer_id_ptr,
    LSA_UINT16  timer_type,
    LSA_UINT16  time_base
);
#endif

#ifndef HIF_START_TIMER
/**
 * \brief Starts the LSA timer, previously allocated with HIF_ALLOC_TIMER.
 * 
 * The system adaptation must call hif_timeout when the timer expires.
 * 
 * @param [out] ret_val_ptr - Pointer to address for response
 * @param [in] timer_id     - Timer ID that was obtained at HIF_ALLOC_TIMER
 * @param [in] user_id      - User identification of the timer
 * @param [in] time         - Runtime of the timer as multiple of the timer_base. >0
 */
LSA_VOID HIF_START_TIMER(
    LSA_UINT16          *ret_val_ptr,
    LSA_TIMER_ID_TYPE   timer_id,
    LSA_USER_ID_TYPE    user_id,
    LSA_UINT16          time
);
#endif

#ifndef HIF_STOP_TIMER
/**
 * \brief Stops the LSA timer, previously started with HIF_START_TIMER.
 * @param [out] ret_val_ptr - Pointer to address for response
 * @param [in] timer_id     - Timer ID that was obtained at HIF_ALLOC_TIMER
 */
LSA_VOID HIF_STOP_TIMER(
    LSA_UINT16          *ret_val_ptr,
    LSA_TIMER_ID_TYPE   timer_id
);
#endif

#ifndef HIF_FREE_TIMER
/**
 * \brief Frees the LSA timer, previously allocated with HIF_ALLOC_TIMER.
 * @param [out] ret_val_ptr - Pointer to address for response
 * @param [in] timer_id     - Timer ID that was obtained at HIF_ALLOC_TIMER
 */
LSA_VOID HIF_FREE_TIMER(
    LSA_UINT16          *ret_val_ptr,
    LSA_TIMER_ID_TYPE   timer_id
);
#endif

#ifndef HIF_GET_NS_TICKS
/**
 * \brief Returns ns ticks for performance measurements.
 * @return LSA_UINT64 - current ns ticks from performance counter.
 */
LSA_UINT64 HIF_GET_NS_TICKS(LSA_VOID);
#endif

#ifndef HIF_WAIT_NS
/**
 * \brief This function waits time in ns.
 *
 * The system adaptation shall wait synchronous unti the specified time is over (active wait).
 * 
 * @param [in] uTimeNs      - time to wait in ns
 */
LSA_VOID HIF_WAIT_NS(
    LSA_UINT64 uTimeNs
);
#endif

#ifndef HIF_ENTER
/**
 * \brief sets reentrance lock
 * 
 */
LSA_VOID HIF_ENTER(LSA_VOID);
#endif

#ifndef HIF_EXIT
/**
 * \brief reset reentrance lock
 */
LSA_VOID HIF_EXIT(LSA_VOID);
#endif

#ifndef HIF_ALLOC_REENTRANCE_LOCK
/**
 * \brief alloc reentrance lock
 * @param [out] ret_val_ptr         - pointer for the return value 
 * @param [out] lock_handle_ptr     - pointer to the lock handle
 */
LSA_VOID HIF_ALLOC_REENTRANCE_LOCK(
    LSA_UINT16  *ret_val_ptr,
    LSA_UINT16  *lock_handle_ptr
);
#endif

#ifndef HIF_FREE_REENTRANCE_LOCK
/**
 * \brief free reentrance lock allocated by HIF_ALLOC_REENTRANCE_LOCK
 */
LSA_VOID HIF_FREE_REENTRANCE_LOCK(
    LSA_UINT16  *ret_val_ptr,
    LSA_UINT16  lock_handle 
);
#endif

#ifndef HIF_ENTER_REENTRANCE_LOCK
/**
 * \brief sets reentrance lock for the given lock_handle
 * @param [in] lock_handle - handle of the lock
 */
LSA_VOID HIF_ENTER_REENTRANCE_LOCK(
    LSA_UINT16 lock_handle
);
#endif

#ifndef HIF_EXIT_REENTRANCE_LOCK
/**
 * \brief reset reentrance lock
 * @param [in] lock_handle - handle of the lock
 * 
 */
LSA_VOID HIF_EXIT_REENTRANCE_LOCK(
    LSA_UINT16 lock_handle
);
#endif

#ifndef HIF_ALLOC_LOCAL_MEM
/**
 * \brief Allocate local memory.
 * 
 * The function allocates length bytes in the local memory, and
 * returns a pointer to this area.  The memory area has to
 * be correctly aligned for all data types (that is, 32 bit
 * aligned as a rule). If a NULL pointer is returned, it was
 * not possible to allocate memory. This macro is used 
 * internally only for allocating RQBs in the local memory
 * of HIF_UPPER.
 * 
 * @param [out] ptr_ptr_mem - Pointer to address for pointer to memory space. If NULL is returned here, then no  memory could be allocated.
 * @param [in] length       - Length of the memory area to be allocated (in bytes)
 * @param [in] componentId  - component the buffer is allocated for
 * @param [in] memType      - type of buffer (rqb, buffer)
 */
LSA_VOID HIF_ALLOC_LOCAL_MEM(
    LSA_VOID_PTR_TYPE   *ptr_ptr_mem,
    LSA_UINT32          length,
    LSA_UINT16          componentId,
    LSA_UINT16          memType
);
#endif

#ifndef HIF_FREE_LOCAL_MEM
/**
 * \brief Free local memory
 * 
 * The function frees the memory that was previously allocated 
 * with HIF_ALLOC_LOCAL_MEM. mem_ptr is the pointer that
 * was returned by HIF_ALLOC_LOCAL_MEM.
 * 
 * @param [out] ret_val_ptr - pointer to address for response
 * @param [in] ptr_mem      - pointer to memory area that is to be freed
 * @param [in] componentId  - component the buffer was allocated for
 * @param [in] memType      - type of buffer (rqb, buffer)
 */
LSA_VOID HIF_FREE_LOCAL_MEM(
    LSA_UINT16          *ret_val_ptr,
    LSA_VOID_PTR_TYPE   ptr_mem,
    LSA_UINT16          componentId,
    LSA_UINT16          memType
);
#endif

#ifndef HIF_MEMSET
/**
 * \brief Fills the memory with value.
 * @param [in] ptr_mem   - Pointer to the memory that is to be filled with value
 * @param [in] value     - Value with which the memory is to be filled
 * @param [in] length    - Memory length in bytes  
 */
LSA_VOID HIF_MEMSET(
    LSA_VOID_PTR_TYPE   ptr_mem,
    LSA_INT             value,
    LSA_UINT            length
);
#endif

#ifndef HIF_MEMCPY
/**
 * \brief Copies Memory
 * @param [in] dst   - destination pointer
 * @param [in] src   - source pointer
 * @param [in] len   - length  
 */
LSA_VOID HIF_MEMCPY(
	LSA_VOID_PTR_TYPE   dst,
	LSA_VOID_PTR_TYPE   src,
	LSA_UINT            len
);
#endif

#ifndef HIF_MEMCMP
/**
 * \brief Compares Memory
 * 
 * @param [in] pBuf1   - destination pointer
 * @param [in] pBuf2   - source pointer
 * @param [in] Length  - length 
 * 
 * @return < 0 the first byte that does not match in both memory blocks has a lower value in ptr1 than in ptr2 
 * @return 0 the contents of both memory blocks are equal
 * @return > 0 the first byte that does not match in both memory blocks has a greater value in ptr1 than in ptr2 
 */
LSA_UINT HIF_MEMCMP(
	LSA_VOID_PTR_TYPE   pBuf1,
	LSA_VOID_PTR_TYPE   pBuf2,
	LSA_UINT            Length
);
#endif

#ifndef HIF_LTOSHM_S
/**
 * \brief Conversion macro.
 * 
 * Converts the byte order of the local address space into byte
 * order of the shared memory address space. Used for the short
 * unsigned integer variables.
 * 
 * @param [in] local_short - Variable to be converted
 * @return The converted variable.
 * 
 */
LSA_UINT16 HIF_LTOSHM_S(
    LSA_UINT16 local_short
);
#endif

#ifndef HIF_LTOSHM_L
/**
 * \brief Conversion macro.
 * 
 * Converts the byte order of the local address space into byte
 * order of the shared memory address space. Used for the long
 * unsigned integer variables.
 * 
 * @param [in] local_long - Variable to be converted
 * @return The converted variable.
 * 
 */
LSA_UINT32 HIF_LTOSHM_L(
    LSA_UINT32 local_long
);
#endif

#ifndef HIF_LTOSHM_LL
/**
 * \brief Conversion macro.
 * 
 * Converts the byte order of the local address space into byte
 * order of the shared memory address space. Used for the double long
 * unsigned integer variables (64Bit).
 * 
 * @param [in] local_dlong - Variable to be converted
 * @return The converted variable.
 */
LSA_UINT64 HIF_LTOSHM_LL(
    LSA_UINT64 local_dlong
);
#endif

#ifndef HIF_SHMTOL_S
/**
 * \brief Conversion macro.
 * 
 * Converts the byte order of the shared memory address space into
 * byte order of the local address space. Used for the short
 * unsigned integer variables.
 * 
 * @param [in] shm_short - Variable to be converted
 * @return The converted variable.
 */
LSA_UINT16 HIF_SHMTOL_S(
    LSA_UINT16 shm_short
);
#endif

#ifndef HIF_SHMTOL_L
/**
 * \brief Conversion macro.
 * 
 * Converts the byte order of the shared memory address space into
 * byte order of the local address space. Used for the long
 * unsigned integer variables.
 * 
 * @param [in] shm_long - Variable to be converted
 * @return The converted variable.
 */
LSA_UINT32 HIF_SHMTOL_L(
    LSA_UINT32 shm_long
);
#endif

#ifndef HIF_SHMTOL_LL
/**
 * \brief Conversion macro.
 * 
 * Converts the byte order of the shared memory address space into
 * byte order of the local address space. Used for the long
 * unsigned integer variables.
 * 
 * @param [in] shm_dlong - Variable to be converted
 * @return The converted variable.
 */
LSA_UINT64 HIF_SHMTOL_LL(
    LSA_UINT64 shm_dlong
);
#endif

#ifndef HIF_NTOH_S
/**
 * \brief Conversion macro.
 * 
 * Converts the byte order of the network into
 * byte order of the local address space.
 * 
 * @param [in] value - Variable to be converted
 * @return The converted variable.
 */
LSA_UINT16 HIF_NTOH_S(
    LSA_UINT16 value
);
#endif

#ifndef HIF_FATAL_ERROR
/**
 * \brief Fatal error handler
 * 
 * Function writes information to the administration buffer 
 * (exception handling).
 * 
 * HIF expects that this function never returns. 
 * 
 * @param [in] length       - LSA_COMP_ID
 * @param [in] error_ptr    - pointer to error description
 * @return
 */
LSA_VOID HIF_FATAL_ERROR(
    LSA_UINT16                  length,
    HIF_FATAL_ERROR_PTR_TYPE    error_ptr
);
#endif

#ifndef HIF_STRLEN
/**
 * \brief HIF macro for strlen
 * 
 * returns the length of a zero-terminated string
 * 
 * @param [in] str       - ptr to string
 * @return length of the string
 */
LSA_UINT HIF_STRLEN(
    LSA_VOID_PTR_TYPE str
);
#endif

#ifndef HIF_SER_SYS_PATH_HTSHM
/**
 * \brief Serialization function for the sys_path
 * 
 * The goal of this function is to deserialize the “sys_path” of a RQB from shared-memory. 
 * In case the LSA_SYS_PATH_TYPE is a structure, the according copy statements have to be 
 * adapted in the System-adaption to deserialize all the necessary variables. 
 * 
 * @param [in] dest           - pointer to the transfer memory 
 * @param [in] sys_path       - copy of the sys_path struture
 * @return
 */
LSA_VOID HIF_SER_SYS_PATH_HTSHM(
    LSA_UINT32          **dest,
    LSA_SYS_PATH_TYPE   sys_path
);
#endif

/****************************************************************************
*  Name:        HIF_SER_SYS_PATH_SHMTH()
*  Purpose:     - deserializes the sys_path
*  Input:       sys_path, src in shared_mem
*  Return:      -
****************************************************************************/
#ifndef HIF_SER_SYS_PATH_SHMTH
/**
 * \brief Deserialization function for the sys_path
 * 
 * The goal of this function is to deserialize the “sys_path” of a RQB from shared-memory. 
 * In case the LSA_SYS_PATH_TYPE is a structure, the according copy statements have to be adapted in 
 * the System-adaption to deserialize all the necessary variables.  
 * 
 * @param [in] src           - pointer to the serialized sys_path in the transfer memory
 * @param [in] sys_path      - pointer to a sys_path structure
 * @return
 */
LSA_VOID HIF_SER_SYS_PATH_SHMTH(
    LSA_SYS_PATH_TYPE   *sys_path,
    LSA_UINT32          **src
);
#endif

#ifndef HIF_NRT_ALLOC_TX_MEM
/**
 * \brief Allocate send memory
 * 
 * Used during de-/serializing to alloc EDD NRT SEND/RECV Frames.
 * Note: HIF must use the same allocation pool that the local EDD uses to allocate RQBs.
 * 
 * @param [out] mem_ptr_ptr - pointer to new allocated buffer
 * @param [in] length       - length of allocated buffer
 * @param [in] hd_id        - hd_id of the pipe requesting memory
 * @return
 */
LSA_VOID HIF_NRT_ALLOC_TX_MEM( 
	LSA_VOID_PTR_TYPE   *mem_ptr_ptr,
	LSA_UINT32          length,
	HIF_HD_ID_TYPE      hd_id
);
#endif

#ifndef HIF_NRT_ALLOC_RX_MEM
/**
 * \brief Allocate receive memory
 * 
 * Used during de-/serializing to alloc EDD NRT SEND/RECV Frames.
 * Note: HIF must use the same allocation pool that the local EDD uses to allocate RQBs.
 * 
 * @param [out] mem_ptr_ptr - pointer to new allocated buffer
 * @param [in] length       - length of allocated buffer
 * @param [in] hd_id        - hd_id of the pipe requesting memory
 * @return
 */
LSA_VOID HIF_NRT_ALLOC_RX_MEM( 
	LSA_VOID_PTR_TYPE   *mem_ptr_ptr,
	LSA_UINT32          length,
	HIF_HD_ID_TYPE      hd_id
);
#endif

#ifndef HIF_NRT_FREE_TX_MEM
/**
 * \brief Free send memory
 * 
 * Used during de-/serializing to alloc EDD NRT SEND/RECV Frames.
 * Note: HIF must use the same allocation pool that the local EDD uses to allocate RQBs.
 * 
 * @param [out] ret_val_ptr - pointer to free result
 * @param [in] mem_ptr      - pointer of nrt mem to be freed
 * @param [in] hd_id        - hd_id of the pipe requesting memory
 * @return
 */
LSA_VOID HIF_NRT_FREE_TX_MEM(
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	HIF_HD_ID_TYPE      hd_id
);
#endif

#ifndef HIF_NRT_FREE_RX_MEM
/**
 * \brief Free receive memory
 * 
 * Used during de-/serializing to alloc EDD NRT SEND/RECV Frames.
 * Note: HIF must use the same allocation pool that the local EDD uses to allocate RQBs.
 * 
 * @param [out] ret_val_ptr - pointer to free result
 * @param [in] mem_ptr      - pointer of nrt mem to be freed
 * @param [in] hd_id        - hd_id of the pipe requesting memory
 * @return
 */
LSA_VOID HIF_NRT_FREE_RX_MEM(
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	HIF_HD_ID_TYPE      hd_id
);
#endif

/*===========================================================================*/
/*                 Check Shared Memory for corruption                        */
/*===========================================================================*/
#ifndef HIF_SHM_CHECK_OVERWRITE_SERIALIZE
/**
 * \brief The system adaptation may check for overwrites in the transfer memory during serialization.
 * 
 * Since the allocated buffers may have additional headers and trailers or a specific alignment, the system adaptation shall check for overwrites.
 * The product must decide how to handle serialization errors. For debug variants, we suggest to trace the function name, OPC of the RQB and cause a FATAL_ERROR.
 * 
 * @param [in] opcode               - OPC of the RQB
 * @param [out] ptr_ptr_rqb_dest    - pointer to RQB
 * @param [in] rqb_size             - size of the RQB
 * @param [in] func                 - function name where this macro is called (used to trace errors)
 * 
 */
LSA_VOID HIF_SHM_CHECK_OVERWRITE_SERIALIZE(
    LSA_UINT32      opcode,
    LSA_VOID        *ptr_ptr_rqb_dest,
    LSA_UINT32      rqb_size,
    LSA_UINT8       *help_ptr,
    const LSA_CHAR  *func
);
#endif

#ifndef HIF_SHM_CHECK_OVERWRITE_DESERIALIZE
/**
 * \brief The system adaptation may check for overwrites in the transfer memory during deserialization.
 * 
 * Since the allocated buffers may have additional headers and trailers or a specific alignment, the system adaptation shall check for overwrites.
 * The product must decide how to handle serialization errors. For debug variants, we suggest to trace the function name, OPC of the RQB and cause a FATAL_ERROR.
 * 
 * @param [in] ptr_rqb_src          - pointer to RQB
 * @param [in] opcode               - The OPC of the RQB
 * @param [in] help_ptr             - The current offset in the shared memory. The deserialization increment this pointer during their function.
 * @param [in] func                 - function name where this macro is called (used to trace errors)
 * 
 */
LSA_VOID HIF_SHM_CHECK_OVERWRITE_DESERIALIZE(
    LSA_VOID        *ptr_rqb_src,
    LSA_UINT32      opcode,
    LSA_UINT8       *help_ptr,
    const LSA_CHAR  *func
);
#endif

/*===========================================================================*/
/*                           Common RQB                                      */
/*===========================================================================*/
#if ( HIF_CFG_USE_REQUEST_LOWER_DONE_OUTPUT_MACRO == 1 )
#ifndef HIF_REQUEST_LOWER_DONE
/**
 * \brief Hook for system adaptation: Lower request done
 * 
 * This function is only used if HIF_CFG_USE_REQUEST_LOWER_DONE_OUTPUT_MACRO is defined 1.
 * This function gives the system adaption the possibility to change the context before calling hif_request_lower_done_ptr callback function.
 * This is especially needed in case HIF_ENTER()/HIF_EXIT() is not implemented as the callback function needs to be executed in HIF context.
 * 
 * @param [in] hif_request_lower_done_ptr   - pointer to the HIF callback function
 * @param [in] ptr_lower_rqb                - pointer to RQB
 * @param [in] hSysDev                      - Sys handle delivered on device open
 * 
 */
LSA_VOID HIF_REQUEST_LOWER_DONE(
    HIF_UPPER_CALLBACK_FCT_PTR_TYPE hif_request_lower_done_ptr,
    HIF_RQB_PTR_TYPE                ptr_lower_rqb,
    LSA_SYS_PTR_TYPE                hSysDev
);
#endif
#endif

#if ( HIF_CFG_USE_SYS_REQUEST_LOWER_DONE_OUTPUT_MACRO == 1 )
/**
 * \brief Hook for system adaptation: Lower system request done
 * 
 * This function is only used if HIF_CFG_USE_SYS_REQUEST_LOWER_DONE_OUTPUT_MACRO is defined 1.
 * This function gives the system adaption the possibility to change the context before calling hif_request_lower_done_ptr callback function.
 * This is especially needed in case HIF_ENTER()/HIF_EXIT() is not implemented as the callback function needs to be executed in HIF context.
 *
 * @param [in] hif_request_lower_done_ptr   - pointer to the HIF callback function
 * @param [in] ptr_lower_rqb                - pointer to RQB
 * @param [in] hSysDev                      - Sys handle delivered on device open
 * 
 */
#ifndef HIF_SYS_REQUEST_LOWER_DONE
LSA_VOID HIF_SYS_REQUEST_LOWER_DONE(
    HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE    hif_request_lower_done_ptr,
    HIF_RQB_PTR_TYPE                    ptr_lower_rqb,
    LSA_SYS_PTR_TYPE                    hSysDev
);
#endif
#endif

/*===========================================================================*/
/*                           Logical Device (LD)                             */
/*===========================================================================*/

#ifndef HIF_LD_ENTER
/**
 * \brief Function sets reentrance lock
 */
LSA_VOID HIF_LD_ENTER(LSA_VOID);
#endif

#ifndef HIF_LD_EXIT
/**
 * \brief Function cancels reentrance lock
 */
LSA_VOID HIF_LD_EXIT(LSA_VOID);
#endif

#ifndef HIF_LD_DO_POLL_REQUEST
/**
 * \brief Triggers a context switch from the low prio task to HIF task. 
 * 
 * This is a additional poll mechanism to enhance HIF reaction time when the system is in idle.
 * 
 * This function is called from within the function hif_poll().
 * System Adaptation must then call the hif_internal_request() 
 * function with the specified RQB. System Adaptation must execute 
 * a transition in the context of HIF, if necessary. However, in a 
 * typical implementation System Adaptation sends a mail containing 
 * the RQB to the HIF task.
 * 
 * <Low Prio task>:: hif_internal_request --> <Normal Prio Task>:: hif_request
 * 
 * @param [in] ptr_upper_rqb Pointer to the RQB that is to be transferred to hif_request()
 */
LSA_VOID HIF_LD_DO_POLL_REQUEST(
    HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

#ifndef HIF_LD_DO_TIMEOUT_REQUEST
/**
 * \brief Triggers a context switch from timer task to the HIF task.
 * 
 * This function is called from within the function hif_timeout().
 * System Adaptation must then call the hif_internal_request() 
 * function with the specified RQB. System Adaptation must execute 
 * a transition in the context of HIF, if necessary. However, in a 
 * typical implementation System Adaptation sends a mail containing 
 * the RQB to the HIF task.
 * 
 * <Timer task>:: hif_timeout --> <Normal Prio Task>:: hif_request
 * 
 * @param [in] ptr_upper_rqb Pointer to the RQB that is to be transferred to hif_request()
 */
LSA_VOID HIF_LD_DO_TIMEOUT_REQUEST(
    HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

#ifndef HIF_LD_DO_REQUEST_LOCAL
/**
 * \brief Enqueues a internal RQB into the mailbox of the HIF task. No context switch necessary.
 * 
 * This function is only used in short mode. This function is called from within the function hif_ld_request().
 * 
 * System Adaptation must then call the hif_ld_request() 
 * function with the specified RQB. System Adaptation must execute 
 * a transition in the context of HIF, if necessary. However, in a 
 * typical implementation System Adaptation sends a mail containing 
 * the RQB to the HIF task.
 * 
 * @param [in] ptr_upper_rqb - Pointer to the RQB that is to be transferred to hif_request()
 */
LSA_VOID HIF_LD_DO_REQUEST_LOCAL(
    HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

#ifndef HIF_LD_U_GET_PATH_INFO
/**
 * \brief Get path info from LD Upper.
 * 
 * Function obtains channel path information. Function is called
 * in the Open Channel service in order to get the information from
 * system adaptation regarding the channel. If system adaptation 
 * recognizes an invalid path, it returns the error code 
 * LSA_RET_ERR_SYS_PATH
 * 
 * @param [out] ret_val_ptr      - Pointer to address for the response
 * @param [out] sys_ptr_ptr      - Pointer to address for LSA Sys Ptr
 * @param [out] detail_ptr_ptr   - Pointer to address for Detail Ptr
 * @param [in] path              - LSA path info as specified for Open Channel
 * 
 */
LSA_VOID HIF_LD_U_GET_PATH_INFO(
    LSA_UINT16          *ret_val_ptr,
    LSA_SYS_PTR_TYPE	*sys_ptr_ptr,
    HIF_DETAIL_PTR_TYPE	*detail_ptr_ptr,
    LSA_SYS_PATH_TYPE   path
);
#endif

#ifndef HIF_LD_L_GET_PATH_INFO
/**
 * \brief Get path info from LD Lower.
 * 
 * Function obtains channel path information. Function is called
 * in the Open Channel service in order to get the information from
 * system adaptation regarding the channel. If system adaptation 
 * recognizes an invalid path, it returns the error code 
 * LSA_RET_ERR_SYS_PATH.
 * 
 * @param [out] ret_val_ptr      - Pointer to address for the response
 * @param [out] sys_ptr_ptr      - Pointer to address for LSA Sys Ptr
 * @param [out] detail_ptr_ptr   - Pointer to address for Detail Ptr
 * @param [in] path              - LSA path info as specified for Open Channel
 * 
 */
LSA_VOID HIF_LD_L_GET_PATH_INFO(
    LSA_UINT16          *ret_val_ptr,
    LSA_SYS_PTR_TYPE	*sys_ptr_ptr,
    HIF_DETAIL_PTR_TYPE	*detail_ptr_ptr,
    LSA_SYS_PATH_TYPE   path
);
#endif

#ifndef HIF_LD_U_RELEASE_PATH_INFO
/**
 * \brief release path info from LD Upper.
 * 
 * Function releases channel path information. This function is 
 * called when a channel is closed (Close Channel). In the process,
 * the two pointers sys_ptr and detail_ptr are returned to system
 * adaptation. If the pointers are invalid, the output macro returns 
 * LSA_RET_ERR_PARAM.
 * 
 * @param [out] ptr_dst     - Pointer to address for the response
 * @param [in] sys_ptr      - Sys Ptr that was transferred to the HIF when HIF_LD_U_GET_PATH_INFO was called
 * @param [in] detail_ptr   - Detail Ptr that was transferred to the HIF when HIF_LD_U_GET_PATH_INFO was called.
 * 
 */
LSA_VOID HIF_LD_U_RELEASE_PATH_INFO(
    LSA_UINT16	        *ptr_dst,
    LSA_SYS_PTR_TYPE    sys_ptr,
    HIF_DETAIL_PTR_TYPE detail_ptr
);
#endif

#ifndef HIF_LD_L_RELEASE_PATH_INFO
/**
 * \brief release path info from LD Lower.
 * 
 * Function releases channel path information. This function is 
 * called when a channel is closed (Close Channel). In the process,
 * the two pointers sys_ptr and detail_ptr are returned to system
 * adaptation. If the pointers are invalid, the output macro returns 
 * LSA_RET_ERR_PARAM.
 * 
 * @param [out] ptr_dst     - Pointer to address for the response
 * @param [in] sys_ptr      - Sys Ptr that was transferred to the HIF when HIF_LD_L_GET_PATH_INFO was called
 * @param [in] detail_ptr   - Detail Ptr that was transferred to the HIF when HIF_LD_L_GET_PATH_INFO was called.
 * 
 */
LSA_VOID HIF_LD_L_RELEASE_PATH_INFO(
    LSA_UINT16	        *ptr_dst,
    LSA_SYS_PTR_TYPE    sys_ptr,
    HIF_DETAIL_PTR_TYPE detail_ptr
);
#endif

#ifndef HIF_LD_U_GET_PARAMS
/**
 * \brief Gets Parameters from system adaptation.
 * 
 * Function is called during the opening sequence of HIF. 
 * Function retrieves parameters, required for the setup of the 
 * upper layer of Logical Device.
 * 
 * @param [out] ret_val_ptr - Pointer to address for response
 * @param [out] pPars       - Pointer to the data structure of type HIF_GET_PAR_TYPE
 * @param [in] hSysDev      - Device handle
 */
LSA_VOID HIF_LD_U_GET_PARAMS(
    LSA_UINT16              *ret_val_ptr,
    HIF_SYS_HANDLE          hSysDev,
    HIF_GET_PAR_PTR_TYPE    pPars
);
#endif

#ifndef HIF_LD_L_GET_PARAMS
/**
 * \brief Gets Parameters from system adaptation.
 * 
 * Function is called during the opening sequence of HIF. 
 * Function retrieves parameters, required for the setup of the 
 * lower layer of Logical Device.
 * 
 * @param [out] ret_val_ptr - Pointer to address for response
 * @param [out] pPars       - Pointer to the data structure of type HIF_GET_PAR_TYPE
 * @param [in] hSysDev      - Device handle
 */
LSA_VOID HIF_LD_L_GET_PARAMS(
    LSA_UINT16              *ret_val_ptr,
    HIF_SYS_HANDLE          hSysDev,
    HIF_GET_PAR_PTR_TYPE    pPars
);
#endif

#ifndef HIF_LD_DO_INTERRUPT_REQUEST
/**
 * \brief Triggers a context switch from the interrupt context to the HIF task.
 * 
 * This function is called from within the function hif_interrupt().
 * System Adaptation must then call the hifl_request() function with
 * the specified RQB. System Adaptation must execute a transition
 * in the context of HIF, if necessary. However, in a typical
 * implementation System Adaptation sends a mail containing the RQB
 * to the HIF task.
 * 
 * <ISR context>:: hif_interrupt --> <Normal Prio Task>:: hif_request
 * 
 * @param [in] ptr_upper_rqb - Pointer to the RQB that is to be transferred to hif_ request()
 */
LSA_VOID HIF_LD_DO_INTERRUPT_REQUEST(
    HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

#ifndef HIF_LD_ENABLE_ISR
/**
 * \brief Enbale interrups für HIF LD.
 * 
 * Function enables interrupts to the other layer of HIF LD.
 * 
 * @param [in] hSysDev  - Device handle
 * @param [in] hH       - Reference to the HIF instance
 */
LSA_VOID HIF_LD_ENABLE_ISR(
    HIF_SYS_HANDLE  hSysDev,
    HIF_HANDLE      hH
);
#endif

#ifndef HIF_LD_DISABLE_ISR
/**
 * \brief Disable interrups für HIF LD.
 * 
 * Function disables interrupts to the other layer of HIF LD.
 * 
 * @param [in] hSysDev  - Device handle
 * @param [in] hH       - Reference to the HIF instance
 */
LSA_VOID HIF_LD_DISABLE_ISR(
    HIF_SYS_HANDLE  hSysDev,
    HIF_HANDLE      hH
);
#endif

#ifndef HIF_LD_SEND_IRQ
/**
 * \brief Trigger an interrupt for HIF LD.
 * 
 * This macro shall trigger the interrupt to the receiver side.
 * Macro is called by hif_request() and hif_return_rqb() after the 
 * RQB is serialized into the shared memory. 
 * 
 * @param [in] hSysDev  - Device handle
 * @param [in] hH       - Reference to the HIF instance
 */
LSA_VOID HIF_LD_SEND_IRQ(
    HIF_SYS_HANDLE  hSysDev,
    HIF_HANDLE      hH
);
#endif

#ifndef HIF_LD_ACK_IRQ
/**
 * \brief Acknowledge an interrupt for HIF LD.
 * 
 * This macro shall acknowledge the interrupt on initator side.
 * Macro is called by hif_request() in front of access to the shared memory.
 * 
 * @param [in] hSysDev  - Device handle
 * @param [in] hH       - Reference to the HIF instance
 */
LSA_VOID HIF_LD_ACK_IRQ(
    HIF_SYS_HANDLE  hSysDev,
    HIF_HANDLE      hH
);
#endif

#ifndef HIF_LD_OPEN_DEVICE_LOWER 
/**
 * Transfers the Open Device RQB to the lower LSA layer.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB) for open device request
 */
LSA_VOID HIF_LD_OPEN_DEVICE_LOWER(
    HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

#ifndef HIF_LD_CLOSE_DEVICE_LOWER 
/**
 * Transfers the Close Device RQB to the lower LSA layer.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB) for close device request
 */
LSA_VOID HIF_LD_CLOSE_DEVICE_LOWER(
    HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

#ifndef HIF_LD_SYSTEM_REQUEST_LOWER 
/**
 * Transfers the System RQB to the lower LSA layer.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB)
 */
LSA_VOID HIF_LD_SYSTEM_REQUEST_LOWER(
    HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

#ifndef HIF_LD_OPEN_CHANNEL_LOWER 
/**
 * Opens a channel to the lower LSA layer of the Logical Device.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB) for open channel request
 * @param [in] hSysDev       - Sys Device Ptr
 */
LSA_VOID HIF_LD_OPEN_CHANNEL_LOWER(
    HIF_RQB_PTR_TYPE    lower_rqb_ptr,
    HIF_SYS_HANDLE      hSysDev
);
#endif

#ifndef HIF_LD_CLOSE_CHANNEL_LOWER 
/**
 * Closes a channel to the lower LSA layer of the Logical Device.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB) for close channel request
 * @param [in] hSysDev       - Sys Device Ptr
 */
LSA_VOID HIF_LD_CLOSE_CHANNEL_LOWER(
    HIF_RQB_PTR_TYPE    lower_rqb_ptr,
    HIF_SYS_HANDLE      hSysDev
);
#endif

#ifndef HIF_LD_REQUEST_LOWER 
/**
 * This function transfers asynchronous requests to the lower
 * LSA layer of the Logical Device.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB) for LSA request
 * @param [in] hSysDev       - Sys Device Ptr
 */
LSA_VOID HIF_LD_REQUEST_LOWER(
    HIF_RQB_PTR_TYPE    lower_rqb_ptr,
    HIF_SYS_HANDLE      hSysDev
);
#endif

#ifndef HIF_LD_REQUEST_UPPER_DONE 
/**
 * This macro is called by HIF when a user channel request (RQB)
 * from calling hif_open_channel(), hif_request() or 
 * hif_close_channel() is finished.
 * The system adaptation has to call the upper callback function
 * to finish the request.
 * 
 * @param [in] hif_request_upper_done_ptr   - Function pointer to the request done function
 * @param [in] ptr_upper_rqb                - Pointer to the LSA RQB
 * @param [in] sys_ptr                      - Pointer to the sys path
 */
LSA_VOID HIF_LD_REQUEST_UPPER_DONE(
    HIF_UPPER_CALLBACK_FCT_PTR_TYPE hif_request_upper_done_ptr,
    HIF_RQB_PTR_TYPE                ptr_upper_rqb,
    LSA_SYS_PTR_TYPE                sys_ptr
);
#endif

#ifndef HIF_LD_L_SYSTEM_REQUEST_DONE 
/**
 * This function is called by HIF LD Lower when a request (RQB)
 * from calling hif_system() is finished.
 * The system adaptation has to call the callback function to
 * finish the request.
 * 
 * @param [in] hif_request_upper_done_ptr   - Function pointer to the request done function
 * @param [in] ptr_upper_rqb                - Pointer to the LSA RQB
 */
LSA_VOID HIF_LD_L_SYSTEM_REQUEST_DONE(
    HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE    hif_system_request_done_ptr,
    HIF_RQB_PTR_TYPE                    ptr_upper_rqb
);
#endif

#ifndef HIF_LD_U_SYSTEM_REQUEST_DONE 
/**
 * This function is called by HIF LD Upper when a request (RQB)
 * from calling hif_system() is finished.
 * The system adaptation has to call the callback function to
 * finish the request.
 * 
 * @param [in] hif_request_upper_done_ptr   - Function pointer to the request done function
 * @param [in] ptr_upper_rqb                - Pointer to the LSA RQB
 */
LSA_VOID HIF_LD_U_SYSTEM_REQUEST_DONE(
    HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE    hif_system_request_done_ptr,
    HIF_RQB_PTR_TYPE                    ptr_upper_rqb
);
#endif

/*===========================================================================*/
/*                           Hardware Device (HD)                            */
/*===========================================================================*/

#ifndef HIF_HD_ENTER
/**
 * Function sets reentrance lock
 */
LSA_VOID HIF_HD_ENTER(LSA_VOID);
#endif

#ifndef HIF_HD_EXIT
/**
 * Function cancels reentrance lock
 */
LSA_VOID HIF_HD_EXIT(LSA_VOID);
#endif

#ifndef HIF_HD_DO_POLL_REQUEST
/**
 * \brief Triggers a context switch from the low prio task to HIF task. 
 * 
 * This is a additional poll mechanism to enhance HIF reaction time when the system is in idle.
 * 
 * This function is called from within the function hif_poll().
 * System Adaptation must then call the hif_internal_request() 
 * function with the specified RQB. System Adaptation must execute 
 * a transition in the context of HIF, if necessary. However, in a 
 * typical implementation System Adaptation sends a mail containing 
 * the RQB to the HIF task.
 * 
 * <Low Prio task>:: hif_internal_request --> <Normal Prio Task>:: hif_request
 * 
 * @param [in] ptr_upper_rqb Pointer to the RQB that is to be transferred to hif_request()
 */
LSA_VOID HIF_HD_DO_POLL_REQUEST(
    HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

#ifndef HIF_HD_DO_TIMEOUT_REQUEST
/**
 * \brief Triggers a context switch from timer task to the HIF task.
 * 
 * This function is called from within the function hif_timeout().
 * System Adaptation must then call the hif_internal_request() 
 * function with the specified RQB. System Adaptation must execute 
 * a transition in the context of HIF, if necessary. However, in a 
 * typical implementation System Adaptation sends a mail containing 
 * the RQB to the HIF task.
 * 
 * <Timer task>:: hif_timeout --> <Normal Prio Task>:: hif_request
 * 
 * @param [in] ptr_upper_rqb Pointer to the RQB that is to be transferred to hif_request()
 */
LSA_VOID HIF_HD_DO_TIMEOUT_REQUEST(
    HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

#ifndef HIF_HD_DO_REQUEST_LOCAL
/**
 * \brief Enqueues a internal RQB into the mailbox of the HIF task. No context switch necessary.
 * 
 * This function is only used in short mode. This function is called from within the function hif_hd_request().
 * 
 * System Adaptation must then call the hif_hd_request() 
 * function with the specified RQB. System Adaptation must execute 
 * a transition in the context of HIF, if necessary. However, in a 
 * typical implementation System Adaptation sends a mail containing 
 * the RQB to the HIF task.
 * 
 * @param [in] ptr_upper_rqb - Pointer to the RQB that is to be transferred to hif_request()
 */
LSA_VOID HIF_HD_DO_REQUEST_LOCAL(
    HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

#ifndef HIF_DO_LOW_REQUEST
/**
 * \brief Enqueues a RQB into the mailbox of the low prio HIF task
 * 
 * This function is called from within the function hif_request().
 * System Adaptation must then call the hif_internal_poll()
 * function with the specified RQB. System Adaptation must execute
 * a transition in the low prio context of HIF. However, in a
 * typical implementation System Adaptation sends a mail containing
 * the RQB to the low prio HIF task.
 * 
 * @param [in] ptr_upper_rqb - Pointer to the RQB that is to be transferred to hif_internal_poll()
 */
LSA_VOID HIF_DO_LOW_REQUEST(
    HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

#ifndef HIF_HD_U_GET_PATH_INFO
/**
 * \brief Get path info from HD Upper.
 * 
 * Function obtains channel path information. Function is called
 * in the Open Channel service in order to get the information from
 * system adaptation regarding the channel. If system adaptation 
 * recognizes an invalid path, it returns the error code 
 * LSA_RET_ERR_SYS_PATH.
 * 
 * @param [out] ret_val_ptr      - Pointer to address for the response
 * @param [out] sys_ptr_ptr      - Pointer to address for LSA Sys Ptr
 * @param [out] detail_ptr_ptr   - Pointer to address for Detail Ptr
 * @param [in] path              - LSA path info as specified for Open Channel
 * 
 */
LSA_VOID HIF_HD_U_GET_PATH_INFO(
    LSA_UINT16          *ret_val_ptr,
    LSA_SYS_PTR_TYPE	*sys_ptr_ptr,
    HIF_DETAIL_PTR_TYPE	*detail_ptr_ptr,
    LSA_SYS_PATH_TYPE   path
);
#endif

#ifndef HIF_HD_L_GET_PATH_INFO
/**
 * \brief Get path info from HD Lower.
 * 
 * Function obtains channel path information. Function is called
 * in the Open Channel service in order to get the information from
 * system adaptation regarding the channel. If system adaptation 
 * recognizes an invalid path, it returns the error code 
 * LSA_RET_ERR_SYS_PATH.
 * 
 * @param [out] ret_val_ptr      - Pointer to address for the response
 * @param [out] sys_ptr_ptr      - Pointer to address for LSA Sys Ptr
 * @param [out] detail_ptr_ptr   - Pointer to address for Detail Ptr
 * @param [in] path              - LSA path info as specified for Open Channel
 * 
 */
LSA_VOID HIF_HD_L_GET_PATH_INFO(
    LSA_UINT16          *ret_val_ptr,
    LSA_SYS_PTR_TYPE	*sys_ptr_ptr,
    HIF_DETAIL_PTR_TYPE	*detail_ptr_ptr,
    LSA_SYS_PATH_TYPE   path
);
#endif

#ifndef HIF_HD_U_RELEASE_PATH_INFO
/**
 * \brief release path info from HD Upper.
 * 
 * Function releases channel path information. This function is 
 * called when a channel is closed (Close Channel). In the process,
 * the two pointers sys_ptr and detail_ptr are returned to system
 * adaptation. If the pointers are invalid, the output macro returns 
 * LSA_RET_ERR_PARAM.
 * 
 * @param [out] ptr_dst     - Pointer to address for the response
 * @param [in] sys_ptr      - Sys Ptr that was transferred to the HIF when HIF_HD_U_GET_PATH_INFO was called
 * @param [in] detail_ptr   - Detail Ptr that was transferred to the HIF when HIF_HD_U_GET_PATH_INFO was called.
 * 
 */
LSA_VOID HIF_HD_U_RELEASE_PATH_INFO(
    LSA_UINT16	        *ptr_dst,
    LSA_SYS_PTR_TYPE    sys_ptr,
    HIF_DETAIL_PTR_TYPE detail_ptr
);
#endif

#ifndef HIF_HD_L_RELEASE_PATH_INFO
/**
 * \brief release path info from HD Lower.
 * 
 * Function releases channel path information. This function is 
 * called when a channel is closed (Close Channel). In the process,
 * the two pointers sys_ptr and detail_ptr are returned to system
 * adaptation. If the pointers are invalid, the output macro returns 
 * LSA_RET_ERR_PARAM.
 * 
 * @param [out] ptr_dst     - Pointer to address for the response
 * @param [in] sys_ptr      - Sys Ptr that was transferred to the HIF when HIF_HD_L_GET_PATH_INFO was called
 * @param [in] detail_ptr   - Detail Ptr that was transferred to the HIF when HIF_HD_L_GET_PATH_INFO was called.
 * 
 */
LSA_VOID HIF_HD_L_RELEASE_PATH_INFO(
    LSA_UINT16	        *ptr_dst,
    LSA_SYS_PTR_TYPE    sys_ptr,
    HIF_DETAIL_PTR_TYPE detail_ptr
);
#endif

#ifndef HIF_HD_U_GET_PARAMS
/**
 * \brief Gets Parameters from system adaptation.
 * 
 * Function is called during the opening sequence of HIF. 
 * Function retrieves parameters, required for the setup of the 
 * upper layer of Hardware Device.
 * 
 * @param [out] ret_val_ptr - Pointer to address for response
 * @param [out] pPars       - Pointer to the data structure of type HIF_GET_PAR_TYPE
 * @param [in] hSysDev      - Device handle
 *
 */
LSA_VOID HIF_HD_U_GET_PARAMS(
    LSA_UINT16              *ret_val_ptr,
    HIF_SYS_HANDLE          hSysDev,
    HIF_GET_PAR_PTR_TYPE    pPars
);
#endif

#ifndef HIF_HD_L_GET_PARAMS
/**
 * \brief Gets Parameters from system adaptation.
 * 
 * Function is called during the opening sequence of HIF. 
 * Function retrieves parameters, required for the setup of the 
 * lower layer of Hardware Device.
 * 
 * @param [out] ret_val_ptr - Pointer to address for response
 * @param [out] pPars       - Pointer to the data structure of type HIF_GET_PAR_TYPE
 * @param [in] hSysDev      - Device handle
 *
 */
LSA_VOID HIF_HD_L_GET_PARAMS(
    LSA_UINT16              *ret_val_ptr,
    HIF_SYS_HANDLE          hSysDev,
    HIF_GET_PAR_PTR_TYPE    pPars
);
#endif

#ifndef HIF_HD_DO_INTERRUPT_REQUEST
/**
 * \brief Triggers a context switch from the interrupt context to the HIF task.
 * 
 * This function is called from within the function hif_interrupt().
 * System Adaptation must then call the hifl_request() function with
 * the specified RQB. System Adaptation must execute a transition
 * in the context of HIF, if necessary. However, in a typical
 * implementation System Adaptation sends a mail containing the RQB
 * to the HIF task.
 * 
 * <ISR context>:: hif_interrupt --> <Normal Prio Task>:: hif_request
 * 
 * @param [in] ptr_upper_rqb - Pointer to the RQB that is to be transferred to hif_ request()
 */
LSA_VOID HIF_HD_DO_INTERRUPT_REQUEST(
    HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

#ifndef HIF_HD_ENABLE_ISR
/**
 * \brief Enbale interrups für HIF HD.
 * 
 * Function enables interrupts to the other layer of HIF LD.
 * 
 * @param [in] hSysDev  - Device handle
 * @param [in] hH       - Reference to the HIF instance
 */
LSA_VOID HIF_HD_ENABLE_ISR(
    HIF_SYS_HANDLE  hSysDev,
    HIF_HANDLE      hH
);
#endif

#ifndef HIF_HD_DISABLE_ISR
/**
 * \brief Disable interrups für HIF HD.
 * 
 * Function disables interrupts to the other layer of HIF LD.
 * 
 * @param [in] hSysDev  - Device handle
 * @param [in] hH       - Reference to the HIF instance
 */
LSA_VOID HIF_HD_DISABLE_ISR(
    HIF_SYS_HANDLE  hSysDev,
    HIF_HANDLE      hH
);
#endif

#ifndef HIF_HD_SEND_IRQ
/**
 * \brief Trigger an interrupt for HIF LD.
 * 
 * This macro shall trigger the interrupt to the receiver side.
 * Macro is called by hif_request() and hif_return_rqb() after the 
 * RQB is serialized into the shared memory. 
 * 
 * @param [in] hSysDev  - Device handle
 * @param [in] hH       - Reference to the HIF instance
 */
LSA_VOID HIF_HD_SEND_IRQ(
    HIF_SYS_HANDLE  hSysDev,
    HIF_HANDLE      hH
);
#endif

#ifndef HIF_HD_ACK_IRQ
/**
 * \brief Acknowledge an interrupt for HIF HD.
 * 
 * This macro shall acknowledge the interrupt on initator side.
 * Macro is called by hif_request() in front of access to the shared memory.
 * 
 * @param [in] hSysDev  - Device handle
 * @param [in] hH       - Reference to the HIF instance
 */
LSA_VOID HIF_HD_ACK_IRQ(
    HIF_SYS_HANDLE  hSysDev,
    HIF_HANDLE      hH
);
#endif

#ifndef HIF_HD_OPEN_DEVICE_LOWER 
/**
 * Transfers the Open Device RQB to the lower LSA layer.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB) for open device request
 */
LSA_VOID HIF_HD_OPEN_DEVICE_LOWER(
    HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

#ifndef HIF_HD_CLOSE_DEVICE_LOWER
/**
 * Transfers the Close Device RQB to the lower LSA layer.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB) for close device request
 */
LSA_VOID HIF_HD_CLOSE_DEVICE_LOWER(
    HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

#ifndef HIF_HD_SYSTEM_REQUEST_LOWER 
/**
 * Transfers the System RQB to the lower LSA layer.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB)
 */
LSA_VOID HIF_HD_SYSTEM_REQUEST_LOWER(
    HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

#ifndef HIF_HD_OPEN_CHANNEL_LOWER 
/**
 * Opens a channel to the lower LSA layer of the Logical Device.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB) for open channel request
 * @param [in] hSysDev       - Sys Device Ptr
 */
LSA_VOID HIF_HD_OPEN_CHANNEL_LOWER(
    HIF_RQB_PTR_TYPE    lower_rqb_ptr,
    HIF_SYS_HANDLE      hSysDev
);
#endif

#ifndef HIF_HD_CLOSE_CHANNEL_LOWER 
/**
 * Closes a channel to the lower LSA layer of the Logical Device.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB) for close channel request
 * @param [in] hSysDev       - Sys Device Ptr
 */
LSA_VOID HIF_HD_CLOSE_CHANNEL_LOWER(
    HIF_RQB_PTR_TYPE    lower_rqb_ptr,
    HIF_SYS_HANDLE      hSysDev
);
#endif

#ifndef HIF_HD_REQUEST_LOWER 
/**
 * This function transfers asynchronous requests to the lower
 * LSA layer of the Logical Device.
 * 
 * @param [in] lower_rqb_ptr - Pointer to request block (RQB) for LSA request
 * @param [in] hSysDev       - Sys Device Ptr
 */
LSA_VOID HIF_HD_REQUEST_LOWER(
    HIF_RQB_PTR_TYPE    lower_rqb_ptr,
    HIF_SYS_HANDLE      hSysDev
);
#endif

#ifndef HIF_HD_REQUEST_UPPER_DONE 
/**
 * This macro is called by HIF when a user channel request (RQB)
 * from calling hif_open_channel(), hif_request() or 
 * hif_close_channel() is finished.
 * The system adaptation has to call the upper callback function
 * to finish the request.
 * 
 * @param [in] hif_request_upper_done_ptr   - Function pointer to the request done function
 * @param [in] ptr_upper_rqb                - Pointer to the LSA RQB
 * @param [in] sys_ptr                      - Pointer to the sys path
 */
LSA_VOID HIF_HD_REQUEST_UPPER_DONE(
    HIF_UPPER_CALLBACK_FCT_PTR_TYPE hif_request_upper_done_ptr,
    HIF_RQB_PTR_TYPE                ptr_upper_rqb,
    LSA_SYS_PTR_TYPE                sys_ptr
);
#endif

#ifndef HIF_HD_L_SYSTEM_REQUEST_DONE 
/**
 * This function is called by HIF HD Lower when a request (RQB)
 * from calling hif_system() is finished.
 * The system adaptation has to call the callback function to
 * finish the request.
 * 
 * @param [in] hif_request_upper_done_ptr   - Function pointer to the request done function
 * @param [in] ptr_upper_rqb                - Pointer to the LSA RQB
 */
LSA_VOID HIF_HD_L_SYSTEM_REQUEST_DONE(
    HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE    hif_system_request_done_ptr,
    HIF_RQB_PTR_TYPE                    ptr_upper_rqb
);
#endif

#ifndef HIF_HD_U_SYSTEM_REQUEST_DONE 
/**
 * This function is called by HIF HD Upper when a request (RQB)
 * from calling hif_system() is finished.
 * The system adaptation has to call the callback function to
 * finish the request.
 * 
 * @param [in] hif_request_upper_done_ptr   - Function pointer to the request done function
 * @param [in] ptr_upper_rqb                - Pointer to the LSA RQB
 */
LSA_VOID HIF_HD_U_SYSTEM_REQUEST_DONE(
    HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE    hif_system_request_done_ptr,
    HIF_RQB_PTR_TYPE                    ptr_upper_rqb
);
#endif

/*===========================================================================*/
/* non-LSA-predefined             prototyping                                */
/*===========================================================================*/

/**
 * \brief fills the global fatal-error structure and call HIF_FATAL_ERROR
 * 
 * @param [in] module_id
 * @param [in] line
 * @param [in] error_code_0
 * @param [in] error_code_1
 * @param [in] error_code_2
 * @param [in] error_code_3
 * @param [in] error_data_len
 * @param [in] error_data
 * @return
 */
LSA_VOID hif_fatal_error(
	LSA_UINT16          module_id,
	LSA_UINT16          line,
	LSA_UINT32	        eror_code_0,
	LSA_UINT32	        eror_code_1,
	LSA_UINT32	        eror_code_2,
	LSA_UINT32	        eror_code_3,
	LSA_UINT16          error_data_len,
	LSA_VOID_PTR_TYPE   error_data
);

/**
 * \brief send a system request
 * 
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_*
 *                                  or others
 * All other RQB-parameters depend on kind of request.
 * 
 * @param [in] pRQB pointer to system-RQB
 * @return LSA_RET_OK
 * @return LSA_RET_ERR_PARAM
 * @return LSA_RET_ERR_SEQUENCE
 */
LSA_UINT16 hif_ld_system(
    HIF_RQB_PTR_TYPE pRQB
);

/**
 * \brief send a system request
 * 
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:                    Depend on kind of request.
 *     
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_*
 *                                  or others
 * All other RQB-parameters depend on kind of request.
 * 
 * @param [in] pRQB:  pointer to system-RQB
 * @return LSA_RET_OK
 * @return LSA_RET_ERR_PARAM
 * @return LSA_RET_ERR_SEQUENCE
 */
LSA_UINT16 hif_hd_system(
    HIF_RQB_PTR_TYPE pRQB
);

/**
 * \brief finish a HIF_XX_UPPER_DEVICE_OPEN request
 * 
 * 
 * 
 * @param [in] pRQB  - Request from open device
 * @return
 */
LSA_VOID hif_open_device_lower_done(
	HIF_RQB_PTR_TYPE pRQB
);

/**
 * \brief finish a HIF_XX_UPPER_DEVICE_CLOSE request
 * 
 * 
 * 
 * @param [in] pRQB  - Request from open device
 * @return
 */
LSA_VOID hif_close_device_lower_done(
	HIF_RQB_PTR_TYPE pRQB
);

#if (HIF_CFG_USE_DMA == 1)
/**
* \brief initialize the buffer administration
*
* @param
*
* @return
*/
LSA_VOID hif_buffer_admin_init(LSA_VOID);

/**
* \brief add a buffer to the hif internal management structure
* The management takes care of freeing the buffer when the
* application prompts to by setting the finished flag.
*
* @param [in] pBuffer                  - pointer to the buffer which shall be mamanged
* @param [in] compId                   - component id the buffer was allocated with
* @param [in] memType                  - memType id the buffer was allocated with
* @param [in] hd_id                    - hd id the buffer was allocated with
* @param [in] expectedFinsihedFlag     - value which is compared to the ready for free flag
* @param [in] pReadyForFree            - address where ready for free flag will be written by the application
*/
LSA_VOID hif_buffer_admin_add(
    LSA_VOID        *pBuffer,
    LSA_UINT16      compId,
    LSA_UINT16      memType,
    HIF_HD_ID_TYPE  hd_id,
    LSA_UINT32      expectedFinsihedFlag,
    LSA_UINT32      *pReadyForFree
);

/**
* \brief free all buffers which are already processed
*
* check the all buffers and free the buffers which are 
* completely processed. Checked by the finished flag which
* is written by the user.
*
* @param
*
* @return
*/
LSA_VOID hif_buffer_admin_free(LSA_VOID);

/**
* \brief undo initialization of the buffer administration and free all remaining buffers
*
* @param
*
* @return
*/
LSA_VOID hif_buffer_admin_undo_init(LSA_VOID);

#endif

/*=============================================================================
 * function name:  HIF_SER_COPY_SHMTH_LSA_BOOL
 * function name:  HIF_SER_COPY_HTSHM_LSA_BOOL
 *
 * function:       resolves the COPY-function to the correct LSA_BOOL-type
 *
 *===========================================================================*/

#if (HIF_LSA_BOOL_SIZE == 1)
    #define HIF_SER_COPY_HTSHM_LSA_BOOL     HIF_SER_COPY_HTSHM_8
    #define HIF_SER_COPY_SHMTH_LSA_BOOL     HIF_SER_COPY_SHMTH_8
#elif(HIF_LSA_BOOL_SIZE == 2)
    #define HIF_SER_COPY_HTSHM_LSA_BOOL     HIF_SER_COPY_HTSHM_16
    #define HIF_SER_COPY_SHMTH_LSA_BOOL     HIF_SER_COPY_SHMTH_16
#elif(HIF_LSA_BOOL_SIZE == 4)
    #define HIF_SER_COPY_HTSHM_LSA_BOOL     HIF_SER_COPY_HTSHM_32
    #define HIF_SER_COPY_SHMTH_LSA_BOOL     HIF_SER_COPY_SHMTH_32
#else
    #error "Invalid HIF_LSA_BOOL_SIZE (!=1 und !=2 und !=4) defined in HIF_CFG.H!"
#endif

#endif // (HIF_CFG_USE_HIF == 1)

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
#endif  /* of HIF_SYS_H */

#ifndef PND_PNSTACK_H
#define PND_PNSTACK_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_pnstack.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Prototypes for all PN stack component adaption functions (ie PATH)       */
/*                                                                           */
/*****************************************************************************/

#define PND_INVALID_PNIO_USER_HANDLE ((LSA_HANDLE_TYPE)-1)  // Invalid PNIO User Handle

#define PND_IOC_GROUP_ID  PSI_PROVIDER_GROUP_ID_CMCL   // 0 <= group_id < EDD_CFG_CSRT_MAX_PROVIDER_GROUP
#define PND_IOD_GROUP_ID  PSI_PROVIDER_GROUP_ID_CMSV   // 0 <= group_id < EDD_CFG_CSRT_MAX_PROVIDER_GROUP
#define PND_IOM_GROUP_ID  PSI_PROVIDER_GROUP_ID_CMMC   // 0 <= group_id < EDD_CFG_CSRT_MAX_PROVIDER_GROUP

/*----------------------------------------------------------------------------*/
typedef enum pnd_ioc_ar_state_enum { // AR states IOC/IOM
	PND_IOC_AR_STATE_UNKNOWN  = 0x00,  // Initial state (added only)
	PND_IOC_AR_STATE_OFFLINE  = 0x01,  // Offline state
	PND_IOC_AR_STATE_START    = 0x02,  // AR connecting till IN-DATA
	PND_IOC_AR_STATE_PREDATA  = 0x03,  // AR IN-DATA follows, Alarm.ind and Record-RW enabled
	PND_IOC_AR_STATE_INDATA   = 0x04,  // AR IN-DATA
	PND_IOC_AR_STATE_PASSIVE  = 0x05,  // AR passivated
	PND_IOC_AR_STATE_MAX
} PND_IOC_AR_STATE_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct pnd_ar_ds_state_tag {  // see also cm_event_apdu_status_enum in cm_usr.h
	LSA_BOOL is_run;          // state run(=LSA_TRUE) or Stop(=LSA_FALSE)
	LSA_BOOL is_station_ok;   // state station OK(=LSA_TRUE) or probelm(=LSA_FALSE)
	                          // following state only for SRD ARs
	LSA_BOOL is_primary;      // state primary(=LSA_TRUE) or backup(=LSA_FALSE)
	LSA_BOOL is_sysred_ok;    // state sysred OK(=LSA_TRUE) or problem(=LSA_FALSE)
} PND_AR_DS_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct pnd_pi_prop_tag {  // basic PI buffer properties (from IOCR config)
	LSA_BOOL    is_provider;           // provider buffer yes/no
	LSA_BOOL    is_dfp;                // DFP buffer yes/no
	LSA_BOOL    is_rtc3;               // RTC3 buffer yes/no
	LSA_UINT32  partial_length;        // size of PI data (not alligned) in bytes
	LSA_UINT32  apdu_status_length;    // size of APDU state in byte
	LSA_UINT16  group_id;              // ACP Group-ID (CL,MC,SV-Group)
	LSA_UINT8   prov_type;             // Provider Type see EDD_PROVADD_TYPE_xx
	LSA_UINT16  frame_id;              // image FrameID
	LSA_UINT32  subframe_offset;       // DFP subframe offset
	LSA_UINT32  subframe_data;         // DFP subframe data (contains position and subframe length)

} PND_PI_PROP_TYPE, *PND_PI_PROP_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct pnd_pnuc_iocr_tag {  // holds IOCR config data for one input/output PI buffer
	LSA_UINT16         iocr_ref_nr;       // CR number
	LSA_VOID_PTR_TYPE  arcb_iocr_ptr;     // link to IOCR ptr in ARCB (for update after PI ALLOC)
	LSA_VOID_PTR_TYPE  iodu_handle;       // logical buffer handle from IODU
	PND_PI_PROP_TYPE   m_piProperties;    // PI properties (from IOCR configuration)

} PND_PNUC_IOCR_TYPE, *PND_PNUC_IOCR_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct pnd_hd_mem_tag {  // Memory block information 
    PNIO_UINT8* base_ptr;            // base address
    PNIO_UINT32 size;                // Size of memory block
    PNIO_UINT32 phy_addr;            // Phy. address 
} PND_HD_MEM_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct {
    PNIO_UINT32 if_nr;                   //IN
    PNIO_UINT16 hd_nr;                   //IN
    PNIO_BOOL   runs_on_level_basic;     //IN
    struct
    {
        struct
        {
            PNIO_BOOL            iocc_enable;        //IN
            PNIO_UINT32          iocc_virtual_addr;  //IN
            PNIO_UINT32          iocc_ahb_addr;      //IN
            PNIO_UINT32          kram_ahb_addr;      //IN 
            PND_HD_MEM_TYPE      shared_mem;         //IN
            PND_HD_MEM_TYPE      sdram;              //IN
            PND_HD_MEM_TYPE      pi;
        } soc;
    } hw;
} PND_SYSTEM_DEVICE_INFO_TYPE;
typedef PND_SYSTEM_DEVICE_INFO_TYPE * PND_SYSTEM_DEVICE_INFO_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Adaption functions for PNIO base system (PNIO user to PNIO stack component)*/

LSA_VOID_PTR_TYPE pnd_cm_alloc_rqb( 
	LSA_OPCODE_TYPE     opcode, 
	LSA_HANDLE_TYPE     handle, 
	PND_HANDLE_PTR_TYPE handle_ptr, 
	LSA_UINT16          more_size );

LSA_VOID pnd_cm_free_rqb( 
	LSA_VOID_PTR_TYPE   rqb_ptr, 
	PND_HANDLE_PTR_TYPE handle_ptr );

LSA_VOID_PTR_TYPE pnd_cm_alloc_upper_mem( 
	LSA_UINT16          len, 
	PND_HANDLE_PTR_TYPE handle_ptr );

LSA_VOID pnd_cm_free_upper_mem( 
	LSA_VOID_PTR_TYPE   mem_ptr, 
	PND_HANDLE_PTR_TYPE handle_ptr );

LSA_VOID_PTR_TYPE pnd_oha_alloc_rqb( 
	LSA_OPCODE_TYPE     opcode, 
	LSA_HANDLE_TYPE     handle, 
	PND_HANDLE_PTR_TYPE handle_ptr );

LSA_VOID pnd_oha_free_rqb( 
	LSA_VOID_PTR_TYPE   rqb_ptr, 
	PND_HANDLE_PTR_TYPE handle_ptr );

LSA_VOID_PTR_TYPE pnd_oha_alloc_upper_mem( 
	LSA_UINT16          len, 
	PND_HANDLE_PTR_TYPE handle_ptr );
	
LSA_VOID pnd_oha_free_upper_mem( 
	LSA_VOID_PTR_TYPE   mem_ptr,
	PND_HANDLE_PTR_TYPE handle_ptr );

/* Adaption function for PSI base system */
PNIO_UINT32 pnd_psi_get_path_info(
    PND_SYSTEM_DEVICE_INFO_PTR_TYPE pDetail,
    PNIO_UINT16          hdNr,
    PNIO_UINT16          ifNr);

PNIO_UINT32 pnd_psi_release_path_info(
    LSA_SYS_PTR_TYPE     pSys,
    PNIO_VOID_PTR_TYPE   pDetail);

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_PNSTACK_H */

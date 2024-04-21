#ifndef EPS_USR_H                        /* ----- reinclude-protection ----- */
#define EPS_USR_H


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
/*  F i l e               &F: eps_usr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Impelements the LSA Componenent Interface.                               */
/*	Even though eps is not an LSA component we want to use its interface to  */ 
/*  communicate with other instances of eps.                                 */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "lsa_cfg.h"
//#include "psi_sys.h"
#include "eps_mem.h"


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define EPS_FCT_PTR_NULL        (0)
#define EPS_TEST_POINTER(ptr_)  (ptr_)

/* Return types */
#define EPS_RET_ERR     0
#define EPS_RET_OK      1


/*****************************************************************************/
/* Local data                                                                */
/* ...                                                                       */
/*****************************************************************************/

/* Configure EPS rqb interface                                               */
/*---------------------------------------------------------------------------*/

///Maximum channels for eps rqb interface
#define EPS_CFG_MAX_CHANNELS              6


/// Enum to mark a channel as either free to use or already in use. 
enum eps_channel_state_enum {
  EPS_CHANNEL_STATE_FREE = 0,
  EPS_CHANNEL_STATE_IN_USE,
};

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* set and get parameter of the rqb-header                                   */
/*---------------------------------------------------------------------------*/

#define EPS_UPPER_RQB_SET_NEXT_RQB_PTR      LSA_RQB_SET_NEXT_RQB_PTR
#define EPS_UPPER_RQB_GET_NEXT_RQB_PTR      LSA_RQB_GET_NEXT_RQB_PTR
#define EPS_UPPER_RQB_SET_PREV_RQB_PTR      LSA_RQB_SET_PREV_RQB_PTR
#define EPS_UPPER_RQB_GET_PREV_RQB_PTR      LSA_RQB_GET_PREV_RQB_PTR
#define EPS_UPPER_RQB_SET_OPCODE            LSA_RQB_SET_OPCODE
#define EPS_UPPER_RQB_GET_OPCODE            LSA_RQB_GET_OPCODE
#define EPS_UPPER_RQB_SET_HANDLE            LSA_RQB_SET_HANDLE
#define EPS_UPPER_RQB_GET_HANDLE            LSA_RQB_GET_HANDLE
#define EPS_UPPER_RQB_SET_USERID_UVAR32     LSA_RQB_SET_USER_ID_UVAR32
#define EPS_UPPER_RQB_GET_USERID_UVAR32     LSA_RQB_GET_USER_ID_UVAR32
#define EPS_UPPER_RQB_SET_USERID_UVAR16_1   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW
#define EPS_UPPER_RQB_GET_USERID_UVAR16_1   LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW
#define EPS_UPPER_RQB_SET_USERID_UVAR16_2   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH
#define EPS_UPPER_RQB_GET_USERID_UVAR16_2   LSA_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH
#define EPS_UPPER_RQB_SET_RESPONSE          LSA_RQB_SET_RESPONSE
#define EPS_UPPER_RQB_GET_RESPONSE          LSA_RQB_GET_RESPONSE
#define EPS_UPPER_RQB_SET_USER_ID_PTR       LSA_RQB_SET_USER_ID_PTR
#define EPS_UPPER_RQB_GET_USER_ID_PTR       LSA_RQB_GET_USER_ID_PTR

#define EPS_RQB_SET_NEXT_RQB_PTR      LSA_RQB_SET_NEXT_RQB_PTR
#define EPS_RQB_GET_NEXT_RQB_PTR      LSA_RQB_GET_NEXT_RQB_PTR
#define EPS_RQB_SET_PREV_RQB_PTR      LSA_RQB_SET_PREV_RQB_PTR
#define EPS_RQB_GET_PREV_RQB_PTR      LSA_RQB_GET_PREV_RQB_PTR
#define EPS_RQB_SET_OPCODE            LSA_RQB_SET_OPCODE
#define EPS_RQB_GET_OPCODE            LSA_RQB_GET_OPCODE
#define EPS_RQB_SET_HANDLE            LSA_RQB_SET_HANDLE
#define EPS_RQB_GET_HANDLE            LSA_RQB_GET_HANDLE
#define EPS_RQB_SET_USERID_UVAR32     LSA_RQB_SET_USER_ID_UVAR32
#define EPS_RQB_GET_USERID_UVAR32     LSA_RQB_GET_USER_ID_UVAR32
#define EPS_RQB_SET_USERID_UVAR16_1   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW
#define EPS_RQB_GET_USERID_UVAR16_1   LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW
#define EPS_RQB_SET_USERID_UVAR16_2   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH
#define EPS_RQB_GET_USERID_UVAR16_2   LSA_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH
#define EPS_RQB_SET_RESPONSE          LSA_RQB_SET_RESPONSE
#define EPS_RQB_GET_RESPONSE          LSA_RQB_GET_RESPONSE
#define EPS_RQB_SET_USER_ID_PTR       LSA_RQB_SET_USER_ID_PTR
#define EPS_RQB_GET_USER_ID_PTR       LSA_RQB_GET_USER_ID_PTR

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*=====  forward declaration  =====*/
//struct eps_rqb_tag;
//typedef struct eps_channel_tag * EPS_CHANNEL_PTR_TYPE; 

/*=====  opcodes within request block  =====*/
PTRC_SUBLABEL_LIST_OPEN("EPS_OPC")
#define EPS_OPC_OPEN_CHANNEL                 0x01
#define EPS_OPC_CLOSE_CHANNEL                0x02
#define EPS_OPC_GET_STATISTICS               0x03
#define EPS_OPC_UNUSED                       0xff
PTRC_SUBLABEL_LIST_CLOSE("EPS_OPC")

/*=====  response codes within request block  =====*/

PTRC_SUBLABEL_LIST_OPEN("EPS_RSP")
#define EPS_OK                      LSA_RSP_OK
#define EPS_OK_CANCELLED            LSA_RSP_OK_OFFSET + 1)

#define EPS_ERR_RESOURCE            LSA_RSP_ERR_RESOURCE
#define EPS_ERR_PARAM               LSA_RSP_ERR_PARAM
#define EPS_ERR_SEQUENCE            LSA_RSP_ERR_SEQUENCE
#define EPS_ERR_SYS_PATH            LSA_RSP_ERR_SYS_PATH

#define	EPS_ERR_OPCODE		        LSA_RSP_ERR_OFFSET
PTRC_SUBLABEL_LIST_CLOSE("EPS_RSP")

/*=============================================================================
 *      eps request block (== upper request block)
 *===========================================================================*/

typedef 
/// Saves the details of the channel, used to check where a RQB is meant for.
/// Basically this information is the same as in the SysPath passed at Channel_Open,
/// but this struct is used to save this pathing information to be associated with a channel
struct eps_detail_tag
{
  ///number of the HD, see PSI documentation for further details
  LSA_UINT16 hd_nr;                             
  ///shows whether the eps instance is LD or not (it is possible for an eps instance to be LD as well as HD)
  LSA_BOOL   is_ld_eps;			
} EPS_DETAIL_TYPE,* EPS_DETAIL_PTR_TYPE;

///declare EPS_RQB_PTR_TYPE2 so that we may use it before we actually define the type
typedef struct eps_rqb_tag* EPS_RQB_PTR_TYPE2;

///callback function type for eps_open_channel,eps_close_channel,eps_request
typedef  LSA_VOID  LSA_FCT_PTR(,EPS_CALLBACK_FCT_PTR_TYPE2)(EPS_RQB_PTR_TYPE2 pRQB);

typedef 
///holds all the information necessary to use a channel
struct eps_channel_tag
{
  ///channel state 
  LSA_UINT16                    state;			                                                        	
  ///the handle of the user            
  LSA_HANDLE_TYPE               handle_upper;	
  ///the 'hint' for the system adaption
  LSA_SYS_PTR_TYPE              sysptr;		
  ///pointer to channel-details        
  EPS_DETAIL_PTR_TYPE           detailptr;		
  ///the callback-function of the user 
  EPS_CALLBACK_FCT_PTR_TYPE2    callback;	                                       
} EPS_CHANNEL_TYPE,* EPS_CHANNEL_PTR_TYPE;
typedef EPS_CHANNEL_TYPE const* EPS_CHANNEL_CONST_PTR_TYPE;

typedef
/// Holds all the information necessary for eps to open a new channel.
/// This struct must be passed as args when the opcode is set to EPS_OPC_OPEN_CHANNEL
struct eps_rqb_args_channel_tag
{
	                               /** req: -                                 
                                     * cnf: channel-handle of eps             
                                     * At all later RQBs user will write it   
                                     * into handle of EPS_RQB_HEADER.         */
    LSA_HANDLE_TYPE                 handle;
                                   /** req: channel-handle of user            
                                     * cnf: -                                 
                                     * At all confirmations eps writes it     
                                     * into handle of EPS_RQB_HEADER.         */
    LSA_HANDLE_TYPE                 handle_upper;
	                               /** req: system-path of channel            
                                     * cnf: -                                 */
    LSA_SYS_PATH_TYPE               sys_path;
	                               /** req: pointer to callback-function      
                                     * cnf: -                                 */
    EPS_CALLBACK_FCT_PTR_TYPE2      eps_request_done_callback_fct_ptr;
} EPS_RQB_ARGS_CHANNEL_TYPE,* EPS_RQB_ARGS_CHANNEL_PTR_TYPE;

typedef struct eps_memory_statistics_tag
{
	LSA_UINT32  sum_alloc_count;
	LSA_UINT32  sum_act_alloc_count;
	LSA_UINT32  sum_act_alloc_size;
	LSA_UINT32  max_alloc_size;
} EPS_MEM_STAT_TYPE, *EPS_MEM_STAT_PTR_TYPE;

typedef 
///arguements for EPS_OPC_GET_STATISTICS
struct eps_memory_statistics_args_tag
{
	///id of the component from which you want to get the statistics
	LSA_UINT16          comp_id;
	///pointer to array of memory statistic data
	EPS_MEM_STAT_TYPE   mem_stats_for_component; 
} EPS_MEM_STAT_ARGS_TYPE,* EPS_MEM_STAT_ARGS_PTR_TYPE;

typedef 
///union of arguments of eps request block
union eps_rqb_args_tag
{
  ///pointer to the channel arguements, used with EPS_OPC_CHANNEL_OPEN
  EPS_RQB_ARGS_CHANNEL_PTR_TYPE                       channel;
  /// memory statistics arguments, used with EPS_OPC_GET_STATISTICS
  EPS_MEM_STAT_ARGS_PTR_TYPE						  mem_statistics;
} EPS_RQB_ARGS_TYPE,* EPS_RQB_ARGS_PTR_TYPE;

///EPS_RQB_HEADER is standard LSA_RQB_HEADER, with the previous and next pointer being of type EPS_RQB_PTR_TYPE2
#define EPS_RQB_HEADER           LSA_RQB_HEADER(EPS_RQB_PTR_TYPE2)
///EPS_RQB_TRAILER is standard LSA_RQB_TRAILER
#define EPS_RQB_TRAILER          LSA_RQB_TRAILER


typedef 
///eps request block definition
struct eps_rqb_tag
{
	///EPS-RQB-header
    EPS_RQB_HEADER                                                                               
	///arguments corresponding to  
    ///the opcode, see             
    ///EPS_RQB_ARGS_TYPE 
    EPS_RQB_ARGS_TYPE args;    
	///specification wheter the RQB should be delivered to LD-EPS or HD-EPS
	///User needs to specify this because the Handle alone is not unique in case of EPS.
	LSA_BOOL meant_for_ld;
	///additional RQB trailer, see 
    EPS_RQB_TRAILER                                                           
} EPS_RQB_TYPE2;


typedef 
///Struct to store the information necessary for channel management 
struct eps_rqb_interface_data_tag
{
  ///stores all channels which of the eps rqb interface
  EPS_CHANNEL_TYPE                  channels[EPS_CFG_MAX_CHANNELS];
} EPS_RQB_INTERFACE_DATA_TYPE,* EPS_RQB_INTERFACE_DATA_PTR_TYPE;


/*===========================================================================*/
/*                                 variables                                 */
/*===========================================================================*/
///Variable to store the information necessary for channel management 
extern EPS_RQB_INTERFACE_DATA_TYPE eps_rqb_interface_data;


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/**
  function name:  eps_open_channel
 
  function:       open a communication channel
 
  parameters:     EPS_RQB_PTR_TYPE2  pRQB:  pointer to RQB
 
  return value:   LSA_VOID
 
  RQB parameters: 
      RQB header:
      LSA_OPCODE_TYPE    opcode:        EPS_OPC_OPEN_CHANNEL
      LSA_HANDLE_TYPE    handle:        channel-handle of user
      LSA_USER_ID_TYPE   user-id:       id of user
      RQB args:
      LSA_HANDLE_TYPE    handle_upper:  channel-handle of user
      LSA_SYS_PATH_TYPE  sys_path:      system-path of channel                 
      EPS_CALLBACK_FCT_PTR_TYPE2  
                         eps_request_done_callback_fct_ptr:
                                        pointer to callback-function
 
  RQB return values via callback-function:
      RQB header:
      LSA_RESPONSE_TYPE  response:      EPS_RSP_*
      RQB args:
      LSA_HANDLE_TYPE    handle:        channel-handle of eps
      All other RQB parameters are unchanged.
*/
LSA_VOID  eps_open_channel(
    EPS_RQB_PTR_TYPE2  pRQB
);

/**
  function name:  eps_close_channel
 
  function:       close a communication channel
 
  parameters:     EPS_RQB_PTR_TYPE2  pRQB:  pointer to RQB
 
  return value:   LSA_VOID
 
  RQB parameters: 
      RQB header:
      LSA_OPCODE_TYPE    opcode:    EPS_OPC_CLOSE_CHANNEL
      LSA_HANDLE_TYPE    handle:    channel-handle of eps
      LSA_USER_ID_TYPE   user-id:   id of user
 
  RQB return values via callback-function:
      RQB header:
      LSA_HANDLE_TYPE    handle:    channel-handle of user
      LSA_RESPONSE_TYPE  response:  EPS_RSP_*
      All other RQB parameters are unchanged.
*/
LSA_VOID  eps_close_channel(
    EPS_RQB_PTR_TYPE2  pRQB
);

/**
  function name:  eps_request
 
  function:       send a request to a communication channel
 
  parameters:     EPS_RQB_PTR_TYPE2  pRQB:  pointer to RQB
 
  return value:   LSA_VOID
 
  RQB parameters: 
      RQB header:
      LSA_OPCODE_TYPE    opcode:    EPS_OPC_*
      LSA_HANDLE_TYPE    handle:    channel-handle of eps
      LSA_USER_ID_TYPE   user-id:   id of user
      RQB args:                     Depend on kind of request.
      
  RQB return values via callback-function:
      RQB header:
      LSA_OPCODE_TYPE    opcode:    EPS_OPC_*
      LSA_HANDLE_TYPE    handle:    channel-handle of user
      LSA_RESPONSE_TYPE  response:  EPS_RSP_*
      LSA_USER_ID_TYPE   user-id:   id of user
      All other RQB parameters depend on kind of request.
*/
LSA_VOID  eps_request(
    EPS_RQB_PTR_TYPE2  pRQB
);

/**
  function name:  EPS_RQB_ERROR
 
  function:       notify a user application error
*/
#ifndef EPS_RQB_ERROR
LSA_VOID  EPS_RQB_ERROR(
  EPS_RQB_PTR_TYPE2  pRQB
);
#endif

/**
  function name:  EPS_REQUEST_DONE
 
  function:       returns a finished request to the upper layer
*/
#ifndef EPS_REQUEST_DONE
LSA_VOID  EPS_REQUEST_DONE(
    EPS_CALLBACK_FCT_PTR_TYPE2  eps_request_done_callback_fct_ptr,
    EPS_RQB_PTR_TYPE2          upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                 sys_ptr
);
#endif

/**
  function name:  eps_init_rqb_interface
 
  function:       does the initialization for the rqb interface of eps  
*/
LSA_RESPONSE_TYPE eps_init_rqb_interface(LSA_VOID);

/**
  function name:  eps_undo_init_rqb_interface
 
  function:       reverses the initialization for the rqb interface of eps 
*/
LSA_RESPONSE_TYPE eps_undo_init_rqb_interface(LSA_VOID);


/**
  function name:  eps_get_path_info
 
  function:       retrieves information about the LSA-path 
*/
LSA_RESULT eps_get_path_info(
	LSA_SYS_PTR_TYPE        *sys_ptr_ptr,
	EPS_DETAIL_PTR_TYPE     *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       sys_path );

/**
  function name:  eps_release_path_info
 
  function:       releases information about an LSA-path 
*/
LSA_RESULT eps_release_path_info(
	LSA_SYS_PTR_TYPE      sys_ptr,
	EPS_DETAIL_PTR_TYPE   detail_ptr );

/**
  function name:  eps_callback
 
  function:       use the callback function of the channel to pass the rqb back 
*/
LSA_VOID eps_callback(
	EPS_CHANNEL_CONST_PTR_TYPE channel, 
	LSA_UINT16 response, 
	EPS_RQB_PTR_TYPE2* ppRQB);

/*****************************************************************************/
/*  end of file eps_usr.h                                                    */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
#endif  /* of EPS_USR_H */

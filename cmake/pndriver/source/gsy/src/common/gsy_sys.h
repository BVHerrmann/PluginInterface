
#ifndef GSY_SYS_H                     /* ----- reinclude-protection ----- */
#define GSY_SYS_H

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
/*  C o m p o n e n t     &C: GSY (Generic Sync Module)                 :C&  */
/*                                                                           */
/*  F i l e               &F: gsy_sys.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface                                                         */
/*  Defines constants, types, macros and prototyping for GSY.                */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                  Who  What                           */
/*  2004-11-29  P01.00.00.00_00.01.00.00 lrg  from LSA_GLOB P2.1.0.0_0.4.1.1 */
/*  2005-06-02  P03.06.00.00_00.03.00.01 lrg  Prototype for PT2              */
/*  2005-07-26  P03.07.00.00_00.04.01.01 lrg  GSY_DRIFT_GET                  */
/*  2005-11-22  P03.08.00.00_00.01.02.00 lrg  #ifndef GSY_DRIFT_GET          */
/*  2005-12-06  P03.08.00.00_00.01.02.01 lrg  Parameter DriftInterval        */
/*              removed from GSY_SYNC_ALGORITHM(), added to GSY_SYNC_RESET() */
/*              Parameter AdjustValue removed from GSY_SYNC_ALGORITHM() and  */
/*              from GSY_SYNC_ADJUST(): now AdjustInterval is signed.        */
/*              GSY_DRIFT_GET() removed.                                     */
/*  2006-03-08  P03.10.00.00_00.01.01.02 lrg SyncId in gsy_detail            */
/*  2006-03-29  P03.10.00.00_00.02.01.02 lrg FwdRecv und PortFwd[] aus       */
/*              gsy_detail entfernt.                                         */
/*  2006-04-26  P03.11.00.00_00.01.01.04 lrg neu; GSY_GET_TIMESTAMP()        */
/*  2006-05-29  P03.11.00.00_00.04.01.02 lrg neu: GSY_GET_EXT_PLL_OFFSET()   */
/*  2006-06-02  P03.11.00.00_00.04.03.02 lrg  GSY_SYNC_ADJUST() entfernt     */
/*  2006-06-13  P03.12.00.00_00.01.01.02 130606lrg003: GSY_GET_SENDCLOCK()   */
/*  2006-06-29  P03.12.00.00_00.02.01.02 lrg  Neue Rueckgabewerte fuer GSY_  */
/*              SYNC_ALGORITHM(): *pProportionalTerm,*pIntegralTerm          */
/*              LSA_GLOB_P02.01.00.00_00.07.02.01:                           */
/*              PTRC_SUBLABEL_LIST_*, *_TIMER_ID, sys_ptr fuer *_TIMER()     */
/*  2006-07-24  P03.12.00.00_00.25.01.02 lrg  GSY_INT64 -> LSA_INT64         */
/*              GSY_SYNC_SET() um RCF-Intervall erweitert                    */
/*  2007-01-29  P03.12.00.00_00.40.08.02 lrg  GSY_COPY_UPPER_TO_LOCAL_MEM    */
/*  2007-06-01  P04.00.00.00_00.10.02.02 lrg  LSA_GLOB P2.1.0.0_0.8.1.1      */
/*  2007-07-13  P04.00.00.00_00.30.01.00 lrg  GSY_PATH_TYPE_SYNC             */
/*              GSY_SYNC_SET/GET() um EpochNumber erweitert                  */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2010-03-03  P05.00.00.00_00.01.03.02 lrg: Announce channel               */
/*  2010-10-08  P05.00.00.00_00.01.08.04 lrg: GSY_SYNC_GET_EXT_PLL_IN        */
/*  2012-11-07  P05.02.00.00_00.04.08.01 AP01069493 GSY_REQUEST_UPPER_DONE   */
/*              and all other functions with GSY_SYSTEM_OUT_FCT_ATTR moved   */
/*              from gsy_usr.h to gsy_sys.h                                  */
/*  2014-11-14  P06.00.00.00_00.01.49.01 1121583 GSY_UPPER_MEM_PTR_TYPE      */
/*  2015-05-11  V06.00, Inc05, Feature 1227823, Task 1311090                 */
/*              only one user channel for clock or time sync                 */
/*              SyncId removed from gsy_detail_tag                           */
/*  2015-07-15  V06.00, Inc08, Feature 1227823, Task 1260584                 */
/*              [TSyncOverPTCP][GSY] Integrate CLP in GSY: GSY_SYNC_ADJUST   */
/*              added, GSY_SYNC_ALGORITHM and GSY_SYNC_RESET removed         */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                             other constants                               */
/*     (normally you shouldn´t use these definitions, use LSA_* instead)     */
/*===========================================================================*/

/*=====  function return-values  =====*/
PTRC_SUBLABEL_LIST_OPEN("GSY_RET")
#define GSY_RET_OK                         LSA_RET_OK
#define GSY_RET_OK_TIMER_RESTARTED         LSA_RET_OK_TIMER_RESTARTED
#define GSY_RET_OK_TIMER_NOT_RUNNING       LSA_RET_OK_TIMER_NOT_RUNNING

#define GSY_RET_ERR_RESOURCE               LSA_RET_ERR_RESOURCE
#define GSY_RET_ERR_PARAM                  LSA_RET_ERR_PARAM
#define GSY_RET_ERR_SYS_PATH               LSA_RET_ERR_SYS_PATH
#define GSY_RET_ERR_NO_TIMER               LSA_RET_ERR_NO_TIMER
#define GSY_RET_ERR_TIMER_IS_RUNNING       LSA_RET_ERR_TIMER_IS_RUNNING
PTRC_SUBLABEL_LIST_CLOSE("GSY_RET")

/* Error codes for error function and trace
*/
PTRC_SUBLABEL_LIST_OPEN("GSY_ERR")
#define GSY_ERR_FATAL						(LSA_UINT16) 0x8000
#define GSY_ERR_BAD_HANDLE					(LSA_UINT16) 0x8001
#define GSY_ERR_NO_CBF						(LSA_UINT16) 0x8002
#define GSY_ERR_BAD_OPCODE					(LSA_UINT16) 0x8003
#define GSY_ERR_CH_FREE						(LSA_UINT16) 0x8004
#define GSY_ERR_CH_PTR						(LSA_UINT16) 0x8005
#define GSY_ERR_NO_RBU						(LSA_UINT16) 0x8006
#define GSY_ERR_ASYNC_PTR					(LSA_UINT16) 0x8007
#define GSY_ERR_BAD_SERVICE					(LSA_UINT16) 0x8008
#define GSY_ERR_DEVICE						(LSA_UINT16) 0x8009
#define GSY_ERR_FREE_LOWER_MEM				(LSA_UINT16) 0x800a
#define GSY_ERR_FREE_LOWER_RQB				(LSA_UINT16) 0x800b
#define GSY_ERR_NO_SYSTYPE 					(LSA_UINT16) 0x800c
#define GSY_ERR_RECEIVE 					(LSA_UINT16) 0x800d
#define GSY_ERR_SEND	 					(LSA_UINT16) 0x800e
#define GSY_ERR_TIMER	 					(LSA_UINT16) 0x800f
#define GSY_ERR_TIMER_STOP 					(LSA_UINT16) 0x8010
#define GSY_ERR_TIMER_FREE					(LSA_UINT16) 0x8011
#define GSY_ERR_REQLEN	 					(LSA_UINT16) 0x8012
#define GSY_ERR_RBU_FREE	 				(LSA_UINT16) 0x8013
#define GSY_ERR_REL_PATH	 				(LSA_UINT16) 0x8014
#define GSY_ERR_CONFIG		 				(LSA_UINT16) 0x8015
#define GSY_ERR_UID			 				(LSA_UINT16) 0x8016
#define GSY_ERR_CANCEL		 				(LSA_UINT16) 0x8017
#define GSY_ERR_NO_RBL						(LSA_UINT16) 0x8018
#define GSY_ERR_FREE_MEM				    (LSA_UINT16) 0x8019
#define GSY_ERR_MC_REQUEST					(LSA_UINT16) 0x801a
#define GSY_ERR_SYNCSTATE					(LSA_UINT16) 0x801b
#define GSY_ERR_RTC2						(LSA_UINT16) 0x801c
PTRC_SUBLABEL_LIST_CLOSE("GSY_ERR")
	

/*===========================================================================*/
/*                       constants for detail pointer                        */
/*===========================================================================*/

/* channel types: */
PTRC_SUBLABEL_LIST_OPEN("GSY_PATH_TYPE")
#define GSY_PATH_TYPE_USR		1		/* user channel		*/
#define GSY_PATH_TYPE_SYNC		2		/* system channel 0	*/
#define GSY_PATH_TYPE_ANNO		3		/* system channel 1	*/
PTRC_SUBLABEL_LIST_CLOSE("GSY_PATH_TYPE")


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#ifndef GSY_UPPER_MEM_PTR_TYPE
#define GSY_UPPER_MEM_PTR_TYPE					/* pointer to upper memory */ \
    LSA_VOID                    GSY_UPPER_MEM_ATTR *
#endif

#define GSY_LOCAL_MEM_PTR_TYPE               /* pointer to local memory */ \
    LSA_VOID                    GSY_LOCAL_MEM_ATTR *

#define GSY_DETAIL_PTR_TYPE                    /* pointer to GSY_DETAIL */ \
    struct gsy_detail_tag    GSY_SYSTEM_MEM_ATTR *
                               
#define GSY_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
    struct lsa_fatal_error_tag  GSY_LOCAL_MEM_ATTR *


/*=====  gsy_fatal_error  =====*/
/*      (normally you shouldn´t use this definition, use LSA_* instead)      */
#define GSY_FATAL_ERROR_TYPE  LSA_FATAL_ERROR_TYPE


/*=====  gsy_detail  =====*/
typedef struct gsy_detail_tag		//Task 1311090: SyncId removed
{
	LSA_UINT16	NicId;				/* User channel or system channel ANNO:  */
									/* NicId of system channel SYNC to use.  */
									/* System channel SYNC: own NicId        */
	LSA_UINT8	PathType;			/* User channel:   GSY_PATH_TYPE_USR     */
									/* System channel: GSY_PATH_TYPE_SYNC    */
} GSY_DETAIL_TYPE;					/*                 or GSY_PATH_TYPE_ANNO */



/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/



/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  gsy_init
 *
 * function:       initialize GSY
 *
 * parameters:     ....
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/
LSA_UINT16  GSY_SYSTEM_IN_FCT_ATTR  gsy_init(
	LSA_VOID
);

/*=============================================================================
 * function name:  gsy_undo_init
 *
 * function:       undo the initialization of GSY
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/
LSA_UINT16  GSY_SYSTEM_IN_FCT_ATTR  gsy_undo_init(
    LSA_VOID
);

/*=============================================================================
 * function name:  gsy_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * parameters:     LSA_USER_ID_TYPE  user_id:   id of GSY
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_timeout(
    LSA_TIMER_ID_TYPE timer_id,
    LSA_USER_ID_TYPE  user_id
);

/*====  GSY out functions  =====*/

/*=============================================================================
 * function name:  GSY_REQUEST_UPPER_DONE
 *
 * function:       returns a finished request to the upper layer
 *
 * parameters:     GSY_UPPER_CALLBACK_FCT_PTR_TYPE 
 *                                            gsy_request_upper_done_ptr:
 *                     pointer to gsy_request_upper_done_ptr, given by
 *                     gsy_open_channel()
 *
 *                 GSY_UPPER_RQB_PTR_TYPE     upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
#ifndef GSY_REQUEST_UPPER_DONE
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR    GSY_REQUEST_UPPER_DONE(
    GSY_UPPER_CALLBACK_FCT_PTR_TYPE  gsy_request_upper_done_ptr,
    GSY_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                 sys_ptr
);
#endif

#ifdef GSY_DO_TIMEOUT_EXTERN
/*=============================================================================
 * function name:  GSY_DO_TIMER_REQUEST     
 *
 * function:       Sends the spezified RQB to gsy_request(). This
 *                 function is called within DCP timer handler to signal
 *                 a Timer event to the main context of GSY.  
 *                 Systemadaption has to call gsy_request() with the specified
 *                 RQB. Before calling, the systemadaption should change to 
 *                 taskstate. 
 *
 * parameters:     GSY_UPPER_RQB_PTR_TYPE   pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_DO_TIMER_REQUEST
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_DO_TIMER_REQUEST(
    GSY_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

/*=============================================================================
 * function name:  GSY_ALLOC_UPPER_RQB_LOCAL
 *
 * function:       allocate an upper RQB for local use 
 *
 * parameters:     GSY_UPPER_RQB_PTR_TYPE  ...  *  upper_rqb_ptr_ptr:
 *                                   return value: pointer to local memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory 
 *                                                 available
 *                 LSA_UINT16                      length:   length of RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_ALLOC_UPPER_RQB_LOCAL
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_UPPER_RQB_LOCAL(
    GSY_UPPER_RQB_PTR_TYPE  GSY_LOCAL_MEM_ATTR *  upper_rqb_ptr_ptr,
    LSA_UINT16                                    length
);
#endif

/*=============================================================================
 * function name:  GSY_FREE_UPPER_RQB_LOCAL
 *
 * function:       free an upper RQB allocated with GSY_ALLOC_UPPER_RQB_LOCAL
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper RQB or
 *                 GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_FREE_UPPER_RQB_LOCAL
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_UPPER_RQB_LOCAL(
    LSA_UINT16                 GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    GSY_UPPER_RQB_PTR_TYPE                           upper_rqb_ptr
);
#endif
#endif /* GSY_DO_TIMEOUT_EXTERN */

/*=============================================================================
 * function name:  GSY_ALLOC_UPPER_RQB
 *
 * function:       allocate an upper RQB
 *
 * parameters:     GSY_UPPER_RQB_PTR_TYPE  ...  *  upper_rqb_ptr_ptr:
 *                                   return value: pointer to upper RQB pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 gsy_upper_rqb().
 *                 LSA_USER_ID_TYPE                   user_id:  id of GSY
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_ALLOC_UPPER_RQB
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_UPPER_RQB(
    GSY_UPPER_RQB_PTR_TYPE  GSY_LOCAL_MEM_ATTR *  upper_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_FREE_UPPER_RQB
 *
 * function:       free an upper RQB
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper RQB or
 *                                                      wrong system pointer
 *                 GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_FREE_UPPER_RQB
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_UPPER_RQB(
    LSA_UINT16                 GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    GSY_UPPER_RQB_PTR_TYPE                           upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_ALLOC_UPPER_MEM
 *
 * function:       allocate an upper memory
 *
 * parameters:     GSY_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to upper memory-
 *                                                 pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 gsy_upper_mem().
 *                 LSA_USER_ID_TYPE                   user_id:  id of GSY
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_ALLOC_UPPER_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_UPPER_MEM(
    GSY_UPPER_MEM_PTR_TYPE  GSY_LOCAL_MEM_ATTR *  upper_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_FREE_UPPER_MEM
 *
 * function:       free an upper memory
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper memory or
 *                                                      wrong system pointer
 *                 GSY_UPPER_MEM_PTR_TYPE  upper_mem_ptr:  pointer to upper-
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_FREE_UPPER_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_UPPER_MEM(
    LSA_UINT16                 GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    GSY_UPPER_MEM_PTR_TYPE                           upper_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_SYNC_GET
 *
 * function:       read PTCP time and reference timestamp
 *
 *
 * parameters:     pTimeEpoch    for return of time epoch number
 *                 pSeconds                    time seconds
 *                 pNanoSeconds        "       time nanoseconds (max.999.999.999) 
 *                 pTimestamp          "       reference time stamp
 *                                             = act. value of the send timestamp
 *                                               counter of the system channels
 *                                               device
 *                 SyncId        0=clock, 1=time...
 *                 sys_ptr       of user channel: for channel identification
 *  
 *
 * return value:   LSA_RET_OK or LSA_RET_ERR_PARAM or...
 *===========================================================================*/
#ifndef GSY_SYNC_GET
LSA_UINT16  GSY_SYSTEM_OUT_FCT_ATTR  GSY_SYNC_GET(
	LSA_UINT16	GSY_LOCAL_MEM_ATTR *	pTimeEpoch,
    LSA_UINT32  GSY_LOCAL_MEM_ATTR *	pSeconds,
    LSA_UINT32  GSY_LOCAL_MEM_ATTR *	pNanoSeconds,
    LSA_UINT32  GSY_LOCAL_MEM_ATTR *	pTimeStamp,
	LSA_UINT8							SyncId,
	LSA_SYS_PTR_TYPE					sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_SYNC_GET_EXT_PLL_IN
 *
 * function:       read PTCP time and reference timestamp
 *
 *
 * parameters:     pTimestampPLL for return of time stamp of last PLL_IN signal
 *                 pSeconds                    time seconds
 *                 pNanoSeconds        "       time nanoseconds (max.999.999.999) 
 *                 pTimestamp          "       reference time stamp
 *                                             = act. value of the send timestamp
 *                                               counter of the system channels
 *                                               device
 *                 SyncId        0=clock, 1=time...
 *                 sys_ptr       of user channel: for channel identification
 *  
 *
 * return value:   LSA_RET_OK or LSA_RET_ERR_PARAM or...
 *===========================================================================*/
#ifndef GSY_SYNC_GET_EXT_PLL_IN
LSA_UINT16  GSY_SYSTEM_OUT_FCT_ATTR  GSY_SYNC_GET_EXT_PLL_IN(
    LSA_UINT32  GSY_LOCAL_MEM_ATTR *	pTimeStampPLL,
    LSA_UINT32  GSY_LOCAL_MEM_ATTR *	pSeconds,
    LSA_UINT32  GSY_LOCAL_MEM_ATTR *	pNanoSeconds,
    LSA_UINT32  GSY_LOCAL_MEM_ATTR *	pTimeStamp,
	LSA_UINT8							SyncId,
	LSA_SYS_PTR_TYPE					sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_SYNC_SET
 *
 * function:       set PTCP time with reference timestamp
 *
 *
 * parameters:     EpochNumber   time epoch number
 *                 Seconds       time seconds
 *                 NanoSeconds   time nanoseconds (max.999.999.999) 
 *                 Timestamp     reference time stamp
 *                               = value of the recv timestamp
 *                                 counter of the system channels device
 *                 RcfInterval   Drift interval of lokal time to master time in ns
 *                               (to be corrected by +/- one nanosecond, 0=unkown)
 *                                +100 ==> 101, -100 ==> 99       
 *                                        
 *                 SyncId        0=clock, 1=time...
 *                 sys_ptr       of user channel: for channel identification
 *  
 *
 * return value:   LSA_RET_OK or LSA_RET_ERR_PARAM or...
 *===========================================================================*/
#ifndef GSY_SYNC_SET
LSA_UINT16  GSY_SYSTEM_OUT_FCT_ATTR  GSY_SYNC_SET(
    LSA_UINT16							EpochNumber,
    LSA_UINT32							Seconds,
    LSA_UINT32							NanoSeconds,
    LSA_UINT32							TimeStamp,
    LSA_INT32							RcfInterval,
	LSA_UINT8							SyncId,
	LSA_SYS_PTR_TYPE					sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_SYNC_ADJUST
 *
 * function:       adjust clock for PTCP time
 *
 *
 * parameters:     AdjustInterval    correction interval in nanoseconds 
 *                                   (to be corrected by +/- one nanosecond)
 *                                   +100 ==> 101, -100 ==> 99
 *                 SyncId        0=clock, 1=time...
 *                 sys_ptr       of user channel: for channel identification
 *
 * return value:   LSA_RET_OK or LSA_RET_ERR_PARAM or...
 *===========================================================================*/
#ifndef GSY_SYNC_ADJUST
LSA_UINT16  GSY_SYSTEM_OUT_FCT_ATTR  GSY_SYNC_ADJUST(
    LSA_INT32							AdjustInterval,
	LSA_UINT8							SyncId,
	LSA_SYS_PTR_TYPE					sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_RATE_SET
 *
 * function:       report actual Value of master rate
 *
 *
 * parameters:     RateInterval  Rate interval of lokal time to master time in ns
 *                               (to be corrected by +/- one nanosecond, 0=unkown)
 *                                +100 ==> 101, -100 ==> 99
 *                 RateValid     RateInterval value is valid or not
 *                 SyncId        0=clock, 1=time...
 *                 sys_ptr       of user channel: for channel identification
 *  
 *
 * return value:   LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
#ifndef GSY_RATE_SET
LSA_UINT16  GSY_SYSTEM_OUT_FCT_ATTR  GSY_RATE_SET(
	LSA_INT32							RateInterval,
    LSA_UINT8							RateValid,
	LSA_UINT8							SyncId,
	LSA_SYS_PTR_TYPE					sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_GET_TIMESTAMP
 *
 * function:       get timestamp from ERTEC register
 *
 *
 * parameters:     -
 *                 sys_ptr       of system channel: for channel identification
 *  
 *
 * return value:   timestamp
 *===========================================================================*/
#ifndef GSY_GET_TIMESTAMP
LSA_UINT32  GSY_SYSTEM_OUT_FCT_ATTR  GSY_GET_TIMESTAMP(                
	LSA_SYS_PTR_TYPE					sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_GET_EXT_PLL_OFFSET
 *
 * function:       get offset of local clock from EXT_PLL_SIGNAL
 *
 *
 * parameters:     -
 *                 SyncId        0=clock, 1=time...
 *                 sys_ptr       of user channel: for channel identification
 *  
 *
 * return value:   current offset from ext. PLL signal in +-nanoseconds
 *                 (< 0: local clock is too fast)
 *===========================================================================*/
#ifndef GSY_GET_EXT_PLL_OFFSET
LSA_INT32  GSY_SYSTEM_OUT_FCT_ATTR  GSY_GET_EXT_PLL_OFFSET(                
	LSA_UINT8							SyncId,
	LSA_SYS_PTR_TYPE					sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_GET_SENDCLOCK
 *
 * function:       get sendclock in ticks
 *
 *
 * parameters:     -
 *                 sys_ptr       of system channel: for interface identification
 *  
 *
 * return value:   sendclock in ticks
 *
 *===========================================================================*/
#ifndef GSY_GET_SENDCLOCK
LSA_UINT32  GSY_SYSTEM_OUT_FCT_ATTR	GSY_GET_SENDCLOCK(                
	LSA_SYS_PTR_TYPE					sys_ptr
);
#endif

/*====  LSA out functions  =====*/

/*=============================================================================
 * function name:  GSY_INIT_SYS_PTR
 *
 * function:       initialize system pointer
 *
 *                 Attention:
 *                 The LSA component only may call this macro when the 
 *                 component hasn't called GSY_GET_PATH_INFO() yet.
 *                 The real value assignment then must be made by the call of
 *                 GSY_GET_PATH_INFO().
 *
 * parameters:     LSA_SYS_PTR_TYPE  ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_INIT_SYS_PTR
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_INIT_SYS_PTR(
    LSA_SYS_PTR_TYPE  GSY_LOCAL_MEM_ATTR *  sys_ptr_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_SYS_PTR_IS_INITIALIZED
 *
 * function:       Is the system pointer initialized
 *                 (by GSY_INIT_SYS_PTR())?
 *
 * parameters:     LSA_UINT8  ...  *  ret_val_ptr:
 *                                        return value: 1   yes
 *                                                      0  no
 *                 LSA_SYS_PTR_TYPE  sys_ptr:   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_SYS_PTR_IS_INITIALIZED
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_SYS_PTR_IS_INITIALIZED(
    LSA_UINT8         GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE                        sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_GET_PATH_INFO
 *
 * function:       get system pointer and detail pointer of a communication 
 *                 channel
 *
 * parameters:     LSA_UINT16              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system pointer
 *                 GSY_DETAIL_PTR_TYPE  ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel  
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_GET_PATH_INFO
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_GET_PATH_INFO(
    LSA_UINT16              GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE        GSY_LOCAL_MEM_ATTR *  sys_ptr_ptr,
    GSY_DETAIL_PTR_TYPE  	GSY_LOCAL_MEM_ATTR *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                             path
);
#endif

/*=============================================================================
 * function name:  GSY_RELEASE_PATH_INFO
 *
 * function:       release system pointer and detail pointer of a communication 
 *                 channel
 *
 * parameters:     LSA_UINT16      ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no release because of
 *                                                      wrong system pointer or
 *                                                      wrong detail pointer
 *                 LSA_SYS_PTR_TYPE        sys_ptr:     system pointer
 *                 GSY_DETAIL_PTR_TYPE  detail_ptr:  detail pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_RELEASE_PATH_INFO
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_RELEASE_PATH_INFO(
    LSA_UINT16              GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr,
    GSY_DETAIL_PTR_TYPE		detail_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local memory
 *
 * parameters:     GSY_LOCAL_MEM_PTR_TYPE  ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory 
 *                                                 available
 *                 LSA_UINT16                         length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_ALLOC_LOCAL_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_LOCAL_MEM(
    GSY_LOCAL_MEM_PTR_TYPE  GSY_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
    LSA_UINT16                                    length
);
#endif

/*=============================================================================
 * function name:  GSY_FREE_LOCAL_MEM
 *
 * function:       free a local memory
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local memory
 *                 GSY_LOCAL_MEM_PTR_TYPE  local_mem_ptr:  pointer to local-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_FREE_LOCAL_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_LOCAL_MEM(
    LSA_UINT16                 GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    GSY_LOCAL_MEM_PTR_TYPE                           local_mem_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_MEMSET_LOCAL
 *
 * function:       fills memory with value
 *
 * parameters:      GSY_LOCAL_MEM_PTR_TYPE         pMem
 *                  LSA_UINT8                      Value
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_MEMSET_LOCAL
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_MEMSET_LOCAL(
    GSY_LOCAL_MEM_PTR_TYPE              pMem,                       
    LSA_UINT8                           Value,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  GSY_MEMSET_UPPER
 *
 * function:       fills memory with value
 *
 * parameters:      GSY_UPPER_MEM_PTR_TYPE         pMem
 *                  LSA_UINT8                      Value
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_MEMSET_LOCAL
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_MEMSET_UPPER(
    GSY_UPPER_MEM_PTR_TYPE              pMem,                       
    LSA_UINT8                           Value,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  GSY_LOWER_TO_UPPER_MEM
 *
 * function:       convert lower memory to upper memory
 *
 * parameters:     GSY_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to upper memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible 
 *                 GSY_LOWER_MEM_PTR_TYPE          lower_mem_ptr: 
 *                                                 pointer to lower memory
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_LOWER_TO_UPPER_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_LOWER_TO_UPPER_MEM(
    GSY_UPPER_MEM_PTR_TYPE  GSY_LOCAL_MEM_ATTR *  upper_mem_ptr_ptr,
    GSY_LOWER_MEM_PTR_TYPE                        lower_mem_ptr,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_UPPER_TO_LOWER_MEM
 *
 * function:       convert upper memory to lower memory
 *
 * parameters:     GSY_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible 
 *                 GSY_UPPER_MEM_PTR_TYPE          upper_mem_ptr:  
 *                                                 pointer to upper memory
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system pointer
 *                                                              
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_UPPER_TO_LOWER_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_UPPER_TO_LOWER_MEM(
    GSY_LOWER_MEM_PTR_TYPE  GSY_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    GSY_UPPER_MEM_PTR_TYPE                        upper_mem_ptr,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_LOWER_TO_LOWER_MEM
 *
 * function:       convert lower memory to lower memory
 *
 * parameters:     GSY_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible 
 *                 GSY_LOWER_MEM_PTR_TYPE          lower_mem_ptr: 
 *                                                 pointer to lower memory
 *                 LSA_SYS_PTR_TYPE                   dst_sys_ptr:
 *                                                 system pointer of
 *                                                 destination memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_LOWER_TO_LOWER_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_LOWER_TO_LOWER_MEM(
    GSY_LOWER_MEM_PTR_TYPE  GSY_LOCAL_MEM_ATTR *  lower_mem_ptr_ptr,
    GSY_LOWER_MEM_PTR_TYPE                        lower_mem_ptr,
    LSA_SYS_PTR_TYPE                              dst_sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_COPY_COMMON_MEM
 *
 * function:       copy common memory
 *                 The memory areas of source and destination mustn't overlap.
 *
 * parameters:     LSA_VOID  GSY_COMMON_MEM_ATTR *  src_common_mem_ptr:
 *                                                 void pointer to source-
 *                                                 common memory
 *                 LSA_VOID  GSY_COMMON_MEM_ATTR *  dst_common_mem_ptr:
 *                                                 void pointer to destination-
 *                                                 common memory
 *                 LSA_UINT16                          length: number of bytes
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_COPY_COMMON_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_COPY_COMMON_MEM(
    LSA_VOID  GSY_COMMON_MEM_ATTR *  src_common_mem_ptr,
    LSA_VOID  GSY_COMMON_MEM_ATTR *  dst_common_mem_ptr,
    LSA_UINT16                       length
);
#endif

/*=============================================================================
 * function name:  GSY_COPY_LOCAL_TO_UPPER_MEM
 *
 * function:       copy local data to upper memory
 *
 * parameters:     GSY_LOCAL_MEM_PTR_TYPE	src_ptr:	pointer to local memory
 *                 GSY_UPPER_MEM_PTR_TYPE	dst_ptr:	pointer to upper memory
 *                 LSA_UINT16               length: 	number of bytes to copy
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_COPY_LOCAL_TO_UPPER_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR	GSY_COPY_LOCAL_TO_UPPER_MEM(
    GSY_LOCAL_MEM_PTR_TYPE		 	src_ptr,
    GSY_UPPER_MEM_PTR_TYPE			dst_ptr,
    LSA_UINT16              		length,
    LSA_SYS_PTR_TYPE				sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_COPY_UPPER_TO_LOCAL_MEM
 *
 * function:       copy local data to upper memory
 *
 * parameters:     GSY_UPPER_MEM_PTR_TYPE	src_ptr:	pointer to upper memory
 *                 GSY_LOCAL_MEM_PTR_TYPE	dst_ptr:	pointer to local memory
 *                 LSA_UINT16               length: 	number of bytes to copy
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_COPY_UPPER_TO_LOCAL_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR	GSY_COPY_UPPER_TO_LOCAL_MEM(
    GSY_UPPER_MEM_PTR_TYPE		 	src,
    GSY_LOCAL_MEM_PTR_TYPE			dst,
    LSA_UINT16              		len,
    LSA_SYS_PTR_TYPE				sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_INIT_TIMER_ID
 *
 * function:       initialize id of timer
 *
 *                 Attention:
 *                 The LSA component only may call this macro when the 
 *                 component hasn't called GSY_ALLOC_TIMER() yet.
 *                 The real value assignment then must be made by the call of
 *                 GSY_ALLOC_TIMER().
 *
 * parameters:     LSA_TIMER_ID_TYPE  ...  *  timer_id_ptr:
 *                                         return value: pointer to id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_INIT_TIMER_ID
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_INIT_TIMER_ID(
    LSA_TIMER_ID_TYPE  GSY_LOCAL_MEM_ATTR *  timer_id_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_TIMER_ID_IS_INITIALIZED
 *
 * function:       Is the id of timer initialized
 *                 (by GSY_INIT_TIMER_ID())?
 *
 * parameters:     LSA_UINT8  ...   *  ret_val_ptr:
 *                                        return value: 1   yes
 *                                                      0  no
 *                 LSA_TIMER_ID_TYPE  timer_id:   id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_TIMER_ID_IS_INITIALIZED
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_TIMER_ID_IS_INITIALIZED(
    LSA_UINT8          GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                        timer_id
);
#endif

/*=============================================================================
 * function name:  GSY_ALLOC_TIMER
 *
 * function:       allocate a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                           return value: LSA_RET_OK            timer has been
 *                                                               allocated 
 *                                         LSA_RET_ERR_NO_TIMER  no timer has 
 *                                                               been allocated 
 *                 LSA_TIMER_ID_TYPE  ...  *  timer_id_ptr:
 *                           return value: pointer to id of timer
 *                 LSA_UINT16          timer_type:  LSA_TIMER_TYPE_ONE_SHOT or
 *                                                  LSA_TIMER_TYPE_CYCLIC
 *                 LSA_UINT16          time_base:   LSA_TIME_BASE_1US,
 *                                                  LSA_TIME_BASE_10US,
 *                                                  LSA_TIME_BASE_100US,
 *                                                  LSA_TIME_BASE_1MS,
 *                                                  LSA_TIME_BASE_10MS,
 *                                                  LSA_TIME_BASE_100MS,
 *                                                  LSA_TIME_BASE_1S,
 *                                                  LSA_TIME_BASE_10S or
 *                                                  LSA_TIME_BASE_100S
 *                 LSA_SYS_PTR_TYPE    sys_ptr:     system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_ALLOC_TIMER
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_TIMER(
    LSA_UINT16         GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE  GSY_LOCAL_MEM_ATTR *  timer_id_ptr,
    LSA_UINT16                               timer_type,
    LSA_UINT16                               time_base/*,
    LSA_SYS_PTR_TYPE                         sys_ptr*/
);
#endif

/*=============================================================================
 * function name:  GSY_START_TIMER
 *
 * function:       start a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                  timer has been
 *                                                             started
 *                                 LSA_RET_OK_TIMER_RESTARTED  timer has been
 *                                                             restarted
 *                                 LSA_RET_ERR_PARAM           timer hasn't
 *                                                             been started
 *                                                             because of wrong
 *                                                             timer-id
 *                                 After the expiration of the running time 
 *                                 system will call gsy_timeout().
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *                 LSA_USER_ID_TYPE    user_id:   id of GSY
 *                 LSA_UINT16          time:      running time
 *                 LSA_SYS_PTR_TYPE    sys_ptr:   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_START_TIMER
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_START_TIMER(
    LSA_UINT16        GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                       timer_id,
    LSA_USER_ID_TYPE                        user_id,
    LSA_UINT16                              time/*,
    LSA_SYS_PTR_TYPE                        sys_ptr*/
);
#endif

/*=============================================================================
 * function name:  GSY_STOP_TIMER
 *
 * function:       stop a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value of LSA_TIMER_TYPE_ONE_SHOT-timer:
 *                                 LSA_RET_OK                    timer has been
 *                                                               stopped:
 *                                    ===> gsy_timeout() will not be called.
 *                                 LSA_RET_OK_TIMER_NOT_RUNNING  timer was not
 *                                                               running; timer
 *                                                               is stopped:
 *                                        ===> gsy_timeout() will be called.
 *                                 LSA_RET_ERR_PARAM             timer hasn't
 *                                                               stopped
 *                                                               because
 *                                                               of wrong 
 *                                                               timer-id
 *                   return value of LSA_TIMER_TYPE_CYCLIC-timer:
 *                                 LSA_RET_OK                    timer has been
 *                                                               stopped:
 *                                  ===> gsy_timeout() may be called or not.
 *                                 LSA_RET_ERR_PARAM             timer hasn't
 *                                                               stopped
 *                                                               because
 *                                                               of wrong 
 *                                                               timer-id
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *                 LSA_SYS_PTR_TYPE    sys_ptr:   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_STOP_TIMER
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_STOP_TIMER(
    LSA_UINT16         GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                        timer_id/*,
    LSA_SYS_PTR_TYPE                         sys_ptr*/
);
#endif

/*=============================================================================
 * function name:  GSY_FREE_TIMER
 *
 * function:       free a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                return value: LSA_RET_OK                    timer has been
 *                                                            deallocated
 *                              LSA_RET_ERR_TIMER_IS_RUNNING  because timer is
 *                                                            running timer has
 *                                                            not been
 *                                                            deallocated
 *                              LSA_RET_ERR_PARAM             no deallocation
 *                                                            because of wrong
 *                                                            timer-id
 *                 LSA_TIMER_ID_TYPE   timer_id:  id of timer
 *                 LSA_SYS_PTR_TYPE    sys_ptr:   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_FREE_TIMER
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_TIMER(
    LSA_UINT16  GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                 timer_id/*,
    LSA_SYS_PTR_TYPE                  sys_ptr*/
);
#endif

/*=============================================================================
 * function name:  GSY_ENTER
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_ENTER
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ENTER(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  GSY_EXIT
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_EXIT
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_EXIT(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  GSY_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * parameters:     LSA_UINT16                   length:     number of bytes
 *                 GSY_FATAL_ERROR_PTR_TYPE  error_ptr:  pointer to
 *                                                          LSA_FATAL_ERROR
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_FATAL_ERROR
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FATAL_ERROR(
    LSA_UINT16                  length,
    GSY_FATAL_ERROR_PTR_TYPE  	error_ptr
);
#endif



/*****************************************************************************/
/*  end of file GSY_SYS.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of GSY_SYS_H */

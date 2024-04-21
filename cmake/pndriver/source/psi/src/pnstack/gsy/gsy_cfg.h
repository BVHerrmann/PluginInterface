
#ifndef GSY_CFG_H                        /* ----- reinclude-protection ----- */
#define GSY_CFG_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: gsy_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                      Frame for file "gsy_cfg.h".                          */
/*                      ==============================                       */
/*                                                                           */
/*  Configuration for GSY:                                                   */
/*  Defines constants, types and macros for GSY.                             */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */ 
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                  Who  What                           */
/*  2004-11-29  P01.00.00.00_00.01.00.00 lrg  from LSA_GLOB P2.1.0.0_0.4.1.1 */
/*  2004-04-04  P01.00.00.00_00.01.00.01 lrg  GSY_LOWER_RQB_RECV_ID_GET,     */
/*              GSY_LOWER_RQB_SEND/RECV_TIME_GET/SET,                        */
/*              GSY_LOWER_RQB_PARAM_NANOSECONDS_GET                          */
/*  2005-06-02  P03.06.00.00_00.03.00.01  lrg  Prototype for PT2             */
/*  2005-06-24  P03.06.00.00_00.03.02.02  lrg  neues Sync-Userinterface      */
/*  2005-07-26  P03.07.00.00_00.04.01.01  lrg  GSY_DRIFT_EXTERNAL            */
/*  2005-08-08  P03.07.00.00_00.08.01.01  js                                 */
/*              GSY_CFG_FWD_ERTEC_200_DRIFT_CORRECTION                       */
/*  2005-10-18  P03.0x.00.00_00.0x.0x.0x  js                                 */
/*              GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL changed from 2 to 4     */
/*  2005-10-28  P03.08.00.00_00.01.01.01  lrg  neu: GSY_LINK_MODE_FULL       */
/*  2005-12-06  P03.08.00.00_00.01.02.01 lrg GSY_DRIFT_EXTERNAL removed      */
/*              new: 64-Bit calculation makros. Upper RQB macros moved to    */
/*              gsy_usr.h.                                                   */
/*  2005-12-22  P03.09.00.00_00.01.01.02 lrg  Upper RQB header macros moved  */
/*              back here from gsy_usr.h                                     */
/*  2006-03-31  P03.10.00.00_00.02.01.02 lrg  GSY_DEL_TRACE_11(..., para11_) */
/*  2006-04-27  P03.11.00.00_00.01.01.04 lrg  Upper RQB macros for mandatory */
/*              elements moved back here from gsy_usr.h (ArtsRQ 00291071)    */
/*              GSY_SYNC_SET() wird wegdefiniert, wenn nur Taktsynchroni-    */
/*              sation unterstützt wird.                                     */
/*  2006-05-12  P03.11.00.00_00.03.01.02 lrg #ifndef GSY_TEST_MODE_FULL      */
/*  2006-05-15  P03.11.00.00_00.03.01.03 lrg GSY_LOWER_RQB_PORT_MACADDR_GET()*/
/*  2006-05-30  P03.11.00.00_00.04.01.02 lrg GSY_GET_EXT_PLL_OFFSET          */
/*  2006-06-13  P03.12.00.00_00.01.01.02 130606lrg003: GSY_GET_SENDCLOCK()   */
/*  2006-06-27  P03.12.00.00_00.02.01.02 LSA_RQB_HEADER/TRAILER, LSA_UINT64  */
/*              gemaess LSA_GLOB_P02.01.00.00_00.07.01.01                    */
/*              neu: GSY_LOWER_RQB_CYCLE_RCF_INTERVAL_SET(). GSY_INT64       */
/*              neu: GSY_FILE_SYSTEM_EXTENSION()                             */
/*  2006-07-24  P03.12.00.00_00.25.01.02 lrg GSY_EDD_SET_CYCLE,              */
/*              GSY_SYNC_SET() um RCF-Intervall erweitert.                   */
/*  2006-07-27  P03.12.00.00_00.25.02.02 lrg GSY_RT_CLASS3_SUPPORT_ON        */
/*  2006-08-14  P03.11.00.00_00.11.01.01 js USERID access macros added       */
/*  2006-08-31  P03.12.00.00_00.25.03.02 lrg GSY_ERROR_TRACE_00/02 added     */
/*  2006-09-29  P03.12.00.00_00.25.04.02 lrg GSY_LOWER_RQB_RTC2_PARAMS_ and  */
/*              GSY_MASTER_TRACE_ and GSY_SYNC_DEBUG_DIAG                    */
/*  2006-10-04  P03.12.00.00_00.30.01.02 lrg ArtsRQ 371299: GSY_SR_TRACE -   */
/*              -> GSY_SNDRCV_TRACE, GSY_LOWER_RQB_RTC2_PARAMS_RESENDMAX_GET */
/*  2006-10-18  P03.12.00.00_00.30.02.02 lrg neu: GSY_LOWER_RSP_ERR_SEQUENCE */
/*  2006-10-23  P03.12.00.00_00.30.03.02 lrg GSY_PUT_INFO_1                  */
/*  2006-11-09  P03.12.00.00_00.30.04.02 lrg GSY_DELAY_DEBUG_DIAG            */
/*  2006-11-20  P03.12.00.00_00.40.01.02 lrg: Neue Zugriffsmakros            */
/*              GSY_LOWER_RQB_DIAG_SUBDOMAIN...                              */
/*  2006-11-30  P03.12.00.00_00.40.03.02 301106lrg004: Das Verwenden aller   */
/*              Driftwerte mit #define GSY_CHECK_RCF abschaltbar machen      */ 
/*  2007-01-10  P03.12.00.00_00.40.06.02 lrg GSY_CHECK_RCF entfernt          */
/*              100107lrg002: Arts-RQ AP00401007 EDD_NRT_SEND_PRIO_ORG       */
/*  2007-01-10  P03.12.00.00_00.40.07.02 lrg GSY_PRM_TRACE                   */
/*  2007-04-26  P03.12.00.00_00.40.11.02 lrg GSY_MAC_ADR_LENGTH              */
/*  2007-07-16  P04.00.00.00_00.30.01.00 lrg GSY_RT_CLASS3_SUPPORT_* removed */
/*  2007-08-29  P04.00.00.00_00.30.04.02 290807lrg001: MasterStartupTime     */
/*  2007-09-28  P04.00.00.00_00.30.06.02 GSY_SYNC_DATA_MASTER_STARTUP removed*/
/*              new: GSY_IGNORE_TOPO_NOT_OK                                  */
/*  2007-11-22  P04.00.00.00_00.60.01.02 GSY_IGNORE_TOPO_NOT_OK removed      */
/*              Macros for upper RQB header access set to LSA macros if      */
/*              those ar defined (in lsa_cfg.h e.g. LSA_RQB_GET_HANDLE).     */
/*              Macros for lower RQB UserId.ptr access set to EDD macros     */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-10-13  P04.01.00.00_00.01.03.02 131008lrg001: Casts for C++         */
/*  2008-10-17  P04.01.00.00_00.02.02.02 GSY_HW_TYPE statt EDD_HW_TYPE_USED  */
/*  2008-11-28  P04.01.00.00_00.02.02.03 KRISC32-Unterstuetzung              */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2008-12-08  P04.01.00.00_00.02.03.02 lrg: undef GSY_SYNC_ONLY_IN_KRISC32 */
/*  2009-01-08  P04.01.00.00_00.02.03.03 lrg: GSY_LOCAL_FCT_ATTR             */
/*  2009-01-26  P04.01.00.00_00.02.03.04 lrg: TxFrmGroup, DoCheck            */
/*  2009-01-28  P04.01.00.00_00.02.03.05 lrg: GSY_LOWER_RQB_KRISC32_PUT_TYPE */
/*  2009-04-23  P04.01.00.00_00.03.04.02 lrg: EDD_SRV_PTCP_xxx               */
/*              240409lrg001: PortRx/Tx-Delays nur noch aus ExtLinkInd holen */
/*  2009-10-08  P04.02.00.00_00.01.04.02 lrg:                                */
/*              EDD_HW_TYPE_FEATURE_SYNC_SLAVE_MSK...                        */
/*  2009-11-13  P05.00.00.00_00.01.01.02 lrg: GSY_LOWER_SLAVE/MASTER_UPDATE  */
/*  2010-03-23  P05.00.00.00_00.01.04.02 lrg: removed: GSY_UUID_*            */
/*              Arts-RQ AP00933526: Support only SyncId CLOCK and TIME       */
/*  2010-04-15  P05.00.00.00_00.01.04.03 150410lrg001:                       */
/*              GSY_LOWER_RQB_SEND_PRIO_SET(), GSY_SEND_PRIO_ANNO            */
/*  2010-07-20  P05.00.00.00_00.01.06.02 lrg: AP00999032 _REQUEST_UPPER_DONE */
/*              AP00999110 GSY_CFG_TRACE_MODE, private trace removed         */
/*  2010-09-24  P05.00.00.00_00.01.08.02 lrg: new edd_usr.h:                 */
/*              Tx/RxDelay --> PortTx/RxDelay                                */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2011-08-04  T05.00.00.00_00.05.44.02 040811lrg: AP01205450:              */
/*              Struktur err aus RQB entfernen                               */
/*  2011-09-23  P05.00.00.00_00.05.56.02 AP01191740 GSY_EDD_SET_CYCLE removed*/
/*  2012-11-07  P05.02.00.00_00.04.08.01 AP01447671 GSY_MAC_ADR_TYPE moved   */
/*              from gsy_cfg.h/txt to gsy_pck.h/txt                          */
/*              GSY_LOWER_RQB_..  moved from gsy_cfg.h/txt to gsy_low.h      */
/*  2015-06-09  V06.00, Inc05, Feature 1227823, Task 1311090                 */
/*              only one user channel for clock or time sync                 */
/*              GSY_MAX_USR_CHANNELS adapted (in psi_cfg.h)                  */
/*  2015-06-09  V06.00, Inc07, Feature 1227823, Task 1261424                 */
/*              GSY und CLP Konfigurationsschalter bereinigen                */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

void gsy_psi_startstop( int start );

/* check if condition holds or call fatal (programming error) */
#if PSI_DEBUG
#define GSY_ASSERT(cond_) { if (!(cond_)) { PSI_FATAL_COMP( LSA_COMP_ID_GSY, GSY_MODULE_ID,0 ) } }
#else
#define GSY_ASSERT(cond_) { /* empty-block in release version */ }
#endif

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

/* Set GSY_CFG_SYNC_ONLY_IN_KRISC32 if GSY only runs on ERTEC200+ system channels

#define GSY_CFG_SYNC_ONLY_IN_KRISC32
*/

#if (PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDS == 1)
#undef GSY_CFG_SYNC_ONLY_IN_KRISC32 /*pcIOX: no KRISC*/ 
#else
#define GSY_CFG_SYNC_ONLY_IN_KRISC32 
#endif

/*=============================================================================
 *       set local_mem static or dynamic:
 * static (no use of GSY_ALLOC_LOCAL_MEM):                
#define GSY_LOCAL_MEM_STATIC 
 * or dynamic (use of GSY_ALLOC_LOCAL_MEM):
#define GSY_LOCAL_MEM_DYNAMIC
 *===========================================================================*/
#define GSY_LOCAL_MEM_STATIC 

/* If GSY_DO_TIMEOUT_EXTERN is defined, gsy_timeout() only sends a timer RQB
 * using the output macro GSY_DO_TIMER_REQUEST(). The real work is done when
 * the timer RQB arrives at gsy_request(). 
 *
#define GSY_DO_TIMEOUT_EXTERN
*/
#define GSY_DO_TIMEOUT_EXTERN 

/* Little Endian is used if GSY_BIG_ENDIAN is not defined
*/
#ifdef LSA_HOST_ENDIANESS_BIG 
#define GSY_BIG_ENDIAN 
#endif


/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/

#define GSY_ATTR_NEAR
#define GSY_ATTR_FAR
#define GSY_ATTR_SHUGE
#define GSY_ATTR_HUGE

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*=====  code attributes  =====*/
#define GSY_UPPER_IN_FCT_ATTR    GSY_ATTR_HUGE        /* GSY-user to GSY */
#define GSY_UPPER_OUT_FCT_ATTR   GSY_ATTR_HUGE        /* GSY to GSY-user */
#define GSY_LOWER_IN_FCT_ATTR    GSY_ATTR_HUGE        /* lower component to GSY */
#define GSY_LOWER_OUT_FCT_ATTR   GSY_ATTR_HUGE        /* GSY to lower component */
#define GSY_SYSTEM_IN_FCT_ATTR   GSY_ATTR_FAR         /* system to GSY */
#define GSY_SYSTEM_OUT_FCT_ATTR  GSY_ATTR_FAR         /* GSY to system */
#define GSY_LOCAL_FCT_ATTR       GSY_ATTR_NEAR        /* local function */

/*=====  data attributes  =====*/
#define GSY_UPPER_RQB_ATTR       GSY_ATTR_HUGE        /* upper-rqb data */
#define GSY_UPPER_MEM_ATTR       GSY_ATTR_NEAR        /* upper-memory data */
#define GSY_LOWER_RQB_ATTR       EDD_UPPER_RQB_ATTR   /* lower-rqb data */
#define GSY_LOWER_MEM_ATTR       EDD_UPPER_MEM_ATTR   /* lower-memory data */
#define GSY_LOWER_TXMEM_ATTR     EDD_UPPER_MEM_ATTR   /* lower-memory data */
#define GSY_LOWER_RXMEM_ATTR     EDD_UPPER_MEM_ATTR   /* lower-memory data */
#define GSY_SYSTEM_MEM_ATTR      GSY_ATTR_HUGE        /* system-memory data */
#define GSY_LOCAL_MEM_ATTR       GSY_ATTR_SHUGE       /* local data */
#define GSY_COMMON_MEM_ATTR      GSY_ATTR_SHUGE       /* greatest common */
                                                      /* data attribute  */

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*=============================================================================
 *      GSY request block (== upper request block)
 *===========================================================================*/

/*====  Definition of PREFIX-RQB-HEADER:  =====*/

#define GSY_RQB_HEADER            LSA_RQB_HEADER(GSY_UPPER_RQB_PTR_TYPE)

/*====  Definition of PREFIX-RQB-TRAILER:  =====*/
/* If necessary during system integration, additional parameters can be 
 * added!
 */
#define GSY_RQB_TRAILER           LSA_RQB_TRAILER

/*---------------------------------------------------------------------------*
 * Access to upper RQB header
*/
#ifdef  LSA_RQB_GET_HANDLE

#define GSY_UPPER_RQB_HANDLE_GET(_pRbu)						LSA_RQB_GET_HANDLE(_pRbu)
#define GSY_UPPER_RQB_HANDLE_SET(_pRbu, _Val)				LSA_RQB_SET_HANDLE(_pRbu, _Val)
#define GSY_UPPER_RQB_OPCODE_GET(_pRbu)						LSA_RQB_GET_OPCODE(_pRbu)
#define GSY_UPPER_RQB_OPCODE_SET(_pRbu, _Val)				LSA_RQB_SET_OPCODE(_pRbu, _Val)
#define GSY_UPPER_RQB_RESPONSE_GET(_pRbu)					LSA_RQB_GET_RESPONSE(_pRbu)
#define GSY_UPPER_RQB_RESPONSE_SET(_pRbu, _Val)				LSA_RQB_SET_RESPONSE(_pRbu, _Val)
#define GSY_UPPER_RQB_NEXT_GET(_pRbu)						LSA_RQB_GET_NEXT_RQB_PTR(_pRbu)
#define GSY_UPPER_RQB_NEXT_SET(_pRbu, _Val)					LSA_RQB_SET_NEXT_RQB_PTR(_pRbu, _Val)
#define GSY_UPPER_RQB_PREV_GET(_pRbu)						LSA_RQB_GET_PREV_RQB_PTR(_pRbu)
#define GSY_UPPER_RQB_PREV_SET(_pRbu, _Val)					LSA_RQB_SET_PREV_RQB_PTR(_pRbu, _Val)
#define GSY_UPPER_RQB_COMPID_SET(_pRbu, _Val)				LSA_RQB_SET_COMP_ID(_pRbu, _Val)
#define GSY_UPPER_RQB_USERID_PTR_GET(_pRbu)					LSA_RQB_GET_USER_ID_PTR(_pRbu)
#define GSY_UPPER_RQB_USERID_PTR_SET(_pRbu, _Val)			LSA_RQB_SET_USER_ID_PTR(_pRbu, _Val)
#define GSY_UPPER_RQB_USER32_GET(_pRbu)						LSA_RQB_GET_USER_ID_UVAR32(_pRbu)
#define GSY_UPPER_RQB_USER32_SET(_pRbu, _Val)				LSA_RQB_SET_USER_ID_UVAR32(_pRbu, _Val)
#else
#define GSY_UPPER_RQB_HANDLE_GET(_pRbu)  					((_pRbu)->handle)
#define GSY_UPPER_RQB_HANDLE_SET(_pRbu, _Val)  				((_pRbu)->handle = (_Val))
#define GSY_UPPER_RQB_OPCODE_GET(_pRbu)  					((_pRbu)->opcode)
#define GSY_UPPER_RQB_OPCODE_SET(_pRbu, _Val)				((_pRbu)->opcode = (_Val))
#define GSY_UPPER_RQB_RESPONSE_GET(_pRbu)		  			((_pRbu)->response)
#define GSY_UPPER_RQB_RESPONSE_SET(_pRbu, _Val)  			((_pRbu)->response = (_Val))
#define GSY_UPPER_RQB_NEXT_GET(_pRbu)  						((_pRbu)->next_rqb_ptr)
#define GSY_UPPER_RQB_NEXT_SET(_pRbu, _Val)  				((_pRbu)->next_rqb_ptr = (_Val))
#define GSY_UPPER_RQB_PREV_GET(_pRbu)  						((_pRbu)->prev_rqb_ptr)
#define GSY_UPPER_RQB_PREV_SET(_pRbu, _Val)  				((_pRbu)->prev_rqb_ptr = (_Val))
#define GSY_UPPER_RQB_COMPID_SET(_pRbu, _Val)				((_pRbu)->comp_id = (_Val))
#define GSY_UPPER_RQB_USERID_PTR_GET(_pRbu)					((_pRbu)->user_id.void_ptr)
#define GSY_UPPER_RQB_USERID_PTR_SET(_pRbu, _Val)			((_pRbu)->user_id.void_ptr = (_Val))
#define GSY_UPPER_RQB_USER32_GET(_pRbu)						((_pRbu)->user_id.uvar32)
#define GSY_UPPER_RQB_USER32_SET(_pRbu, _Val)				((_pRbu)->user_id.uvar32 = (_Val))
#endif

/*---------------------------------------------------------------------------*
 * Access to upper RQB
*/
/* 040811lrg
#define GSY_UPPER_RQB_ERR_COMPID_GET(_pRbu)					((_pRbu)->err.lsa_component_id)
#define GSY_UPPER_RQB_ERR_COMPID_SET(_pRbu, _Val)			((_pRbu)->err.lsa_component_id = (_Val))
*/
#define GSY_UPPER_RQB_ERR_COMPID_GET(_pRbu)					(LSA_COMP_ID_UNUSED)
#define GSY_UPPER_RQB_ERR_COMPID_SET(_pRbu, _Val)
#define GSY_UPPER_RQB_SYSPATH_GET(_pRbu)					((_pRbu)->args.channel.sys_path)
#define GSY_UPPER_RQB_SYSPATH_SET(_pRbu, _Val)				((_pRbu)->args.channel.sys_path = (_Val))
#define GSY_UPPER_RQB_HANDLE_LOWER_GET(_pRbu)				((_pRbu)->args.channel.handle)
#define GSY_UPPER_RQB_HANDLE_LOWER_SET(_pRbu, _Val)			((_pRbu)->args.channel.handle = (_Val))
#define GSY_UPPER_RQB_HANDLE_UPPER_GET(_pRbu)				((_pRbu)->args.channel.handle_upper)
#define GSY_UPPER_RQB_HANDLE_UPPER_SET(_pRbu, _Val)			((_pRbu)->args.channel.handle_upper = (_Val))
#define GSY_UPPER_RQB_CBF_GET(_pRbu)						((_pRbu)->args.channel.gsy_request_upper_done_ptr)
#define GSY_UPPER_RQB_CBF_SET(_pRbu, _Val)					((_pRbu)->args.channel.gsy_request_upper_done_ptr = (_Val))

/*=============================================================================
 *      lower request block
 *===========================================================================*/
#define GSY_LOWER_RQB_PTR_TYPE  	EDD_UPPER_RQB_PTR_TYPE
#define GSY_LOWER_MEM_PTR_TYPE 		EDD_UPPER_MEM_PTR_TYPE
#define GSY_LOWER_TXMEM_PTR_TYPE 	EDD_UPPER_MEM_PTR_TYPE
#define GSY_LOWER_RXMEM_PTR_TYPE 	EDD_UPPER_MEM_PTR_TYPE

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* GSY synchronisation protocols (SyncIDs): Bit[SyncID] = 1 if supported
*/
#define GSY_SYNC_ID_SUPPORTED	0x00000003 	// clock & time

/* maximum number of communication channels
 * = pair of sync and anno channel to EDD
*/
#define GSY_MAX_SYS_CHANNELS (PSI_CFG_GSY_CFG_MAX_SYS_CHANNELS)

#define GSY_MAX_USR_CHANNELS (PSI_CFG_GSY_CFG_MAX_USR_CHANNELS)

/* maximum number of ports on any system channel
*/
#define GSY_CH_MAX_PORTS     (PSI_CFG_GSY_CFG_CH_MAX_PORTS)


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*=============================================================================
 * macro name:    GSY_FILE_SYSTEM_EXTENSION(module_id_)
 *
 * macro:         System integrators can use the macro to smuggle something 
 *                into the component source files. By default it's empty.
 *
 *                The system integrators are responsible for the contents of 
 *                that macro.
 *
 *                Internal data of LSA components may not be changed!
 *
 * parameter:     Module-id
 *
 *                There's no prototype for that macro!
 *===========================================================================*/
#ifndef GSY_FILE_SYSTEM_EXTENSION
#define GSY_FILE_SYSTEM_EXTENSION(module_id_) /* Currently not supported */
#endif

/* GSY_SYNC_SET() is not used if only clock synchronisation is supported
*/
//lrgtime#if (GSY_SYNC_ID_SUPPORTED == 0x00000001) 
#define GSY_SYNC_SET(a_,b_,c_,d_,e_,f_,_g)		LSA_RET_OK  
//lrgtime#endif

/* For Diagnosis of clock synchronisation: 
 * map this macro to edd_SyncGetRealOffsetNs() if supported
*/
#if 0 /*PSI: function in gsy_syncalgorithm.c*/  
#define GSY_GET_EXT_PLL_OFFSET(a_,b_)		0  
#endif

/*------------------------------------------------------------------------------
// interface to LSA trace
//	0 .. no LSA trace
//	1 .. LSA trace
//	2 .. LSA index trace [default]
//----------------------------------------------------------------------------*/
#define GSY_CFG_TRACE_MODE      (PSI_CFG_TRACE_MODE)

/*=============================================================================
 * If the LSA component LTRC isn't used for trace in the target system, then 
 * the trace macros can be defined here.
 *
 * See also files gsy_trc.h and lsa_cfg.h/txt.
 *
 *===========================================================================*/


 /*- alloc mem macros----------------------------------------------------------*/
#define GSY_ALLOC_UPPER_RQB_LOCAL(mem_ptr_ptr, length)              PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_GSY, PSI_MTYPE_UPPER_RQB)
#define GSY_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_GSY, PSI_MTYPE_LOWER_RQB_EDD)
#define GSY_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_GSY, PSI_MTYPE_LOWER_MEM_EDD)
#define GSY_ALLOC_LOCAL_MEM(mem_ptr_ptr, length)                    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_GSY, PSI_MTYPE_LOCAL_MEM)

#define GSY_FREE_UPPER_RQB_LOCAL(ret_val_ptr, upper_rqb_ptr)        PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), 0, LSA_COMP_ID_GSY, PSI_MTYPE_UPPER_RQB)
#define GSY_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_GSY, PSI_MTYPE_LOWER_RQB_EDD)
#define GSY_FREE_LOWER_MEM(ret_val_ptr, lower_mem_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_mem_ptr), (sys_ptr), LSA_COMP_ID_GSY, PSI_MTYPE_LOWER_MEM_EDD)
#define GSY_FREE_LOCAL_MEM(ret_val_ptr, local_mem_ptr)              PSI_FREE_LOCAL_MEM((ret_val_ptr), (local_mem_ptr), 0, LSA_COMP_ID_GSY, PSI_MTYPE_LOCAL_MEM)


/*****************************************************************************/
/*  end of file GSY_CFG.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of GSY_CFG_H */

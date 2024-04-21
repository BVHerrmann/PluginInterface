
#ifndef GSY_LOW_H                   /* ----- reinclude-protection ----- */
#define GSY_LOW_H

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
/*  F i l e               &F: gsy_low.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Lower component interface                                                */
/*  Defines constants, types, macros and prototyping.                        */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                  Who  What                           */
/*  2004-11-29  P01.00.00.00_00.01.00.00 lrg  from LSA_GLOB P2.1.0.0_0.4.1.1 */
/*  2005-04-04  P01.00.00.00_00.01.00.01 lrg  GSY_LOWER_PARAM_TYPE geändert. */
/*  2005-06-02  P03.06.00.00_00.03.00.01 lrg  Prototype for PT2              */
/*  2006-09-27  P03.12.00.00_00.25.04.02 lrg  GSY_LOWER_PARAM_TYPE erweitert */
/*                                            um Link und Rparam             */
/*  2007-06-01  P04.00.00.00_00.10.02.02 lrg  LSA_GLOB P2.1.0.0_0.8.1.1      */
/*  2007-07-16  P04.00.00.00_00.30.01.00 160707lrg001: GSY_LOWER_RQB_SYNC... */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2009-01-28  P04.01.00.00_00.02.03.02 lrg: GSY_LOWER_RQB_KRISC32_PUT_TYPE */
/*  2010-03-23  P05.00.00.00_00.01.04.02 lrg: gsy_request_lower_anno_done()  */
/*  2010-04-21  P05.00.00.00_00.01.04.03 200410lrg001: AP00958586 VLAN tag   */
/*  2011-02-14  lrg: Remove global edd_usr.h for KRISC32                     */
/*  2012-11-07  P05.02.00.00_00.04.08.01 AP01447671 GSY_LOWER_RQB_..  moved  */
/*              from gsy_cfg.h/txt to gsy_low.h                              */
/*  2013-01-08  P05.02.00.00_00.04.12.02 AP01482167:LineDelayMessung         */
/*              lint warning 733 Expression-like macro ... not parenthesized */
/*  2014-04-02  P05.03.00.00_00.03.04.01 994875: (LSA_BOOL) doSwFwd          */
/*  2014-05-16  P06.00.00.00_00.01.30.01 987888,1033202: EDD API-Änderung    */
/*              des Dienstes EDD_SRV_SET_SYNC_TIME                           */
/*  2014-11-05  P06.00.00.00_00.01.49.01 1126470 [GSY] Announce-MAC-Adressen */
/*              nur bei Masterprojektierung aktivieren                       */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#define GSY_LOWER_TXMEM_PTR	/* pointer to ethernet driver tx memory */ \
    LSA_VOID					GSY_LOWER_TXMEM_ATTR *

#define GSY_LOWER_RXMEM_PTR	/* pointer to ethernet driver rx memory */ \
    LSA_VOID					GSY_LOWER_RXMEM_ATTR *

#define GSY_LOWER_TXBYTE_PTR	/* pointer to ethernet driver tx memory */ \
    LSA_UINT8					GSY_LOWER_TXMEM_ATTR *

#define GSY_LOWER_RXBYTE_PTR	/* pointer to ethernet driver rx memory */ \
    LSA_UINT8					GSY_LOWER_RXMEM_ATTR *

#define GSY_LOWER_RXWORD_PTR	/* pointer to ethernet driver rx memory */ \
    LSA_UINT16					GSY_LOWER_RXMEM_ATTR *

#define GSY_LOWER_TX_PDU_PTR \
	GSY_PTCP_PDU 				GSY_LOWER_TXMEM_ATTR *

#define GSY_LOWER_TX_ANNO_PDU_PTR \
	GSY_PTCP_PDU_VLAN			GSY_LOWER_TXMEM_ATTR *	//200410lrg001

#define GSY_LOWER_RX_PDU_PTR \
	GSY_PTCP_DATA				GSY_LOWER_RXMEM_ATTR *

#define GSY_LOWER_RX_RCV_PTR \
	GSY_PTCP_PDU_RECV			GSY_LOWER_RXMEM_ATTR *

#define GSY_LOWER_RX_MAC_PTR \
	GSY_MAC_ADR_TYPE			GSY_LOWER_RXMEM_ATTR *

/*=============================================================================
 *      lower request block
 *===========================================================================*/

/*---------------------------------------------------------------------------*
 * Opcodes, service codes ...  of lower RQB (ethernet device driver)
*/
#define GSY_LOWER_RQB_TYPE  		EDD_RQB_TYPE
#define GSY_LOWER_SERVICE_TYPE		EDD_SERVICE
#define GSY_LOWER_MEM_MAX_LEN		EDD_FRAME_BUFFER_LENGTH
#define GSY_LOWER_RSP_OK  			EDD_STS_OK
#define GSY_LOWER_RSP_OK_ACTIVE		EDD_STS_OK_PENDING
#define GSY_LOWER_RSP_OK_CANCEL		EDD_STS_OK_CANCEL
#define GSY_LOWER_RSP_TIMEOUT		EDD_STS_ERR_TIMEOUT
#define GSY_LOWER_RSP_ERR_SYS_PATH	EDD_STS_ERR_SYS_PATH
#define GSY_LOWER_RSP_ERR_PARAM		EDD_STS_ERR_PARAM
#define GSY_LOWER_RSP_ERR_SEQUENCE	EDD_STS_ERR_SEQUENCE
#define GSY_LOWER_RSP_ERR_RX        EDD_STS_ERR_RX
#define GSY_LOWER_RSP_ERR_TX        EDD_STS_ERR_TX
#define GSY_LOWER_OPC_OPEN_CHANNEL	EDD_OPC_OPEN_CHANNEL
#define GSY_LOWER_OPC_CLOSE_CHANNEL	EDD_OPC_CLOSE_CHANNEL
#define GSY_LOWER_OPC_REQUEST		EDD_OPC_REQUEST
#define GSY_LOWER_MULTICAST			EDD_SRV_MULTICAST
#define GSY_LOWER_PARAMS_GET		EDD_SRV_GET_PARAMS
#define GSY_LOWER_PORT_PARAMS_GET	EDD_SRV_GET_PORT_PARAMS
#define GSY_LOWER_LINK_STATUS_EXT	EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT
#define GSY_LOWER_RECV				EDD_SRV_NRT_RECV
#define GSY_LOWER_SEND				EDD_SRV_NRT_SEND
#define GSY_LOWER_SEND_TS			EDD_SRV_NRT_SEND_TIMESTAMP
#define GSY_LOWER_CANCEL			EDD_SRV_NRT_CANCEL
#define GSY_LOWER_DELAY_SET			EDD_SRV_SET_LINE_DELAY
#define GSY_LOWER_TIME_SET			EDD_SRV_SET_SYNC_TIME
#define GSY_LOWER_RATE_SET			EDD_SRV_SYNC_SET_RATE
#define GSY_LOWER_SYNC_SET			EDD_SRV_SET_SYNC_STATE
#define GSY_LOWER_RECV_SYNC			EDD_SRV_SYNC_RECV
#define GSY_LOWER_SEND_SYNC			EDD_SRV_SYNC_SEND
#define GSY_LOWER_CANCEL_SYNC		EDD_SRV_SYNC_CANCEL

#define GSY_LOWER_DELAY_CTRL		EDD_SRV_PTCP_DELAY_CONTROL
#define GSY_LOWER_DELAY_INACTIVE	EDD_PTCP_DELAY_INACTIVE
#define GSY_LOWER_DELAY_ACTIVE		EDD_PTCP_DELAY_ACTIVE

#define GSY_LOWER_SYNC_CTRL			EDD_SRV_PTCP_SYNC_CONTROL
#define GSY_LOWER_SLAVE_STOP		EDD_PTCP_SYNC_SLAVE_MODE_STOP
#define GSY_LOWER_SLAVE_START		EDD_PTCP_SYNC_SLAVE_MODE_START
#define GSY_LOWER_SLAVE_UPDATE		EDD_PTCP_SYNC_SLAVE_MODE_UPDATE
#define GSY_LOWER_MASTER_OFF		EDD_PTCP_SYNC_MASTER_MODE_OFF
#define GSY_LOWER_MASTER_START		EDD_PTCP_SYNC_MASTER_MODE_START
#define GSY_LOWER_MASTER_UPDATE		EDD_PTCP_SYNC_MASTER_MODE_UPDATE
#define GSY_LOWER_MASTER_ACTIVE		EDD_PTCP_SYNC_MASTER_MODE_ACTIVE
#define GSY_LOWER_MASTER_SLEEP		EDD_PTCP_SYNC_MASTER_MODE_SLEEP
#define GSY_LOWER_MASTER_TLV_LEN	EDD_PTCP_TLV_MAX_LEN

#define GSY_LOWER_SYNC_DIAG			EDD_SRV_PTCP_DIAG_IND_PROVIDE

/* Structures of parameter blocks of lower RQB
*/
#define EDD_RQB_SYNC_RECV_TYPE	   	EDD_RQB_NRT_RECV_TYPE
#define EDD_RQB_SYNC_SEND_TYPE	   	EDD_RQB_NRT_SEND_TYPE

#define GSY_LOWER_RQB_DELAY_TYPE		   	EDD_RQB_PTCP_DELAY_CONTROL_TYPE
#define GSY_LOWER_RQB_SYNC_TYPE			   	EDD_RQB_PTCP_SYNC_CONTROL_TYPE
#define GSY_LOWER_RQB_DIAG_TYPE			   	EDD_RQB_PTCP_DIAG_IND_PROVIDE_TYPE
#define GSY_LOWER_RQB_MULTICAST_TYPE	   	EDD_RQB_MULTICAST_TYPE
#define GSY_LOWER_RQB_GET_PARAMS_TYPE		EDD_RQB_GET_PARAMS_TYPE
#define GSY_LOWER_RQB_GET_PORT_PARAMS_TYPE	EDD_RQB_GET_PORT_PARAMS_TYPE
#define GSY_LOWER_RQB_NRT_RECV_TYPE	   		EDD_RQB_NRT_RECV_TYPE
#define GSY_LOWER_RQB_NRT_SEND_TYPE	   		EDD_RQB_NRT_SEND_TYPE
#define GSY_LOWER_RQB_NRT_SEND_TS_TYPE 		EDD_RQB_NRT_SEND_TYPE
#define GSY_LOWER_RQB_SYNC_RECV_TYPE		EDD_RQB_SYNC_RECV_TYPE
#define GSY_LOWER_RQB_SYNC_SEND_TYPE		EDD_RQB_SYNC_SEND_TYPE
#define GSY_LOWER_RQB_LINK_STATUS_EXT_TYPE	EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE

/* Mode for CANCEL request
*/
#define GSY_LOWER_CANCEL_MODE_ALL	EDD_NRT_CANCEL_MODE_ALL
#define GSY_LOWER_CANCEL_MODE_ID 	EDD_NRT_CANCEL_MODE_RX_BY_ID

/* LINK Status of port
*/
#define GSY_LINK_UNKNOWN			EDD_LINK_UNKNOWN
#define GSY_LINK_UP					EDD_LINK_UP
#define GSY_LINK_DOWN				EDD_LINK_DOWN
#define GSY_LINK_UP_CLOSED			EDD_LINK_UP_CLOSED
#define GSY_LINK_UP_DISABLED    	EDD_LINK_UP_DISABLED
#define GSY_LINK_MODE_FULL	    	EDD_LINK_MODE_FULL

/* Set clock sync state
*/
#define GSY_SYNC_STATE_BAD			EDD_SYNC_STATE_BAD
#define GSY_SYNC_STATE_GOOD			EDD_SYNC_STATE_GOOD

/* Hardware types
*/
#define GSY_HW_TYPE_MSK			EDD_HW_TYPE_USED_MSK			// Mask for used hardware
#define GSY_HW_TYPE_STANDARD	EDD_HW_TYPE_USED_STANDARD_MAC	// No special RT hardware support
#define GSY_HW_TYPE_ERTEC_400	EDD_HW_TYPE_USED_ERTEC_400		// ERTEC 400 hardware support
#define GSY_HW_TYPE_ERTEC_200	EDD_HW_TYPE_USED_ERTEC_200		// ERTEC 200 hardware support
#define GSY_HW_TYPE_ERTEC_200P	EDD_HW_TYPE_USED_ERTEC_200P		// ERTEC 200+ hardware support
#define GSY_HW_TYPE_SOC			EDD_HW_TYPE_USED_SOC			// SoC hardware support
#define GSY_HW_TYPE_I210		EDD_HW_TYPE_USED_EDDL_I210		// I210 (Springville) hardware support
#define GSY_HW_TYPE_HERA        EDD_HW_TYPE_USED_HERA           // HERA hardware support
    
/* Lower sync support types
*/
#define GSY_HW_TYPE_SLAVE_MSK	EDD_HW_TYPE_FEATURE_SYNC_SLAVE_MSK			// Bit mask
#define GSY_HW_TYPE_SLAVE_NO	EDD_HW_TYPE_FEATURE_SYNC_SLAVE_NO_SUPPORT	// No slave support
#define GSY_HW_TYPE_SLAVE_SW	EDD_HW_TYPE_FEATURE_SYNC_SLAVE_SW_SUPPORT	// Slave services supported
#define GSY_HW_TYPE_SLAVE_HW	EDD_HW_TYPE_FEATURE_SYNC_SLAVE_HW_SUPPORT	// Slave hardware support

#define GSY_HW_TYPE_DELAY_MSK	EDD_HW_TYPE_FEATURE_SYNC_DELAY_MSK			// Bit mask
#define GSY_HW_TYPE_DELAY_NO	EDD_HW_TYPE_FEATURE_SYNC_DELAY_NO_SUPPORT	// No delay support
#define GSY_HW_TYPE_DELAY_SW	EDD_HW_TYPE_FEATURE_SYNC_DELAY_SW_SUPPORT	// Delay services supported
#define GSY_HW_TYPE_DELAY_HW	EDD_HW_TYPE_FEATURE_SYNC_DELAY_HW_SUPPORT	// Delay hardware support

#define GSY_HW_TYPE_MASTER_MSK	EDD_HW_TYPE_FEATURE_SYNC_MASTER_MSK			// Bit mask
#define GSY_HW_TYPE_MASTER_NO	EDD_HW_TYPE_FEATURE_SYNC_MASTER_NO_SUPPORT	// No master support
#define GSY_HW_TYPE_MASTER_SW	EDD_HW_TYPE_FEATURE_SYNC_MASTER_SW_SUPPORT	// Master services supported
#define GSY_HW_TYPE_MASTER_HW	EDD_HW_TYPE_FEATURE_SYNC_MASTER_HW_SUPPORT	// Master hardware support

/* 150410lrg001: Priority for sending announce messages
*/
#define GSY_SEND_PRIO_ANNO		EDD_NRT_SEND_PRIO_7

#ifndef GSY_INCLUDE_FOR_KRISC32
typedef union _GSY_LOWER_PARAM_TYPE
{
	GSY_LOWER_RQB_MULTICAST_TYPE	   	Mcst;
	GSY_LOWER_RQB_GET_PARAMS_TYPE		Param;
	GSY_LOWER_RQB_GET_PORT_PARAMS_TYPE	Pparam;
	GSY_LOWER_RQB_NRT_RECV_TYPE	   		Recv;
	GSY_LOWER_RQB_NRT_SEND_TYPE	   		Send;
	GSY_LOWER_RQB_NRT_SEND_TS_TYPE		SendTS;
	GSY_LOWER_RQB_SYNC_SEND_TYPE   		SendSync;
	GSY_LOWER_RQB_SYNC_RECV_TYPE		RecvSync;
	GSY_LOWER_RQB_LINK_STATUS_EXT_TYPE	Link;
	GSY_LOWER_RQB_DELAY_TYPE			DelayCtrl;
	GSY_LOWER_RQB_SYNC_TYPE				SyncCtrl;
	GSY_LOWER_RQB_DIAG_TYPE				DiagInd;
} GSY_LOWER_PARAM_TYPE;
#endif

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/
/* access to elements of lower RQB	(ethernet device driver) 
*/
#define GSY_LOWER_RQB_NEXT_GET				(EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR	//131008lrg001
#define GSY_LOWER_RQB_PREV_GET				(EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_PREV_RQB_PTR	//131008lrg001
#define GSY_LOWER_RQB_NEXT_SET				EDD_RQB_SET_NEXT_RQB_PTR
#define GSY_LOWER_RQB_PREV_SET				EDD_RQB_SET_PREV_RQB_PTR
#define GSY_LOWER_RQB_OPCODE_GET 			EDD_RQB_GET_OPCODE
#define GSY_LOWER_RQB_OPCODE_SET 			EDD_RQB_SET_OPCODE
#define GSY_LOWER_RQB_HANDLE_GET 			EDD_RQB_GET_HANDLE
#define GSY_LOWER_RQB_HANDLE_SET	 		EDD_RQB_SET_HANDLE
#define GSY_LOWER_RQB_LOWER_HANDLE_GET	 	EDD_RQB_GET_HANDLE_LOWER
#define GSY_LOWER_RQB_LOWER_HANDLE_SET	 	EDD_RQB_SET_HANDLE_LOWER
#define GSY_LOWER_RQB_USERID_UVAR32_GET 	EDD_RQB_GET_USERID_UVAR32
#define GSY_LOWER_RQB_USERID_UVAR32_SET 	EDD_RQB_SET_USERID_UVAR32
#define GSY_LOWER_RQB_USER_PTR_GET			EDD_RQB_GET_USERID_PTR
#define GSY_LOWER_RQB_USER_PTR_SET			EDD_RQB_SET_USERID_PTR
#define GSY_LOWER_RQB_USERID_GET	 		EDD_RQB_GET_USERID
#define GSY_LOWER_RQB_COMPID_GET 			EDD_RQB_GET_COMPID
#define GSY_LOWER_RQB_COMPID_SET 			EDD_RQB_SET_COMPID
#define GSY_LOWER_RQB_SERVICE_GET 			EDD_RQB_GET_SERVICE
#define GSY_LOWER_RQB_SERVICE_SET 			EDD_RQB_SET_SERVICE
#define GSY_LOWER_RQB_RESPONSE_GET			EDD_RQB_GET_RESPONSE
#define GSY_LOWER_RQB_RESPONSE_SET			EDD_RQB_SET_RESPONSE
#define GSY_LOWER_RQB_PPARAM_SET 			EDD_RQB_SET_PPARAM

/* Set CompID in err structure if extended error handling is supported
*/ 
#define GSY_LOWER_RQB_ERR_COMPID_SET(_RbPtr, _Value)

#define GSY_LOWER_RQB_PPARAM_GET(_RbPtr)	\
		((_RbPtr)->pParam)

/* ATTENTIONE: *** first call GSY_LOWER_PPARAM_SET *** :ATTENTIONE
*/
#define GSY_LOWER_RQB_HANDLE_UPPER_GET(_RbPtr)	\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->HandleUpper)

#define GSY_LOWER_RQB_HANDLE_UPPER_SET(_RbPtr, _Value)	\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->HandleUpper = (_Value))

#define GSY_LOWER_RQB_HANDLE_LOWER_SET(_RbPtr, _Value)	\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->HandleLower = (_Value))

#define GSY_LOWER_RQB_HANDLE_LOWER_GET(_RbPtr)	\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->HandleLower)

#define GSY_LOWER_RQB_SYS_PATH_GET(_RbPtr)		\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->SysPath)

#define GSY_LOWER_RQB_SYS_PATH_SET(_RbPtr, _Value)		\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->SysPath = (_Value))

#define GSY_LOWER_RQB_CBF_PTR_GET(_RbPtr)			\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->Cbf)

#define GSY_LOWER_RQB_CBF_PTR_SET(_RbPtr, _Value)			\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->Cbf = (_Value))

#define GSY_LOWER_RQB_MULTICAST_ENABLE(_RbPtr, _AdrPtr)		 \
		{LSA_INT _i; for (_i = 0; _i < EDD_MAC_ADDR_SIZE; _i++)		\
			  ((EDD_RQB_MULTICAST_TYPE *)((_RbPtr)->pParam))->MACAddr.MacAdr[_i] = *((_AdrPtr) + _i);	\
		((EDD_RQB_MULTICAST_TYPE *)((_RbPtr)->pParam))->Mode = EDD_MULTICAST_ENABLE;}

#define GSY_LOWER_RQB_MULTICAST_DISABLE(_RbPtr, _AdrPtr)		 \
		{LSA_INT _i; for (_i = 0; _i < EDD_MAC_ADDR_SIZE; _i++)		\
			  ((EDD_RQB_MULTICAST_TYPE *)((_RbPtr)->pParam))->MACAddr.MacAdr[_i] = *((_AdrPtr) + _i);	\
		((EDD_RQB_MULTICAST_TYPE *)((_RbPtr)->pParam))->Mode = EDD_MULTICAST_DISABLE;}

#define GSY_LOWER_RQB_MULTICAST_SYNCID_GET(_RbPtr)		 \
		(((EDD_RQB_MULTICAST_TYPE *)((_RbPtr)->pParam))->MACAddr.MacAdr[5] & (LSA_UINT8)GSY_SYNCID_MASK)

#define GSY_LOWER_RQB_MULTICAST_MODE_GET(_RbPtr)			 \
		(((EDD_RQB_MULTICAST_TYPE *)((_RbPtr)->pParam))->Mode)

/* 100107lrg002: EDD_NRT_SEND_PRIO_o --> EDD_NRT_SEND_PRIO_ORG
 * 131008lrg001: EDD_UPPER_MEM_U8_PTR_TYPE
 * 260108lrg001: TxFrmGroup
*/
#define GSY_LOWER_RQB_SEND_DATA_SET(_RbPtr, _DataPtr, _DataLen)		 \
		{((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(_DataPtr);	\
		((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->Length = (LSA_UINT32)(_DataLen);		\
		((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->Priority = EDD_NRT_SEND_PRIO_MGMT_HIGH;		\
		((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->TxFrmGroup = EDD_NRT_TX_GRP_PTCP;		\
		((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->PortID = EDD_PORT_ID_AUTO;}

/* 150410lrg001: GSY_LOWER_RQB_SEND_PRIO_SET
*/
#define GSY_LOWER_RQB_SEND_PRIO_SET(_RbPtr, _Prio)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->Priority = (LSA_UINT32)(_Prio))

#define GSY_LOWER_RQB_SEND_LEN_SET(_RbPtr, _DataLen)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->Length = (LSA_UINT32)(_DataLen))

#define GSY_LOWER_RQB_SEND_LEN_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->Length)

#define GSY_LOWER_RQB_SEND_PORT_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->PortID = (LSA_UINT16)(_Value))

#define GSY_LOWER_RQB_SEND_PORT_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->PortID)

#define GSY_LOWER_RQB_SEND_PTR_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->pBuffer)

#define GSY_LOWER_RQB_SEND_TIME_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->TxTime)

#define GSY_LOWER_RQB_SEND_TIME_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->TxTime = (_Value))

#define GSY_LOWER_RQB_SEND_LINE_DELAY_HW_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->LineDelayInTicksHardware)

#define GSY_LOWER_RQB_SEND_LINE_DELAY_HW_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->LineDelayInTicksHardware = (_Value))

#define GSY_LOWER_RQB_RECV_PORT_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->PortID = (LSA_UINT16)(_Value))

#define GSY_LOWER_RQB_RECV_PORT_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->PortID)

#define GSY_LOWER_RQB_RECV_PTR_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->pBuffer)

#define GSY_LOWER_RQB_RECV_PTR_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(_Value))	//131008lrg001

#define GSY_LOWER_RQB_RECV_LEN_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->IOCount)

#define GSY_LOWER_RQB_RECV_DATA_OFFSET_GET(_RbPtr)	 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->UserDataOffset)

#define GSY_LOWER_RQB_RECV_DATA_OFFSET_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->UserDataOffset = (_Value))

#define GSY_LOWER_RQB_RECV_DATA_LEN_GET(_RbPtr)	 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->UserDataLength)

#define GSY_LOWER_RQB_RECV_DATA_LEN_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->UserDataLength = (_Value))

#define GSY_LOWER_RQB_RECV_LEN_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->IOCount = (_Value))

#define GSY_LOWER_RQB_RECV_ID_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RequestID = (_Value))

#define GSY_LOWER_RQB_RECV_ID_GET(_RbPtr)   \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RequestID)

#define GSY_LOWER_RQB_RECV_CNT_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RequestCnt = (_Value))

#define GSY_LOWER_RQB_RECV_CNT_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RequestCnt)

#define GSY_LOWER_RQB_RECV_TIME_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RxTime)

#define GSY_LOWER_RQB_RECV_TIME_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RxTime = (_Value))

/* 131008lrg001: EDD_UPPER_MEM_U8_PTR_TYPE
*/
#define GSY_LOWER_RQB_RECV_DATA_SET(_RbPtr, _DataPtr, _DataLen)		 \
		{((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(_DataPtr);	\
		((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RequestCnt = 0;		\
		((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->IOCount = (_DataLen);}

#define GSY_LOWER_RQB_CANCEL_MODE_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_CANCEL_TYPE *)((_RbPtr)->pParam))->Mode)

#define GSY_LOWER_RQB_CANCEL_MODE_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_CANCEL_TYPE *)((_RbPtr)->pParam))->Mode = (_Value))

#define GSY_LOWER_RQB_CANCEL_ID_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_CANCEL_TYPE *)((_RbPtr)->pParam))->RequestID = (_Value))

#define GSY_LOWER_RQB_CANCEL_ID_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_CANCEL_TYPE *)((_RbPtr)->pParam))->RequestID)

#define GSY_LOWER_RQB_PARAM_PORTS_GET(_RbPtr)	 \
		(((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->MaxPortCnt)

#define GSY_LOWER_RQB_PARAM_MACADDR_GET(_MacStruct, _RbPtr) \
  		{int _i; for (_i = 0; _i < GSY_MAC_ADR_LENGTH; _i++)	\
		_MacStruct.MacAdr[_i] = ((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->MACAddr.MacAdr[_i];}		//041112lrg001

#define GSY_LOWER_RQB_PARAM_NANOSECONDS_GET(_RbPtr)	 \
		(((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->TimeResolutionInNs)

#define GSY_LOWER_RQB_PARAM_NANOSECONDS_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->TimeResolutionInNs = (_Value))

#define GSY_LOWER_RQB_PARAM_HWTYPE_GET(_RbPtr)	 \
		(((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->HardwareType)

#define GSY_LOWER_RQB_PARAM_HWTYPE_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->HardwareType = (_Value))

#define GSY_LOWER_RQB_PARAM_TRACEIDX_GET(_RbPtr)	 \
		(((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->TraceIdx)

#define GSY_LOWER_RQB_PARAM_TRACEIDX_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->TraceIdx = (_Value))

#define GSY_LOWER_RQB_SET_LINE_DELAY_PORT_GET(_RbPtr)	 \
		(((EDD_RQB_SET_LINE_DELAY_TYPE *)((_RbPtr)->pParam))->PortID)

#define GSY_LOWER_RQB_SET_LINE_DELAY_PORT_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SET_LINE_DELAY_TYPE *)((_RbPtr)->pParam))->PortID = (_Value))

#define GSY_LOWER_RQB_SET_LINE_DELAY_DELAY_GET(_RbPtr)	 \
		(((EDD_RQB_SET_LINE_DELAY_TYPE *)((_RbPtr)->pParam))->LineDelayInTicksMeasured)

#define GSY_LOWER_RQB_SET_LINE_DELAY_DELAY_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SET_LINE_DELAY_TYPE *)((_RbPtr)->pParam))->LineDelayInTicksMeasured = (_Value))

#define GSY_LOWER_RQB_SET_LINE_DELAY_CABLE_GET(_RbPtr)	 \
		(((EDD_RQB_SET_LINE_DELAY_TYPE *)((_RbPtr)->pParam))->CableDelayInNsMeasured)

#define GSY_LOWER_RQB_SET_LINE_DELAY_CABLE_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SET_LINE_DELAY_TYPE *)((_RbPtr)->pParam))->CableDelayInNsMeasured = (_Value))

#define GSY_LOWER_RQB_PORT_DELAY_ID_GET(_RbPtr)	 \
		(((EDD_RQB_GET_PORT_PARAMS_TYPE *)((_RbPtr)->pParam))->PortID)

#define GSY_LOWER_RQB_PORT_DELAY_ID_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_GET_PORT_PARAMS_TYPE *)((_RbPtr)->pParam))->PortID = (_Value))

#define GSY_LOWER_RQB_PORT_MACADDR_GET(_MacStruct, _RbPtr) \
  		{int _i; for (_i = 0; _i < GSY_MAC_ADR_LENGTH; _i++)	\
		_MacStruct.MacAdr[_i] = ((EDD_RQB_GET_PORT_PARAMS_TYPE *)((_RbPtr)->pParam))->MACAddr.MacAdr[_i];}		//041112lrg001

/* 240409lrg001: PortRx/Tx-Delays nur noch aus ExtLinkInd holen
*/
#define GSY_LOWER_RQB_LINK_STATUS_PORT_GET(_RbPtr)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->PortID)

#define GSY_LOWER_RQB_LINK_STATUS_PORT_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->PortID = (_Value))

#define GSY_LOWER_RQB_LINK_STATUS_STATUS_GET(_RbPtr)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->Status)

#define GSY_LOWER_RQB_LINK_STATUS_STATUS_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->Status = (_Value))

#define GSY_LOWER_RQB_LINK_STATUS_SPEED_GET(_RbPtr)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->Speed)

#define GSY_LOWER_RQB_LINK_STATUS_SPEED_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->Speed = (_Value))

#define GSY_LOWER_RQB_LINK_STATUS_MODE_GET(_RbPtr)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->Mode)

#define GSY_LOWER_RQB_LINK_STATUS_MODE_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->Mode = (_Value))

#define GSY_LOWER_RQB_LINK_STATUS_TXDELAY_GET(_RbPtr)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->PortTxDelay)

#define GSY_LOWER_RQB_LINK_STATUS_TXDELAY_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->PortTxDelay = (_Value))

#define GSY_LOWER_RQB_LINK_STATUS_RXDELAY_GET(_RbPtr)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->PortRxDelay)

#define GSY_LOWER_RQB_LINK_STATUS_RXDELAY_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->PortRxDelay = (_Value))

#define GSY_LOWER_RQB_LINK_STATUS_TOPO_OK_GET(_RbPtr)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->SyncId0_TopoOk)

#define GSY_LOWER_RQB_LINK_STATUS_TOPO_OK_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->SyncId0_TopoOk = ((_Value) ? EDD_PORT_SYNCID0_TOPO_OK : EDD_PORT_SYNCID0_TOPO_NOT_OK))

#define GSY_LOWER_RQB_LINK_STATUS_LINEDELAY_GET(_RbPtr)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->LineDelayInTicksMeasured)

#define GSY_LOWER_RQB_LINK_STATUS_CABLEDELAY_GET(_RbPtr)	 \
		(((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->CableDelayInNsMeasured)

#define GSY_LOWER_RQB_LINK_STATUS_ALL_SET_UNKNOWN(_RbPtr) \
		{((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->Status = EDD_LINK_UNKNOWN;	\
		 ((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->Speed = EDD_LINK_UNKNOWN;	\
		 ((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->Mode = EDD_LINK_UNKNOWN;	\
		 ((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->LineDelayInTicksMeasured = EDD_DELAY_UNKNOWN;	\
		 ((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->CableDelayInNsMeasured = EDD_DELAY_UNKNOWN;	\
		 ((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->PortTxDelay = EDD_DELAY_UNKNOWN;	\
		 ((EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE *)((_RbPtr)->pParam))->PortRxDelay = EDD_DELAY_UNKNOWN;}

#define GSY_LOWER_RQB_SYNC_NANOSECONDS_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SET_SYNC_TIME_TYPE *)((_RbPtr)->pParam))->PTCPNanoSeconds = (_Value))
#define GSY_LOWER_RQB_SYNC_SECONDS_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SET_SYNC_TIME_TYPE *)((_RbPtr)->pParam))->PTCPSeconds = (_Value))
#define GSY_LOWER_RQB_SYNC_RCF_INTERVAL_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SET_SYNC_TIME_TYPE *)((_RbPtr)->pParam))->RCFInterval = (_Value))
#define GSY_LOWER_RQB_SYNC_TIMESTAMP_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SET_SYNC_TIME_TYPE *)((_RbPtr)->pParam))->ReferenceTimeStampInTicks = (_Value))
#define GSY_LOWER_RQB_SYNC_MODE_SET_OFFSET(_RbPtr)	 \
		(((EDD_RQB_SET_SYNC_TIME_TYPE *)((_RbPtr)->pParam))->Mode = (EDD_SET_SYNC_PTCP_MODE_OFFSET))
#define GSY_LOWER_RQB_SYNC_MODE_SET_ABSOLUTE(_RbPtr)	 \
		(((EDD_RQB_SET_SYNC_TIME_TYPE *)((_RbPtr)->pParam))->Mode = (EDD_SET_SYNC_PTCP_MODE_ABSOLUTE))
#define GSY_LOWER_RQB_SYNC_SIGN_SET_POSITIVE(_RbPtr)	 \
		(((EDD_RQB_SET_SYNC_TIME_TYPE *)((_RbPtr)->pParam))->PTCPSign = (EDD_SET_SYNC_PTCP_MODE_OFFSET_POSITIVE))
#define GSY_LOWER_RQB_SYNC_SIGN_SET_NEGATIVE(_RbPtr)	 \
		(((EDD_RQB_SET_SYNC_TIME_TYPE *)((_RbPtr)->pParam))->PTCPSign = (EDD_SET_SYNC_PTCP_MODE_OFFSET_NEGATIVE))

/* New SYNC-Services
 * 131008lrg001: EDD_UPPER_MEM_U8_PTR_TYPE
*/
#define GSY_LOWER_RQB_SEND_SYNC_DATA_SET(_RbPtr, _DataPtr, _DataLen)		 \
		{((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(_DataPtr);	\
		((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->Length = (LSA_UINT32)(_DataLen);		\
		((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->Priority = EDD_NRT_SEND_PRIO_MGMT_HIGH;		\
		((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->PortID = EDD_PORT_ID_AUTO;}

#define GSY_LOWER_RQB_SEND_SYNC_LEN_SET(_RbPtr, _DataLen)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->Length = (LSA_UINT32)(_DataLen))

#define GSY_LOWER_RQB_SEND_SYNC_LEN_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->Length)

#define GSY_LOWER_RQB_SEND_SYNC_PORT_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->PortID = (LSA_UINT16)(_Value))

#define GSY_LOWER_RQB_SEND_SYNC_PORT_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->PortID)

#define GSY_LOWER_RQB_SEND_SYNC_PTR_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->pBuffer)

#define GSY_LOWER_RQB_SEND_SYNC_TIME_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->TxTime)

#define GSY_LOWER_RQB_SEND_SYNC_TIME_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->TxTime = (_Value))

#define GSY_LOWER_RQB_SEND_SYNC_TS_VALID_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->TxTimePresent)

#define GSY_LOWER_RQB_SEND_SYNC_TS_VALID_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->TxTimePresent = ((_Value) ? EDD_SYNC_SEND_TX_TIME_PRESENT : EDD_SYNC_SEND_TX_TIME_NOT_PRESENT))

#define GSY_LOWER_RQB_SEND_SYNC_LINE_DELAY_HW_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->LineDelayInTicksHardware)

#define GSY_LOWER_RQB_SEND_SYNC_LINE_DELAY_HW_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->LineDelayInTicksHardware = (_Value))

#define GSY_LOWER_RQB_SEND_SYNC_FRAMEID_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->FrameID)

#define GSY_LOWER_RQB_SEND_SYNC_FRAMEID_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_SEND_TYPE *)((_RbPtr)->pParam))->FrameID = (_Value))

#define GSY_LOWER_RQB_RECV_SYNC_PORT_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->PortID = (LSA_UINT16)(_Value))

#define GSY_LOWER_RQB_RECV_SYNC_PORT_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->PortID)

#define GSY_LOWER_RQB_RECV_SYNC_PTR_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->pBuffer)

#define GSY_LOWER_RQB_RECV_SYNC_PTR_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(_Value))	//131008lrg001

#define GSY_LOWER_RQB_RECV_SYNC_LEN_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->IOCount)

#define GSY_LOWER_RQB_RECV_SYNC_DATA_OFFSET_GET(_RbPtr)	 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->UserDataOffset)

#define GSY_LOWER_RQB_RECV_SYNC_DATA_OFFSET_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->UserDataOffset = (_Value))

#define GSY_LOWER_RQB_RECV_SYNC_DATA_LEN_GET(_RbPtr)	 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->UserDataLength)

#define GSY_LOWER_RQB_RECV_SYNC_DATA_LEN_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->UserDataLength = (_Value))

#define GSY_LOWER_RQB_RECV_SYNC_LEN_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->IOCount = (_Value))

#define GSY_LOWER_RQB_RECV_SYNC_TIME_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->RxTime)

#define GSY_LOWER_RQB_RECV_SYNC_TIME_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->RxTime = (_Value))

#define GSY_LOWER_RQB_RECV_SYNC_CNT_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->RequestCnt)

#define GSY_LOWER_RQB_RECV_SYNC_CNT_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->RequestCnt = (_Value))

#define GSY_LOWER_RQB_RECV_SYNC_ID_GET(_RbPtr)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->RequestID)

#define GSY_LOWER_RQB_RECV_SYNC_ID_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->RequestID = (_Value))

/* 131008lrg001: EDD_UPPER_MEM_U8_PTR_TYPE
*/
#define GSY_LOWER_RQB_RECV_SYNC_DATA_SET(_RbPtr, _DataPtr, _DataLen)		 \
		{((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(_DataPtr);	\
		((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->IOCount = (_DataLen);}

#define GSY_LOWER_RQB_RECV_SYNC_DOFWD_GET(_RbPtr, _pDoFwd, _PortCnt) \
		{LSA_UINT16 _ii; for (_ii = 0; _ii < _PortCnt; _ii++) \
		*(((LSA_BOOL*)_pDoFwd)+_ii) = (((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->doSwFwd[_ii])?LSA_TRUE:LSA_FALSE;}	//994875: LSA_UINT8 -> LSA_BOOL

#define GSY_LOWER_RQB_RECV_SYNC_DOFWD_SET(_RbPtr, _PortIdx, _Value) \
		(((EDD_RQB_SYNC_RECV_TYPE *)((_RbPtr)->pParam))->doSwFwd[_PortIdx] = ((_Value) ? EDD_NRT_RECV_DO_FWD_BY_UPPER_LAYER : EDD_NRT_RECV_DO_NOT_FWD_BY_UPPER_LAYER) )

#define GSY_LOWER_RQB_SET_RATE_VALUE_GET(_RbPtr)	 \
		(((EDD_RQB_SYNC_SET_RATE_TYPE *)((_RbPtr)->pParam))->RateIntervalNs)

#define GSY_LOWER_RQB_SET_RATE_VALUE_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SYNC_SET_RATE_TYPE *)((_RbPtr)->pParam))->RateIntervalNs = (_Value))

#define GSY_LOWER_RQB_SET_RATE_VALID_GET(_RbPtr)	 \
		(((EDD_RQB_SYNC_SET_RATE_TYPE *)((_RbPtr)->pParam))->RateValid)

#define GSY_LOWER_RQB_SET_RATE_VALID_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SYNC_SET_RATE_TYPE *)((_RbPtr)->pParam))->RateValid = (_Value))

#define GSY_LOWER_RQB_SET_RATE_SYNCID_GET(_RbPtr)	 \
		(((EDD_RQB_SYNC_SET_RATE_TYPE *)((_RbPtr)->pParam))->SyncID)

#define GSY_LOWER_RQB_SET_RATE_SYNCID_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SYNC_SET_RATE_TYPE *)((_RbPtr)->pParam))->SyncID = (_Value))

#define GSY_LOWER_RQB_SET_STATE_SYNC_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_SET_SYNC_STATE_TYPE *)((_RbPtr)->pParam))->SyncState = (_Value))	

#define GSY_LOWER_RQB_SET_STATE_SYNC_GET(_RbPtr)	 \
		(((EDD_RQB_SET_SYNC_STATE_TYPE *)((_RbPtr)->pParam))->SyncState)

#define GSY_LOWER_RQB_K32_DELAY_REQ_ACTIVE_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_DELAY_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->ReqActive = (_Value))

#define GSY_LOWER_RQB_K32_DELAY_RSP_ACTIVE_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_DELAY_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->RspActive = (_Value))

#define GSY_LOWER_RQB_K32_DELAY_PORT_ID_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_DELAY_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->PortID = (_Value))

#define GSY_LOWER_RQB_K32_DELAY_PORT_RXDELAY_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_DELAY_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->RxPortDelay = (_Value))

#define GSY_LOWER_RQB_K32_DELAY_PORT_TXDELAY_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_DELAY_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->TxPortDelay = (_Value))

#define GSY_LOWER_RQB_K32_DELAY_PORT_ADDR_SET(_RbPtr, _MacStruct) \
  		{int _i; for (_i = 0; _i < GSY_MAC_ADR_LENGTH; _i++)	\
		((EDD_UPPER_PTCP_DELAY_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->PortMacAddr.MacAdr[_i] = _MacStruct.MacAdr[_i];}		//041112lrg001

#define GSY_LOWER_RQB_K32_DIAG_ADDR_GET(_MacStruct, _RbPtr) \
  		{int _i; for (_i = 0; _i < GSY_MAC_ADR_LENGTH; _i++)	\
		_MacStruct.MacAdr[_i] = ((EDD_UPPER_PTCP_DIAG_IND_PROVIDE_PTR_TYPE)((_RbPtr)->pParam))->MasterMacAddr.MacAdr[_i];}		//041112lrg001

#define GSY_LOWER_RQB_K32_DIAG_PRIO_GET(_RbPtr) \
		(((EDD_UPPER_PTCP_DIAG_IND_PROVIDE_PTR_TYPE)((_RbPtr)->pParam))->RcvSyncPrio)

#define GSY_LOWER_RQB_K32_DIAG_SEQID_GET(_RbPtr) \
		(((EDD_UPPER_PTCP_DIAG_IND_PROVIDE_PTR_TYPE)((_RbPtr)->pParam))->SequenceId)

#define GSY_LOWER_RQB_K32_DIAG_RATE_GET(_RbPtr) \
		(((EDD_UPPER_PTCP_DIAG_IND_PROVIDE_PTR_TYPE)((_RbPtr)->pParam))->RateInterval)

#define GSY_LOWER_RQB_K32_DIAG_STATE_GET(_RbPtr) \
		(((EDD_UPPER_PTCP_DIAG_IND_PROVIDE_PTR_TYPE)((_RbPtr)->pParam))->SlaveState)

#define GSY_LOWER_RQB_K32_DIAG_SOURCE_GET(_RbPtr) \
		(((EDD_UPPER_PTCP_DIAG_IND_PROVIDE_PTR_TYPE)((_RbPtr)->pParam))->DiagSrc)

#define GSY_LOWER_RQB_K32_DIAG_ID_GET(_RbPtr) \
		(((EDD_UPPER_PTCP_DIAG_IND_PROVIDE_PTR_TYPE)((_RbPtr)->pParam))->SyncId)

#define GSY_LOWER_RQB_K32_DIAG_ID_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_DIAG_IND_PROVIDE_PTR_TYPE)((_RbPtr)->pParam))->SyncId = (_Value))

#define GSY_LOWER_RQB_K32_SYNC_ID_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_SYNC_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->SyncId = (_Value))

#define GSY_LOWER_RQB_K32_SYNC_SLAVE_MODE_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_SYNC_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->SlaveMode = (_Value))

#define GSY_LOWER_RQB_K32_SYNC_MASTER_MODE_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_SYNC_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->MasterMode = (_Value))

#define GSY_LOWER_RQB_K32_SYNC_LENGTH_TLV_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_SYNC_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->LengthTLV = (_Value))

#define GSY_LOWER_RQB_K32_SYNC_SEND_FACTOR_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_SYNC_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->SyncSendFactor = (_Value))

#define GSY_LOWER_RQB_K32_SYNC_TIMEOUT_FACTOR_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_SYNC_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->TimeoutFactor = (_Value))

#define GSY_LOWER_RQB_K32_SYNC_TAKEOVER_FACTOR_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_SYNC_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->TakeoverFactor = (_Value))

#define GSY_LOWER_RQB_K32_SYNC_PLL_WINDOW_SET(_RbPtr, _Value) \
		(((EDD_UPPER_PTCP_SYNC_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->PLLWindow = (_Value))

#define GSY_LOWER_RQB_K32_SYNC_TLV_SET(_RbPtr, _SrcPtr, _Length) \
  		{int _i; \
  		 for (_i = 0; _i < _Length; _i++)	\
		 	((EDD_UPPER_PTCP_SYNC_CONTROL_PTR_TYPE)((_RbPtr)->pParam))->TLV[_i] = *(((LSA_UINT8 *)(_SrcPtr))+_i);}

/* MAC Address copy macros
*/
#define GSY_LOWER_FRAME_MACADR_SET(_DstPtr, _SrcPtr)			\
  		{int _i; for (_i = 0; _i < GSY_MAC_ADR_LENGTH; _i++)	\
			*(((LSA_UINT8 *)(_DstPtr))+_i) = *(((LSA_UINT8 *)(_SrcPtr))+_i);}

#define GSY_LOWER_FRAME_MACADR_GET(_DstPtr, _SrcPtr)			\
  		GSY_LOWER_FRAME_MACADR_SET(_SrcPtr, _DstPtr)

#define GSY_LOWER_TXMEM8_SET(_p, _l, _v) \
	{LSA_INT _i; for (_i = 0; _i < _l; _i++) *(((GSY_LOWER_TXBYTE_PTR)_p)+_i) = _v;}

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  gsy_request_lower_done
 *
 * function:       callback function for 
 *                 confirmation of GSY_OPEN_CHANNEL_LOWER(),
 *                 GSY_CLOSE_CHANNEL_LOWER() and GSY_REQUEST_LOWER() 
 *
 * parameters:     GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB return values, see GSY_OPEN_CHANNEL_LOWER(), GSY_CLOSE_CHANNEL_LOWER()
 *                        respectively GSY_REQUEST_LOWER()
 *===========================================================================*/
LSA_VOID  GSY_LOWER_IN_FCT_ATTR  gsy_request_lower_done(
    GSY_LOWER_RQB_PTR_TYPE     lower_rqb_ptr
);

/*=============================================================================
 * function name:  gsy_request_lower_anno_done
 *
 * function:       callback function for 
 *                 confirmation of GSY_OPEN_CHANNEL_LOWER(),
 *                 GSY_CLOSE_CHANNEL_LOWER() and GSY_REQUEST_LOWER() 
 *                 on announce channel
 *
 * parameters:     GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB return values, see GSY_OPEN_CHANNEL_LOWER(), GSY_CLOSE_CHANNEL_LOWER()
 *                        respectively GSY_REQUEST_LOWER()
 *===========================================================================*/
LSA_VOID  GSY_LOWER_IN_FCT_ATTR  gsy_request_lower_anno_done(
    GSY_LOWER_RQB_PTR_TYPE     lower_rqb_ptr
);

/*=============================================================================
 * function name:  gsy_lower_rqb
 *
 * function:       asynchronous allocation of a lower RQB
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of prefix
 *                 LSA_UINT16                 length:         length of RQB
 *                 GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_lower_rqb(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    GSY_LOWER_RQB_PTR_TYPE     lower_rqb_ptr
);

/*=============================================================================
 * function name:  gsy_lower_mem
 *
 * function:       asynchronous allocation of a lower-memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of prefix
 *                 LSA_UINT16                 length:         length of memory
 *                 GSY_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_lower_mem(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    GSY_LOWER_MEM_PTR_TYPE     lower_mem_ptr
);

/*=============================================================================
 * function name:  gsy_lower_tx_mem
 *
 * function:       asynchronous allocation of a lower transmit memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of dcp
 *                 LSA_UINT16                 length:         length of memory
 *                 GSY_LOWER_TXMEM_PTR_TYPE   lower_txmem_ptr:pointer to lower-
 *                                                            transmit memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_lower_tx_mem(
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    GSY_LOWER_TXMEM_PTR_TYPE                            lower_txmem_ptr
);

/*=============================================================================
 * function name:  gsy_lower_rx_mem
 *
 * function:       asynchronous allocation of a lower receive memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of dcp
 *                 LSA_UINT16                 length:         length of memory
 *                 GSY_LOWER_RXMEM_PTR_TYPE   lower_rxmem_ptr:pointer to lower-
 *                                                            receive memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_lower_rx_mem(
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    GSY_LOWER_RXMEM_PTR_TYPE                            lower_rxmem_ptr
);


/*====  out functions  =====*/

/*=============================================================================
 * function name:  GSY_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to lower layer
 *
 * parameters:     GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of prefix
 *     LSA_USER_ID_TYPE   user-id:       id of prefix
 *     RQB args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of prefix
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel                 
 *     LSA_VOID  LSA_FCT_PTR(GSY_LOWER_IN_FCT_ATTR, gsy_request_lower_done_ptr)
 *                                    (GSY_LOWER_RQB_PTR_TYPE lower_rqb_ptr)
 *                                       pointer to callback-function
 *
 * RQB return values via callback-function:
 *     RQB args:
 *     LSA_HANDLE_TYPE    handle:        channel-handle of lower layer
 *     All other RQB parameters are unchanged.
 *===========================================================================*/
#ifndef GSY_OPEN_CHANNEL_LOWER
LSA_VOID  GSY_LOWER_OUT_FCT_ATTR  GSY_OPEN_CHANNEL_LOWER(
    GSY_LOWER_RQB_PTR_TYPE     lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to lower layer
 *
 * parameters:     GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:  id of prefix
 *
 * RQB return values via callback-function:
 *     RQB header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of prefix
 *     All other RQB parameters are unchanged.
 *===========================================================================*/
#ifndef GSY_CLOSE_CHANNEL_LOWER
LSA_VOID  GSY_LOWER_OUT_FCT_ATTR  GSY_CLOSE_CHANNEL_LOWER(
    GSY_LOWER_RQB_PTR_TYPE     lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to lower layer
 *
 * parameters:     GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:   id of prefix
 *     RQB args:                    Depend on kind of request.
 *     
 * RQB return values via callback-function:
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of prefix
 *     LSA_USER_ID_TYPE  user-id:   id of prefix
 *     All other RQB parameters depend on kind of request.
 *===========================================================================*/
#ifndef GSY_REQUEST_LOWER
LSA_VOID  GSY_LOWER_OUT_FCT_ATTR  GSY_REQUEST_LOWER(
    GSY_LOWER_RQB_PTR_TYPE     lower_rqb_ptr,
    LSA_SYS_PTR_TYPE           sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower RQB
 *
 * parameters:     GSY_LOWER_RQB_PTR_TYPE  ...  *  lower_rqb_ptr_ptr:
 *                                   return value: pointer to lower RQB pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 gsy_lower_rqb().
 *                 LSA_USER_ID_TYPE                   user_id:  id of prefix
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_ALLOC_LOWER_RQB
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_LOWER_RQB(
    GSY_LOWER_RQB_PTR_TYPE  GSY_LOCAL_MEM_ATTR *        lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_FREE_LOWER_RQB
 *
 * function:       free a lower RQB
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower RQB or
 *                                                      wrong system pointer
 *                 GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_FREE_LOWER_RQB
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_LOWER_RQB(
    LSA_UINT16                 GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    GSY_LOWER_RQB_PTR_TYPE                           lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                 sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower-memory
 *
 * parameters:     GSY_LOWER_MEM_PTR_TYPE  ...  *  lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 gsy_lower_mem().
 *                 LSA_USER_ID_TYPE                   user_id:  id of prefix
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_ALLOC_LOWER_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_LOWER_MEM(
    GSY_LOWER_MEM_PTR_TYPE  GSY_LOCAL_MEM_ATTR *     lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                 user_id,
    LSA_UINT16                                       length,
    LSA_SYS_PTR_TYPE                                 sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_FREE_LOWER_MEM
 *
 * function:       free a lower-memory
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system pointer
 *                 GSY_LOWER_MEM_PTR_TYPE  lower_mem_ptr:  pointer to lower-
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_FREE_LOWER_MEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_LOWER_MEM(
    LSA_UINT16                 GSY_LOCAL_MEM_ATTR *  ret_val_ptr,
    GSY_LOWER_MEM_PTR_TYPE                           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                 sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_ALLOC_LOWER_TXMEM
 *
 * function:       allocate a lower memory for send PDUs
 *
 * parameters:     GSY_LOWER_TXMEM_PTR_TYPE  ...  *     lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 gsy_lower_mem().
 *                 LSA_USER_ID_TYPE                   user_id:  id of dcp
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_ALLOC_LOWER_TXMEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_LOWER_TXMEM(
    GSY_LOWER_TXMEM_PTR_TYPE  GSY_LOCAL_MEM_ATTR *      lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_FREE_LOWER_TXMEM
 *
 * function:       free a lower memory for send PDUs
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system pointer
 *                 GSY_LOWER_MEM_PTR_TYPE  lower_mem_ptr:     pointer to lower-
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_FREE_LOWER_TXMEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_LOWER_TXMEM(
    LSA_UINT16                 GSY_LOCAL_MEM_ATTR *     ret_val_ptr,
    GSY_LOWER_TXMEM_PTR_TYPE                            lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_ALLOC_LOWER_RXMEM
 *
 * function:       allocate a lower memory for receive PDUs
 *
 * parameters:     GSY_LOWER_RXMEM_PTR_TYPE  ...  *     lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 gsy_lower_mem().
 *                 LSA_USER_ID_TYPE                   user_id:  id of dcp
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_ALLOC_LOWER_RXMEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_ALLOC_LOWER_RXMEM(
    GSY_LOWER_RXMEM_PTR_TYPE  GSY_LOCAL_MEM_ATTR *      lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  GSY_FREE_LOWER_RXMEM
 *
 * function:       free a lower memory for receive PDUs
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower-memory or
 *                                                      wrong system pointer
 *                 GSY_LOWER_RXMEM_PTR_TYPE  lower_mem_ptr:     pointer to lower-
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_FREE_LOWER_RXMEM
LSA_VOID  GSY_SYSTEM_OUT_FCT_ATTR  GSY_FREE_LOWER_RXMEM(
    LSA_UINT16                 GSY_LOCAL_MEM_ATTR *     ret_val_ptr,
    GSY_LOWER_RXMEM_PTR_TYPE                            lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif


/*****************************************************************************/
/*  end of file GSY_LOW.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of GSY_LOW_H */

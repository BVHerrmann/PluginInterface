#ifndef PERIF_XRSL_H
#define PERIF_XRSL_H

#ifdef __cplusplus
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
/*  C o m p o n e n t     &C: PNExplorer                                :C&  */
/*                                                                           */
/*  F i l e               &F: perif_xrsl.h								 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*  P a r a m e t e r s     : Version = 2, Ports = , Interface =  */
/*****************************************************************************/


/**
@defgroup       COMMAND_IF_CONTROL_APP COMMAND_IF_CONTROL_APP
@ingroup        Register
@brief          Command Interface Control
@param Address  0x00000000
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_CONTROL_APP              0x00000000

/**
@defgroup       COMMAND_IF_CONTROL_APP__CR_NUMBER CR_NUMBER
@ingroup        COMMAND_IF_CONTROL_APP
@brief          CR Number  
@param Address  0x00000000
@param Mask     0x0000001F
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_CONTROL_APP__MSK_CR_NUMBER       0x0000001F
#define PERIF_REG_COMMAND_IF_CONTROL_APP_SHFT_CR_NUMBER       0
#define PERIF_REG_COMMAND_IF_CONTROL_APP_RSTV_CR_NUMBER       0x00000000

/**
@defgroup       COMMAND_IF_CONTROL_APP__CONFREQUEST CONFREQUEST
@ingroup        COMMAND_IF_CONTROL_APP
@brief          Command Confirmation Request
@param Address  0x00000000
@param Mask     0x00000020
@param Shift    5
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_CONTROL_APP__MSK_CONFREQUEST       0x00000020
#define PERIF_REG_COMMAND_IF_CONTROL_APP_SHFT_CONFREQUEST       5
#define PERIF_REG_COMMAND_IF_CONTROL_APP_RSTV_CONFREQUEST       0x00000000

/**
@defgroup       COMMAND_IF_CONTROL_APP__F_CODE F_CODE
@ingroup        COMMAND_IF_CONTROL_APP
@brief          Consistence Commando
@param Address  0x00000000
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_CONTROL_APP__MSK_F_CODE       0x000000C0
#define PERIF_REG_COMMAND_IF_CONTROL_APP_SHFT_F_CODE       6
#define PERIF_REG_COMMAND_IF_CONTROL_APP_RSTV_F_CODE       0x00000000

/**
@defgroup       COMMAND_IF_CONTROL_APP__USER_ID USER_ID
@ingroup        COMMAND_IF_CONTROL_APP
@brief          User_ID
@param Address  0x00000000
@param Mask     0x0000E000
@param Shift    13
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_CONTROL_APP__MSK_USER_ID       0x0000E000
#define PERIF_REG_COMMAND_IF_CONTROL_APP_SHFT_USER_ID       13
#define PERIF_REG_COMMAND_IF_CONTROL_APP_RSTV_USER_ID       0x00000000


/**
@defgroup       COMMAND_IF_STATUS_APP COMMAND_IF_STATUS_APP
@ingroup        Register
@brief          Command Interface Status
@param Address  0x00000004
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_STATUS_APP              0x00000004

/**
@defgroup       COMMAND_IF_STATUS_APP__CR_NUMBER CR_NUMBER
@ingroup        COMMAND_IF_STATUS_APP
@brief          CR Number  
@param Address  0x00000004
@param Mask     0x0000001F
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_STATUS_APP__MSK_CR_NUMBER       0x0000001F
#define PERIF_REG_COMMAND_IF_STATUS_APP_SHFT_CR_NUMBER       0
#define PERIF_REG_COMMAND_IF_STATUS_APP_RSTV_CR_NUMBER       0x00000000

/**
@defgroup       COMMAND_IF_STATUS_APP__CONFRESPONSE CONFRESPONSE
@ingroup        COMMAND_IF_STATUS_APP
@brief          ConfResponse
@param Address  0x00000004
@param Mask     0x00000020
@param Shift    5
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_STATUS_APP__MSK_CONFRESPONSE       0x00000020
#define PERIF_REG_COMMAND_IF_STATUS_APP_SHFT_CONFRESPONSE       5
#define PERIF_REG_COMMAND_IF_STATUS_APP_RSTV_CONFRESPONSE       0x00000000

/**
@defgroup       COMMAND_IF_STATUS_APP__F_CODE F_CODE
@ingroup        COMMAND_IF_STATUS_APP
@brief          Consistence Commando
@param Address  0x00000004
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_STATUS_APP__MSK_F_CODE       0x000000C0
#define PERIF_REG_COMMAND_IF_STATUS_APP_SHFT_F_CODE       6
#define PERIF_REG_COMMAND_IF_STATUS_APP_RSTV_F_CODE       0x00000000

/**
@defgroup       COMMAND_IF_STATUS_APP__CONFVALUE CONFVALUE
@ingroup        COMMAND_IF_STATUS_APP
@brief          Command Confirmation Value
@param Address  0x00000004
@param Mask     0x00000400
@param Shift    10
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_STATUS_APP__MSK_CONFVALUE       0x00000400
#define PERIF_REG_COMMAND_IF_STATUS_APP_SHFT_CONFVALUE       10
#define PERIF_REG_COMMAND_IF_STATUS_APP_RSTV_CONFVALUE       0x00000000

/**
@defgroup       COMMAND_IF_STATUS_APP__USER_ID USER_ID
@ingroup        COMMAND_IF_STATUS_APP
@brief          User_ID
@param Address  0x00000004
@param Mask     0x0000E000
@param Shift    13
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_COMMAND_IF_STATUS_APP__MSK_USER_ID       0x0000E000
#define PERIF_REG_COMMAND_IF_STATUS_APP_SHFT_USER_ID       13
#define PERIF_REG_COMMAND_IF_STATUS_APP_RSTV_USER_ID       0x00000000


/**
@defgroup       HOST_IRQ_LOW HOST_IRQ_LOW
@ingroup        Register
@brief          Host_IRQ_low
@param Address  0x00000008
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQ_LOW              0x00000008
#define PERIF_REG_HOST_IRQ_LOW_RST__VAL     0x00000000

/**
@defgroup       HOST_IRQ_LOW__IRQ_BITS IRQ_BITS
@ingroup        HOST_IRQ_LOW
@brief          IRQ_Bits '0'=PN_Event_low='0' OR Host_IRQmask_low='1' '1'=PN_Event_low='01' AND Host_IRQmask_low='0'
@param Address  0x00000008
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQ_LOW__MSK_IRQ_BITS       0xFFFFFFFF
#define PERIF_REG_HOST_IRQ_LOW_SHFT_IRQ_BITS       0
#define PERIF_REG_HOST_IRQ_LOW_RSTV_IRQ_BITS       0x00000000


/**
@defgroup       HOST_IRQ_HIGH HOST_IRQ_HIGH
@ingroup        Register
@brief          Host_IRQ_high
@param Address  0x0000000C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQ_HIGH              0x0000000C
#define PERIF_REG_HOST_IRQ_HIGH_RST__VAL     0x00000000

/**
@defgroup       HOST_IRQ_HIGH__IRQ_BITS IRQ_BITS
@ingroup        HOST_IRQ_HIGH
@brief          IRQ_Bits '0'=PN_Event_high='0' OR Host_IRQmask_high='1' '1'=PN_Event_high='01' AND Host_IRQmask_high='0'
@param Address  0x0000000C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQ_HIGH__MSK_IRQ_BITS       0xFFFFFFFF
#define PERIF_REG_HOST_IRQ_HIGH_SHFT_IRQ_BITS       0
#define PERIF_REG_HOST_IRQ_HIGH_RSTV_IRQ_BITS       0x00000000


/**
@defgroup       HOST_IRQMASK_LOW HOST_IRQMASK_LOW
@ingroup        Register
@brief          Host_IRQmask_low
@param Address  0x00000010
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQMASK_LOW              0x00000010
#define PERIF_REG_HOST_IRQMASK_LOW_RST__VAL     0xFFFFFFFF

/**
@defgroup       HOST_IRQMASK_LOW__MASK_BITS MASK_BITS
@ingroup        HOST_IRQMASK_LOW
@brief          Mask_Bits '0'=das event wird in PN_IRQ_low eingetragen '1'=das event wird nicht in PN_IRQ_low eingetragen
@param Address  0x00000010
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0xFFFFFFFF
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQMASK_LOW__MSK_MASK_BITS       0xFFFFFFFF
#define PERIF_REG_HOST_IRQMASK_LOW_SHFT_MASK_BITS       0
#define PERIF_REG_HOST_IRQMASK_LOW_RSTV_MASK_BITS       0xFFFFFFFF


/**
@defgroup       HOST_IRQMASK_HIGH HOST_IRQMASK_HIGH
@ingroup        Register
@brief          Host_IRQmask_high
@param Address  0x00000014
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQMASK_HIGH              0x00000014
#define PERIF_REG_HOST_IRQMASK_HIGH_RST__VAL     0xFFFFFFFF

/**
@defgroup       HOST_IRQMASK_HIGH__MASK_BITS MASK_BITS
@ingroup        HOST_IRQMASK_HIGH
@brief          Mask_Bits '0'=das event wird in PN_IRQ_high eingetragen '1'=das event wird nicht in PN_IRQ_high eingetragen
@param Address  0x00000014
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0xFFFFFFFF
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQMASK_HIGH__MSK_MASK_BITS       0xFFFFFFFF
#define PERIF_REG_HOST_IRQMASK_HIGH_SHFT_MASK_BITS       0
#define PERIF_REG_HOST_IRQMASK_HIGH_RSTV_MASK_BITS       0xFFFFFFFF


/**
@defgroup       HOST_EVENT_LOW HOST_EVENT_LOW
@ingroup        Register
@brief          Host_Event_low
@param Address  0x00000018
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_EVENT_LOW              0x00000018
#define PERIF_REG_HOST_EVENT_LOW_RST__VAL     0x00000000

/**
@defgroup       HOST_EVENT_LOW__EVENT_BITS EVENT_BITS
@ingroup        HOST_EVENT_LOW
@brief          Mask_Bits '0'=das event wird in PN_IRQ_low eingetragen '1'=das event wird nicht in PN_IRQ_low eingetragen
@param Address  0x00000018
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_EVENT_LOW__MSK_EVENT_BITS       0xFFFFFFFF
#define PERIF_REG_HOST_EVENT_LOW_SHFT_EVENT_BITS       0
#define PERIF_REG_HOST_EVENT_LOW_RSTV_EVENT_BITS       0x00000000


/**
@defgroup       HOST_EVENT_HIGH HOST_EVENT_HIGH
@ingroup        Register
@brief          Host_Event_high
@param Address  0x0000001C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_EVENT_HIGH              0x0000001C
#define PERIF_REG_HOST_EVENT_HIGH_RST__VAL     0x00000000

/**
@defgroup       HOST_EVENT_HIGH__EVENT_BITS EVENT_BITS
@ingroup        HOST_EVENT_HIGH
@brief          Mask_Bits '0'=das event wird in PN_IRQ_low eingetragen '1'=das event wird nicht in PN_IRQ_low eingetragen
@param Address  0x0000001C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_EVENT_HIGH__MSK_EVENT_BITS       0xFFFFFFFF
#define PERIF_REG_HOST_EVENT_HIGH_SHFT_EVENT_BITS       0
#define PERIF_REG_HOST_EVENT_HIGH_RSTV_EVENT_BITS       0x00000000


/**
@defgroup       HOST_IRQACK_LOW HOST_IRQACK_LOW
@ingroup        Register
@brief          Host_IRQack_low
@param Address  0x00000020
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQACK_LOW              0x00000020
#define PERIF_REG_HOST_IRQACK_LOW_RST__VAL     0x00000000

/**
@defgroup       HOST_IRQACK_LOW__ACK_BITS ACK_BITS
@ingroup        HOST_IRQACK_LOW
@brief          Ack_Bits write   '0'=das event-bit wird in PN_Event_low nicht zurueckgesetzt '1'=das event-bit wird in PN_Event_low zurueckgesetzt
@param Address  0x00000020
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQACK_LOW__MSK_ACK_BITS       0xFFFFFFFF
#define PERIF_REG_HOST_IRQACK_LOW_SHFT_ACK_BITS       0
#define PERIF_REG_HOST_IRQACK_LOW_RSTV_ACK_BITS       0x00000000


/**
@defgroup       HOST_IRQACK_HIGH HOST_IRQACK_HIGH
@ingroup        Register
@brief          Host_IRQack_high
@param Address  0x00000024
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQACK_HIGH              0x00000024
#define PERIF_REG_HOST_IRQACK_HIGH_RST__VAL     0x00000000

/**
@defgroup       HOST_IRQACK_HIGH__ACK_BITS ACK_BITS
@ingroup        HOST_IRQACK_HIGH
@brief          Ack_Bits write   '0'=das event-bit wird in PN_Event_low nicht zurueckgesetzt '1'=das event-bit wird in PN_Event_low zurueckgesetzt
@param Address  0x00000024
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_IRQACK_HIGH__MSK_ACK_BITS       0xFFFFFFFF
#define PERIF_REG_HOST_IRQACK_HIGH_SHFT_ACK_BITS       0
#define PERIF_REG_HOST_IRQACK_HIGH_RSTV_ACK_BITS       0x00000000


/**
@defgroup       HOST_EOI HOST_EOI
@ingroup        Register
@brief          Host_EOI
@param Address  0x00000028
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_EOI              0x00000028
#define PERIF_REG_HOST_EOI_RST__VAL     0x00000000

/**
@defgroup       HOST_EOI__WAIT_TIME WAIT_TIME
@ingroup        HOST_EOI
@brief          wait_time
@param Address  0x00000028
@param Mask     0x0003FFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_HOST_EOI__MSK_WAIT_TIME       0x0003FFFF
#define PERIF_REG_HOST_EOI_SHFT_WAIT_TIME       0
#define PERIF_REG_HOST_EOI_RSTV_WAIT_TIME       0x00000000


/**
@defgroup       PN_IRQ_LOW PN_IRQ_LOW
@ingroup        Register
@brief          PN_IRQ_low
@param Address  0x0000002C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQ_LOW              0x0000002C
#define PERIF_REG_PN_IRQ_LOW_RST__VAL     0x00000000

/**
@defgroup       PN_IRQ_LOW__IRQ_BITS IRQ_BITS
@ingroup        PN_IRQ_LOW
@brief          IRQ_Bits '0'=PN_Event_low='0' OR PN_IRQmask_low='1' '1'=PN_Event_low='01' AND PN_IRQmask_low='0'
@param Address  0x0000002C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQ_LOW__MSK_IRQ_BITS       0xFFFFFFFF
#define PERIF_REG_PN_IRQ_LOW_SHFT_IRQ_BITS       0
#define PERIF_REG_PN_IRQ_LOW_RSTV_IRQ_BITS       0x00000000


/**
@defgroup       PN_IRQ_HIGH PN_IRQ_HIGH
@ingroup        Register
@brief          PN_IRQ_high
@param Address  0x00000030
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQ_HIGH              0x00000030
#define PERIF_REG_PN_IRQ_HIGH_RST__VAL     0x00000000

/**
@defgroup       PN_IRQ_HIGH__IRQ_BITS IRQ_BITS
@ingroup        PN_IRQ_HIGH
@brief          IRQ_Bits '0'=PN_Event_high='0' OR PN_IRQmask_high='1' '1'=PN_Event_high='01' AND PN_IRQmask_high='0'
@param Address  0x00000030
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQ_HIGH__MSK_IRQ_BITS       0xFFFFFFFF
#define PERIF_REG_PN_IRQ_HIGH_SHFT_IRQ_BITS       0
#define PERIF_REG_PN_IRQ_HIGH_RSTV_IRQ_BITS       0x00000000


/**
@defgroup       PN_IRQMASK_LOW PN_IRQMASK_LOW
@ingroup        Register
@brief          PN_IRQmask_low
@param Address  0x00000034
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQMASK_LOW              0x00000034
#define PERIF_REG_PN_IRQMASK_LOW_RST__VAL     0xFFFFFFFF

/**
@defgroup       PN_IRQMASK_LOW__MASK_BITS MASK_BITS
@ingroup        PN_IRQMASK_LOW
@brief          Mask_Bits '0'=das event wird in PN_IRQ_low eingetragen '1'=das event wird nicht in PN_IRQ_low eingetragen
@param Address  0x00000034
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0xFFFFFFFF
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQMASK_LOW__MSK_MASK_BITS       0xFFFFFFFF
#define PERIF_REG_PN_IRQMASK_LOW_SHFT_MASK_BITS       0
#define PERIF_REG_PN_IRQMASK_LOW_RSTV_MASK_BITS       0xFFFFFFFF


/**
@defgroup       PN_IRQMASK_HIGH PN_IRQMASK_HIGH
@ingroup        Register
@brief          PN_IRQmask_high
@param Address  0x00000038
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQMASK_HIGH              0x00000038
#define PERIF_REG_PN_IRQMASK_HIGH_RST__VAL     0xFFFFFFFF

/**
@defgroup       PN_IRQMASK_HIGH__MASK_BITS MASK_BITS
@ingroup        PN_IRQMASK_HIGH
@brief          Mask_Bits '0'=das event wird in PN_IRQ_high eingetragen '1'=das event wird nicht in PN_IRQ_high eingetragen
@param Address  0x00000038
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0xFFFFFFFF
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQMASK_HIGH__MSK_MASK_BITS       0xFFFFFFFF
#define PERIF_REG_PN_IRQMASK_HIGH_SHFT_MASK_BITS       0
#define PERIF_REG_PN_IRQMASK_HIGH_RSTV_MASK_BITS       0xFFFFFFFF


/**
@defgroup       PN_EVENT_LOW PN_EVENT_LOW
@ingroup        Register
@brief          PN_Event_low
@param Address  0x0000003C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_EVENT_LOW              0x0000003C
#define PERIF_REG_PN_EVENT_LOW_RST__VAL     0x00000000

/**
@defgroup       PN_EVENT_LOW__EVENT_BITS EVENT_BITS
@ingroup        PN_EVENT_LOW
@brief          Event_Bits_low
@param Address  0x0000003C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_EVENT_LOW__MSK_EVENT_BITS       0xFFFFFFFF
#define PERIF_REG_PN_EVENT_LOW_SHFT_EVENT_BITS       0
#define PERIF_REG_PN_EVENT_LOW_RSTV_EVENT_BITS       0x00000000


/**
@defgroup       PN_EVENT_HIGH PN_EVENT_HIGH
@ingroup        Register
@brief          PN_Event_high
@param Address  0x00000040
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_EVENT_HIGH              0x00000040
#define PERIF_REG_PN_EVENT_HIGH_RST__VAL     0x00000000

/**
@defgroup       PN_EVENT_HIGH__EVENT_BITS EVENT_BITS
@ingroup        PN_EVENT_HIGH
@brief          Event_Bits high
@param Address  0x00000040
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_EVENT_HIGH__MSK_EVENT_BITS       0xFFFFFFFF
#define PERIF_REG_PN_EVENT_HIGH_SHFT_EVENT_BITS       0
#define PERIF_REG_PN_EVENT_HIGH_RSTV_EVENT_BITS       0x00000000


/**
@defgroup       PN_IRQACK_LOW PN_IRQACK_LOW
@ingroup        Register
@brief          PN_IRQack_low
@param Address  0x00000044
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQACK_LOW              0x00000044
#define PERIF_REG_PN_IRQACK_LOW_RST__VAL     0x00000000

/**
@defgroup       PN_IRQACK_LOW__ACK_BITS ACK_BITS
@ingroup        PN_IRQACK_LOW
@brief          Ack_Bits write   '0'=das event-bit wird in PN_Event_low nicht zurueckgesetzt '1'=das event-bit wird in PN_Event_low zurueckgesetzt
@param Address  0x00000044
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQACK_LOW__MSK_ACK_BITS       0xFFFFFFFF
#define PERIF_REG_PN_IRQACK_LOW_SHFT_ACK_BITS       0
#define PERIF_REG_PN_IRQACK_LOW_RSTV_ACK_BITS       0x00000000


/**
@defgroup       PN_IRQACK_HIGH PN_IRQACK_HIGH
@ingroup        Register
@brief          PN_IRQack_high
@param Address  0x00000048
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQACK_HIGH              0x00000048
#define PERIF_REG_PN_IRQACK_HIGH_RST__VAL     0x00000000

/**
@defgroup       PN_IRQACK_HIGH__ACK_BITS ACK_BITS
@ingroup        PN_IRQACK_HIGH
@brief          Ack_Bits write   '0'=das event-bit wird in PN_Event_low nicht zurueckgesetzt '1'=das event-bit wird in PN_Event_low zurueckgesetzt
@param Address  0x00000048
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_IRQACK_HIGH__MSK_ACK_BITS       0xFFFFFFFF
#define PERIF_REG_PN_IRQACK_HIGH_SHFT_ACK_BITS       0
#define PERIF_REG_PN_IRQACK_HIGH_RSTV_ACK_BITS       0x00000000


/**
@defgroup       PN_EOI PN_EOI
@ingroup        Register
@brief          PN_EOI 
@param Address  0x0000004C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_EOI              0x0000004C
#define PERIF_REG_PN_EOI_RST__VAL     0x00000000

/**
@defgroup       PN_EOI__WAIT_TIME WAIT_TIME
@ingroup        PN_EOI
@brief          wait_time
@param Address  0x0000004C
@param Mask     0x0003FFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define PERIF_REG_PN_EOI__MSK_WAIT_TIME       0x0003FFFF
#define PERIF_REG_PN_EOI_SHFT_WAIT_TIME       0
#define PERIF_REG_PN_EOI_RSTV_WAIT_TIME       0x00000000

/**
@defgroup        PERIF_IO_RAM PERIF_IO_RAM
@ingroup         Memory
@brief           Zugriff auf das IO-RAM auf 0x2000 - 0x2FFF: Zugriff mit Address-Substitute 0x3000 - 0x4FFF: direkter RAM-Zugriff. Hinweis: Im Page-Mode erfolgt der Zugriff auf das IO-RAM ueber spezielle Adressen.
@param Address   0x00002000 
@param Length    0x00003000
@param Access    r,w
@param Version   1
@param Type      -1
*/
#define PERIF_R1_RAM_PERIF_IO_RAM_START     0x00002000
#define PERIF_R1_RAM_PERIF_IO_RAM_END       0x00004FFF
#define PERIF_R1_RAM_PERIF_IO_RAM_LEN       0x00003000

/**
@defgroup        PERIF_IO_RAM PERIF_IO_RAM
@ingroup         Memory
@brief           Zugriff auf das IO-RAM auf 0x2000 - 0x2FFF: Zugriff mit Address-Substitute 0x3000 - 0x5FFF: direkter RAM-Zugriff. Hinweis: Im Page-Mode erfolgt der Zugriff auf das IO-RAM ueber spezielle Adressen.
@param Address   0x00002000 
@param Length    0x00004000
@param Access    r,w
@param Version   2
@param Type      -1
*/
#define PERIF_R2_RAM_PERIF_IO_RAM_START     0x00002000
#define PERIF_R2_RAM_PERIF_IO_RAM_END       0x00005FFF
#define PERIF_R2_RAM_PERIF_IO_RAM_LEN       0x00004000

#ifdef __cplusplus
}
#endif

#endif

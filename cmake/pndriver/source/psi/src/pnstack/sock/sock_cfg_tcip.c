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
/*  F i l e               &F: sock_cfg_tcip.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of SOCK using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   2801
#define PSI_MODULE_ID       2801 /* PSI_MODULE_ID_SOCK_CFG_TCIP */

#include "psi_int.h"

#if ((PSI_CFG_USE_LD_COMP == 1) && (PSI_CFG_USE_TCIP == 0))

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

#ifndef SOCK_GET_ANY_MAC_ADDR
LSA_BOOL SOCK_GET_ANY_MAC_ADDR(
	LSA_UINT32*     ptr_interface_id,
	LSA_UINT8      *ptr,
	LSA_UINT32      size )
{
	LSA_UINT16   idx;
	PSI_MAC_TYPE any_mac;

	PSI_ASSERT( size == 6 );

	// reads the IF MAC of first HD interface (for ANY MAC)
	psi_hd_get_if_mac (1, ptr_interface_id, &any_mac);
	PSI_ASSERT( *ptr_interface_id != 0 );

	for ( idx = 0; idx < size; idx++ )
	{
		ptr[idx] = any_mac[idx];
	}

	return( LSA_TRUE );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_SET_IPSUITE
LSA_BOOL SOCK_SET_IPSUITE(
	LSA_UINT32    IfID,         /* If ID to requesting interface   */
	SockIF_IPAddr ip_addr,      /* IP address to be set            */
	SockIF_IPAddr net_mask )     /*  associated net mask            */
{
	LSA_BOOL result = PSI_SOCK_SET_IPSUITE( IfID, ip_addr.s_addr, net_mask.s_addr, 0 );

	return result;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_SET_ROUTE
LSA_UINT8 SOCK_SET_ROUTE(
	SockIF_IPAddr target,
	SockIF_IPAddr netmask,
	SockIF_IPAddr gateway
) {
	LSA_UNUSED_ARG(target);
	LSA_UNUSED_ARG(netmask);
	LSA_UNUSED_ARG(gateway);

	return SOCK_SET_ROUTE_OK;
}
#endif

/*===========================================================================*/

#ifndef SOCK_SET_MIB2_IFGROUP_DATA
LSA_BOOL SOCK_SET_MIB2_IFGROUP_DATA(
	LSA_UINT32     IfID,  /* If ID to requesting interface  */
	LSA_UINT16     PortId,
	SOCK_NAME_TYPE IfDescr )
{
	LSA_BOOL           result;
	PSI_SOCK_NAME_TYPE ifName;

	ifName.pName    = IfDescr.pName;
	ifName.Length   = IfDescr.Length;
	ifName.Remanent = IfDescr.Remanent;

	result = PSI_SOCK_SET_MIB2_IFGROUP_DATA( IfID, PortId, ifName );

	return result;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_SET_MIB2_SYSGROUP_DATA
LSA_BOOL SOCK_SET_MIB2_SYSGROUP_DATA(
	SOCK_NAME_TYPE SysName,
	SOCK_NAME_TYPE SysDescr,
	SOCK_NAME_TYPE SysContact,
	SOCK_NAME_TYPE SysLocation )
{
	LSA_BOOL           result;
	PSI_SOCK_NAME_TYPE sys_name, sys_descr, sys_con, sys_loc;

	sys_name.pName    = SysName.pName;
	sys_name.Length   = SysName.Length;
	sys_name.Remanent = SysName.Remanent;

	sys_descr.pName    = SysDescr.pName;
	sys_descr.Length   = SysDescr.Length;
	sys_descr.Remanent = SysDescr.Remanent;

	sys_con.pName    = SysContact.pName;
	sys_con.Length   = SysContact.Length;
	sys_con.Remanent = SysContact.Remanent;

	sys_loc.pName    = SysLocation.pName;
	sys_loc.Length   = SysLocation.Length;
	sys_loc.Remanent = SysLocation.Remanent;

	result = PSI_SOCK_SET_MIB2_SYSGROUP_DATA( sys_name, sys_descr, sys_con, sys_loc );

	return result;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_GET_MIB2_IFINDEX
LSA_BOOL SOCK_GET_MIB2_IFINDEX(
	LSA_UINT32   IfID,
	LSA_UINT16   PortID,
	LSA_UINT32 * p_IfIndex )
{
	LSA_BOOL result = PSI_SOCK_GET_MIB2_IFINDEX( IfID, PortID, p_IfIndex );

	return result;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_TCPIP_SET_TIMEPARAMS
LSA_BOOL SOCK_TCPIP_SET_TIMEPARAMS(
	LSA_UINT32 KeepAliveTime,
	LSA_UINT32 ConnectionTimeout,
	LSA_UINT32 RetransTimeout,
	LSA_UINT32 RetransCount )
{
	LSA_BOOL result = PSI_SOCK_TCPIP_SET_TIMEPARAMS( KeepAliveTime, ConnectionTimeout, RetransTimeout, RetransCount );

	return result;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_TCPIP_GET_TIMEPARAMS
LSA_BOOL SOCK_TCPIP_GET_TIMEPARAMS(
	LSA_UINT32 * KeepAliveTime,
	LSA_UINT32 * ConnectionTimeout,
	LSA_UINT32 * RetransTimeout,
	LSA_UINT32 * RetransCount )
{
	LSA_BOOL result = PSI_SOCK_TCPIP_GET_TIMEPARAMS( KeepAliveTime, ConnectionTimeout, RetransTimeout, RetransCount );

	return result;
}
#else
#error "by design a function!"
#endif

#endif // #if ((PSI_CFG_USE_LD_COMP == 1) && (PSI_CFG_USE_TCIP == 0))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

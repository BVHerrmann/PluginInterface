#ifndef PSI_PLS_H                   /* ----- reinclude-protection ----- */
#define PSI_PLS_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_pls.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Check the PSI configuration.                                             */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* General                                                                   */
/*---------------------------------------------------------------------------*/

#if defined(PSI_CFG_MAX_IF_CNT)
    #if ((PSI_CFG_MAX_IF_CNT < 1) || (PSI_CFG_MAX_IF_CNT > 4))
        #error "PSI_CFG_MAX_IF_CNT is out of valid range [1..4]"
    #endif
#else
    #error "PSI_CFG_MAX_IF_CNT not defined"
#endif

#if defined (PSI_CFG_USE_IOD_MC)
    #if (PSI_CFG_USE_IOD_MC != 0)
        #error "don't activate, feature IOD MC not released"
    #endif
#endif

#ifndef PSI_CFG_TRACE_MODE
    #error "config error, PSI_CFG_TRACE_MODE not defined"
    #else
    #if (PSI_CFG_TRACE_MODE == 1)
        #error "config error, PSI_CFG_TRACE_MODE not set to OFF, or indexed trace"
    #endif
#endif
   
#if (PSI_CFG_USE_LD_COMP == 0) && (PSI_CFG_USE_HD_COMP == 0)
    #if(PSI_CFG_USE_HIF_LD == 0)
        #error "config error, PSI_CFG_USE_HIF_LD has to be set when PSI_CFG_USE_LD_COMP and PSI_CFG_USE_HD_COMP is not set"
    #endif
#endif

#if (PSI_CFG_USE_LD_COMP == 1) && (PSI_CFG_USE_HD_COMP == 0)
    #if(PSI_CFG_USE_HIF_HD == 0)
        #error "config error, PSI_CFG_USE_HIF_HD has to be set when PSI_CFG_USE_LD_COMP is set and PSI_CFG_USE_HD_COMP is not set"
    #endif
#endif

#if (PSI_CFG_USE_LD_COMP == 0) && (PSI_CFG_USE_HD_COMP == 1)
    #if(PSI_CFG_USE_HIF_HD == 0)
        #error "config error, PSI_CFG_USE_HIF_HD has to be set when PSI_CFG_USE_LD_COMP is not set and PSI_CFG_USE_HD_COMP is set"
    #endif
#endif

/*---------------------------------------------------------------------------*/
/* EDDX                                                                      */
/*---------------------------------------------------------------------------*/
    
#if (PSI_HD_FEATURE_IRT_FORWARDING_MODE_ABSOLUTE != EDD_DPB_FEATURE_IRTFWDMODE_SUPPORTED_ABSOLUTE)
    #error "Values must be equal since PSI copies the input values to DPB structure"
#endif
    
#if (PSI_HD_FEATURE_IRT_FORWARDING_MODE_RELATIVE != EDD_DPB_FEATURE_IRTFWDMODE_SUPPORTED_RELATIVE)
    #error "Values must be equal since PSI copies the input values to DPB structure"
#endif

#if (PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_NO != EDD_DPB_FEATURE_FRAGTYPE_SUPPORTED_NO)
#error "Values must be equal since PSI copies the input values to DPB structure"
#endif

#if (PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_STATIC != EDD_DPB_FEATURE_FRAGTYPE_SUPPORTED_STATIC)
#error "Values must be equal since PSI copies the input values to DPB structure"
#endif

#if (PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_DYNAMIC != EDD_DPB_FEATURE_FRAGTYPE_SUPPORTED_DYNAMIC)
#error "Values must be equal since PSI copies the input values to DPB structure"
#endif

/*---------------------------------------------------------------------------*/
/* SNMPX                                                                     */
/*---------------------------------------------------------------------------*/
#if defined(PSI_CFG_USE_SNMPX_AGT)
    #if (PSI_CFG_USE_SNMPX_AGT != 0)
        #error "don't activate, feature SNMPX AGENT not released"
    #endif
#endif

/*---------------------------------------------------------------------------*/
/* TRACE                                                                     */
/*---------------------------------------------------------------------------*/
#if !defined(PSI_CFG_TRACE_MODE)
    #error "config error, PSI_CFG_TRACE_MODE not defined"
#endif

/*---------------------------------------------------------------------------*/
/* SOCK                                                                      */
/*---------------------------------------------------------------------------*/

#if ( (!defined(PSI_CFG_MAX_SOCK_APP_CHANNELS)) || (PSI_CFG_MAX_SOCK_APP_CHANNELS < 1) || (PSI_CFG_MAX_SOCK_APP_CHANNELS > 8) )
    #error "PSI_CFG_MAX_SOCK_APP_CHANNELS not defined or out of range (1..8)"
#endif

#if (!defined (PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS)) || !((PSI_CFG_MAX_SOCK_APP_CHANNELS >= 0) && (PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS <= 4))
#error "PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS not defined or out of range (0..4)"
#endif

/*---------------------------------------------------------------------------*/
/* TCIP                                                                      */
/*---------------------------------------------------------------------------*/
#ifndef PSI_CFG_TCIP_CFG_COPY_ON_SEND
    #error "PSI_CFG_TCIP_CFG_COPY_ON_SEND not defined"
#endif

#if defined(PSI_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT) && (PSI_CFG_TCIP_CFG_COPY_ON_SEND != 1)
    #error "PSI_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT is only allowed in combination with PSI_CFG_TCIP_CFG_COPY_ON_SEND"
#endif

#if ( (EDD_CFG_MAX_INTERFACE_CNT > 1) && (!defined (PSI_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT)) )
    #error "EDD_CFG_MAX_INTERFACE_CNT > 1 but PSI_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT not defined"
#endif

#if ( (EDD_CFG_MAX_INTERFACE_CNT > 1) && (PSI_CFG_TCIP_CFG_COPY_ON_SEND != 1) )
    #error "EDD_CFG_MAX_INTERFACE_CNT > 1 and PSI_CFG_TCIP_CFG_COPY_ON_SEND != 1"
#endif

#if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1)
    #if (PSI_CFG_TCIP_CFG_COPY_ON_SEND != 1)
        #error "OpenBSD and PSI_CFG_TCIP_CFG_COPY_ON_SEND != 1"
    #endif
    #if ((PSI_CFG_TCIP_STACK_INTERNICHE == 1) || (PSI_CFG_TCIP_STACK_WINSOCK == 1) || (PSI_CFG_TCIP_STACK_CUSTOM == 1) )
    #error "Invalid configuration. Only one IP stack may be configured"
    #endif
#endif

#if (PSI_CFG_TCIP_STACK_INTERNICHE == 1)
    #if ((PSI_CFG_TCIP_STACK_OPEN_BSD == 1) || (PSI_CFG_TCIP_STACK_WINSOCK == 1) || (PSI_CFG_TCIP_STACK_CUSTOM == 1) )
    #error "Invalid configuration. Only one IP stack may be configured"
    #endif
#endif

#if (PSI_CFG_TCIP_STACK_WINSOCK == 1)
    #if ((PSI_CFG_TCIP_STACK_OPEN_BSD == 1) || (PSI_CFG_TCIP_STACK_INTERNICHE == 1) || (PSI_CFG_TCIP_STACK_CUSTOM == 1) )
    #error "Invalid configuration. Only one IP stack may be configured"
    #endif
#endif
    
#if (PSI_CFG_TCIP_STACK_CUSTOM == 1)
    #if ((PSI_CFG_TCIP_STACK_OPEN_BSD == 1) || (PSI_CFG_TCIP_STACK_INTERNICHE == 1) || (PSI_CFG_TCIP_STACK_WINSOCK == 1) )
    #error "Invalid configuration. Only one IP stack may be configured"
    #endif
#endif
    
#if (((PSI_CFG_TCIP_STACK_OPEN_BSD)   == 0) && ((PSI_CFG_TCIP_STACK_INTERNICHE) == 0) && ((PSI_CFG_TCIP_STACK_WINSOCK) == 0) && ((PSI_CFG_TCIP_STACK_CUSTOM) == 0))
    #error "Invalid configuration, set at least one IP stack"
#endif

/*---------------------------------------------------------------------------*/
/* EDDx                                                                      */
/*---------------------------------------------------------------------------*/
#if (PSI_CFG_USE_EDDI == 0) && (PSI_CFG_USE_EDDP == 0) && (PSI_CFG_USE_EDDS == 0) && (PSI_CFG_USE_EDDT == 0)
    #error "neither PSI_CFG_USE_EDDI nor PSI_CFG_USE_EDDP nor PSI_CFG_USE_EDDS nor PSI_CFG_USE_EDDT is defined"
#endif

#if (PSI_CFG_USE_EDDP == 1) && !((defined PSI_CFG_EDDP_CFG_HW_HERA_SUPPORT) || (defined PSI_CFG_EDDP_CFG_HW_ERTEC200P_SUPPORT))
    #error "when EDDP is used, either HERA or ERTEC200P or both must be supported"
#endif

/*---------------------------------------------------------------------------*/
/* HIF                                                                       */
/*---------------------------------------------------------------------------*/
#if ( (PSI_CFG_USE_HIF == 0) && (defined(PSI_CFG_USE_HIF_HD) && (PSI_CFG_USE_HIF_HD == 1)) )
    #error "PSI_CFG_USE_HIF_HD is only allowed in combination with PSI_CFG_USE_HIF"
#endif

/*---------------------------------------------------------------------------*/
/* HIF dynamic - Check OPEN_CHANNEL-Opcodes of components to be recognized   */
/*               properly                                                    */
/*---------------------------------------------------------------------------*/
#if ((ACP_OPC_OPEN_CHANNEL) != (PSI_OPC_GENERAL_OPEN_CHANNEL))
    #error "ACP_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
#endif

#if ((CM_OPC_OPEN_CHANNEL) != (PSI_OPC_GENERAL_OPEN_CHANNEL))
    #error "CM_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
#endif

// Not possible cause cast in DCP_OPC_xxx
//#if ((DCP_OPC_OPEN_CHANNEL) == (PSI_OPC_GENERAL_OPEN_CHANNEL))
//    #error "DCP_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
//#endif

// Not possible cause cast in EDD_OPC_xxx
//#if ( ((PSI_CFG_USE_EDDI) == 1 || (PSI_CFG_USE_EDDP) == 1 || (PSI_CFG_USE_EDDS) == 1) || (PSI_CFG_USE_EDDT) == 1) && ((EDD_OPC_OPEN_CHANNEL) != (PSI_OPC_GENERAL_OPEN_CHANNEL)) )
//    #error "EDD_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
//#endif

#if ( ((PSI_CFG_USE_GSY) == 1) && ((GSY_OPC_OPEN_CHANNEL) != (PSI_OPC_GENERAL_OPEN_CHANNEL)) )
    #error "GSY_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
#endif

#if ( ((PSI_CFG_USE_IOH) == 1) && ((IOH_OPC_OPEN_CHANNEL) != (PSI_OPC_GENERAL_OPEN_CHANNEL)) )
    #error "IOH_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
#endif

// Not possible cause cast in LLDP_OPC_xxx
//#if ((LLDP_OPC_OPEN_CHANNEL) != (PSI_OPC_GENERAL_OPEN_CHANNEL))
//    #error "LLDP_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
//#endif

#if ( ((PSI_CFG_USE_MRP) == 1) && ((MRP_OPC_OPEN_CHANNEL) != (PSI_OPC_GENERAL_OPEN_CHANNEL)) )
    #error "MRP_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
#endif

// Not possible cause cast in NARE_OPC_xxx
//#if ((NARE_OPC_OPEN_CHANNEL) != (PSI_OPC_GENERAL_OPEN_CHANNEL))
//    #error "NARE_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
//#endif

// Not possible cause cast in POF_OPC_xxx
//#if ( ((PSI_CFG_USE_POF) == 1) && ((POF_OPC_OPEN_CHANNEL) != (PSI_OPC_GENERAL_OPEN_CHANNEL)) )
//    #error "POF_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
//#endif

#if ( ((PSI_CFG_USE_EPS_RQBS) == 1) && ((EPS_OPC_OPEN_CHANNEL) != (PSI_OPC_GENERAL_OPEN_CHANNEL)) )
    #error "EPS_OPC_OPEN_CHANNEL does not match PSI_OPC_GENERAL_OPEN_CHANNEL"
#endif

/*---------------------------------------------------------------------------*/
/* DNS                                                                       */
/*---------------------------------------------------------------------------*/
#if ( (PSI_CFG_USE_DNS == 1) && (PSI_CFG_TCIP_STACK_OPEN_BSD != 1) )
    #error "PSI_CFG_USE_DNS == 1 is only allowed in combination with PSI_CFG_TCIP_STACK_OPEN_BSD"
#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of PSI_PLS_H */

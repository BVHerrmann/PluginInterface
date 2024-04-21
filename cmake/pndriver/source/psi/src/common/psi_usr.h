#ifndef PSI_USR_H                   /* ----- reinclude-protection ----- */
#define PSI_USR_H

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
/*  F i l e               &F: psi_usr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  User interface                                                           */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcodes within RQB (System requests)                                      */
/*---------------------------------------------------------------------------*/

PTRC_SUBLABEL_LIST_OPEN("PSI_OPC")
#define PSI_OPC_LD_OPEN_DEVICE      0x01
#define PSI_OPC_LD_CLOSE_DEVICE     0x02
#define PSI_OPC_HD_OPEN_DEVICE      0x03
#define PSI_OPC_HD_CLOSE_DEVICE     0x04
PTRC_SUBLABEL_LIST_CLOSE("PSI_OPC")

/*---------------------------------------------------------------------------*/
/*  Response within RQB (Response)                                           */
/*---------------------------------------------------------------------------*/

PTRC_SUBLABEL_LIST_OPEN("PSI_RSP")
#define PSI_OK              (LSA_RSP_OK)            /* 0x00 or 0x01 */
#define PSI_OK_CANCEL       (LSA_RSP_OK_OFFSET + 1) /* 0x41 / 65    */

#define PSI_ERR_RESOURCE    (LSA_RSP_ERR_RESOURCE)  /* 0x84 / 132   */
#define PSI_ERR_PARAM       (LSA_RSP_ERR_PARAM)     /* 0x86 / 134   */
#define PSI_ERR_SEQUENCE    (LSA_RSP_ERR_SEQUENCE)  /* 0x87 / 135   */
#define PSI_ERR_SYS_PATH    (LSA_RSP_ERR_SYS_PATH)  /* 0x91 / 145   */
#define	PSI_ERR_OPCODE      (LSA_RSP_ERR_OFFSET)    /* 0xc0 / 192 wrong opcode */
PTRC_SUBLABEL_LIST_CLOSE("PSI_RSP")

/*===========================================================================*/
/*                                  defines                                  */
/*===========================================================================*/

#define PSI_INVALID_HANDLE                  ((LSA_HANDLE_TYPE)-1)

#define PSI_DEV_POOL_HANDLE_INVALID			((LSA_INT)0)
#define PSI_NRT_POOL_HANDLE_INVALID			((LSA_INT)0)
#define PSI_CRT_SLOW_POOL_HANDLE_INVALID    ((LSA_INT)0)
#define PSI_PI_POOL_HANDLE_INVALID          ((LSA_INT)0)

#define PSI_PROVIDER_GROUP_ID_CMCL      	0   // group ID for CM-CL Provider (IOC)
#define PSI_PROVIDER_GROUP_ID_CMSV      	1   // group ID for CM-SV Provider (IOD)
#define PSI_PROVIDER_GROUP_ID_CMMC      	2   // group ID for CM-MC Provider (IOM)

#define PSI_MAX_STATION_NAME_LEN        	255 // Max size of NoS (see oha_usr.h for OHA_MAX_STATION_NAME_LEN)

#define PSI_GLOBAL_PNIO_IF_NR           	0   // PNIO IF Number for global channels
#define PSI_MAX_PNIO_IF_NR              	16  // greatest PNIO IF Number for IF specific channels
                                            	// note IF Nr [1..16] (see possible EDD IF Nr)

/* Definition for external I2C HW multiplexer selection in PSI (=> EDD variable I2CMuxSelect) */
#define PSI_I2C_MUX_SELECT_NONE         	0   // Default: no I2C HW multiplexer used (i.e.: no POF port)
                                            	// otherwise: HwPortId is used for I2CMuxSelect for selection of an external I2C HW multiplexer

/* for "hd_runs_on_level_ld" */
#define PSI_HD_RUNS_ON_LEVEL_LD_NO          0   // HD is running not on LD level (separate system/firmware from LD)
#define PSI_HD_RUNS_ON_LEVEL_LD_YES         1   // HD is running on LD level (same system/firmware as LD)
#define PSI_HD_RUNS_ON_LEVEL_LD_MAX         2   // HD is running not on LD level (separate system/firmware from LD)
#define PSI_HD_CURRENT_HD                   0   // 0 is always the current HD

/* for "giga_bit_support" */
#define PSI_HD_GIGA_BIT_SUPPORT_ENABLE      1
#define PSI_HD_GIGA_BIT_SUPPORT_DISABLE     0

/* for "isRingPort" */
#define PSI_PORT_DEFAULT_RING_PORT          0
#define PSI_PORT_NO_DEFAULT_RING_PORT  		1

#define PSI_HSYNC_ROLE_NONE                 0   // HSYNC neither
#define PSI_HSYNC_ROLE_FORWARDER            1   // HSYNC Forwarder
#define PSI_HSYNC_ROLE_APPL_SUPPORT         2   // HSYNC Application

/* for "use_setting" */
#define PSI_USE_SETTING_YES             1 
#define PSI_USE_SETTING_NO              0 

#define PSI_FEATURE_ENABLE              1
#define PSI_FEATURE_DISABLE             0
/*-----------------------------------------------------------------------------
// forward declarations / common types
//---------------------------------------------------------------------------*/

typedef struct psi_rqb_tag * PSI_UPPER_RQB_PTR_TYPE;

/*===========================================================================*/
/*                              basic types for PSI                          */
/*===========================================================================*/

#define PSI_MAC_ADDR_SIZE   6
typedef LSA_UINT8   PSI_MAC_TYPE[PSI_MAC_ADDR_SIZE];  /* MAC Type */

/*----------------------------------------------------------------------------*/
/* Types for LD params                                                        */
/*----------------------------------------------------------------------------*/
typedef enum psi_ld_runs_type_enum
{
	PSI_LD_RUNS_ON_UNKNOWN  = 0x00, // reserved for invalid
	PSI_LD_RUNS_ON_LIGHT    = 0x01, // LD runs on application level (i.E.: pcIOX)
	PSI_LD_RUNS_ON_ADVANCED = 0x02, // LD runs on advanced level (i.E.: High perfomance system) 
	PSI_LD_RUNS_ON_BASIC    = 0x03, // LD runs on basic level (as FW on pnboard system)
	PSI_LD_RUNS_ON_MAX
} PSI_LD_RUNS_ON_TYPE;

// component ID
typedef enum psi_ld_opt_comp_enum
{
    PSI_DNS_COMP_IDX    = 0,
    PSI_LD_OPT_COMP_MAX
} PSI_LD_OPT_COMP_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for HD params                                                        */
/*----------------------------------------------------------------------------*/
typedef enum psi_asic_type_enum
{
    PSI_ASIC_TYPE_UNKNOWN           = 0x00, // reserved for invalid
    PSI_ASIC_TYPE_ERTEC200          = 0x01, // ERTEC200
    PSI_ASIC_TYPE_ERTEC400          = 0x02, // ERTEC400 
    PSI_ASIC_TYPE_SOC1              = 0x03, // SOC1
    PSI_ASIC_TYPE_FPGA_ERTEC200P    = 0x04, // FPGA-ERTEC200P (PN-IP)
    PSI_ASIC_TYPE_ERTEC200P         = 0x05, // ERTEC200P (PN-IP)
    PSI_ASIC_TYPE_INTEL_SPRINGVILLE = 0x06, // Intel I210 (Springville)
    PSI_ASIC_TYPE_INTEL_HARTWELL    = 0x07, // Intel I82574 (formerly Hartwell)
    PSI_ASIC_TYPE_KSZ88XX           = 0x08, // Micrel KSZ88XX (1/2 Ports)
    PSI_ASIC_TYPE_TI_AM5728         = 0x09, // TI AM5728
    PSI_ASIC_TYPE_HERA              = 0x0A, // HERA
    PSI_ASIC_TYPE_MAX
} PSI_ASIC_TYPE;

/*----------------------------------------------------------------------------*/
/* for calling of PSI_EDD_GET_MEDIA_TYPE() */
#define PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_YES      1   // execute POF port auto detection
#define PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_NO       0   // do not execute POF port auto detection

typedef enum psi_port_media_type_enum
{
	PSI_MEDIA_TYPE_UNKNOWN  = 0x00,     // reserved for invalid
	PSI_MEDIA_TYPE_COPPER   = 0x01,     // media type copper cable
	PSI_MEDIA_TYPE_FIBER    = 0x02,     // media type fiber cable (POF)
	PSI_MEDIA_TYPE_WIRELESS = 0x03,     // media type wireless
    PSI_MEDIA_TYPE_AUTO     = 0x04,     // automatic detection of media type: fiber cable (POF) or copper cable
	PSI_MEDIA_TYPE_MAX
} PSI_PORT_MEDIA_TYPE;

/*----------------------------------------------------------------------------*/
// for "media_type_psubmodidtnr"
typedef enum psi_port_media_psubmodidtnr_type_enum
{
	PSI_MEDIA_TYPE_PSUBMODIDTNR_COPPER  = 0x00, // copper cable
	PSI_MEDIA_TYPE_PSUBMODIDTNR_FO      = 0x01, // fiber cable (POF)
	PSI_MEDIA_TYPE_PSUBMODIDTNR_RADIO   = 0x02, // wireless
	PSI_MEDIA_TYPE_PSUBMODIDTNR_MAX
} PSI_PORT_MEDIA_PSUBMODIDTNR_TYPE;

/*----------------------------------------------------------------------------*/
/* for "fiberoptic_type" */
typedef enum psi_port_fiberoptic_type_enum
{
	PSI_FIBEROPTIC_NO_FO        = 0x00, // no fiber optic
	PSI_FIBEROPTIC_MULTIMODE    = 0x01, // Multimode
	PSI_FIBEROPTIC_POF          = 0x02, // POF
    PSI_FIBEROPTIC_TYPE_MAX
} PSI_PORT_FIBEROPTIC_TYPE;

/*----------------------------------------------------------------------------*/
/* for "phy_type" */
typedef enum psi_port_phy_type_enum
{
    PSI_PHY_TYPE_UNKNOWN    = 0x00, // unknown PHY
	PSI_PHY_TYPE_NEC        = 0x01, // PHY = NEC
	PSI_PHY_TYPE_NSC        = 0x02, // PHY = NSC
	PSI_PHY_TYPE_TI         = 0x03, // PHY = TI
	PSI_PHY_TYPE_BCM        = 0x04, // PHY = Broadcom
	PSI_PHY_TYPE_INTEL      = 0x05, // PHY = Intel
    PSI_PHY_TYPE_MAX
} PSI_PORT_PHY_TYPE;

/*----------------------------------------------------------------------------*/
/* for "xmii_type" */
typedef enum psi_port_xmii_type_enum
{
    PSI_PORT_MII_UNKNOWN    = 0x00, // unknown MII
    PSI_PORT_MII            = 0x01, // MII
    PSI_PORT_RMII           = 0x02, // RMII
    PSI_PORT_MII_MAX
} PSI_PORT_XMII_TYPE;

/*----------------------------------------------------------------------------*/
typedef enum psi_ringport_type_enum
{
	PSI_NO_RING_PORT      = 0x00,   // no ring port
	PSI_RING_PORT         = 0x01,   // possible ring port but no default
	PSI_RING_PORT_DEFAULT = 0x02,   // default ring port
} PSI_RINGPORT_TYPE;

/*----------------------------------------------------------------------------*/
typedef enum psi_mrp_role_enum
{
	PSI_MRP_ROLE_NONE         = 0x00,   // no MRP
	PSI_MRP_ROLE_CLIENT       = 0x01,   // MRP role client
	PSI_MRP_ROLE_MANAGER      = 0x02,   // MRP role manager
	PSI_MRP_ROLE_AUTO_MANAGER = 0x03,   // MRP role auto manager
} PSI_MRP_ROLE_TYPE;

/*----------------------------------------------------------------------------*/
typedef enum psi_irte_buffer_capacity_usecase_enum
{
	PSI_IRTE_USE_CASE_DEFAULT       = 0x00, // Default: all other use cases
	PSI_IRTE_USE_CASE_IOC_SOC1_2P   = 0x01, // PLC with SOC1, 2 ports. Max 512 IODs
	PSI_IRTE_USE_CASE_IOC_SOC1_3P   = 0x02, // PLC with SOC1, 3 ports. Max 256 IODs
	PSI_IRTE_USE_CASE_IOD_ERTEC400  = 0x03, // IOD with ERTEC400 (Scalance). No IOC resources.
} PSI_IRTE_BUFFER_CAPACITY_USECASE_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for HD Selection                                                     */
/*----------------------------------------------------------------------------*/
// Possible HD ID key settings
typedef enum psi_hd_select_enum
{
	PSI_HD_SELECT_WITH_UNKNOWN  = 0,    // Unknwon Select method
	PSI_HD_SELECT_WITH_LOCATION = 1,    // HD selected by PCI Location (i.E. PnDevDrv boards)
	PSI_HD_SELECT_WITH_MAC      = 2,    // HD selected by MAC (i.E. StdMAC boards (WPCAP)
	PSI_HD_SELECT_EMBEDDED		= 3     // HD has not to be selected, because we are within the HD (i.E. LinuxSoC1 embedded)
} PSI_HD_SELECT_TYPE;

/*----------------------------------------------------------------------------*/
// for HD addressing over PCI location
typedef struct psi_pci_location_tag
{
	LSA_UINT16      bus_nr;             // Bus number
	LSA_UINT16      device_nr;          // Device number
	LSA_UINT16      function_nr;        // Function number
} PSI_PCI_LOCATION_TYPE, * PSI_PCI_LOCATION_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// Possible interface ID key settings
typedef enum psi_interface_selector_enum
{
	PSI_INTERFACE_UNKNOWN = 0,          // Unknwon Select method
	PSI_INTERFACE_1       = 1,          // Interface 1: HERA: PNIP A; TI-AM5728: GMAC
	PSI_INTERFACE_2       = 2           // Interface 2: HERA: PNIP B; TI-AM5728: ICSS
} PSI_INTERFACE_SELECTOR_TYPE;

/*----------------------------------------------------------------------------*/
// for select HD at startup
typedef struct psi_hd_id_tag
{
	PSI_HD_SELECT_TYPE          hd_selection;           // Selector for board
	PSI_MAC_TYPE                hd_mac;                 // MAC of StdMAC board (used by pcSRT, WPCAP)
	PSI_PCI_LOCATION_TYPE       hd_location;            // Location key of PN-IP hd (PnDevDrv boards)
    PSI_INTERFACE_SELECTOR_TYPE hd_interface_selector;  // Selector for interface
} PSI_HD_ID_TYPE, * PSI_HD_ID_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for I&M input                                                        */
/*----------------------------------------------------------------------------*/
#define PSI_MAX_SIZE_IM_ORDER_ID        (20 + 1)        // Max size of order ID visible string with "\0"
#define PSI_MAX_SIZE_IM_DEVICE_TYPE     (25 + 1)        // Max size of device type visible string with "\0"
#define PSI_MAX_SIZE_IM_SERIAL_NR       (16 + 1)        // Max size of serial number visible string with "\0"

typedef struct psi_im_sw_version_input_tag
{
    LSA_UINT8       revision_prefix;                    // revision prefix (i.E. T, P, ..)
	LSA_UINT8       functional_enhancement;             // revision number for functional enhancement
	LSA_UINT8       bug_fix;                            // revision number for bug fix
	LSA_UINT8       internal_change;                    // revision number for internal change
} PSI_IM_SW_VERSION_TYPE, * PSI_IM_SW_VERSION_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// I&M input data from INI (see PNIO spec for values)
typedef struct psi_im_input_tag
{
	LSA_UINT16                  vendor_id_high;                             // vendor ID high
	LSA_UINT16                  vendor_id_low;                              // vendor ID low
	LSA_UINT16                  hw_revision;                                // HW revision
	PSI_IM_SW_VERSION_TYPE      sw_version;                                 // SW rev with prefix and single rev nr.
	LSA_UINT16                  revision_counter;                           // IM rev counter
	LSA_UINT16                  profile_id;                                 // IM profile ID
	LSA_UINT16                  profile_specific_type;                      // IM profile specific type
	LSA_UINT16                  version;                                    // IM Version High and Low
	LSA_UINT16                  supported;                                  // Bit field for supported I&M records
	LSA_UINT8                   order_id[PSI_MAX_SIZE_IM_ORDER_ID];         // manufacturer specific visible string
	LSA_UINT8                   device_type[PSI_MAX_SIZE_IM_DEVICE_TYPE];   // manufacturer specific visible string
    LSA_UINT8                   serial_nr[PSI_MAX_SIZE_IM_SERIAL_NR];       // IM Serial Number string
} PSI_IM_INPUT_TYPE, * PSI_IM_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for Sock input                                                       */
/*----------------------------------------------------------------------------*/

typedef struct psi_sock_app_ch_detail_socket_option_tag
{
	LSA_UINT8    SO_REUSEADDR_;      // IN PSI_FEATURE_ENABLE: allow local address reuse
    LSA_UINT8    SO_DONTROUTE_;      // IN PSI_FEATURE_ENABLE: just use interface addresses
    LSA_UINT8    SO_SNDBUF_;         // IN PSI_FEATURE_ENABLE: send buffer size
    LSA_UINT8    SO_RCVBUF_;         // IN PSI_FEATURE_ENABLE: receive buffer size
    LSA_UINT8    SO_LINGER_;         // IN PSI_FEATURE_ENABLE: socket lingers on close
    LSA_UINT8    SO_TCPNODELAY_;     // IN PSI_FEATURE_ENABLE: delay sending data (Nagle algorithm)
    LSA_UINT8    SO_TCPNOACKDELAY_;  // IN PSI_FEATURE_ENABLE: delay sending ACKs
    LSA_UINT8    SO_BROADCAST_;      // IN PSI_FEATURE_ENABLE: broadcast allowed
} PSI_SOCK_APP_CH_DETAIL_SOCKET_OPTION_INPUT_TYPE;

typedef struct psi_sock_app_ch_detail_details_tag
{
	PSI_SOCK_APP_CH_DETAIL_SOCKET_OPTION_INPUT_TYPE     socket_option;
	LSA_INT32                                           send_buffer_max_len;
	LSA_INT32                                           rec_buffer_max_len;
	LSA_INT16                                           sock_close_at_once;
	LSA_INT16                                           linger_time;
} PSI_SOCK_APP_CH_DETAIL_DETAILS_TYPE, * PSI_SOCK_APP_CH_DETAIL_DETAILS_PTR_TYPE;

typedef struct psi_sock_app_ch_detail_input_tag
{
	PSI_SOCK_APP_CH_DETAIL_DETAILS_TYPE     sock_detail;    /* If not using default values fill in following details for sock app channels */
} PSI_SOCK_APP_CH_DETAIL_INPUT_TYPE, * PSI_SOCK_APP_CH_DETAIL_INPUT_PTR_TYPE;

typedef struct psi_sock_input_tag
{
	PSI_SOCK_APP_CH_DETAIL_INPUT_TYPE       sock_app_ch_details[PSI_CFG_MAX_SOCK_APP_CHANNELS];
#if (PSI_CFG_USE_SOCKAPP == 1)
    PSI_SOCK_APP_CH_DETAIL_INPUT_TYPE       sock_sockapp_ch_details[PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS];
#else
    PSI_SOCK_APP_CH_DETAIL_INPUT_TYPE   *   sock_sockapp_ch_details; // prevent compilier warnings in HIF when PSI_CFG_USE_SOCKAPP != 1
#endif
} PSI_SOCK_INPUT_TYPE, * PSI_SOCK_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for HD input                                                         */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#define PSI_HD_FEATURE_IRT_FORWARDING_MODE_ABSOLUTE     1
#define PSI_HD_FEATURE_IRT_FORWARDING_MODE_RELATIVE     2

#define PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_NO           0     /* No fragmentation supported        */
#define PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_STATIC       1     /* Static fragmentation supported    */
#define PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_DYNAMIC      2     /* Dynamic fragmentation supported   */


typedef struct psi_hd_feature_input_tag
{
    LSA_UINT8       use_settings;                       // IN PSI_USE_SETTING_YES: Use the settings from this structure, PSI_USE_SETTING_NO: Use default settings
    LSA_UINT8       short_preamble_supported;           // IN PSI_FEATURE_ENABLE: Short preamble supported.         PSI_FEATURE_DISABLE: Short preamble not supported
    LSA_UINT8       mrpd_supported;                     // IN PSI_FEATURE_ENABLE: MRPD supported.                   PSI_FEATURE_DISABLE: MRPD not supported
    LSA_UINT8       fragmentationtype_supported;        // IN PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_NO: No Fragmentation, PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_STATIC - static forwarding. PSI_HD_FEATURE_FRAGTYPE_SUPPORTED_DYNAMIC - dynamic forwarding.
    LSA_UINT8       irt_forwarding_mode_supported;      // IN This values can be ORED: PSI_HD_FEATURE_IRT_FORWARDING_MODE_ABSOLUTE: IRT absolute forwarding mode supported. PSI_HD_FEATURE_IRT_FORWARDING_MODE_RELATIVE: IRT relative forwarding mode supported.
    LSA_UINT16      max_dfp_frames;
    LSA_UINT8       mrp_interconn_fwd_rules_supported;  // IN PSI_FEATURE_ENABLE: MRP interconnect - enables forwarding rules
    LSA_UINT8       mrp_interconn_originator_supported; // IN PSI_FEATURE_ENABLE: MRP interconnect - enables MRP as originator for frames
} PSI_HD_FEATURE_INPUT_TYPE;
/*----------------------------------------------------------------------------*/

// IRTE input data from HW ini
typedef struct psi_irte_input_tag
{
	LSA_UINT8                   use_setting;                    // IN IN PSI_USE_SETTING_YES: Use the settings from this structure, PSI_USE_SETTING_NO: Use default settings
	LSA_UINT8                   irt_supported;                  // IN PSI_FEATURE_ENABLE: IRT supported, PSI_FEATURE_DISABLE: IRT not supported.
	LSA_UINT16                  nr_irt_forwarder;               // IN nr of IRT forwarder
    LSA_UINT16                  buffer_capacity_use_case;       // IN IRTE BufferCapacityUseCase -> see enum above: PSI_IRTE_BUFFER_CAPACITY_USECASE_TYPE
    LSA_UINT8                   support_irt_flex;               // IN PSI_FEATURE_ENABLE: IRT Flex parametrization is accepted; PSI_FEATURE_DISABLE: IRT Flex parametrization is rejected
    LSA_UINT8                   disable_max_port_delay_check;   // IN PSI_FEATURE_ENABLE: checks against PDIRGlobalData.MaxPortRxDelay and PDIRGlobalData.MaxPortTxData are disabled. PSI_FEATURE_DISABLE: enable checks
    PSI_HD_FEATURE_INPUT_TYPE   FeatureSupport;                 // IN Features are supported
} PSI_HD_IRTE_INPUT_TYPE, * PSI_HD_IRTE_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// PNIP input data from HW ini
typedef struct psi_pnip_input_tag
{
	LSA_UINT8                   use_setting;        // IN PSI_USE_SETTING_YES: Use the settings from this structure, PSI_USE_SETTING_NO: Use default settings
    LSA_UINT8                   gigabit_support;    // PSI_FEATURE_ENABLE: enable Gigabit support, PSI_FEATURE_DISABLE: disable gigabit support
	LSA_UINT16                  k32fw_trace_level;  // Initial K32FW trace level
    PSI_HD_FEATURE_INPUT_TYPE   FeatureSupport;     // Features are supported
} PSI_HD_PNIP_INPUT_TYPE, * PSI_HD_PNIP_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// EDDS input data from HW ini
typedef struct psi_stdmac_input_tag
{
    LSA_UINT8                   use_setting;        // IN PSI_USE_SETTING_YES: Use the settings from this structure, PSI_USE_SETTING_NO: Use default settings
    LSA_UINT8                   gigabit_support;    // PSI_FEATURE_ENABLE: enable Gigabit support, PSI_FEATURE_DISABLE disable gigabit support
    LSA_UINT8                   nrt_copy_interface;	// PSI_FEATURE_ENABLE: enable NRT-Copy-Interface, PSI_FEATURE_DISABLE: disable NRT-Copy-Interface (Zero-Copy-Interface is used)
    PSI_HD_FEATURE_INPUT_TYPE   FeatureSupport;     // Features are supported
} PSI_HD_STDMAC_INPUT_TYPE, * PSI_HD_STDMAC_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// All HW port specific INI inputs from HW INI
typedef struct psi_hw_port_input_tag
{
	LSA_UINT16          user_nr;            // User Port number for IF
	PSI_RINGPORT_TYPE   mrp_ringport_type;  // None, Possible, Default
} PSI_HD_PORT_INPUT_TYPE, * PSI_HD_PORT_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for HD-IF input                                                      */
/*----------------------------------------------------------------------------*/
// CM-CL/CM-MC (IOC/IOM) input data for the HD-IF
typedef struct psi_ioc_input_tag
{
    LSA_UINT8       use_setting;            // IN PSI_USE_SETTING_YES: Use the settings from this structure, PSI_USE_SETTING_NO: Use default settings
	LSA_UINT16      nr_of_rt_devices;       // nr of RT devices
	LSA_UINT16      nr_of_irt_devices;      // nr of IRT devices
	LSA_UINT16      nr_of_qv_provider;      // nr of QV provider
	LSA_UINT16      nr_of_qv_consumer;      // nr of QV consumer
	LSA_UINT16      logical_addr_space;     // logical address space
	LSA_UINT16      nr_of_submod;           // nr of submodules
	LSA_UINT16      max_iocr_data_size;     // max IO-CR data size
	LSA_UINT16      max_mcr_data_size;      // max MC-R data size

    /* Input settings from PNIO configuration */
	LSA_UINT32      max_record_length;      // max record length (min = 4k)
	LSA_UINT16      max_alarm_data_length;  // max alarm data length
} PSI_HD_IF_IOC_INPUT_TYPE, * PSI_HD_IF_IOC_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// CM-SV (IOD) input data for the HD-IF
typedef struct psi_iod_input_tag
{
    LSA_UINT8       use_setting;        // IN PSI_USE_SETTING_YES: Use the settings from this structure, PSI_USE_SETTING_NO: Use default settings
	LSA_UINT16      nr_of_instances;    // nr of parallel device instances
	LSA_UINT16      iod_max_ar_DAC;
	LSA_UINT16      iod_max_ar_IOC;     // nr of parallel AR for IOC
	LSA_UINT16      max_icr_data_size;  // max I-CR data size for each AR
	LSA_UINT16      max_ocr_data_size;  // max O-CR data size for each AR
	LSA_UINT16      max_mcr_data_size;  // max M-CR data size for each AR

    /* Input settings from PNIO configuration */
	LSA_UINT32      max_record_length;  // max record length (min = 4k)
} PSI_HD_IF_IOD_INPUT_TYPE, * PSI_HD_IF_IOD_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// CBA input data for the HD-IF
typedef struct psi_cba_input_tag
{
    LSA_UINT8       use_setting;            // IN PSI_USE_SETTING_YES: Use the settings from this structure, PSI_USE_SETTING_NO: Use default settings
	LSA_UINT16      nr_of_provider_con;     // nr of CBA provider connections
	LSA_UINT16      nr_of_consumer_con;     // nr of CBA consumer connections
	LSA_UINT16      nr_of_rt_provider;      // nr of CBA RT provider
	LSA_UINT16      nr_of_rt_consumer;      // nr of CBA RT consumer
	LSA_UINT16      data_volume_size;       // Data volume size
} PSI_HD_IF_CBA_INPUT_TYPE, * PSI_HD_IF_CBA_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// IP input data for the HD-IF
typedef struct psi_ip_input_tag
{
    LSA_UINT8       use_setting;            // IN PSI_USE_SETTING_YES: Use the settings from this structure, PSI_USE_SETTING_NO: Use default settings
	LSA_UINT16      nr_of_send;             // nr of Send resources
	LSA_UINT16      nr_of_arp;              // nr of ARP receive resources
	LSA_UINT16      nr_of_icmp;             // nr of ICMP receive resources
	LSA_UINT16      nr_of_udp;              // nr of UDP receive resources
	LSA_UINT16      nr_of_tcp;              // nr of TCP receive resources
	LSA_UINT8       multicast_support_on;   // PSI_FEATURE_ENABLE: MC support turned on, PSI_FEATURE_DISABLE turned off
} PSI_HD_IF_IP_INPUT_TYPE, * PSI_HD_IF_IP_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// OHA input data for the HD-IF
typedef struct psi_oha_input_tag
{
    LSA_UINT8       use_setting;                    // IN PSI_USE_SETTING_YES: Use the settings from this structure, PSI_USE_SETTING_NO: Use default settings
    /* Input settings from PNIO configuration */
	LSA_UINT16      vendor_id;                      // Device ID
	LSA_UINT16      device_id;                      // Vendor ID
	LSA_UINT8       sys_description[PSI_CFG_OHA_MAX_DESCRIPTION_LEN];   // System Description
	LSA_UINT8       if_description[PSI_CFG_OHA_MAX_DESCRIPTION_LEN];    // Interface Description
    struct {
        LSA_UINT8   description[PSI_CFG_OHA_MAX_DESCRIPTION_LEN];       // Port Description
    } port_description[PSI_CFG_OHA_CFG_MAX_PORTS];
	LSA_UINT32      instance_opt;                   // Device Instance (0..0xFFFF, 0xFFFFFFFF=not-valid)
    LSA_UINT8       check_ip_enabled;               // PSI_FEATURE_ENABLE: Enable IP check, PSI_FEATURE_DISABLE: Disable IP check
	LSA_UINT8       nos_allow_upper_cases;          // PSI_FEATURE_ENABLE: Enable conversion of capital letters in NoS, PSI_FEATURE_DISABLE: Disable conversion of capital letters in NoS
	LSA_UINT16      rema_station_format;            // OHA rema station format (Advanced / Standard)
	LSA_UINT32      dcp_indication_filter;          // 0: all dcp indications are sent to the user; <>0: bitcoded, see OHA_CTRL_IP_SUITE, OHA_CTRL_STATION_NAME,...
} PSI_HD_IF_OHA_INPUT_TYPE, * PSI_HD_IF_OHA_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// MRP input data for the HD-IF
typedef struct psi_mrp_input_tag
{
    LSA_UINT8           use_setting;                        // IN PSI_USE_SETTING_YES: Use the settings from this structure, PSI_USE_SETTING_NO: Use default settings
	LSA_UINT8           max_instances;                      // max instances [1..N]
	LSA_UINT8           supported_role;                     // Supported MRP roles for instance0
	LSA_UINT8           supported_multiple_role;            // Supported MRP roles for instanceN (!= 0)
	PSI_MRP_ROLE_TYPE   default_role_instance0;             // default-role for instance0
    LSA_UINT8           max_mrp_interconn_instances;        // number of MRP interconnect instances supported for this interface
    LSA_UINT8           supported_mrp_interconn_role;       // possible MRP interconnect roles
    LSA_UINT8           supports_mrp_interconn_port_config[PSI_CFG_MAX_PORT_CNT];  // defines if this port is a potential interconnection port used with MRP interconnect
} PSI_HD_IF_MRP_INPUT_TYPE, * PSI_HD_IF_MRP_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef struct psi_nrt_input_tag
{
	LSA_UINT8    RxFilterUDP_Unicast;    // PSI_FEATURE_ENABLE: Enable Unicast UDP filtering,   PSI_FEATURE_DISABLE: No filtering. Refer to EDDx Spec
    LSA_UINT8    RxFilterUDP_Broadcast;  // PSI_FEATURE_ENABLE: Enable Multicast UDP filtering, PSI_FEATURE_DISABLE: No filtering. Refer to EDDx Spec
} PSI_HD_IF_NRT_INPUT_TYPE, * PSI_HD_IF_NRT_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// PNIO IF specific inputs for the HD-IF
typedef struct psi_hd_if_input_tag
{
	LSA_UINT16                  edd_if_id;  // EDD IF ID (=PNIO IF ID for PSI)
	LSA_UINT32                  trace_idx;  // Trace Index used for traces of this interface
	PSI_HD_IF_IOC_INPUT_TYPE    ioc;        // CMCL (IOC) input data
	PSI_HD_IF_IOD_INPUT_TYPE    iod;        // CMSV (IOD) input data
	PSI_HD_IF_CBA_INPUT_TYPE    cba;        // CBA input data
	PSI_HD_IF_IP_INPUT_TYPE     ip;         // TCIP input data
	PSI_HD_IF_OHA_INPUT_TYPE    oha;        // OHA input data
	PSI_HD_IF_MRP_INPUT_TYPE    mrp;        // MRP input data
	PSI_HD_IF_NRT_INPUT_TYPE    nrt;
} PSI_HD_IF_INPUT_TYPE, * PSI_HD_IF_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
// HD input configuration for the HD
typedef struct psi_hd_input_tag
{
	LSA_UINT16                  hd_id;                          // HD number [1..N]
	PSI_HD_ID_TYPE              hd_location;                    // HD address information (PCI, MAC)
    LSA_UINT8                   hd_runs_on_level_ld;            // 1: HD is on LD level / 0: HD is on separate level from LD
	LSA_UINT16                  nr_of_ports;                    // nr of ports (within one HD)
    LSA_UINT16                  nr_of_all_ports;                // nr of ports over all HDs (all IFs), PSI LD sets this value
    LSA_UINT16                  nr_of_all_if;                   // nr of all IFs, PSI LD sets this value
	LSA_UINT16                  edd_type;                       // EDDx type (LSA-COMP-ID) for this HD
	LSA_UINT16                  asic_type;                      // ASIC type for this HD
	LSA_UINT16                  rev_nr;                         // Rev number for this HD
	LSA_UINT8                   allow_none_pnio_mautypes;		// PSI_FEATURE_ENABLE: 10Mbps and half-duplex (for all speeds) are allowed, PSI_FEATURE_DISABLE: not allowed
	LSA_UINT8                   allow_overlapping_subnet;       // PSI_FEATURE_ENABLE: generate diagnosis if subnets are overlapping, PSI_FEATURE_DISABLE: generate diagnosis using the rule set of IP forwarding
    LSA_UINT16                  send_clock_factor;              // SendClockFactor = CycleBaseFactor
	LSA_UINT8		            fill_active;					// PSI_FEATURE_ENABLE: enable FeedInLoadLimitation support, PSI_FEATURE_DISABLE: disable FeedInLoadLimitation support
	LSA_UINT8		            io_configured;					// PSI_FEATURE_ENABLE: IO data are configured, PSI_FEATURE_DISABLE: This PN Interface is unable to have provider and consumer (= only NRT interface)
    LSA_UINT8                   cluster_IP_support;             // PSI_FEATURE_ENABLE: enable cluster IP support, PSI_FEATURE_DISABLE: disable cluster IP support
  	LSA_UINT8			      	hsync_role;						// set hsync_role default parameter
	LSA_UINT8			        additional_forwarding_rules_supported;// set additional forwarding rules
	LSA_UINT8			        application_exist;				// application exist
    LSA_UINT8                   mra_enable_legacy_mode;         // enable MRA legacy mode. 0 - no legacy mode; != 0 - legacy mode. See parameter MRAEnableLegacyMode in edd docu.
	PSI_MAC_TYPE                if_mac;                         // interface mac
	PSI_MAC_TYPE                port_mac[PSI_CFG_MAX_PORT_CNT]; // port mac
	
	PSI_HD_IRTE_INPUT_TYPE      irte;                           // IRTE input data (only for EDDI)
	PSI_HD_PNIP_INPUT_TYPE      pnip;                           // PNIP input data (only for EDDP)
	PSI_HD_STDMAC_INPUT_TYPE    stdmac;                         // StdMAC input data (only for EDDS)
	PSI_HD_PORT_INPUT_TYPE      hw_port[PSI_CFG_MAX_PORT_CNT];  // HW port settings (index = PortIndex)
	PSI_HD_IF_INPUT_TYPE        pnio_if;                        // PNIO Interface setting
} PSI_HD_INPUT_TYPE, * PSI_HD_INPUT_PTR_TYPE;
typedef PSI_HD_INPUT_TYPE const* PSI_HD_INPUT_CONST_PTR_TYPE;
/*----------------------------------------------------------------------------*/

// All HW port specific outputs
typedef struct psi_hw_port_output_tag
{
	PSI_PORT_MEDIA_TYPE                 media_type;
    PSI_PORT_MEDIA_PSUBMODIDTNR_TYPE    media_type_psubmodidtnr; // for Port Submodule Ident Numbers
    PSI_PORT_FIBEROPTIC_TYPE            fiberoptic_type;
    PSI_PORT_PHY_TYPE                   phy_type;
    PSI_PORT_XMII_TYPE                  xmii_type;
    PSI_ASIC_TYPE                       edd_asic_type;
    LSA_UINT8                           isRingPort;
} PSI_HD_PORT_OUTPUT_TYPE, * PSI_HD_PORT_OUTPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/

// All PNIO IF specific calculated outputs
typedef struct psi_hd_if_output_tag
{
	LSA_UINT16      nr_of_usr_ports;    // Used ports for IF (used by CM-PD)
	LSA_UINT16      ioc_max_devices;    // Max Nr. of devices for CM-CL
	LSA_UINT16      iom_max_devices;    // Max Nr. of devices for CM-MC
	LSA_UINT16      iod_max_devices;    // Max Nr. of devices for CM-SV
	LSA_UINT16      iod_max_ar_IOC;     // Max Nr. of RT AR for one device
	LSA_UINT16      iod_max_ar_DAC;     // MAx Nr of Device access AR for one device
} PSI_HD_IF_OUTPUT_TYPE, * PSI_HD_IF_OUTPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/

// Memory block information 
typedef struct psi_hd_mem_tag
{
    LSA_UINT8*      base_ptr;   // base address
    LSA_UINT32      size;       // Size of memory block
    LSA_UINT32      phy_addr;   // Phy. address 
} PSI_HD_MEM_TYPE, *PSI_HD_MEM_PTR_TYPE;

// HD output configuration for all HDs
typedef struct psi_hd_output_tag
{
    LSA_UINT8                   nr_of_hw_ports;
    LSA_UINT8                   hw_rev_nr;
    LSA_UINT8                   giga_bit_support;               // PSI_HD_GIGA_BIT_SUPPORT_ENABLE: Is gigabit, PSI_HD_GIGA_BIT_SUPPORT_DISABLE: Is not a gigabit port
	PSI_HD_PORT_OUTPUT_TYPE     hw_port[PSI_CFG_MAX_PORT_CNT];  // HW port settings
	PSI_HD_IF_OUTPUT_TYPE       pnio_if;                        // PNIO Interface setting
    union {
        struct {
            LSA_UINT32          io_mem_size;                    // real size of io-data memory are
        } eddi;
    } edd;
} PSI_HD_OUTPUT_TYPE, * PSI_HD_OUTPUT_PTR_TYPE;

typedef struct psi_ld_opt_comp_output_tag
{
    LSA_UINT16  lsa_comp_id;
    LSA_UINT8   use_comp;
} PSI_LD_COMP_OUTPUT_TYPE, * PSI_LD_COMP_OUTPUT_PTR_TYPE;

typedef struct psi_ld_output_tag
{
    PSI_LD_COMP_OUTPUT_TYPE     supported_comps[PSI_LD_OPT_COMP_MAX];
} PSI_LD_OUTPUT_TYPE, * PSI_LD_OUTPUT_PTR_TYPE;

typedef struct psi_ld_input_tag
{
    PSI_LD_RUNS_ON_TYPE     ld_runs_on;
} PSI_LD_INPUT_TYPE, * PSI_LD_INPUT_PTR_TYPE;

/*------------------------------------------------------------------------------
// PSI_RQB_TYPE .. ReQuestBlock
//----------------------------------------------------------------------------*/

typedef LSA_VOID LSA_FCT_PTR(,PSI_UPPER_CALLBACK_FCT_PTR_TYPE)(PSI_UPPER_RQB_PTR_TYPE pRQB);

struct psi_ld_open_tag
{
	LSA_UINT16                      	hH;
	LSA_UINT32                      	hd_count;                       //In: used element count of hd_args[]
	PSI_SOCK_INPUT_TYPE             	sock_args;                      //In: Sock configuration values for LD
	PSI_IM_INPUT_TYPE               	im_args;                        //In: I&M configuration values for LD
	PSI_HD_INPUT_TYPE               	hd_args[PSI_CFG_MAX_IF_CNT];    //In: configuration of all HD
	PSI_HD_OUTPUT_TYPE              	hd_out[PSI_CFG_MAX_IF_CNT];     //out: calculated config of all HD
    PSI_LD_INPUT_TYPE                   ld_in;                          //In: configuration of LD
    PSI_LD_OUTPUT_TYPE              	ld_out;                         //out: calculated config of ld
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE		psi_request_upper_done_ptr;   	//In: Callback function
};

typedef struct psi_ld_open_tag      PSI_LD_OPEN_TYPE;

typedef PSI_LD_OPEN_TYPE * PSI_UPPER_LD_OPEN_PTR_TYPE;

/*----------------------------------------------------------------------------*/

struct psi_ld_close_tag
{
	LSA_UINT16                          hH;                         // In: HIF LD handle for hif_sys_request
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE     psi_request_upper_done_ptr; // In: Callback function
};

typedef struct psi_ld_close_tag     PSI_LD_CLOSE_TYPE;

typedef PSI_LD_CLOSE_TYPE * PSI_UPPER_LD_CLOSE_PTR_TYPE;

/*----------------------------------------------------------------------------*/

struct psi_hd_open_tag
{
#if ( PSI_CFG_USE_HIF == 1 )
	LSA_UINT16                          hH;                         // In: HIF HD handle for hif_sys_request
#endif
	PSI_HD_INPUT_TYPE                   hd_args;                    // In: identifier for one HD
	PSI_HD_OUTPUT_TYPE                  hd_out;                     // Out: calculated config of one HD
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE     psi_request_upper_done_ptr; // In: Callback function
};

typedef struct psi_hd_open_tag  PSI_HD_OPEN_TYPE;

typedef PSI_HD_OPEN_TYPE * PSI_UPPER_HD_OPEN_PTR_TYPE;

/*----------------------------------------------------------------------------*/

struct psi_hd_close_tag
{
#if ( PSI_CFG_USE_HIF == 1 )
	LSA_UINT16                          hH;                         // In: HIF HD handle for hif_sys_request
#endif
	LSA_UINT32                          hd_id;                      // In:  identifier for this hd
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE     psi_request_upper_done_ptr; // In: Callback function
};

typedef struct psi_hd_close_tag     PSI_HD_CLOSE_TYPE;

typedef PSI_HD_CLOSE_TYPE * PSI_UPPER_HD_CLOSE_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef union psi_rqb_args_tag
{
	PSI_LD_OPEN_TYPE        ld_open;    /* LD open  */
	PSI_LD_CLOSE_TYPE       ld_close;   /* LD close */
	PSI_HD_OPEN_TYPE        hd_open;    /* HD open  */
	PSI_HD_CLOSE_TYPE       hd_close;   /* HD close */
} PSI_RQB_ARGS_TYPE;

typedef struct psi_rqb_tag
{
	PSI_RQB_HEADER
	PSI_RQB_ARGS_TYPE   args;   /* IN/OUT: Args of Request     */
	PSI_RQB_TRAILER
} PSI_RQB_TYPE, * PSI_RQB_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef union psi_rqb_edd_undo_init_args_tag
{
    LSA_UINT16      hd_nr;  // HD number [1..N] of this EDD
} PSI_RQB_EDD_UNDO_INIT_ARGS_TYPE;

typedef struct psi_rqb_edd_undo_init_tag
{
	PSI_RQB_HEADER
	PSI_RQB_EDD_UNDO_INIT_ARGS_TYPE     args;
	PSI_RQB_TRAILER
} PSI_RQB_EDD_UNDO_INIT_TYPE, * PSI_RQB_EDD_UNDO_INIT_PTR_TYPE;

/*-----------------------------------
  Mailbox User functions
------------------------------------*/
typedef LSA_VOID (*PSI_REQUEST_FCT)( LSA_VOID_PTR_TYPE );

LSA_VOID psi_request_direct_start(
	LSA_UINT16           mbx_id,
	PSI_REQUEST_FCT      request_func,
	LSA_VOID_PTR_TYPE    pRQB );

LSA_VOID psi_request_start(
	LSA_UINT16           mbx_id,
	PSI_REQUEST_FCT      request_func,
	LSA_VOID_PTR_TYPE    pRQB );

LSA_VOID psi_request_local(
	LSA_UINT16            mbx_id,
	PSI_REQUEST_FCT       req_func,
	LSA_VOID_PTR_TYPE     pRQB );

LSA_VOID psi_request_done(
	PSI_REQUEST_FCT      req_done_func,
	LSA_VOID_PTR_TYPE    pRQB,
	LSA_SYS_PTR_TYPE     sys_ptr );

LSA_VOID psi_system_request_done(
	LSA_UINT16           mbx_id,
	PSI_REQUEST_FCT      req_done_func,
	LSA_VOID_PTR_TYPE    pRQB );

LSA_INT32 psi_thread_proc(
    LSA_VOID_PTR_TYPE   arg,
    LSA_UINT32          psi_thread_id);

/* ------------------------------------------------------------------------ */
/* PSI SYS Path Macros                                                      */
/* ------------------------------------------------------------------------ */
/*  0x2345
      |||
      ||+---- 45 sys_path
      |+-----  3 logical IF number  -> currently only used by application
      |+-----  2 hardware device id
*/
/* ------------------------------------------------------------------------ */
#define PSI_SYSPATH_SET_PATH(sys_path, val)  { sys_path |= ( val & 0x00FF );}
#define PSI_SYSPATH_SET_IF(sys_path, val)    { sys_path |= ( val << 8  ); } /* currently only used by application */
#define PSI_SYSPATH_SET_HD(sys_path, val)    { sys_path |= ( val << 12 ); }

#define PSI_SYSPATH_GET_PATH(sys_path)       ( sys_path & 0x00FF )
#define PSI_SYSPATH_GET_IF(sys_path)         ( sys_path >> 8 & 0x000F ) /* currently only used by application */
#define PSI_SYSPATH_GET_HD(sys_path)         ( sys_path >> 12 )

#define PSI_HANDLE_SET_HD(handle, hd_nr)    { handle |= ( hd_nr & 0x0F ); }
#define PSI_HANDLE_GET_HD(handle)           ( sys_path & 0x0F )

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions PSI-LD  =====*/
/* System requests for Open-/Close-Device requests */
LSA_VOID psi_ld_system( PSI_RQB_PTR_TYPE const rqb_ptr );

/* PNIO requests for delegating to PNIO component */
LSA_VOID psi_ld_open_channel( PSI_RQB_PTR_TYPE const rqb_ptr );
LSA_VOID psi_ld_close_channel( PSI_RQB_PTR_TYPE const rqb_ptr );
LSA_VOID psi_ld_request( PSI_RQB_PTR_TYPE const rqb_ptr );

/*====  out functions PSI-LD  ====*/
/* Get Runlevel of LD (Light, Advanced, Basic) */
PSI_LD_RUNS_ON_TYPE psi_get_ld_runs_on(LSA_VOID);

/*====  in functions PSI-HD  =====*/
/* System requests for Open-/Close-Device requests */
LSA_VOID psi_hd_system( PSI_RQB_PTR_TYPE const rqb_ptr );

/* PNIO requests for delegating to PNIO component */
LSA_VOID psi_hd_open_channel( PSI_RQB_PTR_TYPE const rqb_ptr );
LSA_VOID psi_hd_close_channel( PSI_RQB_PTR_TYPE const rqb_ptr );
LSA_VOID psi_hd_request( PSI_RQB_PTR_TYPE const rqb_ptr );

/* PNIO IR and HD functions */
LSA_VOID psi_hd_interrupt( LSA_UINT16 const hd_nr, LSA_UINT32 const int_src );
LSA_BOOL psi_hd_isr_poll( LSA_UINT16 const hd_nr );
LSA_VOID psi_hd_eddi_exttimer_interrupt( LSA_UINT16 const hd_nr);

LSA_VOID psi_hd_get_edd_handle( LSA_UINT16 const hd_nr, PSI_EDD_HDDB * const edd_handle_ptr );
LSA_VOID_PTR_TYPE psi_hd_get_edd_io_handle( LSA_UINT16 const hd_nr );
LSA_UINT32 psi_get_real_pi_size( LSA_UINT16 const hd_nr );

/*====  out functions PSI-HD  ====*/
/* Get Runlevel of HD (Within LD or not) */
LSA_UINT8 psi_get_hd_runs_on_ld( LSA_UINT16 const hd_nr );

/* Check if copy is activeted for a hd */
LSA_BOOL psi_hd_is_edd_nrt_copy_if_on( LSA_UINT16 const hd_nr );

/* get the nrt rx/tx pool handles */
LSA_INT psi_hd_get_nrt_tx_pool_handle( LSA_UINT16 const hd_nr );
LSA_INT psi_hd_get_nrt_rx_pool_handle( LSA_UINT16 const hd_nr );

/* function for POF port auto detection */
LSA_VOID psi_pof_port_auto_detection(
    LSA_UINT16 * const ret_val_ptr,
    LSA_UINT16   const hd_nr,
    LSA_UINT32   const HwPortID,
    LSA_UINT8  * const pMediaType,
    LSA_UINT8  * const pIsPOF,
    LSA_UINT8  * const pFXTransceiverType );

/* Alloc & Free functions */
LSA_VOID psi_usr_alloc_nrt_tx_mem( LSA_VOID_PTR_TYPE * const mem_ptr_ptr, LSA_UINT32 const length, LSA_UINT16 const hd_nr, LSA_UINT16 const comp_id );
LSA_VOID psi_usr_alloc_nrt_rx_mem( LSA_VOID_PTR_TYPE * const mem_ptr_ptr, LSA_UINT32 const length, LSA_UINT16 const hd_nr, LSA_UINT16 const comp_id );
LSA_VOID psi_usr_free_nrt_tx_mem( LSA_UINT16 * const ret_val_ptr, LSA_VOID_PTR_TYPE const mem_ptr, LSA_UINT16 const hd_nr, LSA_UINT16 const comp_id );
LSA_VOID psi_usr_free_nrt_rx_mem( LSA_UINT16 * const ret_val_ptr, LSA_VOID_PTR_TYPE const mem_ptr, LSA_UINT16 const hd_nr, LSA_UINT16 const comp_id );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of PSI_USR_H */

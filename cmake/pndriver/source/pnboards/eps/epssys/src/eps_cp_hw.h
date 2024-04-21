#ifndef EPS_CP_HW_H_                /* ----- reinclude-protection ----- */
#define EPS_CP_HW_H_

#ifdef __cplusplus                  /* If C++ - compiler: Use Cn linkage */
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
/*  C o m p o n e n t     &C: EPS (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: eps_cp_hw.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PNIO System CP adaption                                                  */
/*                                                                           */
/*****************************************************************************/
//lint --e(537) eps_enums may also be included in eps_pn_drv_if.h
#include "eps_enums.h"    
    
/*---------------------------------------------------------------------------*/
/* Types                                                                     */
/*---------------------------------------------------------------------------*/

#define EPS_GPIO_NR_UNDEFINED     0xFFFF   // GPIO nr not set (i.E. LED not avaliable)
#define EPS_HW_PORT_ID_UNDEFINED  0xFFFF   // HW PORT-ID not set (i.E. POF Port not selected)
#define EPS_HW_PHY_NR_UNDEFINED   0        // HW PHY NR not set (see  EDDI_PHY_TRANSCEIVER_NOT_DEFINED)

typedef LSA_RESULT (*EPS_SET_PLLPORT_FCT)( LSA_UINT16, LSA_UINT32, LSA_UINT32,LSA_UINT16,LSA_UINT16 );
typedef LSA_VOID (*EPS_PHY_LED_BLINK_FCT)(LSA_SYS_PTR_TYPE, LSA_UINT32, LSA_UINT32);
typedef LSA_VOID (*EPS_PHY_LED_BLINK_MODE_FCT)(LSA_SYS_PTR_TYPE, LSA_UINT32, LSA_UINT32, LSA_UINT16);
typedef LSA_VOID (*EPS_BGZD_LED_FCT)(LSA_UINT16, LSA_UINT16, LSA_BOOL);
typedef LSA_VOID (*EPS_POF_LED_FCT)(LSA_UINT16, LSA_BOOL, LSA_SYS_PTR_TYPE );

typedef LSA_VOID (*EPS_I2C_SET_LOW_HIGH_FCT)( LSA_SYS_PTR_TYPE, LSA_BOOL );
typedef LSA_VOID (*EPS_I2C_SET_IN_OUT_FCT)( LSA_SYS_PTR_TYPE, LSA_BOOL );
typedef LSA_UINT8 (*EPS_I2C_SDA_READ_FCT)( LSA_SYS_PTR_TYPE );
typedef LSA_BOOL (*EPS_I2C_SELECT_FCT)( LSA_SYS_PTR_TYPE, LSA_UINT16, LSA_UINT16 );
typedef LSA_RESULT (*EPS_LL_I2C_WRITE_OFFSET_FCT)( LSA_SYS_PTR_TYPE, LSA_UINT8, LSA_UINT8, LSA_UINT8, LSA_UINT8, LSA_UINT32, LSA_UINT8* );
typedef LSA_RESULT (*EPS_LL_I2C_READ_OFFSET_FCT)( LSA_SYS_PTR_TYPE, LSA_UINT8, LSA_UINT8, LSA_UINT8, LSA_UINT8, LSA_UINT32, LSA_UINT8* );

typedef enum
{
	EPS_PNDEV_BOARD_EB200          	= 0,
	EPS_PNDEV_BOARD_CP1616         	= 1,
	EPS_PNDEV_BOARD_EB400_PCIE     	= 2,
	EPS_PNDEV_BOARD_SOC1_PCI		= 3,
	EPS_PNDEV_BOARD_SOC1_PCIE		= 4,
	EPS_PNDEV_BOARD_FPGA_ERTEC200P 	= 5,
	EPS_PNDEV_BOARD_EB200P         	= 6,
	EPS_PNDEV_BOARD_INTEL_XX       	= 7,
	EPS_PNDEV_BOARD_KSZ88XX        	= 8,
	EPS_PNDEV_BOARD_TI_XX       	= 9,
    EPS_PNDEV_BOARD_FPGA_HERA       = 10,
    EPS_PNDEV_BOARD_HERA            = 11,
    EPS_PNDEV_BOARD_EB200_PCIE      = 12,
    EPS_PNDEV_BOARD_IX1000          = 13,
	EPS_PNDEV_BOARD_CP1625			= 14,
    EPS_PNDEV_BOARD_INVALID         = 15
} EPS_PNDEV_BOARD_TYPE, *EPS_PNDEV_BOARD_PTR_TYPE;

// see also EDD_MRP_RING_xx in edd_usr.h
typedef enum eps_ringport_type_enum
{
	EPS_NO_RING_PORT        = 0x00, // no ring port
	EPS_RING_PORT           = 0x01, // possible ring port but no default
	EPS_RING_PORT_DEFAULT   = 0x02  // default ring port
} EPS_RINGPORT_TYPE;

// see also EDDI_PHY_TRANSCEIVER_TYPE in eddi_sys.h
typedef enum eps_phy_type_enum
{
	EPS_PHY_TYPE_UNKNOWN   = 0x00,  // reserved for invalid
	EPS_PHY_TYPE_BROADCOM  = 0x01,  // BROADCOM transceiver (BCM5221)
	EPS_PHY_TYPE_NEC       = 0x02,  // NEC transceiver (AATHPHYC2)
	EPS_PHY_TYPE_NSC       = 0x03   // NSC transceiver
} EPS_PORT_PHY_TYPE;

// enumeration for eddp board types
typedef enum eps_eddp_board_type_enum
{
    EPS_EDDP_BOARD_TYPE_UNKNOWN                 = 0x00, // unknown board
    EPS_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV1   = 0x01, // FPGA1-Ertec200P board Rev1
    EPS_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV2   = 0x02, // FPGA1-Ertec200P board Rev2
    EPS_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV1  = 0x03, // EB200P board Rev1
    EPS_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV2  = 0x04, // EB200P board Rev2
    EPS_EDDP_BOARD_MAX
} EPS_EDDP_BOARD_TYPE;

typedef struct eps_mac_tag
{
	LSA_UINT8  mac[6];
} EPS_MAC_TYPE, *EPS_MAC_PTR_TYPE;

typedef struct eps_board_mem_tag { // Memory information (from driver)
	LSA_UINT8* base_ptr;            // base address
	LSA_UINT32 size;                // Size of memory block
	LSA_UINT32 phy_addr;            // Phy. address 
} EPS_BOARD_MEM_TYPE, *EPS_BOARD_MEM_PTR_TYPE;

typedef struct eps_usr_port_map_tag { // User to HW port mapping information
	LSA_UINT16         hw_port_id;       // HW port number
	LSA_UINT16         hw_phy_nr;        // PHY nr for HW port
} EPS_PORT_MAP_TYPE, *EPS_PORT_MAP_PTR_TYPE;

/*---------------------------------------------------------------------------*/

typedef struct eps_board_eddi_tag { // Adaption info for EDDI boards
	LSA_BOOL                    is_valid;                       //Adaption info valid yes/no
	LSA_UINT16                  device_type;                    //EDD_HW_TYPE_USED_SOC, EDD_HW_TYPE_USED_ERTEC_400, EDD_HW_TYPE_USED_ERTEC_200
	EPS_BOARD_MEM_TYPE          irte;                           //IRTE memory
	EPS_BOARD_MEM_TYPE          kram;                           //KRAM memory (part of it used for PI)
	EPS_BOARD_MEM_TYPE          iocc;                           //IOCC memory (used for PI on SOC)
	EPS_BOARD_MEM_TYPE          paea;                           //PAEA memory (located at offset in IOCC)	
	EPS_BOARD_MEM_TYPE          sdram;                          //SDRAM memory uncached (used for NRT)
    EPS_BOARD_MEM_TYPE          sdram_cached;                   //SDRAM memory cached (used for NRT)
	EPS_BOARD_MEM_TYPE          shared_mem;                     //Shared memory (used for NRT)
	EPS_BOARD_MEM_TYPE          apb_periph_scrb;                //SCRB
	EPS_BOARD_MEM_TYPE          apb_periph_gpio;                //GPIO
	EPS_BOARD_MEM_TYPE          apb_periph_timer;               //timer
	LSA_UINT8*                  i2c_base_ptr;                   //I2C base address (used on SOC for POF ports)
    LSA_BOOL                    has_ext_pll;                    //EXT_PLL GPIO avaliable yes/no
	LSA_UINT16                  extpll_out_gpio_nr;             //GPIO nr of XPLL_OUT
	LSA_UINT16                  extpll_in_gpio_nr;              //GPIO nr of XPLL_IN
	EPS_SET_PLLPORT_FCT         set_pll_port_fct;               //PLL GPIO function
	EPS_PHY_LED_BLINK_FCT       blink_start_fct;                //PHY blink start function
	EPS_PHY_LED_BLINK_FCT       blink_end_fct;                  //PHY blink end function
	EPS_PHY_LED_BLINK_MODE_FCT  blink_set_mode_fct;             //PHY blink set mode function
	EPS_POF_LED_FCT             pof_led_fct;                    //POF LED function

    //SII configuration parameters
	LSA_UINT32                  SII_IrqSelector;                //EDDI_SII_IRQ_SP or EDDI_SII_IRQ_HP
	LSA_UINT32                  SII_IrqNumber;                  //EDDI_SII_IRQ_0  or EDDI_SII_IRQ_1
	LSA_UINT32                  SII_ExtTimerInterval;           //0, 250, 500, 1000 in us

	EPS_I2C_SET_LOW_HIGH_FCT    i2c_set_scl_low_highz;          //I2C set SCL low/high function
	EPS_I2C_SET_LOW_HIGH_FCT    i2c_set_sda_low_highz;          //I2C set SDA low/high function
	EPS_I2C_SDA_READ_FCT        i2c_sda_read;                   //I2C SDA read function
	EPS_I2C_SELECT_FCT          i2c_select;                     //I2C select function
	EPS_LL_I2C_WRITE_OFFSET_FCT i2c_ll_write_offset;            //I2C LL write offset function (i.E. used for SOC1)
	EPS_LL_I2C_READ_OFFSET_FCT  i2c_ll_read_offset;             //I2C LL read offset function (i.E. used for SOC1)
    LSA_UINT32                  cycle_base_factor;

    LSA_BOOL                    fragmentation_supported;
    LSA_UINT8                   BC5221_MCModeSet;               //0: Pin “MEDIA_CONV#” of a BC5221 is strapped to 1.
                                                                //1: Pin “MEDIA_CONV#” of a BC5221 is strapped to 0.
    EPS_BOARD_MEM_TYPE          nrt_mem_dmacw_default;          //NRT memory (DMACW default)
    EPS_BOARD_MEM_TYPE          nrt_mem_tx_low_frag;            //NRT memory (TX low frag)
    EPS_BOARD_MEM_TYPE          nrt_mem_tx_mid_frag;            //NRT memory (TX mid frag)
    EPS_BOARD_MEM_TYPE          nrt_mem_tx_high_frag;           //NRT memory (TX high frag)
    EPS_BOARD_MEM_TYPE          nrt_mem_tx_mgmtlow_frag;        //NRT memory (TX mgmtlow frag)
    EPS_BOARD_MEM_TYPE          nrt_mem_tx_mgmthigh_frag;       //NRT memory (TX mgmthigh frag)

    PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_dmacw_default;    //NRT Alloc/Free
    PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_rx_default;       //NRT Alloc/Free
    PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_default;       //NRT Alloc/Free
    PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_low_frag;      //NRT Alloc/Free
    PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_mid_frag;      //NRT Alloc/Free
    PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_high_frag;     //NRT Alloc/Free
    PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_mgmtlow_frag;  //NRT Alloc/Free
    PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_mgmthigh_frag; //NRT Alloc/Free
} EPS_BOARD_EDDI_TYPE;

typedef struct eps_board_eddp_tag { // Adaption info for EDDP boards
	LSA_BOOL                is_valid;                           //Adaption info valid yes/no
	LSA_UINT16              icu_location;                       //ICU IF for EDDP setup
	LSA_UINT16              hw_type;                            //EDDP HW-type for EDDP setup
    LSA_UINT16              hw_interface;                       //EDDP Interface type: EDDP_HW_IF_A, EDDP_HW_IF_B
	LSA_UINT8               appl_timer_mode;                    //EDDP IPB CRT Param ApplTimerMode
	LSA_UINT16              appl_timer_reduction_ratio;         //EDDP IPB CRT Param ApplTimerReductionRatio
	LSA_BOOL                is_transfer_end_correction_pos;     //EDDP IPB CRT Param isTransferEndCorrectionPos
	LSA_UINT32              transfer_end_correction_value;      //EDDP IPB CRT Param TransferEndCorrectionValue
	EPS_EDDP_BOARD_TYPE     board_type;                         //type of board
	EPS_BOARD_MEM_TYPE      pnip;                               //PNIP memory
	EPS_BOARD_MEM_TYPE      sdram_NRT;                          //SDRAM memory NRT range
	EPS_BOARD_MEM_TYPE      sdram_CRT;                          //SDRAM memory CRT range (for IOBUFFER in SDRAM memory)
	EPS_BOARD_MEM_TYPE      perif_ram;                          //PERIF memory (for IO-Buffer in PERIF memory)
	EPS_BOARD_MEM_TYPE      k32_tcm;                            //K32 memory
	EPS_BOARD_MEM_TYPE      k32_Atcm;                           //K32 Atcm memory
	EPS_BOARD_MEM_TYPE      k32_Btcm;                           //K32 Btcm memory
	EPS_BOARD_MEM_TYPE      k32_ddr3;                           //K32 ddr3 memory
	EPS_BOARD_MEM_TYPE      apb_periph_scrb;                    //APB - SCRB
	EPS_BOARD_MEM_TYPE      apb_periph_sec_scrb;                //APB - SEC SCRB
	EPS_BOARD_MEM_TYPE      apb_periph_perif;                   //APB - PERIF
	EPS_POF_LED_FCT         pof_led_fct;                        //POF LED function
	LSA_UINT32              cycle_base_factor;                  //current cycle base factor
} EPS_BOARD_EDDP_TYPE;

typedef struct eps_board_edds_tag { // Adaption info for EDDS boards
	LSA_BOOL			    is_valid;				            //Adaption info valid yes/no
	LSA_VOID_PTR_TYPE	    ll_handle;				            //LL handle (depends on LL adaption)
	LSA_VOID_PTR_TYPE	    ll_function_table;		            //LL function table (depends on LL adaption)
	LSA_VOID*			    ll_params;				            //LL params
	EPS_BOARD_MEM_TYPE	    sdram_NRT;				            //SDRAM memory NRT range
    EPS_BOARD_MEM_TYPE      GmacSw;                             //GMAC-SW register
	LSA_BOOL                bFillActive;			            //FillActive?
	LSA_BOOL                bHsyncModeActive;			        //HsyncModeActive?
	LSA_TIMER_ID_TYPE       hTimer;					            //System Timer ID
	LSA_UINT16              hMsgqHigh;				            //Message queue for high prio thread
	LSA_UINT16              hMsgqLow;				            //Message queue for low prio thread
	LSA_UINT16              hMsgqActive;				        //Active Message queue: hMsgqHigh or hMsgqLow
	LSA_UINT32              hThreadHigh;			            //Handle for high prio thread
	LSA_UINT32              hThreadLow;				            //Handle for low prio thread
} EPS_BOARD_EDDS_TYPE;

typedef struct eps_board_eddt_tag { // Adaption info for EDDT boards
    LSA_BOOL                is_valid;                           //Adaption info valid yes/no
    EPS_BOARD_MEM_TYPE      sdram_NRT;                          //SDRAM memory NRT range
    EPS_BOARD_MEM_TYPE      sdram_CRT;                          //SDRAM memory CRT range (for IOBUFFER in SDRAM memory)
	EPS_BOARD_MEM_TYPE      pnunit_mem;                         //PNUNIT memory
    LSA_UINT32              cycle_base_factor;                  //current cycle base factor
} EPS_BOARD_EDDT_TYPE;

/**
 * Adaption info for each PN board
 *
 * Note: the adaption info is used for the HW specific EDDx functions
 */
typedef struct eps_board_store_tag { 
	LSA_UINT16          	edd_type;                                               /**< Used EDDx type (COMP_ID) for this IF                           */
	LSA_UINT16          	nr_of_ports;                                            /**< Nr of ports                                                    */
	LSA_SYS_PTR_TYPE    	hd_sys_handle;                                          /**< hSysDev for HD                                                 */
	EPS_MAC_TYPE       		if_mac;                                                 /**< IF mac from driver or default                                  */
	EPS_MAC_TYPE        	port_mac[PSI_CFG_MAX_PORT_CNT+1];                       /**< port MAC from driver or default                                */
	EPS_PORT_MAP_TYPE   	port_map[PSI_CFG_MAX_PORT_CNT+1];                       /**< Usr to HW port mapping (0 res. for Auto port ID) for each IF   */
	EPS_BOARD_MEM_TYPE  	dev_mem;							                    /**< DEV memory                                                     */
	EPS_BOARD_MEM_TYPE  	nrt_tx_mem;                                             /**< NRT TX memory                                                  */
	EPS_BOARD_MEM_TYPE  	nrt_rx_mem;                                             /**< NRT RX memory                                                  */
	EPS_BOARD_MEM_TYPE  	crt_slow_mem;                                           /**< CRT slow memory                                                */
	EPS_BOARD_MEM_TYPE  	pi_mem;                                                 /**< PI memory used for CRT (KRAM or IOCC)                          */
	EPS_BOARD_MEM_TYPE  	hif_mem;                                                /**< HIF memory                                                     */
    EPS_BOARD_MEM_TYPE  	srd_api_mem;                                            /**< SRD_API memory see  EDDI_GSHAREDMEM_TYPE/EDDP_GSHAREDMEM_TYPE  */
	EPS_BOARD_MEM_TYPE      file_transfer;                                          /**< File transfer mem for internal FW (C66, KRISC...)              */
	EPS_BOARD_EDDI_TYPE 	eddi;                                                   /**< EDDI board information                                         */
	EPS_BOARD_EDDP_TYPE 	eddp;                                                   /**< EDDP board information                                         */
	EPS_BOARD_EDDS_TYPE 	edds;                                                   /**< EDDS board information                                         */
    EPS_BOARD_EDDT_TYPE 	eddt;                                                   /**< EDDT board information                                         */
	EPS_PNDEV_BOARD_TYPE 	board_type;							                    /**< board type                                                     */
    PSI_PORT_MEDIA_TYPE     port_media_type_by_HwPortId[PSI_CFG_MAX_PORT_CNT+1];    /**< media type for all ports for each IF: PSI_MEDIA_TYPE_xxx;
                                                                                            port index = HwPortId (port index 0 is reserved).           */
    LSA_UINT8               b_is_pof_port[PSI_CFG_MAX_PORT_CNT+1];                  /**< info "is pof port" for all ports for each IF:
                                                                                            port index = HwPortId (port index 0 is reserved).
                                                                                            Info only valid at fiber ports: POF or FX port              */
    PSI_PORT_PHY_TYPE       phy_type[PSI_CFG_MAX_PORT_CNT+1];                       /**< PHY Types like NEC, NSC, TI, Broadcom, Intel                   */
    LSA_UINT8               fx_transceiver_type[PSI_CFG_MAX_PORT_CNT+1];            /**< FX transceiver types like EDD_FX_TRANSCEIVER_QFBR5978.
                                                                                            Don´t care for copper ports.                                */
} EPS_BOARD_INFO_TYPE, *EPS_BOARD_INFO_PTR_TYPE;
typedef EPS_BOARD_INFO_TYPE const* EPS_BOARD_INFO_CONST_PTR_TYPE;
typedef const EPS_BOARD_INFO_TYPE * EPS_BOARD_INFO_PTR_CONST_TYPE;

/*---------------------------------------------------------------------------*/
/* Macros                                                                    */
/*---------------------------------------------------------------------------*/

#define EPS_CALC_MEM_SIZE(size_all_, size_chunk_)   ((((size_all_)* (size_chunk_))/100UL) & (~0x1FUL))

/*---------------------------------------------------------------------------*/
/* Prototypes                                                                */
/*---------------------------------------------------------------------------*/

LSA_VOID eps_hw_init( LSA_VOID );
LSA_VOID eps_hw_undo_init( LSA_VOID );
LSA_VOID eps_hw_connect( LSA_UINT16 hd_id, LSA_BOOL bFWDownload, PSI_HD_SYS_ID_CONST_PTR_TYPE hd_sys_id_ptr );
LSA_VOID eps_hw_get_hd_params( LSA_UINT16 hd_id, LSA_UINT8 hd_runs_on_level_ld, PSI_HD_SYS_ID_CONST_PTR_TYPE hd_sys_id_ptr, PSI_HD_PARAM_PTR_TYPE hd_param_ptr );
LSA_VOID eps_hw_free_hd_params( LSA_UINT16 hd_id );
LSA_VOID eps_hw_close_hd( LSA_UINT16 hd_id );
LSA_UINT16 eps_hw_get_edd_type( LSA_UINT16 board_nr );
EPS_BOARD_INFO_PTR_TYPE eps_hw_get_board_store( LSA_UINT16 board_nr );
LSA_VOID eps_hw_set_board_store( LSA_UINT16 board_nr, EPS_BOARD_INFO_CONST_PTR_TYPE p_store );
LSA_VOID eps_hw_set_hd_param( LSA_UINT16 board_nr, LSA_VOID_PTR_TYPE p_param );
LSA_VOID eps_hw_update_hd_param(LSA_UINT16 board_nr, LSA_VOID_PTR_TYPE p_param);
LSA_VOID eps_hw_init_board_port_param( EPS_BOARD_INFO_PTR_TYPE p_boardInfo );
LSA_VOID eps_hw_init_board_port_media_type( EPS_BOARD_INFO_PTR_TYPE p_boardInfo, EPS_PNDEV_ASIC_TYPE asic_type, EPS_PNDEV_BOARD_TYPE board_type );

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
} // extern "C" 
#endif

/*****************************************************************************/
#endif  /* of EPS_CP_HW_H_ */

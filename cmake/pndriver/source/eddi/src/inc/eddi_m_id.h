#ifndef EDDI_M_ID_H             //reinclude-protection
#define EDDI_M_ID_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_m_id.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Version of prefix                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  07.10.03    JM    initial version. LSA P02.00.01.003                     */
/*  17.02.04    ZR    add icc modul                                          */
/*  23.08.07    JS    changed M_ID_SYNC_CTR from 0x0075Ul to 0x007CUl        */
/*                    for compatiblity with older EDD Module IDs             */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* List of module IDs														 */
/*****************************************************************************/
//CRT
#define M_ID_CRT_SB                                 0x0000UL 
#define M_ID_CRT_SB_SW                              0x0001UL 
#define M_ID_CRT_SB_HW                              0x0002UL 
#define M_ID_CRT_USR                                0x0003UL 
#define M_ID_CRT_INI                                0x0004UL 
#define M_ID_CSRT_USR                               0x0005UL 
#define M_ID_CRT_CONS                               0x0006UL 
#define M_ID_CRT_PROV                               0x0007UL 
#define M_ID_CRT_CHECK                              0x0008UL 
#define M_ID_CRT_CONS_3B                            0x0009UL 
#define M_ID_CRT_PROV_3B                            0x000AUL 
#define M_ID_CRT_BRQ                                0x000BUL 
#define M_ID_CRT_XCH                                0x000CUL 
#define M_ID_CRT_SB_SW_TREE                         0x000DUL 
#define M_ID_CRT_COM                                0x000EUL 
#define M_ID_CRT_DFPCONS                            0x000FUL 
#define M_ID_CRT_DFPPROV                            0x0010UL 
#define M_ID_CRT_DFPCOM                             0x0011UL

//BOOT
#define M_ID_EDDI_BOOT                              0x0100UL   
                                                    
//CRT_PHASE                                         
#define M_ID_CRT_PHASE_TX                           0x0200UL 
#define M_ID_CRT_PHASE_RX                           0x0201UL 
#define M_ID_CRT_PHASE_INI                          0x0202UL 

//BASE
#define M_ID_EDDI_DEV                               0x0300UL
#define M_ID_EDDI_GEN                               0x0301UL
#define M_ID_EDDI_GLB                               0x0302UL
#define M_ID_EDDI_ISR_NRT                           0x0303UL
#define M_ID_EDDI_SER                               0x0304UL
#define M_ID_EDDI_SYS                               0x0305UL
#define M_ID_EDDI_CYC                               0x0306UL
#define M_ID_EDDI_ISR_ERR                           0x0307UL
#define M_ID_EDDI_ISR_REST                          0x0308UL
#define M_ID_EDDI_TIM                               0x0309UL
#define M_ID_EDDI_USR                               0x030AUL
#define M_ID_EDDI_BF                                0x030BUL
#define M_ID_EDDI_PM                                0x030CUL
#define M_ID_EDDI_ISR_CO                            0x030DUL  
#define M_ID_EDDI_REQ                               0x030EUL  
#define M_ID_EDDI_OC                                0x030FUL  
#define M_ID_EDDI_LOCK                              0x0310UL  
#define M_ID_EDDI_EV                                0x0311UL   
#define M_ID_EDDI_G_R6                              0x0312UL 
#define M_ID_EDDI_CHECK                             0x0313UL 
#define M_ID_EDDI_CONV                              0x0314UL 
   
#define M_ID_EDDI_SII                               0x031CUL

//PRM
#define M_ID_EDDI_PRM_STATE                         0x0400UL
#define M_ID_EDDI_PRM_COMMON                        0x0401UL
#define M_ID_EDDI_PRM_REQ                           0x0402UL
#define M_ID_EDDI_PRM_STATE_DIAG_PDNC               0x0403UL   
#define M_ID_EDDI_PRM_FCT                           0x0404UL 

#define M_ID_EDDI_PRM_PDNC_DATA_CHECK               0x0405UL 
#define M_ID_EDDI_PRM_PDIR_DATA                     0x0406UL 
#define M_ID_EDDI_PRM_PDSYNC_DATA                   0x0407UL 
#define M_ID_EDDI_PRM_PDPORT_DATA_ADJUST            0x0408UL 
#define M_ID_EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST   0x040AUL 
#define M_ID_EDDI_PRM_PDPORT_MRP_DATA_ADJUST        0x040BUL 
#define M_ID_EDDI_PRM_PDCONTROL_PLL                 0x040CUL 
#define M_ID_EDDI_PRM_PDTRACEUNIT_CONTROL           0x040DUL 
#define M_ID_EDDI_PRM_PDSET_DEFAULT_PORT_STATES     0x040EUL 
#define M_ID_EDDI_PRM_PDSCF_DATA_CHECK              0x040FUL
#define M_ID_EDDI_PRM_PDIR_SUBFRAME_DATA            0x0410UL  
#define M_ID_EDDI_PRM_PDNRT_LOAD_LIMITATION         0x0411UL
//#define M_ID_EDDI_PRM_STATE_DIAG_PDSYNC           0x040XUL  

//IRT
#define M_ID_IRT_USR                                0x0501UL
#define M_ID_IRT_TREE                               0x0502UL

//IRT_STM
#define M_ID_IRT_CLASS2_STM                         0x0601UL
#define M_ID_IRT_CLASS3_STM                         0x0602UL

//NRT
#define M_ID_NRT_RX                                 0x0600UL
#define M_ID_NRT_TX                                 0x0601UL
#define M_ID_NRT_USR                                0x0602UL
#define M_ID_NRT_INI                                0x0603UL
#define M_ID_NRT_ARP                                0x0604UL
#define M_ID_NRT_FT                                 0x0605UL
#define M_ID_NRT_FILTER                             0x0606UL
#define M_ID_NRT_Q                                  0x0607UL
#define M_ID_NRT_OC                                 0x0608UL
#define M_ID_NRT_TS                                 0x0609UL
#define M_ID_NRT_FRAG_RX                            0x060AUL
#define M_ID_NRT_FRAG_RX_SEMA                       0x060BUL
#if defined (EDDI_CFG_FRAG_ON)
#define M_ID_NRT_FRAG_TX                            0x060CUL
#endif

//SER10
#define M_ID_SER_CMD                                0x0700UL
#define M_ID_SER_INI                                0x0701UL
#define M_ID_SER_INT                                0x0702UL
#define M_ID_SER_SB                                 0x0703UL
#define M_ID_SER_TREE                               0x0704UL
#define M_ID_SER_TREE_RCW                           0x0705UL
#define M_ID_SER_TREE_USR                           0x0706UL
#define M_ID_SER_TREE_BCW_MOD                       0x0707UL
#define M_ID_SER_TREE_IRT                           0x0708UL
//#define M_ID_SER_CNS                              0x0709UL

//SWITCH
#define M_ID_SWI_USR                                0x0800UL
#define M_ID_SWI_LOWWATER                           0x0801UL
#define M_ID_SWI_STATISTIC                          0x0802UL
#define M_ID_SWI_PHY                                0x0803UL
#define M_ID_SWI_MC_FDB                             0x0806UL
#define M_ID_SWI_UC_FDB                             0x0807UL
#define M_ID_SWI_MISC                               0x0808UL
#define M_ID_SWI_PORT                               0x0809UL
#define M_ID_SWI_PTP                                0x080AUL
#define M_ID_SWI_UCMC                               0x080BUL
#define M_ID_SWI_PHY_BUGS                           0x080CUL

//TRACE
#define M_ID_TRAC_USR                               0x0900UL

//SYNC
#define M_ID_SYNC_USR                               0x0A00UL
#define M_ID_SYNC_INI                               0x0A01UL
#define M_ID_SYNC_IR                                0x0A02UL
/* 0x0075UL reserved to avoid missinterpretion with trace tools. Was used by older EDDI. */
#define M_ID_SYNC_CHECK                             0x0A03UL
#define M_ID_SYNC_CALC                              0x0A04UL
#define M_ID_SYNC_RCV                               0x0A05UL
#define M_ID_SYNC_SND                               0x0A06UL
#define M_ID_SYNC_PORT_STMS                         0x0A07UL
#define M_ID_SYNC_FWD_STM                           0x0A08UL
//#define M_ID_SYNC_CTR                             0x0A09UL

//MEM
#define M_ID_EDDI_KRAM                              0x0B00UL
#define M_ID_EDDI_TREE                              0x0B01UL    
#define M_ID_EDDI_MEM                               0x0B02UL   
#define M_ID_EDDI_POOL                              0x0B03UL   

//TPROF
#define M_ID_PROFILE                                0x0C00UL

//RTOUDP
#define M_ID_RTO_UDP                                0x0D00UL
#define M_ID_RTO_CONS                               0x0D01UL
#define M_ID_RTO_PROV                               0x0D02UL
#define M_ID_RTO_SB                                 0x0D03UL
#define M_ID_RTO_LOCK                               0x0D04UL
#define M_ID_RTO_CHECK                              0x0D06UL

//TRANSCEIVER
#define M_ID_TRA                                    0x0E00UL
#define M_ID_TRA_BROADCOM                           0x0E01UL
#define M_ID_TRA_NEC                                0x0E02UL
#define M_ID_TRA_NSC                                0x0E03UL
#define M_ID_TRA_TI                                 0x0E04UL
#define M_ID_TRA_USERSPEC                           0x0E05UL

//IO
#define M_ID_IO_PROV_CTRL                           0x0F00UL
#define M_ID_IO_ISO                                 0x0F01UL
#define M_ID_IO_KRAM                                0x0F02UL
#define M_ID_IO_IOCC                                0x0F10UL


//I2C
#define M_ID_EDDI_I2C                               0x0F03UL

/*****************************************************************************/
/* List of LTRC IDs 														 
  BASE: 000 
	base\eddi_check.c	                                        1
    base\eddi_dev.c	                                            2
    base\eddi_g_r6.c	                                        3
    base\eddi_cyc.c	                                            4
    base\eddi_glb.c	                                            5
    base\eddi_isr_nrt.c	                                        6
    base\eddi_ser.c	                                            7
    base\eddi_sys.c	                                            8
    base\eddi_ev.c	                                            9
    base\eddi_usr.c	                                            10
    base\eddi_conv_calc.c	                                    11
    base\eddi_gen.c	                                            12
    base\eddi_tim.c	                                            13
    base\eddi_req.c	                                            14
    base\eddi_lock.c	                                        15
    base\eddi_pm.c	                                            16
    base\eddi_bf.c	                                            17
    base\eddi_isr_err.c	                                        18
    base\eddi_isr_rest.c										19
    base\eddi_sii.c	                                            20
    base\eddi_oc.c	                                            21
    base\eddi_isr_co.c	                                        22
    mem\eddi_kram.c	                                            23
    mem\eddi_mem.c	                                            24
    mem\eddi_pool.c	                                            25
    mem\eddi_tree.c	                                            26
    tprof\eddi_profile.c	                                    27
    trace\eddi_trac_usr.c										28
    boot\eddi_boot.c											29
	
  CRT, IRT, SYNC: 100	
    crt\eddi_crt_brq.c	                                        100
    crt\eddi_crt_check.c										101
    crt\eddi_crt_com.c	                                        102
    crt\eddi_crt_cons.c	                                        103
    crt\eddi_crt_ini.c	                                        104
    crt\eddi_crt_prov.c	                                        105
    crt\eddi_crt_prov3B.c										106
    crt\eddi_crt_sb.c											107
    crt\eddi_crt_sb_hw.c										108
    crt\eddi_crt_sb_sw.c										109
    crt\eddi_crt_sb_sw_tree.c									110
    crt\eddi_crt_usr.c	                                        111
    crt\eddi_crt_xch.c	                                        112
    crt\eddi_crt_dfpprov.c	                                    113
    crt\eddi_crt_dfpcons.c	                                    114
    crt\eddi_crt_dfp.c	                                        115
    crt_phase\eddi_crt_phase_tx.c								116
    crt_phase\eddi_crt_phase_rx.c								117
    crt_phase\eddi_crt_phase_ini.c	                            118
    irt\eddi_irt_tree.c	                                        119
    irt\eddi_irt_usr.c	                                        120
    irt_stm\eddi_irt_class2_stm.c                               121
    irt_stm\eddi_irt_class3_stm.c                               122
    srt\eddi_csrt_usr.c	                                        123
    rtoudp\eddi_rto_cons.c	                                    124
    rtoudp\eddi_rto_lock.c	                                    125
    rtoudp\eddi_rto_prov.c	                                    126
    rtoudp\eddi_rto_sb.c                                        127
    rtoudp\eddi_rto_udp.c                                       128
    rtoudp\eddi_rto_check.c	                                    129
    sync\eddi_sync_check.c	                                    130
    sync\eddi_sync_ini.c                                        131
    sync\eddi_sync_ir.c	                                        132
    sync\eddi_sync_usr.c                                        133
    sync\eddi_sync_rcv.c                                        134
    sync\eddi_sync_fwd_stm.c                                    135
    sync\eddi_sync_snd.c                                        136
    sync\eddi_sync_port_stms.c	                                137
	
 NRT: 200	
    nrt\eddi_nrt_arp.c	                                        200
    nrt\eddi_nrt_filter.c                                       201
    nrt\eddi_nrt_ft.c                                           202
    nrt\eddi_nrt_ini.c	                                        203
    nrt\eddi_nrt_rx.c                                           204
    nrt\eddi_nrt_tx.c                                           205
    nrt\eddi_nrt_q.c                                            206
    nrt\eddi_nrt_usr.c	                                        207
    nrt\eddi_nrt_oc.c                                           208
    nrt\eddi_nrt_ts.c                                           209
    nrt\eddi_nrt_frag_rx.c                                      210
    nrt\eddi_nrt_frag_rx_sema.c                                 211
    nrt\eddi_nrt_frag_tx.c                                      212

  SER, SWITCH: 300	
    ser10\eddi_ser_cmd.c                                        300
    ser10\eddi_ser_ini.c                                        301
    ser10\eddi_ser_int.c                                        302
    ser10\eddi_ser_sb.c	                                        303
    ser10\eddi_ser_tree.c                                       304
    ser10\eddi_ser_tree_rcw.c                                   305
    ser10\eddi_ser_tree_usr.c                                   306
    ser10\eddi_ser_tree_bcw_mod.c                               307
    ser10\eddi_ser_tree_irt.c                                   308
    switch\eddi_swi_mc_fdb.c                                    309
    switch\eddi_swi_misc.c	                                    310
    switch\eddi_swi_phy.c                                       311
    switch\eddi_swi_port.c	                                    312
    switch\eddi_swi_ptp.c                                       313
    switch\eddi_swi_statistic.c	                                314
    switch\eddi_swi_uc_fdb.c                                    315
    switch\eddi_swi_ucmc.c                                      316
    switch\eddi_swi_usr.c                                       317
    switch\eddi_swi_phy_bugs.c	                                318

  PRM: 400	
    prm\eddi_prm_req.c	                                        400
    prm\eddi_prm_state.c                                        401
    prm\eddi_prm_state_diag_pdnc.c                              402
    prm\eddi_prm_record_common.c                                403
    prm\eddi_prm_record_pdcontrol_pll.c	                        404
    prm\eddi_prm_record_pdir_data.c	                            405
    prm\eddi_prm_record_pdnc_data_check.c                       406
    prm\eddi_prm_record_pdport_data_adjust.c                    407
    prm\eddi_prm_record_pdscf_data_check.c	                    408
    prm\eddi_prm_record_pdset_default_port_states.c	            409
    prm\eddi_prm_record_pdsync_data.c                           410
    prm\eddi_prm_record_pdtraceunit_control.c                   411
    prm\eddi_prm_record_pdinterface_mrp_data_adjust.c           412
    prm\eddi_prm_record_pdir_subframe_data.c                    413

  IO, LL, Transceiver: 500	
    io\eddi_io_provctrl.c                                       500
    i2c\eddi_i2c.c	                                            501
    io\eddi_io_iso.c                                            502
    transceiver\eddi_Tra.c	                                    503
    transceiver\eddi_Tra_Broadcom.c	                            504
    transceiver\eddi_Tra_NEC.c	                                505
    transceiver\eddi_Tra_NSC.c	                                506
    transceiver\eddi_Tra_TI.c	                                507
    transceiver\eddi_Tra_Userspec.c                             508
    io\eddi_io_kram.c                                           509
    io\eddi_iocc.c                                              510

  CFG, Trace: 9990	
    cfg\eddi_trc.c	                                            9990
    cfg\eddi_sys.c	                                            9991
******************************************************************************/

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_M_ID_H


/*****************************************************************************/
/*  end of file eddi_m_id.h                                                  */
/*****************************************************************************/

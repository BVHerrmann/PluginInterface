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
/*  F i l e               &F: eps_pndevdrv_events_isr.c                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNDevDriver Implementation - module for events and interrupts.       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20062
#define EPS_MODULE_ID      20062

/* - Includes ------------------------------------------------------------------------------------- */
#include <eps_sys.h>           /* Types / Prototypes / Fucns */
#include <eps_rtos.h> 

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
/*-------------------------------------------------------------------------*/

#include <precomp.h>
#include <PnDev_Driver_Inc.h>       /* PnDevDrv Interface                       */

#include <eps_cp_hw.h>              /* EPS CP PSI adaption                      */
#include <eps_trc.h>                /* Tracing                                  */
#include <eps_pn_drv_if.h>          /* PN Device Driver Interface               */
#include <eps_pndevdrv.h>           /* PNDEV-Driver Interface implementation    */
#include <eps_hw_pnip.h>            /* Ertec200p HW adaption                    */
#include <eps_shm_file.h>           /* EPS Shared Memory File Interface         */
#include <eps_register.h>         	/* Register access macros                   */

#include "eps_pndevdrv_timer_threads.h"
#include "eps_pndevdrv_events_isr.h"
#include "eps_pndevdrv_helper.h"

typedef struct eps_pndevdrv_events_isr_store_tag
{
    LSA_BOOL bInit;
    uPNDEV_DRIVER_DESC *pDrvHandle;
} EPS_PNDEVDRV_EVENTS_ISR_STORE_TYPE, *EPS_PNDEVDRV_EVENTS_ISR_STORE_PTR_TYPE;

static EPS_PNDEVDRV_EVENTS_ISR_STORE_TYPE         g_EpsPnDevDrvEventIsr;
static EPS_PNDEVDRV_EVENTS_ISR_STORE_PTR_TYPE     g_pEpsPnDevDrvEventIsr = LSA_NULL;

/* - Local Board Defines -------------------------------------------------------------------------- */

#define INTEL_REG__IMS                   0x000D0
#define INTEL_REG__IMC                   0x000D8
#define INTEL_REG__ICR                   0x000C0

#define INTEL_IR_ICU_VECTOR__TXDW        (1U<<0)     
#define INTEL_IR_ICU_VECTOR__LSC         (1U<<2)
#define INTEL_IR_ICU_VECTOR__RXDW        (1U<<7)
#define INTEL_IR_ICU_VECTOR__GPHY        (1U<<10)

#define INTEL_IR_ICU_VECTOR__TXQE        (1U<<1)     // I82574 only
#define INTEL_IR_ICU_VECTOR__RXQ0        (1U<<20)    // I82574 only
#define INTEL_IR_ICU_VECTOR__TXQ0        (1U<<22)    // I82574 only
#define INTEL_IR_ICU_VECTOR__OTHER       (1U<<24)    // I82574 only

#define MICREL_IR_INTEN 					0x0028
#define MICREL_IR_INTST 					0x002C
#define MICREL_IR_ICU_VECTOR__DMLCIE    	(1U<<31)
#define MICREL_IR_ICU_VECTOR__DMTIE    		(1U<<30)
#define MICREL_IR_ICU_VECTOR__DMRIE    		(1U<<29)

#define IRTE_DRV_ISO_IRT_NEWCYCLE_MASK      0x01 	// mask for interrupt request of newCycle in Interrupt-MaskRegister-IRT-IRQ1
#define IRTE_DRV_ISO_EOI_VALUE              0x0F 	// value for EOI-IRQ1-Register
#define IRTE_ISO_IRQ_REQ_IRT_HP             0x17418 // Interrupt_Request_IRT_HP Register Offset
#define IRTE_ISO_IRQ_ACK_IRT_HP             0x17410 // Interrupt_Acknowledge_IRT_HP Register Offset
#define IRTE_ISO_IRQ_EOI_IRQ1_HP            0x17424 // EOI_IRQ1_HP

/* - Internal prototypes -------------------------------------------------------------------------- */
static LSA_UINT16 eps_pndevdrv_enable_hw_interrupt_intern(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE const pBoard);
static LSA_UINT16 eps_pndevdrv_disable_hw_interrupt_intern(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE const pBoard);

/**
 * Initializes the global structure g_EpsPnDevDrvEventIsr and assigns the pointer g_pEpsPnDevDrvEventIsr.
 * Note: Do not access g_EpsPnDevDrvEventIsr directly, only access g_pEpsPnDevDrvEventIsr or use the functions to access members of the structure.
 *
 * @see eps_pndevdrv_install - Calls this function
*/
LSA_VOID eps_pndevdrv_event_isr_init( uPNDEV_DRIVER_DESC * pDrvHandle )
{
    eps_memset(&g_EpsPnDevDrvEventIsr, 0, sizeof(g_EpsPnDevDrvEventIsr));

	g_pEpsPnDevDrvEventIsr = &g_EpsPnDevDrvEventIsr;
    g_pEpsPnDevDrvEventIsr->pDrvHandle = pDrvHandle;
    g_pEpsPnDevDrvEventIsr->bInit = LSA_TRUE;
}

/**
 * Deletes the pointer of g_pEpsPnDevDrvEventIsr to g_EpsPnDevDrvEventIsr
 *
 * @see eps_pndevdrv_uninstall - Calls this function
*/
LSA_VOID eps_pndevdrv_event_isr_undo_init( LSA_VOID )
{
	g_pEpsPnDevDrvEventIsr->bInit       = LSA_FALSE;
    g_pEpsPnDevDrvEventIsr->pDrvHandle  = LSA_NULL;
	g_pEpsPnDevDrvEventIsr              = LSA_NULL;
}

/**
 * Callbackfunction for event handling of PnDevDriver
 * 
 * @param [in] hDeviceIn		Handle of Device
 * @param [in] eEventClassIn	Enume with different DeviceClasses
 * @param [in] sEventIn		    string with cause of event
 * @param [in] pCbfParUserIn    pointer to user defined parameter
 * @return
 */
void __stdcall eps_pndevdrv_event_cbf( const HANDLE hDeviceIn, const ePNDEV_EVENT_CLASS eEventClassIn, PNDEV_CHAR * sEventIn, const void * pCbfParUserIn )
{
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pCbfParUserIn;

    #ifdef EPS_PNDEVDRV_PRINTF_EVENTS

	LSA_UNUSED_ARG(hDeviceIn);

	printf("\r\nReceived PnDevDrv Event for %s:\r\n", (LSA_CHAR *)pBoard->OpenDevice.sPciLoc);
	printf("    ");
	switch(eEventClassIn)
	{
		case ePNDEV_EVENT_CLASS_INVALID:
			printf("Invalid:       ");
			break;
		case ePNDEV_EVENT_CLASS_TEST:
			printf("Test:          ");
			break;
		case ePNDEV_EVENT_CLASS_FATAL_FW:
			printf("Fatal FW:      ");
			break;
		case ePNDEV_EVENT_CLASS_FATAL_ASYNC:
			printf("Fatal Async:   ");
			break;
		case ePNDEV_EVENT_CLASS_WARNING:
			printf("Warning:       ");
			break;
		case ePNDEV_EVENT_CLASS_REQ_CLOSE:
			printf("Req Close:     ");
			break;
		case ePNDEV_EVENT_CLASS_DEBUG:
			printf("Debug:         ");
			break;
		default:
			printf("Unknown Event: ");
			break;
	}
	printf("%s",(LSA_CHAR *)sEventIn);
	printf("\r\n");

    #else
	LSA_UNUSED_ARG(hDeviceIn);
    #endif

	if (eEventClassIn == ePNDEV_EVENT_CLASS_FATAL_FW)
	{
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_event_cbf() - FATAL ERROR in Basic FW (hd_id=%u), please check the logfile!", pBoard->sysDev.hd_nr);
		eps_pndevdrv_show_fatal_dump(pBoard);
	}
	
    if (eEventClassIn == ePNDEV_EVENT_CLASS_FATAL_ASYNC)
    {
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
        {
            LSA_CHAR buffer[100];
            wcstombs(buffer, sEventIn, sizeof(buffer));
            EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "eps_pndevdrv_event_cbf() - FATAL event recieved from pndevdrv: %s", buffer);
        }
#else
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "eps_pndevdrv_event_cbf() - FATAL event recieved from pndevdrv: %s", sEventIn);
#endif
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_event_cbf() - FATAL ERROR (hd_id=%u)!", pBoard->sysDev.hd_nr);
#ifdef _TEST 
        // ignore "no sign of life" event on gtest!
#else
        EPS_FATAL("FATAL event received from pndevdrv!!!");
#endif
    }
    //lint --e(818) Pointer parameter 'sEventIn' could be declared as pointing to const - API require it to be non const
}

/**
 * Callbackfunction for interrupt handling of PnDevDriver 
 * 
 * @param [in] hDeviceIn		Handle of Device
 * @param [in] bDeviceTestIn	indicates if it is a interrupt for testing purpose
 * @param [in] eIntSrcIn		Enume with different DeviceClasses
 * @param [in] pCbfParUserIn    pointer to user defined parameter
 * @return
 */
void __stdcall eps_pndevdrv_isr_cbf( const HANDLE hDeviceIn, const BOOLEAN bDeviceTestIn, const ePNDEV_INT_SRC eIntSrcIn, const void * pCbfParUserIn )
{
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pCbfParUserIn;
    #if (PSI_CFG_USE_EDDS == 1)
	LSA_UINT16                  retValEnableIsr;
    #endif
	LSA_UNUSED_ARG(hDeviceIn);

	// Attention:
	//	- it is not allowed to call any service of PnDev_Driver here!

	if (bDeviceTestIn)
	{
		switch (eIntSrcIn)
		{
			case ePNDEV_INT_SRC_TEST:
			{
				#ifdef EPS_PNDEVDRV_PRINTF_ISR
				printf("\r\nReceived PnDevDrv Isr for %s: ePNDEV_INT_SRC_TEST\r\n", (LSA_CHAR *)pBoard->OpenDevice.sPciLoc);
				#endif
				break;
			}
			default:
			{
				/* Unexpected */
				EPS_FATAL("Unknown interrupt src in pndevdrv isr test mode.");
				break;
			}
            //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
		}
	}
	else	// StandardMode
	{
		EPS_ASSERT(pBoard != LSA_NULL);
		EPS_ASSERT(pBoard->bUsed);

		switch (pBoard->sHw.asic_type)
		{
            #if (PSI_CFG_USE_EDDP == 1)
			case EPS_PNDEV_ASIC_ERTEC200P:
			{
				if (eIntSrcIn == ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3)
				{
					if (pBoard->sIsrIsochronous.pCbf != LSA_NULL)
					{
						pBoard->sIsrIsochronous.pCbf(pBoard->sIsrIsochronous.uParam, pBoard->sIsrIsochronous.pArgs);

						eps_hw_e200p_ack_isochronous_interrupt(pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr, LSA_FALSE);
					}
					else
					{
						#ifdef EPS_PNDEVDRV_PRINTF_ISR
						printf("\r\nReceived PnDevDrv Isr for %s: ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3\r\n", (LSA_CHAR *)pBoard->OpenDevice.sPciLoc);
						printf("  Error: Isochronous Interrupt while Isochronous Cbf not registered!\r\n");
						#endif
					}
				}
				else
				{
					#ifdef EPS_PNDEVDRV_PRINTF_ISR
					printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
					printf("  Error: Unknown not supported Interrupt!\r\n");
					#endif
				}

				break;
			}
			case EPS_PNDEV_ASIC_HERA:
			{
				if (   (eIntSrcIn == ePNDEV_INT_SRC_HERA__PNIP_IFA_ICU_IRQ0)
					|| (eIntSrcIn == ePNDEV_INT_SRC_HERA__PNIP_IFB_ICU_IRQ0)
					|| (eIntSrcIn == ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ2)
					|| (eIntSrcIn == ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ2))
				{
					if (pBoard->sIsrInterrupt.pCbf != LSA_NULL)
					{
						if (   (eIntSrcIn == ePNDEV_INT_SRC_HERA__PNIP_IFA_ICU_IRQ0)
							|| (eIntSrcIn == ePNDEV_INT_SRC_HERA__PNIP_IFB_ICU_IRQ0))
						{
							// set IntSource
							pBoard->sIsrInterrupt.uParam = 0;
						}
						else if (   (eIntSrcIn == ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ2)
								 || (eIntSrcIn == ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ2))
						{
							// set IntSource
							// ToDo: once eddp supports IntSources, this value need to be changed
							pBoard->sIsrInterrupt.uParam = 2;
						}

						pBoard->sIsrInterrupt.pCbf(pBoard->sIsrInterrupt.uParam, pBoard->sIsrInterrupt.pArgs);
					}
					else
					{
						#ifdef EPS_PNDEVDRV_PRINTF_ISR
	                    printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
						printf("  Error: Isochronous Interrupt while Isochronous Cbf not registered!\r\n");
						#endif
					}
				}
				else if (eIntSrcIn == ePNDEV_INT_SRC_HERA__SCRB_SW_INT16)
				{
                    if (pBoard->sIsrIpcRx.pCbf != LSA_NULL)
                    {
                        pBoard->sIsrIpcRx.pCbf(pBoard->sIsrIpcRx.uParam, pBoard->sIsrIpcRx.pArgs);
                    }
                    else
                    {
                        #ifdef EPS_PNDEVDRV_PRINTF_ISR
                        printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
                        printf("  Error: IPC RX Interrupt while IPC RX Cbf not registered!\r\n");
                        #endif
                    }
				}
				else
				{
					#ifdef EPS_PNDEVDRV_PRINTF_ISR
					printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
					printf("  Error: Unknown not supported Interrupt!\r\n");
					#endif
				}

				break;
			}
			#endif //PSI_CFG_USE_EDDP
			#if (PSI_CFG_USE_EDDS == 1)
			case EPS_PNDEV_ASIC_INTEL_XX:
			{
                switch (pBoard->pDeviceInfo->eBoard)
                {
                    case ePNDEV_BOARD_I210:
                    {
                        if (eIntSrcIn == ePNDEV_INT_SRC_I210)
                        {
                            if (pBoard->sIsrInterrupt.pCbf != LSA_NULL)
					        {
                                pBoard->sIsrInterrupt.pCbf(pBoard->sIsrInterrupt.uParam, pBoard->sIsrInterrupt.pArgs);
                                
                                if (pBoard->sIsrDesc == EPS_PNDEV_ISR_INTERRUPT)
                                    // just for normal interrupt mode
                                    // for EPS_PNDEV_ISR_CPU this is not allowed !!
                                {   
				                	//enable interrupts at the board again -> PnDevDrv deactivates them
                                    retValEnableIsr = eps_pndevdrv_enable_hw_interrupt_intern(pBoard);
                                    EPS_ASSERT(EPS_PNDEV_RET_OK == retValEnableIsr);
                                }				                
					        }
					        else
					        {
						        #ifdef EPS_PNDEVDRV_PRINTF_ISR
						        printf("\r\nReceived PnDevDrv Isr for %s: ePNDEV_INT_SRC_I210\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc);
						        printf("  Error: Isochronous Interrupt while Interrupt Cbf not registered!\r\n");
						        #endif
					        }
                        }
                        else
				        {
					        #ifdef EPS_PNDEVDRV_PRINTF_ISR
					        printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
					        printf("  Error: Unknown not supported Interrupt!\r\n");
					        #endif
				        }
                        break;
                    }
					case ePNDEV_BOARD_I210SFP:
					{
						if (eIntSrcIn == ePNDEV_INT_SRC_I210SFP)
						{
							if (pBoard->sIsrInterrupt.pCbf != LSA_NULL)
							{
								pBoard->sIsrInterrupt.pCbf(pBoard->sIsrInterrupt.uParam, pBoard->sIsrInterrupt.pArgs);

								if (pBoard->sIsrDesc == EPS_PNDEV_ISR_INTERRUPT)
									// just for normal interrupt mode
									// for EPS_PNDEV_ISR_CPU this is not allowed !!
								{
									//enable interrupts at the board again -> PnDevDrv deactivates them
									retValEnableIsr = eps_pndevdrv_enable_hw_interrupt_intern(pBoard);
									EPS_ASSERT(EPS_PNDEV_RET_OK == retValEnableIsr);
								}
							}
							else
							{
								#ifdef EPS_PNDEVDRV_PRINTF_ISR
								printf("\r\nReceived PnDevDrv Isr for %s: ePNDEV_INT_SRC_I210SFP\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc);
								printf("  Error: Isochronous Interrupt while Interrupt Cbf not registered!\r\n");
								#endif
							}
						}
						else
						{
							#ifdef EPS_PNDEVDRV_PRINTF_ISR
							printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
							printf("  Error: Unknown not supported Interrupt!\r\n");
							#endif
						}
						break;
					}
					case ePNDEV_BOARD_I82574:
                    {
                        if (eIntSrcIn == ePNDEV_INT_SRC_I82574)
                        {
                            if(pBoard->sIsrInterrupt.pCbf != LSA_NULL)
					        {
						        pBoard->sIsrInterrupt.pCbf(pBoard->sIsrInterrupt.uParam, pBoard->sIsrInterrupt.pArgs);

                                if (pBoard->sIsrDesc == EPS_PNDEV_ISR_INTERRUPT)
                                    // just for normal interrupt mode
                                    // for EPS_PNDEV_ISR_CPU this is not allowed !!
                                {  
                                	//enable interrupts at the board again -> PnDevDrv deactivates them
                                    retValEnableIsr = eps_pndevdrv_enable_hw_interrupt_intern(pBoard);
                                    EPS_ASSERT(EPS_PNDEV_RET_OK == retValEnableIsr);
								}
					        }
					        else
					        {
						        #ifdef EPS_PNDEVDRV_PRINTF_ISR
						        printf("\r\nReceived PnDevDrv Isr for %s: ePNDEV_INT_SRC_I82574\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc);
						        printf("  Error: Isochronous Interrupt while Interrupt Cbf not registered!\r\n");
						        #endif
					        }
                        }
                        else
				        {
					        #ifdef EPS_PNDEVDRV_PRINTF_ISR
					        printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
					        printf("  Error: Unknown not supported Interrupt!\r\n");
					        #endif
				        }
                        break;
                    }
                    default:
                    {
				        /* Unsupported Now */
				        EPS_FATAL("Not supported board for pndevdrv interrupt");
				        break;
                    }
                    //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
                }
				break;
			}
			case EPS_PNDEV_ASIC_KSZ88XX:
			{
			    switch(pBoard->pDeviceInfo->eBoard)
			    {
					case ePNDEV_BOARD_MICREL_KSZ8841:
					{
						if(eIntSrcIn == ePNDEV_INT_SRC_MICREL_KSZ8841)
						{
							if(pBoard->sIsrInterrupt.pCbf != LSA_NULL)
							{
								pBoard->sIsrInterrupt.pCbf(pBoard->sIsrInterrupt.uParam, pBoard->sIsrInterrupt.pArgs);
			
								if( pBoard->sIsrDesc == EPS_PNDEV_ISR_INTERRUPT )
									// just for normal interrupt mode
									// for EPS_PNDEV_ISR_CPU this is not allowed !!
								{  
									//enable interrupts at the board again -> PnDevDrv deactivates them
                                    retValEnableIsr = eps_pndevdrv_enable_hw_interrupt_intern(pBoard);
                                    EPS_ASSERT(EPS_PNDEV_RET_OK == retValEnableIsr);
								}
							}
							else
							{
								#ifdef EPS_PNDEVDRV_PRINTF_ISR
								printf("\r\nReceived PnDevDrv Isr for %s: ePNDEV_INT_SRC_MICREL_KSZ8841\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc);
								printf("  Error: Isochronous Interrupt while Interrupt Cbf not registered!\r\n");
								#endif
							}
						}
						else
						{
							#ifdef EPS_PNDEVDRV_PRINTF_ISR
							printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
							printf("  Error: Unknown not supported Interrupt!\r\n");
							#endif
						}
						break;
					}
					case ePNDEV_BOARD_MICREL_KSZ8842:
					{
						if(eIntSrcIn == ePNDEV_INT_SRC_MICREL_KSZ8842)
						{
							if(pBoard->sIsrInterrupt.pCbf != LSA_NULL)
							{
								pBoard->sIsrInterrupt.pCbf(pBoard->sIsrInterrupt.uParam, pBoard->sIsrInterrupt.pArgs);
			
								if(pBoard->sIsrDesc == EPS_PNDEV_ISR_INTERRUPT)
									// just for normal interrupt mode
									// for EPS_PNDEV_ISR_CPU this is not allowed !!
								{  
									//enable interrupts at the board again -> PnDevDrv deactivates them
                                    retValEnableIsr = eps_pndevdrv_enable_hw_interrupt_intern(pBoard);
                                    EPS_ASSERT(EPS_PNDEV_RET_OK == retValEnableIsr);
								}
							}
							else
							{
								#ifdef EPS_PNDEVDRV_PRINTF_ISR
								printf("\r\nReceived PnDevDrv Isr for %s: ePNDEV_INT_SRC_MICREL_KSZ8842\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc);
								printf("  Error: Isochronous Interrupt while Interrupt Cbf not registered!\r\n");
								#endif
							}
						}
						else
						{
							#ifdef EPS_PNDEVDRV_PRINTF_ISR
							printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
							printf("  Error: Unknown not supported Interrupt!\r\n");
							#endif
						}
						break;
					}
					default:
					{
						/* Unsupported Now */
						EPS_FATAL("Not supported board for pndevdrv interrupt");
						break;
					}
                    //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
			    }
			    break;
			}
			case EPS_PNDEV_ASIC_TI_XX:
			{
                switch (pBoard->pDeviceInfo->eBoard)
                {
                    case ePNDEV_BOARD_TI_AM5728:
                    {
				        EPS_FATAL("Interrupt mode not supported for TI board AM5728");
				        break;
                    }
                    default:
                    {
				        /* Unsupported Now */
				        EPS_FATAL("Not supported board for pndevdrv interrupt");
				        break;
                    }
                    //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
                }
                //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
				break;
			}
			#endif //PSI_CFG_USE_EDDS
            #if (PSI_CFG_USE_EDDI == 1)
            case EPS_PNDEV_ASIC_IRTEREV7:
            {   
                if (eIntSrcIn == ePNDEV_INT_SRC_SOC__IRTE_IRQ1)
                {
                    if (pBoard->sIsrIsochronous.pCbf != LSA_NULL)
                    {
                        pBoard->sIsrIsochronous.pCbf(pBoard->sIsrIsochronous.uParam, pBoard->sIsrIsochronous.pArgs);
                    }
                    else
                    {
                    #ifdef EPS_PNDEVDRV_PRINTF_ISR
                        printf("\r\nReceived PnDevDrv Isr for %s: ePNDEV_INT_SRC_SOC__IRTE_IRQ1\r\n", (LSA_CHAR *)pBoard->OpenDevice.sPciLoc);
                        printf("  Error: Isochronous Interrupt while Isochronous Cbf not registered!\r\n");
                    #endif
                    }
                }
                else if (eIntSrcIn == ePNDEV_INT_SRC_SOC__SW_IRQ_14)
                {
                    if (pBoard->sIsrIpcRx.pCbf != LSA_NULL)
                    {
                        pBoard->sIsrIpcRx.pCbf(pBoard->sIsrIpcRx.uParam, pBoard->sIsrIpcRx.pArgs);
                    }
                    else
                    {
                        #ifdef EPS_PNDEVDRV_PRINTF_ISR
                        printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
                        printf("  Error: IPC RX Interrupt while IPC RX Cbf not registered!\r\n");
                        #endif
                    }
                }
                else
                {
                    #ifdef EPS_PNDEVDRV_PRINTF_ISR
                    printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n", (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
                    printf("  Error: Unknown not supported Interrupt!\r\n");
                    #endif
                }
                break;
            }
            #endif // PSI_CFG_USE_EDDI
			default:
			{
				/* Unsupported Now */
				EPS_FATAL("Not supported asic for pndevdrv interrupt");
				break;
			}
            //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
		}
	}
}

/**
 * enable mentioned interrupt method for current board
 * 
 * @param [in] pHwInstIn 		pointer to structure with hardware configuration
 * @param [in] pInterrupt		pointer to enum with method type
 * @param [in] pCbf				pointer to Callbackfunction
 * @return EPS_PNDEV_RET_OK, upon successful execution
 * @return EPS_PNDEV_RET_ERR
 */
static LSA_UINT16 eps_pndevdrv_enable_interrupt_intern( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf )
{
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
	uPNDEV_CHANGE_INT_IN        uChangeIntIn;
	uPNDEV_CHANGE_INT_OUT       uChangeIntOut;
	ePNDEV_RESULT               drvResult;
	LSA_UINT16                  result = EPS_PNDEV_RET_OK;
    LSA_UINT16                  retValEnableIsr;

	pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

    // save the ISR description
	pBoard->sIsrDesc = *pInterrupt;

	switch(*pInterrupt)
	{
		case EPS_PNDEV_ISR_ISOCHRONOUS:
			eps_memset(&uChangeIntIn, 0, sizeof(uChangeIntIn));
			uChangeIntIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;
			uChangeIntIn.eIntAction      = ePNDEV_INT_ACTION_UNMASK;
			uChangeIntIn.lPrioThread     = EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE;

			if(pBoard->sIsrIsochronous.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			switch(pHwInstIn->asic_type)
			{
				case EPS_PNDEV_ASIC_ERTEC400:
		            ///@todo LaM: IRT for ERTEC400 not implemented
		            result = EPS_PNDEV_RET_UNSUPPORTED;		  
					break;
				case EPS_PNDEV_ASIC_ERTEC200:
				    ///@todo LaM: IRT for ERTEC200 not implemented
				    result = EPS_PNDEV_RET_UNSUPPORTED;
					break;
				case EPS_PNDEV_ASIC_IRTEREV7:
                    if  (pBoard->sIsrIpcRx.pCbf != LSA_NULL)
                        // if hif ipc interrupt is enabled
                    {
                        uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_SOC__SW_IRQ_14] = LSA_TRUE; // hif ipc interrupt has to be kept unmasked
                    }
                    // unmask interrupts
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_SOC__IRTE_IRQ1] = LSA_TRUE;

                    // ack interrupts
                    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_SOC__IRTE_IRQ1] = LSA_TRUE;
					break;
                #if (PSI_CFG_USE_EDDP == 1)
				case EPS_PNDEV_ASIC_ERTEC200P:
					eps_hw_e200p_enable_isochronous_interrupt(pHwInstIn->EpsBoardInfo.eddp.pnip.base_ptr, pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr, LSA_FALSE);

					// unmask interrupts
					uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3] = LSA_TRUE;   // enable accumulative interrupts ICU

					// ack interrupts
					uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3] = LSA_TRUE;      // ack pending accumulative interrupts ICU
					break;
				case EPS_PNDEV_ASIC_HERA:
					break;
                #endif //PSI_CFG_USE_EDDP
				case EPS_PNDEV_ASIC_INTEL_XX:
          			///@todo IRT for INTEL_XX not implemented
          			result = EPS_PNDEV_RET_UNSUPPORTED;
					break;
				case EPS_PNDEV_ASIC_KSZ88XX:
          			///@todo IRT for KSZ88XX not implemented
          			result = EPS_PNDEV_RET_UNSUPPORTED;
					break;
				case EPS_PNDEV_ASIC_TI_XX:
          			///@todo IRT for TI_XX not implemented
          			result = EPS_PNDEV_RET_UNSUPPORTED;
					break;
				default:
					EPS_FATAL("not supported ASIC type for enabling intern interrupt");
					break;
			}

            if(result == EPS_PNDEV_RET_OK)
            {
			    // enable desired interrupt source(s)
			    drvResult = g_pEpsPnDevDrvEventIsr->pDrvHandle->uIf.pFnChangeInt(g_pEpsPnDevDrvEventIsr->pDrvHandle, sizeof(uPNDEV_CHANGE_INT_IN),sizeof(uPNDEV_CHANGE_INT_OUT), &uChangeIntIn, &uChangeIntOut);

			    EPS_ASSERT(drvResult == ePNDEV_OK);

			    pBoard->sIsrIsochronous = *pCbf;

                // Acknowledge newcycle irq - TODO. What means: "Reset the Bits in IRR of the Host--Processor, which are set in the IAR--Data--Word"
                if(pHwInstIn->asic_type == EPS_PNDEV_ASIC_IRTEREV7)
                {
                    LSA_UINT32 RegVal;
                    // read the current interrupt request register
                    RegVal = EPS_REG32_READ((LSA_UINT32)pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.uBase.pPtr + IRTE_ISO_IRQ_REQ_IRT_HP);
                    // set the newCycle
                    RegVal |= IRTE_DRV_ISO_IRT_NEWCYCLE_MASK;
                    // and write the value back to register
                    EPS_REG32_WRITE((LSA_UINT32)pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.uBase.pPtr + IRTE_ISO_IRQ_ACK_IRT_HP, RegVal);
                    // Write EOI_IRQ1_HP
                    EPS_REG32_WRITE(pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.uBase.pPtr + IRTE_ISO_IRQ_EOI_IRQ1_HP, IRTE_DRV_ISO_EOI_VALUE);
                }
            }
			break;
		case EPS_PNDEV_ISR_PN_GATHERED:
			/* In PCI Mode only 1 real interrupt is available which is used for Isochrnous Mode */
			/* So we create polling threads for other pn interrupts                             */
			if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			pBoard->sIsrPnGathered = *pCbf;

			#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
			// start high performance pndevdrv timer (only in windows)
			result = eps_pndevdrv_open_pn_gathered_timer(pBoard);
			#else
			// start nea polling thread (adaptive sleeping < 1 ms not possible in non realtime windows)
			result = eps_pndevdrv_create_pn_gathered_polling_thread(pBoard);
			#endif

			if(result != EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}

			break;
		case EPS_PNDEV_ISR_PN_NRT:
			/* In PCI Mode only 1 real interrupt is available which is used for Isochrnous Mode */
			/* So we create polling threads for other pn interrupts                             */
			if(pBoard->sIsrPnNrt.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			pBoard->sIsrPnNrt = *pCbf;

			#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
			// start high performance pndevdrv timer (only in windows)
			result = eps_pndevdrv_open_pn_nrt_timer(pBoard);
			#else
			// start nea polling thread (adaptive sleeping < 1 ms not possible in non realtime windows)
			result = eps_pndevdrv_create_pn_nrt_polling_thread(pBoard);
			#endif

			if(result != EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnNrt.pCbf = LSA_NULL;
			}

			break;
        case EPS_PNDEV_ISR_INTERRUPT:
        case EPS_PNDEV_ISR_CPU:
			/* Interrupt Mode for EDDS */			
			if(pBoard->sIsrInterrupt.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

            eps_memset(&uChangeIntIn,0,sizeof(uChangeIntIn));
			uChangeIntIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;
			uChangeIntIn.eIntAction      = ePNDEV_INT_ACTION_UNMASK;
			uChangeIntIn.lPrioThread     = PNDEV_THREAD_PRIO_UNCHANGED;

            switch (pBoard->pDeviceInfo->eBoard)
            {
                case ePNDEV_BOARD_I210:
                {
                    // unmask interrupts
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_I210] = LSA_TRUE;   // enable accumulative interrupts ICU
                    // ack interrupts
                    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_I210] = LSA_TRUE;      // ack pending accumulative interrupts ICU
                    break;
                }
				case ePNDEV_BOARD_I210SFP:
				{
					// unmask interrupts
					uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_I210SFP] = LSA_TRUE;// enable accumulative interrupts ICU
					// ack interrupts
					uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_I210SFP] = LSA_TRUE;   // ack pending accumulative interrupts ICU
					break;
				}
				case ePNDEV_BOARD_I82574:
                {
                    // unmask interrupts
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_I82574] = LSA_TRUE; // enable accumulative interrupts ICU
			        // ack interrupts
			        uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_I82574] = LSA_TRUE;    // ack pending accumulative interrupts ICU
                    break;
                }
                case ePNDEV_BOARD_MICREL_KSZ8841:
                {
                    // unmask interrupts
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_MICREL_KSZ8841] = LSA_TRUE; // enable accumulative interrupts ICU
			        // ack interrupts
			        uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_MICREL_KSZ8841] = LSA_TRUE;    // ack pending accumulative interrupts ICU
                    break;
                }
                case ePNDEV_BOARD_MICREL_KSZ8842:
                {
                    // unmask interrupts
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_MICREL_KSZ8842] = LSA_TRUE; // enable accumulative interrupts ICU
			        // ack interrupts
			        uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_MICREL_KSZ8842] = LSA_TRUE;    // ack pending accumulative interrupts ICU
                    break;
                }
                case ePNDEV_BOARD_TI_AM5728:
                {
				    EPS_FATAL("Interrupt mode not supported for TI board AM5728");
                    break;
                }
				case ePNDEV_BOARD_FPGA1_HERA:
                {
                    #if (PSI_CFG_USE_EDDP == 1)
                    if (pHwInstIn->EpsBoardInfo.eddp.hw_interface == EDDP_HW_IF_A)
                         // Interface A
                    {
                        // unmask interrupts
					    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_HERA__PNIP_IFA_ICU_IRQ0] = LSA_TRUE;   // enable accumulative interrupts ICU
                        uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ2] = LSA_TRUE;   // enable accumulative interrupts ICU

					    // ack interrupts
					    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_HERA__PNIP_IFA_ICU_IRQ0] = LSA_TRUE;      // ack pending accumulative interrupts ICU
					    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ2] = LSA_TRUE;      // ack pending accumulative interrupts ICU
                    }
                    else // Interface B
                    {
                        // unmask interrupts
					    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_HERA__PNIP_IFB_ICU_IRQ0] = LSA_TRUE;   // enable accumulative interrupts ICU
					    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ2] = LSA_TRUE;   // enable accumulative interrupts ICU

					    // ack interrupts
					    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_HERA__PNIP_IFB_ICU_IRQ0] = LSA_TRUE;      // ack pending accumulative interrupts ICU
					    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ2] = LSA_TRUE;      // ack pending accumulative interrupts ICU
                    }
                    #endif //PSI_CFG_USE_EDDP
                    break;
                }
                default:
                {
                    EPS_FATAL("unsupported BoardType for EPS_PNDEV_ISR_INTERRUPT");
					break;
                }
                //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
            }

            // enable desired interrupt source(s)
			drvResult = g_pEpsPnDevDrvEventIsr->pDrvHandle->uIf.pFnChangeInt(g_pEpsPnDevDrvEventIsr->pDrvHandle, sizeof(uPNDEV_CHANGE_INT_IN),sizeof(uPNDEV_CHANGE_INT_OUT), &uChangeIntIn, &uChangeIntOut);

			if(drvResult != ePNDEV_OK)
			{
                pBoard->sIsrInterrupt.pCbf = LSA_NULL;
                EPS_FATAL("ChangeInt failed");
			}
            else
            {                   
                pBoard->sIsrInterrupt = *pCbf;
                
                //enable interrupts at the board
                retValEnableIsr = eps_pndevdrv_enable_hw_interrupt_intern(pBoard);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retValEnableIsr);
            }

			break;     
		    
        case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_POLLINTERRUPT is implemented in another way");            
            break;
        case EPS_PNDEV_ISR_IPC_RX:
            /* Interrupt Mode for IPC RX */           
            if(pBoard->sIsrIpcRx.pCbf != LSA_NULL)
            {
                result = EPS_PNDEV_RET_ERR;
                break;
            }
            else
            {
                eps_memset(&uChangeIntIn, 0, sizeof(uChangeIntIn));
                uChangeIntIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;
                uChangeIntIn.eIntAction = ePNDEV_INT_ACTION_UNMASK;
                uChangeIntIn.lPrioThread = EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE;

                switch (pBoard->pDeviceInfo->eBoard)
                {
                case ePNDEV_BOARD_FPGA1_HERA:
                {
                    #if (PSI_CFG_USE_EDDP == 1)
					// Inc9 -> unmask HERA INT_SW_8, ab Inc12 unsmask INT_SW_14
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_HERA__SCRB_SW_INT14] = LSA_TRUE;   // enable accumulative interrupts ICU
                    // ack interrupts
                    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_HERA__SCRB_SW_INT14] = LSA_TRUE;      // ack pending accumulative interrupts ICU
                    #endif //PSI_CFG_USE_EDDP
                    break;
                }
                case ePNDEV_BOARD_CP1625:
                case ePNDEV_BOARD_DB_SOC1_PCI:
                case ePNDEV_BOARD_DB_SOC1_PCIE:
                {
                    #if (PSI_CFG_USE_EDDI == 1)
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_SOC__SW_IRQ_14] = LSA_TRUE;   // enable accumulative interrupts ICU
                    // ack interrupts
                    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_SOC__SW_IRQ_14] = LSA_TRUE;      // ack pending accumulative interrupts ICU
                    #endif //PSI_CFG_USE_EDDI
                    break;
                }
                default:
                {
                    EPS_FATAL("unsupported BoardType for EPS_PNDEV_ISR_IPC_RX");
                    break;
                }
                //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
                }

                // enable desired interrupt source(s)
                drvResult = g_pEpsPnDevDrvEventIsr->pDrvHandle->uIf.pFnChangeInt(g_pEpsPnDevDrvEventIsr->pDrvHandle, sizeof(uPNDEV_CHANGE_INT_IN), sizeof(uPNDEV_CHANGE_INT_OUT), &uChangeIntIn, &uChangeIntOut);

                EPS_ASSERT(drvResult == ePNDEV_OK);

                pBoard->sIsrIpcRx = *pCbf;
            }
            break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
	}

	if(result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled++;
	}

	return result;
}

/**
 * disable mentioned interrupt method for current board
 * 
 * @param [in] pHwInstIn 		pointer to structure with hardware configuration
 * @param [in] pInterrupt		pointer to enum with method type
 * @return 	EPS_PNDEV_RET_OK, upon successful execution or EPS_PNDEV_RET_ERR
 */
LSA_UINT16 eps_pndevdrv_disable_interrupt_intern(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt )
{
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
	uPNDEV_CHANGE_INT_IN uChangeIntIn;
	uPNDEV_CHANGE_INT_OUT uChangeIntOut;
	ePNDEV_RESULT drvResult;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(pBoard->uCountIsrEnabled != 0);

	switch(*pInterrupt)
	{
		case EPS_PNDEV_ISR_ISOCHRONOUS:
			eps_memset(&uChangeIntIn,0,sizeof(uChangeIntIn));
			uChangeIntIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;
			uChangeIntIn.eIntAction      = ePNDEV_INT_ACTION_UNMASK;
			uChangeIntIn.lPrioThread     = PNDEV_THREAD_PRIO_UNCHANGED;

			if(pBoard->sIsrIsochronous.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			switch(pHwInstIn->asic_type)
			{
				case EPS_PNDEV_ASIC_ERTEC400:
					break;
				case EPS_PNDEV_ASIC_ERTEC200:
					break;
				case EPS_PNDEV_ASIC_IRTEREV7:
                    if  (pBoard->sIsrIpcRx.pCbf != LSA_NULL)
                        // if hif ipc interrupt is enabled
                    {
                        uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_SOC__SW_IRQ_14] = LSA_TRUE; // hif ipc interrupt has to be kept unmasked
                    }
                    else
                    {
                        uChangeIntIn.eIntAction = ePNDEV_INT_ACTION_MASK_ALL;
                    }
                    
                    // mask interrupts
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_SOC__IRTE_IRQ1] = LSA_FALSE;

                    // disable ack interrupts
                    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_SOC__IRTE_IRQ1] = LSA_FALSE;
					break;
                #if (PSI_CFG_USE_EDDP == 1)
				case EPS_PNDEV_ASIC_ERTEC200P:
					eps_hw_e200p_disable_isochronous_interrupt(pHwInstIn->EpsBoardInfo.eddp.pnip.base_ptr, LSA_FALSE);

					// mask interrupts
					uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3] = LSA_FALSE;  // enable accumulative interrupts ICU

					// disable ack interrupts
					uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3] = LSA_FALSE;     // ack pending accumulative interrupts ICU
					break;
				case EPS_PNDEV_ASIC_HERA:
					break;
                #endif //PSI_CFG_USE_EDDP
				case EPS_PNDEV_ASIC_INTEL_XX:
					EPS_FATAL("IRT interrupts not supported for INTEL_XX.");
					break;
				case EPS_PNDEV_ASIC_KSZ88XX:
					EPS_FATAL("IRT interrupts not supported for KSZ88XX.");
					break;
				case EPS_PNDEV_ASIC_TI_XX:
					EPS_FATAL("IRT interrupts not supported for TI_XX.");
					break;
				default:
				    EPS_FATAL("Unknown ASIC type for disabling IRT interrupt.");
					break;
			}

			// enable desired interrupt source(s)
			drvResult = g_pEpsPnDevDrvEventIsr->pDrvHandle->uIf.pFnChangeInt(g_pEpsPnDevDrvEventIsr->pDrvHandle, sizeof(uPNDEV_CHANGE_INT_IN),sizeof(uPNDEV_CHANGE_INT_OUT), &uChangeIntIn, &uChangeIntOut);

			pBoard->sIsrIsochronous.pCbf = LSA_NULL;

			EPS_ASSERT(drvResult == ePNDEV_OK);

			break;
		case EPS_PNDEV_ISR_PN_GATHERED:
			/* In PCI Mode only 1 real interrupt is available which is used for Isochrnous Mode */
			/* Kill polling threads                                                             */
			if(pBoard->sIsrPnGathered.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
			// stop high performance pndevdrv timer
			result = eps_pndevdrv_close_pn_gathered_timer(pBoard);
			#else
			// kill nea polling thread
			result = eps_pndevdrv_kill_pn_gathered_polling_thread(pBoard);
			#endif

			if(result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}

			break;
		case EPS_PNDEV_ISR_PN_NRT:
			/* In PCI Mode only 1 real interrupt is available which is used for Isochrnous Mode */
			/* Kill polling threads                                                             */
			if(pBoard->sIsrPnNrt.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
			// stop high performance pndevdrv timer
			result = eps_pndevdrv_close_pn_nrt_timer(pBoard);
			#else
			// kill nea polling thread
			result = eps_pndevdrv_kill_pn_nrt_polling_thread(pBoard);
			#endif

			if(result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnNrt.pCbf = LSA_NULL;
			}

			break;
        case EPS_PNDEV_ISR_INTERRUPT:
        case EPS_PNDEV_ISR_CPU:
            /* Interrupt Mode for EDDS */           
            if(pBoard->sIsrInterrupt.pCbf == LSA_NULL)
            {
                result = EPS_PNDEV_RET_ERR;
                break;
            }

            eps_memset(&uChangeIntIn,0,sizeof(uChangeIntIn));
            uChangeIntIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;
            uChangeIntIn.eIntAction      = ePNDEV_INT_ACTION_UNMASK;
            uChangeIntIn.lPrioThread     = PNDEV_THREAD_PRIO_UNCHANGED;

            switch (pBoard->pDeviceInfo->eBoard)
            {               
                case ePNDEV_BOARD_I210:
                {
                    // unmask interrupts
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_I210] = LSA_FALSE;      // enable accumulative interrupts ICU
                    // ack interrupts
                    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_I210] = LSA_FALSE;         // ack pending accumulative interrupts ICU
                    break;
                }
				case ePNDEV_BOARD_I210SFP:
				{
					// unmask interrupts
					uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_I210SFP] = LSA_FALSE;   // enable accumulative interrupts ICU
					// ack interrupts
					uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_I210SFP] = LSA_FALSE;      // ack pending accumulative interrupts ICU
					break;
				}
				case ePNDEV_BOARD_I82574:
                {
                    // unmask interrupts
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_I82574] = LSA_FALSE;    // enable accumulative interrupts ICU
                    // ack interrupts
                    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_I82574] = LSA_FALSE;       // ack pending accumulative interrupts ICU
                    break;
                }
                case ePNDEV_BOARD_MICREL_KSZ8841:	
                {
                    // unmask interrupts
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_MICREL_KSZ8841] = LSA_FALSE; // enable accumulative interrupts ICU
			        // ack interrupts
			        uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_MICREL_KSZ8841] = LSA_FALSE;    // ack pending accumulative interrupts ICU
                    break;
                }
                case ePNDEV_BOARD_MICREL_KSZ8842:	
                {
                    // unmask interrupts
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_MICREL_KSZ8842] = LSA_FALSE; // enable accumulative interrupts ICU
			        // ack interrupts
			        uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_MICREL_KSZ8842] = LSA_FALSE;    // ack pending accumulative interrupts ICU
                    break;
                }
                case ePNDEV_BOARD_TI_AM5728:
                {
				    EPS_FATAL("Interrupt mode not supported for TI board AM5728");
                    break;
                }
				case ePNDEV_BOARD_FPGA1_HERA:
				{
					// mask interrupts
					// ToDo: for Hera all interrupts must be deactivated by corresponding action
					uChangeIntIn.eIntAction = ePNDEV_INT_ACTION_MASK_ALL;
					break;
				}
                default:
                {
                    EPS_FATAL("unsupported BoardType for EPS_PNDEV_ISR_INTERRUPT");
                    break;
                }
                //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
            }

            // disable desired interrupt source(s)
            drvResult = g_pEpsPnDevDrvEventIsr->pDrvHandle->uIf.pFnChangeInt(g_pEpsPnDevDrvEventIsr->pDrvHandle, sizeof(uPNDEV_CHANGE_INT_IN),sizeof(uPNDEV_CHANGE_INT_OUT), &uChangeIntIn, &uChangeIntOut);

            if (drvResult != ePNDEV_OK)
            {
                EPS_FATAL("ChangeInt failed");
            }
            else
            {
                LSA_UINT16 retValDisableIsr;
                pBoard->sIsrInterrupt.pCbf = LSA_NULL;

                //disable interrupts at the board
                retValDisableIsr = eps_pndevdrv_disable_hw_interrupt_intern(pBoard);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retValDisableIsr);
            }

            break;       
        case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_POLLINTERRUPT is implemented in another way");            
            break;
        case EPS_PNDEV_ISR_IPC_RX:
            /* Interrupt Mode for IPC RX */           
            if(pBoard->sIsrIpcRx.pCbf == LSA_NULL)
            {
                result = EPS_PNDEV_RET_ERR;
                break;
            }
            else
            {
                eps_memset(&uChangeIntIn, 0, sizeof(uChangeIntIn));
                uChangeIntIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;
                uChangeIntIn.eIntAction = ePNDEV_INT_ACTION_UNMASK;
                uChangeIntIn.lPrioThread = PNDEV_THREAD_PRIO_UNCHANGED;

                switch (pBoard->pDeviceInfo->eBoard)
                {
                case ePNDEV_BOARD_FPGA1_HERA:
                {
                    #if (PSI_CFG_USE_EDDP == 1)
                    // Inc9 -> mask HERA INT_SW_8, ab Inc12 mask INT_SW_14
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_HERA__SCRB_SW_INT14] = LSA_FALSE;   // mask accumulative interrupts ICU
                    // disable ack interrupts
                    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_HERA__SCRB_SW_INT14] = LSA_FALSE;      // disable ack pending accumulative interrupts ICU
                    #endif //PSI_CFG_USE_EDDP
                    break;
                }
                case ePNDEV_BOARD_CP1625:
                case ePNDEV_BOARD_DB_SOC1_PCI:
                case ePNDEV_BOARD_DB_SOC1_PCIE:
                {
                    #if (PSI_CFG_USE_EDDI == 1)
                    uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_SOC__SW_IRQ_14] = LSA_FALSE;   // mask accumulative interrupts ICU
                    // disable ack interrupts
                    uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_SOC__SW_IRQ_14] = LSA_FALSE;      // disable ack pending accumulative interrupts ICU
                    #endif //PSI_CFG_USE_EDDI
                    break;
                }
                default:
                {
                    EPS_FATAL("unsupported BoardType for EPS_PNDEV_ISR_IPC_RX");
                    break;
                }
                //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
                }

                // disable desired interrupt source(s)
                drvResult = g_pEpsPnDevDrvEventIsr->pDrvHandle->uIf.pFnChangeInt(g_pEpsPnDevDrvEventIsr->pDrvHandle, sizeof(uPNDEV_CHANGE_INT_IN), sizeof(uPNDEV_CHANGE_INT_OUT), &uChangeIntIn, &uChangeIntOut);

                pBoard->sIsrIpcRx.pCbf = LSA_NULL;

                EPS_ASSERT(drvResult == ePNDEV_OK);
            }
            break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
	}

	if (result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled--;
	}

	return result;
}

/**
 * enable interrupt for board
 * 
 * @param	[in] pHwInstIn		pointer to structure with hardware configuration
 * @param	[in] pInterrupt		pointer to enum with method type
 * @param	[in] pCbf			ptr to Callbackfunction
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_enable_interrupt( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf )
{
	LSA_UINT16 result;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_enable_interrupt() - enable interrupt");

	eps_pndevdrv_enter();

	result = eps_pndevdrv_enable_interrupt_intern(pHwInstIn, pInterrupt, pCbf);

	eps_pndevdrv_exit();

	return result;
	//lint --e(818) Pointer parameter 'pCbf' could be declared as pointing to const - API require it to be non const
}

/**
 * disable interrupt for board
 *
 * @param	[in] pHwInstIn		pointer to structure with hardware configuration
 * @param	[in] pInterrupt		pointer to enum with method type
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_disable_interrupt( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt )
{
	LSA_UINT16 result;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_disable_interrupt() - disable interrupt");

	eps_pndevdrv_enter();

	result = eps_pndevdrv_disable_interrupt_intern(pHwInstIn, pInterrupt);

	eps_pndevdrv_exit();

	return result;
}

/**
 * enable interrupt at the board
 * 
 * @param   pHwInstIn       pointer to structure with hardware configuration
 * @return  EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_enable_hw_interrupt_intern( EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE const pBoard )
{
    LSA_UINT16           result          = EPS_PNDEV_RET_OK;
    volatile uint32_t*   pIrRegister     = NULL;
    uint32_t             lTmpIcmWrite    = 0;
    uint32_t             lTmpIcmRead     = 0;

    EPS_ASSERT(pBoard->bUsed);

    switch (pBoard->pDeviceInfo->eBoard)
    {  
        case ePNDEV_BOARD_I210:
		case ePNDEV_BOARD_I210SFP:
		{
            pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uI210.uBar0.uBase.pPtr + INTEL_REG__IMS);

            // the IRs we want to activate
            //lint --e(835) A zero has been given as right argument to operator '+' - intended
            lTmpIcmWrite =  ( INTEL_IR_ICU_VECTOR__TXDW
                            | INTEL_IR_ICU_VECTOR__LSC
                            | INTEL_IR_ICU_VECTOR__RXDW
                            | INTEL_IR_ICU_VECTOR__GPHY);            
            
            // write to register
            EPS_REG32_WRITE(pIrRegister, lTmpIcmWrite);                    
            
            // read back from register for check
            lTmpIcmRead = EPS_REG32_READ(pIrRegister);   
            
            //EPS_ASSERT(lTmpIcmWrite != lTmpIcmRead);
            LSA_UNUSED_ARG(lTmpIcmRead);
            
            break;
        }
        case ePNDEV_BOARD_I82574:
        {
            pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uI82574.uBar0.uBase.pPtr + INTEL_REG__IMS);

            // the IRs we want to activate
            //lint --e(835) A zero has been given as right argument to operator '+' - intended
            lTmpIcmWrite =  ( INTEL_IR_ICU_VECTOR__TXDW
                            | INTEL_IR_ICU_VECTOR__LSC
                            | INTEL_IR_ICU_VECTOR__RXDW
                            | INTEL_IR_ICU_VECTOR__GPHY
                            | INTEL_IR_ICU_VECTOR__TXQE
                            | INTEL_IR_ICU_VECTOR__RXQ0
                            | INTEL_IR_ICU_VECTOR__TXQ0
                            | INTEL_IR_ICU_VECTOR__OTHER);

            // write to register
            EPS_REG32_WRITE(pIrRegister, lTmpIcmWrite);                    
            
            // read back from register for check
            lTmpIcmRead = EPS_REG32_READ(pIrRegister);   
            
            //EPS_ASSERT(lTmpIcmWrite != lTmpIcmRead);
            LSA_UNUSED_ARG(lTmpIcmRead);
            
            break;
        }
        case ePNDEV_BOARD_MICREL_KSZ8841:
        case ePNDEV_BOARD_MICREL_KSZ8842:
        {
        	if (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
        	   // Micrel 1 Porter
        	{
        		pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uKSZ8841.uBar0.uBase.pPtr + MICREL_IR_INTEN);
        	}
        	else if	(pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_MICREL_KSZ8842)
        			// Micrel 2 Porter
        	{
        		pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uKSZ8842.uBar0.uBase.pPtr + MICREL_IR_INTEN);
        	}	
            EPS_ASSERT(pIrRegister != LSA_NULL);
            // the IRs we want to activate
            lTmpIcmWrite =  (LSA_UINT32)
                            ( 	MICREL_IR_ICU_VECTOR__DMLCIE 
            				|	MICREL_IR_ICU_VECTOR__DMTIE  
							|	MICREL_IR_ICU_VECTOR__DMRIE);

            // write to register
            EPS_REG32_WRITE(pIrRegister, lTmpIcmWrite);                    
            
            // read back from register for check
            lTmpIcmRead = EPS_REG32_READ(pIrRegister);   
            LSA_UNUSED_ARG(lTmpIcmRead);
            break;
        }
        case ePNDEV_BOARD_TI_AM5728:
        {
			EPS_FATAL("Interrupt mode not supported for TI board AM5728");
            break;
        }
		case ePNDEV_BOARD_FPGA1_HERA:
		{
			break;
		}
        default:
        {
            EPS_FATAL("Unknown board for enabling HW interrupts");
            break;
        }
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
    }

    return result;
}

/**
 * enable interrupt at the board
 *
 * @param   pHwInstIn       pointer to structure with hardware configuration
 * @return  EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_enable_hw_interrupt( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
{
    LSA_UINT16                  result;
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;

    EPS_ASSERT(pHwInstIn != LSA_NULL);
    EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

    pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_pndevdrv_enable_hw_interrupt() - enable hardware interrupt");

    eps_pndevdrv_enter();     

    result = eps_pndevdrv_enable_hw_interrupt_intern(pBoard);

    eps_pndevdrv_exit();

    return result;
}

/**
 * disable interrupt at the board
 *
 * @param   pHwInstIn       pointer to structure with hardware configuration
 * @return  EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_disable_hw_interrupt_intern( EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE const pBoard )
{
    LSA_UINT16 result               = EPS_PNDEV_RET_OK;
    volatile uint32_t* pIrRegister  = NULL;
    uint32_t          lTmpIcmWrite  = 0;
    uint32_t          lTmpIcmRead   = 0;

    EPS_ASSERT(pBoard->bUsed);

    switch (pBoard->pDeviceInfo->eBoard)
    {
        case ePNDEV_BOARD_I210:
		case ePNDEV_BOARD_I210SFP:
		{
            pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uI210.uBar0.uBase.pPtr + INTEL_REG__IMC);

            // the IRs we want to deactivate (all)
            lTmpIcmWrite = 0xFFFFFFFF;

            // write to register
            EPS_REG32_WRITE(pIrRegister, lTmpIcmWrite);                    

            // read back from register for check
            lTmpIcmRead = EPS_REG32_READ(pIrRegister);   

            //EPS_ASSERT(lTmpIcmRead != 0x00000000);
            LSA_UNUSED_ARG(lTmpIcmRead);

            break;
        }
        case ePNDEV_BOARD_I82574:
        {
            pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uI82574.uBar0.uBase.pPtr + INTEL_REG__IMC);

            // the IRs we want to deactivate (all)
            lTmpIcmWrite = 0xFFFFFFFF;

            // write to register
            EPS_REG32_WRITE(pIrRegister, lTmpIcmWrite);                    
            
            // read back from register for check
            lTmpIcmRead = EPS_REG32_READ(pIrRegister);   
            
            //EPS_ASSERT(lTmpIcmRead != 0x00000000);
            LSA_UNUSED_ARG(lTmpIcmRead);
            
            break;
        }
        case ePNDEV_BOARD_MICREL_KSZ8841:
        case ePNDEV_BOARD_MICREL_KSZ8842:
        {
        	if	(pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
        		// Micrel 1 Porter
        	{
        		pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uKSZ8841.uBar0.uBase.pPtr + MICREL_IR_INTEN);
        	}
        	else if	(pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_MICREL_KSZ8842)
        			// Micrel 2 Porter
        	{
        		pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uKSZ8842.uBar0.uBase.pPtr + MICREL_IR_INTEN);
        	}	
            EPS_ASSERT(LSA_NULL != pIrRegister);
            // the IRs we want to activate
            lTmpIcmWrite = 0x0;

            // write to register
            EPS_REG32_WRITE(pIrRegister, lTmpIcmWrite);                    
            
            // read back from register for check
            lTmpIcmRead = EPS_REG32_READ(pIrRegister);
            LSA_UNUSED_ARG(lTmpIcmRead);
            
            break;
        }
        case ePNDEV_BOARD_TI_AM5728:
        {
			EPS_FATAL("Interrupt mode not supported for TI board AM5728");
            break;
        }
		case ePNDEV_BOARD_FPGA1_HERA:
		{
			break;
		}        
        default:
        {
            EPS_FATAL("Unknown board for disabling HW interrupts");
            break;
        }
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
    }

    return result;
}

/**
 * disable interrupt at the board
 *
 * @param   pHwInstIn       pointer to structure with hardware configuration
 * @return  EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_disable_hw_interrupt( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
{
    LSA_UINT16                  result;
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;

    EPS_ASSERT(pHwInstIn != LSA_NULL);
    EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

    pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_pndevdrv_disable_hw_interrupt() - disable hardware interrupt");

    eps_pndevdrv_enter();

    result = eps_pndevdrv_disable_hw_interrupt_intern(pBoard);

    eps_pndevdrv_exit();

    return result;
}

/**
 * read the interrupts from the board
 *
 * @param   pHwInstIn       pointer to structure with hardware configuration
 * @return  EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_read_hw_interrupt( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 * interrupts )
{
    LSA_UINT16                  result          = EPS_PNDEV_RET_OK;
    volatile uint32_t*          pIrRegister     = NULL;
    LSA_UINT32                  lOccuredInts    = 0;
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;

    EPS_ASSERT(pHwInstIn != LSA_NULL);
    EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
    
    pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

    EPS_ASSERT(pBoard->bUsed);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_pndevdrv_read_hw_interrupt() - read hardware interrupts");    

    eps_pndevdrv_enter();

    switch (pBoard->pDeviceInfo->eBoard)
    {  
        case ePNDEV_BOARD_I210:
		case ePNDEV_BOARD_I210SFP:
		{
            pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uI210.uBar0.uBase.pPtr + INTEL_REG__ICR);

            // read from register
            lOccuredInts = EPS_REG32_READ(pIrRegister);   

            // fill to given var
            *interrupts = lOccuredInts;

            break;
        }
        case ePNDEV_BOARD_I82574:
        {
            pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uI82574.uBar0.uBase.pPtr + INTEL_REG__ICR);

            // read from register
            lOccuredInts = EPS_REG32_READ(pIrRegister);   

            // fill to given var
            *interrupts = lOccuredInts;

            break;
        }
        case ePNDEV_BOARD_MICREL_KSZ8841:
        case ePNDEV_BOARD_MICREL_KSZ8842:
        {
        	if (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
        	   // Micrel 1 Porter
        	{
        		pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uKSZ8841.uBar0.uBase.pPtr + MICREL_IR_INTST);
        	}
        	else if	(pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_MICREL_KSZ8842)
        			// Micrel 2 Porter
        	{
        		pIrRegister = (volatile uint32_t*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uKSZ8842.uBar0.uBase.pPtr + MICREL_IR_INTST);
        	}	
            EPS_ASSERT(LSA_NULL != pIrRegister);
            // read from register
            lOccuredInts = EPS_REG32_READ(pIrRegister);   
            
            // fill to given var
            *interrupts = lOccuredInts;  
            
            // clear interrupts at Icu
            EPS_REG32_WRITE(pIrRegister, lOccuredInts); 
            
            break;
        }
        case ePNDEV_BOARD_TI_AM5728:
        {
			EPS_FATAL("Interrupt mode not supported for TI board AM5728");
            break;
        }
        default:
        {
            EPS_FATAL("Unknown board for reading HW interrupts");
        }
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
    }

    eps_pndevdrv_exit();

    return result;
}

#endif //((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

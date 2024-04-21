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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: INTEL_edd.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Low Level functions for EDDS (INTEL)             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20
#define EDDS_MODULE_ID     LTRC_ACT_MODUL_ID  /* Ids 16..31 reserved for LL */

/**
 * \addtogroup INTEL
 * \ingroup LLIF
 * @{
 */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"
#include "intel_inc.h"
#include "intel_reg.h"
/*===========================================================================*/
/*                              global Variables                             */
/*===========================================================================*/

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/** \brief      Setup MDI control for reading PHY register.
 *
 * \details     The MDI control register of MAC is programmed for PHY register read.
 *              After this functions is called, #intel_endReadPhyReg could be called.
 *
 * \param       handle      Handle of EDDS interface (hardware instance).
 * \param       PhyRegAddr  PHY register address for reading.
 */
static LSA_VOID intel_beginReadPhyReg(
        INTEL_LL_HANDLE_TYPE const * const handle,
        LSA_UINT8 PhyRegAddr
        )
{
    intel_reg32(INTEL_MDIC) = INTEL_BIT(INTEL_MDIC_R,0)                        /* clear ready */
                            | INTEL_BIT(INTEL_MDIC_I,0)                        /* no interrupt */
                            | INTEL_BIT(INTEL_MDIC_E,0)                        /* clear error */
                            | INTEL_BITS(INTEL_MDIC_OP,INTEL_MDIC_OP_MDI_READ) /* read operation */
                            | INTEL_BITS(INTEL_MDIC_PHYADD,1)                  /* internal mii phy address is always 1 */
                            | INTEL_BITS(INTEL_MDIC_REGADD,PhyRegAddr);        /* phy register address */
}


/** \brief      Setup MDI control for writing PHY register.
 *
 * \details     The MDI control register of MAC is programmed for PHY register write.
 *              After this functions is called, #intel_WritePhyReg could be called.
 *
 * \param       handle      handle of EDDS interface (hardware instance).
 * \param       PhyRegAddr  PHY register address for reading
 * \param[in]   data        Data to hand over to the PHY register.
 */
static LSA_VOID intel_beginWritePhyReg(
        INTEL_LL_HANDLE_TYPE const * const handle,
        LSA_UINT8 PhyRegAddr,
        LSA_UINT16 data
        )
{
    intel_reg32(INTEL_MDIC) = INTEL_BITS(INTEL_MDIC_DATA,data)
                            | INTEL_BIT(INTEL_MDIC_R,0)                         /* clear ready */
                            | INTEL_BIT(INTEL_MDIC_I,0)                         /* no interrupt */
                            | INTEL_BIT(INTEL_MDIC_E,0)                         /* clear error */
                            | INTEL_BITS(INTEL_MDIC_OP,INTEL_MDIC_OP_MDI_WRITE) /* write operation */
                            | INTEL_BITS(INTEL_MDIC_PHYADD,1)                   /* internal mii phy address is always 1 */
                            | INTEL_BITS(INTEL_MDIC_REGADD,PhyRegAddr);         /* phy register address */
}



/** \brief      Finish read/write of data from/to PHY register.
 *
 * \details     This function waits until to read/write of data from/to PHY register is finished.
 *              The address of the PHY register was setup with #intel_beginReadPhyReg or #intel_beginWritePhyReg.
 *
 * \param       handle      handle of EDDS interface (hardware instance).
 * \param[out]  ref_data    reference to a data storage for read data. If a NULL reference (NULL-pointer)
 *                          is hand over, no data is transfered to caller.
 *
 * \return      LSA_TRUE, if transfer was successfully finished.
 *              ref_data has PHY data that was read, if a read was programmed with #intel_beginReadPhyReg.
 *              Otherwise, ref_data has no data.
 *              LSA_FALSE, if an error (e.g. timeout) occurred; in this case ref_data has no data.
 *
 * \note        To finish a read, #intel_endReadPhyReg should be called. To Finish a programmed write,
 *              #intel_endWritePhyReg should be called. These functions are using this generalized one.
 */
static LSA_BOOL intel_endPhyReg(
        INTEL_LL_HANDLE_TYPE const * const handle,
        LSA_UINT16* ref_data)
{
    LSA_BOOL transferOK;
    LSA_UINT32 mdic;

    transferOK = LSA_TRUE;

    /* check, if PHY has finished and therefore has set the ready-bit.
     * \see intel i210 data sheet:
     * Note: A MDIO read or write might take as long as 64us from the processor write to the Ready bit assertion.
     */
/*    EDDS_WAIT_UNTIL_TIMEOUT(
            (mdic = intel_reg32(INTEL_MDIC),(1 == INTEL_GETBIT(INTEL_MDIC_R,mdic))),
            100, (transferOK = LSA_FALSE));
 */
    mdic = intel_reg32(INTEL_MDIC);

    if(!INTEL_GETBIT(INTEL_MDIC_R,mdic))
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
            "intel_endPhyReg: PHY is not ready yet.");
        transferOK = LSA_FALSE;
    }

    if(transferOK)
    {
        if(LSA_NULL != ref_data)
        {
            /* write data bits to ref_data (only reasonable data if a read was programmed) */
            *ref_data = INTEL_GETBITS(INTEL_MDIC_DATA,mdic);
        }
    }

    /* data has an error? set transferOK to false */
    if (INTEL_GETBIT(INTEL_MDIC_E,mdic))
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_WARN,
            "intel_endPhyReg: data is faulty.");
        transferOK = LSA_FALSE;
    }

    return transferOK;
}

/** \brief      Finish read of data from PHY register.
 *
 * \details     This function waits until to read of data from PHY register is finished.
 *              The address of the PHY register was setup with #intel_beginReadPhyReg.
 *
 * \param       handle      handle of EDDS interface (hardware instance).
 * \param[out]  ref_data    reference to a data storage for read data. If a NULL reference (NULL-pointer)
 *                          is hand over, no data is transfered to caller.
 *
 * \return      LSA_TRUE, if transfer was successfully finished.
 *              ref_data has PHY data that was read.
 *              LSA_FALSE, if an error (e.g. timeout) occurred; in this case ref_data has no data.
 */
static LSA_BOOL intel_endReadPhyReg(
        INTEL_LL_HANDLE_TYPE const * const handle,
        LSA_UINT16* ref_data)
{
    return intel_endPhyReg(handle,ref_data);
}


/** \brief      Finish write of data to PHY register.
 *
 * \details     This function waits until to write of data to PHY register is finished.
 *              The address of the PHY register was setup with #intel_beginWritePhyReg.
 *
 * \param       handle      handle of EDDS interface (hardware instance).
 *
 * \return      LSA_TRUE, if transfer was successfully finished.
 *              LSA_FALSE, if an error (e.g. timeout) occurred.
 */
static LSA_BOOL intel_endWritePhyReg(
        INTEL_LL_HANDLE_TYPE const * const handle)
{
    return intel_endPhyReg(handle,LSA_NULL);
}


/** \brief      Acquire SW_FW_SYNC semaphore on i210 hardware.
 *
 *  \details    This semaphore is necessary to be able to access the SW_FW_SYNC register.
 *
 *  \param      handle    handle of EDDS interface (hardware instance).
 *  \return     LSA_TRUE if SW_FW_SYNC semaphore was taken and SW_FW_SYNC register
 *              could be accessed exclusively; LSA_FALSE if not.
 *
 *  \note       Software ownership of SWSM.SWESMBI bit should not exceed 100 ms. If software takes
 *              ownership for a longer duration, firmware might implement a timeout mechanism and take
 *              ownership of the SWSM.SWESMBI bit.
 *
 *  \author     am2219
 *  \date       2015-10-14
 *
 */
static LSA_BOOL intel_i210_get_sw_fw_sync (INTEL_LL_HANDLE_TYPE const * const handle)
{
    /* \see Intel Ethernet Controller I210 data sheet:
     * Acquiring Ownership over a shared resource.
     */
    LSA_UINT32 countdown;
    LSA_UINT32 swsm;

    /* 1. Get ownership og SWSM.SMBI
     * \note only one thread is used to access the same hardware:
     * The check of SWSM.SMBI is zero could be skipped.
     * \code
     *  swsm = intel_reg32(INTEL_SWSM);
     * if(INTEL_GETBIT(INTEL_SWSM_SMBI,swsm)
     * {
     *     retval = LSA_FALSE;
     *     break;
     * }
     * \endcode
     */
    /* 2. Get ownership of the software/firmware semaphore SWSM.SWESMBI */
    /* to prevent SWSM.SMBI to be cleared, it must be written with 1.
     * Try to set SWSM.SWESMBI to 1 to acquire this semaphore.
     *
     * \note this semaphore should only be set for a short time (some microseconds)!
     */
    swsm = INTEL_BIT(INTEL_SWSM_SWESMBI,1) | INTEL_BIT(INTEL_SWSM_SMBI, 1);

    /* number of tries is set in countdown variable */
    for (countdown = 2; /* handled in loop (*) */; /* handled in loop (**) */)
    {
        LSA_UINT32 read_swsm;
        /* 2a) set SWSM.SWESMBI */
        intel_reg32(INTEL_SWSM) = swsm;

        /* 2b) read SWSM register and check, if we got the semaphore SWSM.SWESMBI == 1*/
        read_swsm = intel_reg32(INTEL_SWSM);
        if (INTEL_GETBIT(INTEL_SWSM_SWESMBI, read_swsm))
        {
            /* we got it */
            return LSA_TRUE;
        }

        /* (**) decrement countdown; (*) test if countdown is zero: */
        if (countdown--)
        {
            /* no, wait 10us and then we retry to get semaphore. */
            EDDS_WAIT_US (10);
        }
        else
        {
            /* yes, break loop*/
            break;
        }
        /* 2c) go back to step 2a)*/
    }

    return LSA_FALSE;
}


/** Release SW_FW_SYNC semaphore on i210 hardware.
 *
 * This semaphore is necessary to be able to access the SW_FW_SYNC register.
 *
 * @param       handle    handle of EDDS interface (hardware instance).
 * @note        Software ownership of SWSM.SWESMBI bit should not exceed 100 ms. If software takes
 *              ownership for a longer duration, firmware might implement a timeout mechanism and take
 *              ownership of the SWSM.SWESMBI bit.
 */
static void intel_i210_free_sw_fw_sync (INTEL_LL_HANDLE_TYPE const * const handle)
{
    /* \see Intel Ethernet Controller I210 data sheet:
     * Acquiring Ownership over a shared resource.
     */
    LSA_UINT32 swsm;

    /* 4. release ownership of the software/firmware semaphore SWSM.SWESMBI:
     * clear (potentially set) SWSM.SWSM and SWSM.SWESMBI semaphores
     */
    swsm = INTEL_BIT(INTEL_SWSM_SWESMBI,0) | INTEL_BIT(INTEL_SWSM_SMBI,0);
    intel_reg32(INTEL_SWSM) = swsm;
}

/**
*  \brief       Try to acquire a shared resource semaphore.
*
*  \details     Function tries to acquire a semaphore of a shared resource. A shared resource
*               resource could be PHY, EEPROM etc.
*               If acquire was successful, TRUE is returned. On FALSE, software should retry to
*               acquire the desired semaphore with another call of this function ("some time later").
*
*  \param       handle      handle of EDDS interface (hardware instance).
*  \param       semaphore   semaphore of shared resource. Actually,
*               INTEL_SW_FW_SYNC_SW_PHY_SM (PHY), INTEL_SW_FW_SYNC_SW_I2C_SM (I2C),
*               INTEL_SW_FW_SYNC_SW_MAC_CSR_SM (CSR), INTEL_SW_FW_SYNC_SW_SVR_SM (SVR/LVR),
*               INTEL_SW_FW_SYNC_SW_MB_SM (SWMBWR), INTEL_SW_FW_SYNC_SW_MNG_SM (management host interface),
*               are allowed.
*
*  \author      am2219
*  \date        2015-10-05
*/
static LSA_BOOL intel_try_acquire_semaphore(
        INTEL_LL_HANDLE_TYPE* const handle,
        LSA_UINT8 const semaphore)
// semaphore := INTEL_SW_FW_SYNC_SW_PHY_SM;
{
    LSA_BOOL acquired;

    EDDS_INTERNAL_STATISTIC_ENTER(handle->hSysDev,handle->perf_tas);

    acquired = LSA_FALSE;

    switch (handle->params.DeviceID)
    {
        case I210:
        {
            LSA_BOOL sw_fw_sync_acquired;
            /* \see Intel Ethernet Controller I210 data sheet:
             * Acquiring Ownership over a shared resource.
             */
            sw_fw_sync_acquired = intel_i210_get_sw_fw_sync(handle);
            if(sw_fw_sync_acquired)
            {
                /* got semaphore SWSM.SWESMBI !*/
                LSA_UINT32 sw_fw_sync;

                /* 3. Software reads the Software-Firmware Synchronization Register (SW_FW_SYNC) and checks both
                 * bits in the pair of bits that control the resource it wants to own.
                 */
                sw_fw_sync = intel_reg32(INTEL_SW_FW_SYNC);

                /* 3a) check if FW semaphore is set;
                 * SW part should not be checked, since only one software
                 * handles this hardware.
                 */
                if (0 == INTEL_GETBIT(semaphore + 16,sw_fw_sync))
                {
                    /* 3a) set SW semaphore
                     * \note this semaphore should be set only for a maximum time of 100ms!
                     * Otherwise firmware may free it independently.
                     */
                    INTEL_SETBIT(sw_fw_sync, semaphore + 0);
                    intel_reg32(INTEL_SW_FW_SYNC) = sw_fw_sync;
                    acquired = LSA_TRUE;
                }
                else
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                        "intel_try_acquire_semaphore: sync_semaphore is already set.");
                    /* 3b) try again later */
                }
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "intel_try_acquire_semaphore: could not get sync_semaphore.");
                /* we didn't get the semaphore */
            }

            intel_i210_free_sw_fw_sync(handle);
        }
        break;

        case IG82574L:
        {
            LSA_UINT32 countdown;
            LSA_UINT32 extcnf_ctrl;

            /* first read actual value of extcnf_ctrl */
            extcnf_ctrl = intel_reg32(INTEL_EXTCNF_CTRL);

            /* 1.) try do get ownership of the MDIO/NVM semaphore registers.
             *
             * number of tries is set in countdown variable
             */
            for(countdown=2; /* @see (*) */; /* @see (**) */)
            {
                /* try to set semaphore bit */
                INTEL_SETBIT(extcnf_ctrl, INTEL_EXTCNF_CTRL_MDIO_SW_OWNERSHIP);
                intel_reg32(INTEL_EXTCNF_CTRL) = extcnf_ctrl;

                /* read it back again */
                extcnf_ctrl = intel_reg32(INTEL_EXTCNF_CTRL);

                /* test if semaphore was set */
                if(INTEL_GETBIT(INTEL_EXTCNF_CTRL_MDIO_SW_OWNERSHIP,extcnf_ctrl))
                {
                    /* we got it */
                    acquired = LSA_TRUE;
                    break;
                }

                /* (**) decrement countdown; (*) test if countdown is zero: */
                if (countdown--)
                {
                    /* no, wait 10us and then we retry to get semaphore. */
                    EDDS_WAIT_US (10);
                }
                else
                {
                    /* yes, break loop */
                    break;
                }
            }
            /* if acquired is not set to true in for-loop, software should try again later */
        }
        break;

        default:
        {
            /* no semaphore mechanism */
            acquired = LSA_TRUE;
        }
        break;
    }

    EDDS_INTERNAL_STATISTIC_EXIT(handle->hSysDev,handle->perf_tas);

    return acquired;
}


/**
*  \brief       Try to release a shared resource semaphore.
*
*  \details     Function tries to release a semaphore of a shared resource. A shared resource
*               resource could be PHY, EEPROM etc.
*               If release was successful, TRUE is returned. On FALSE, software should retry to
*               release the desired semaphore with another call of this function ("some time later").
*
*  \param       handle      handle of EDDS interface (hardware instance).
*  \param       semaphore   semaphore of shared resource. Actually,
*               INTEL_SW_FW_SYNC_SW_PHY_SM (PHY), INTEL_SW_FW_SYNC_SW_I2C_SM (I2C),
*               INTEL_SW_FW_SYNC_SW_MAC_CSR_SM (CSR), INTEL_SW_FW_SYNC_SW_SVR_SM (SVR/LVR),
*               INTEL_SW_FW_SYNC_SW_MB_SM (SWMBWR), INTEL_SW_FW_SYNC_SW_MNG_SM (management host interface),
*               are allowed.
*
*  \author      am2219
*  \date        2015-10-12
*/
static LSA_BOOL intel_try_release_semaphore(
        INTEL_LL_HANDLE_TYPE* const handle,
        LSA_UINT8 const semaphore)
{
    LSA_BOOL released;

    EDDS_INTERNAL_STATISTIC_ENTER(handle->hSysDev,handle->perf_trs);
    released = LSA_FALSE;

    switch (handle->params.DeviceID)
    {
        case I210:
        {
            LSA_BOOL sw_fw_sync_acquired;
            /* \see Intel Ethernet Controller I210 data sheet:
             * Acquiring Ownership over a shared resource.
             */
            sw_fw_sync_acquired = intel_i210_get_sw_fw_sync(handle);
            if(sw_fw_sync_acquired)
            {
                /* got semaphore SWSM.SWESMBI !*/
                LSA_UINT32 sw_fw_sync;

                /* 3. Software reads the Software-Firmware Synchronization Register (SW_FW_SYNC)... */
                sw_fw_sync = intel_reg32(INTEL_SW_FW_SYNC);

                /* 3. and clears SW semaphore (don't touch FW semaphore) */
                INTEL_CLRBIT(sw_fw_sync, semaphore + 0);
                intel_reg32(INTEL_SW_FW_SYNC) = sw_fw_sync;

                released = LSA_TRUE;
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "intel_try_release_semaphore: could not get sync_semaphore.");
                /* we didn't get the semaphore */
            }

            intel_i210_free_sw_fw_sync(handle);
        }
        break;

        case IG82574L:
        {
            /* 1.) Release ownership of the MDIO/NVM semaphore registers */
            INTEL_CLRBIT(intel_reg32(INTEL_EXTCNF_CTRL),
                    INTEL_EXTCNF_CTRL_MDIO_SW_OWNERSHIP);
            released = LSA_TRUE;
        }
        break;

        default:
        {
            /* no semaphore mechanism */
            released = LSA_TRUE;
        }
        break;
    }
    EDDS_INTERNAL_STATISTIC_EXIT(handle->hSysDev,handle->perf_trs);

    return released;
}


/** Prepare setting link state.
 *
 * \param   handle      handle of EDDS interface (hardware instance).
 * \return  LSA_TRUE, if setup of set link was successfully,
 *          LSA_FALSE, if software should try it again later.
 *
 * \author  am2219
 * \date    2015-10-20
 *
 * \see     File #edd_usr.h
 */
static LSA_BOOL intel_set_link_setup(
        INTEL_LL_HANDLE_TYPE* const handle)
{
    if (EDD_LINK_UNCHANGED != handle->newLinkStat)
    {
        handle->savedLinkStat = handle->newLinkStat;
    }
    if (EDDS_PHY_POWER_UNCHANGED != handle->newPHYPower)
    {
        handle->savedPHYPower = handle->newPHYPower;
    }

    return intel_try_acquire_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM);
}

/** First step of setting link state.
 *
 * \param   handle      handle of EDDS interface (hardware instance).
 *
 * \author  am2219
 * \date    2015-10-20
 */
static LSA_VOID intel_set_link_step1(
        INTEL_LL_HANDLE_TYPE* const handle)
{
    LSA_BOOL autoneg;
    LSA_UINT32 ctrl;

    /* use shadow PHY register PCTRL */

    autoneg = INTEL_GETBITI(12, handle->phyRegs.pctrl);
    ctrl = intel_reg32(INTEL_CTRL);
    //ctrl_ext = intel_reg32(INTEL_CTRL_EXT);
    if (EDD_LINK_UNCHANGED != handle->newLinkStat)
    {
        /* save current phy configuration */
        handle->LinkSpeedModeConfigured = handle->newLinkStat;

        /* smart power down off etc.*/

        INTEL_CLRBIT(handle->phyRegs.pctrl, 6);
        /* speed selection LSB (1== Gigabit); clear it */
        INTEL_CLRBIT(handle->phyRegs.pctrl, 13);
        /* speed selection MSB clear it */
        INTEL_CLRBIT(handle->phyRegs.pctrl, 8);
        /* duplex mode; clear it */
        INTEL_CLRBIT(handle->phyRegs.pctrl, 9);
        /* restart autoneg */

        if (EDD_LINK_AUTONEG != handle->newLinkStat)
        {
            INTEL_CLRBIT(handle->phyRegs.pctrl, 12);
            /* autoneg off */
            /* update MAC ctrl register */
            INTEL_SETBIT(ctrl, INTEL_CTRL_FRCSPD);
            INTEL_SETBIT(ctrl, INTEL_CTRL_FRCDPLX);
            //INTEL_CLRBIT(ctrl,INTEL_CTRL_ASDE);
            INTEL_CLRBIT(ctrl, INTEL_CTRL_FD);
            INTEL_CLRBITS(ctrl, INTEL_CTRL_SPEED);
            //INTEL_SETBIT(ctrl_ext,INTEL_CTRL_EXT_SPD_BYPS);
        }

        switch (handle->newLinkStat)
        {
            case EDD_LINK_AUTONEG:
                //INTEL_SETBIT(ctrl,INTEL_CTRL_ASDE);
                INTEL_CLRBIT(ctrl, INTEL_CTRL_FRCSPD);
                INTEL_CLRBIT(ctrl, INTEL_CTRL_FRCDPLX);
                INTEL_SETBIT(handle->phyRegs.pctrl, 12);
                /* autoneg on */
                if (0 == autoneg)
                {
                    INTEL_SETBIT(handle->phyRegs.pctrl, 9);
                    /* restart autoneg */
                }
                //INTEL_CLRBIT(ctrl_ext,INTEL_CTRL_EXT_SPD_BYPS);
                //INTEL_SETBIT(intel_reg32(INTEL_PHPM),INTEL_PHPM_Disable_1000);

                break;
            case EDD_LINK_100MBIT_FULL:
                INTEL_SETBIT(handle->phyRegs.pctrl, 13);
                /* speed selection 6,13 = 01 = 100Mbps */
                INTEL_SETBIT(handle->phyRegs.pctrl, 8);
                /* full duplex */
                /* update MAC CTRL register */
                INTEL_SETBIT(ctrl, INTEL_CTRL_FD);
                INTEL_SETBITS(ctrl, INTEL_CTRL_SPEED, 0x1);
                break;
            case EDD_LINK_100MBIT_HALF:
            case EDD_LINK_10MBIT_HALF:
            case EDD_LINK_10MBIT_FULL:
            case EDD_LINK_1GBIT_HALF:
            case EDD_LINK_1GBIT_FULL:
            case EDD_LINK_10GBIT_HALF:
            case EDD_LINK_10GBIT_FULL:
            default:
                EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_WARN,
                    "intel_set_link_step1: unexpected newLinkStat(%d).", handle->newLinkStat);
                /* this should not happen -  it is filtered out by EDDS_LL_SET_LINK_STATE */;
                break;
        }
    }

    switch (handle->newPHYPower)
    {
        case EDDS_PHY_POWER_ON:
            /* enable phy */
                INTEL_CLRBIT(handle->phyRegs.pctrl, 11);
            break;
        case EDDS_PHY_POWER_OFF:
            /* disable phy */
                INTEL_SETBIT(handle->phyRegs.pctrl, 11);
            break;
        case EDDS_PHY_POWER_UNCHANGED:
        default:
            break;
    }

    intel_reg32(INTEL_CTRL) = ctrl; //lint !e845 zero needs  to be given (macro handling) (JB 04/2016)
    intel_beginWritePhyReg(handle,INTEL_PHY_PCTRL, handle->phyRegs.pctrl);
}


/** Second step of setting link state.
 *
 * \param   handle      handle of EDDS interface (hardware instance).
 * \return  LSA_TRUE, if this step was successful.
 *          LSA_FALSE, if software should try this step later.
 *
 * \author  am2219
 * \date    2015-10-20
 */
static LSA_BOOL intel_set_link_step2(
        INTEL_LL_HANDLE_TYPE const * const handle)
{
    /* was data of shadow PHY register PCTRL was written back? */
    return intel_endWritePhyReg(handle);
}

/** Finish setting link state.
 *
 * \param   handle      handle of EDDS interface (hardware instance).
 * \return  LSA_TRUE, if setup of set link was successfully,
 *          LSA_FALSE, if software should try it again later.
 *
 * \author  am2219
 * \date    2015-10-20
 */
static LSA_BOOL intel_set_link_finish(
        INTEL_LL_HANDLE_TYPE* const handle)
{
    /* Release ownership of phy, regardless of Status */
    return intel_try_release_semaphore(handle,INTEL_SW_FW_SYNC_SW_PHY_SM);
}


/** \brief      Internal set link state machine.
 *
 *  \details    This function is called to set a new link. It must be called several times,
 *              until state machine is finished (return value != EDD_STS_OK_PENDING).
 *
 *  \return     EDD_STS_OK_PENDING, if a step of phy setup was successfully executed, but more states
 *              are pending.
 *              EDD_STS_OK, if state machine is finished.
 *              EDD_STS_ERR_HW, if PHY setup could not be finished (error on PHY access).
 *
 *  \author     am2219
 *  \date       2015-10-20
 */
static LSA_RESULT intel_set_link_sm(INTEL_LL_HANDLE_TYPE* const handle)
{
    LSA_RESULT status;

    status = EDD_STS_OK_PENDING;        /* Default: OK, state machine not finished */

    switch(handle->setLinkSM)
    {
        /* ---------------------------------------------------------------------------------------- */
        /* Idle State (no link change pending. If called, a link change will be setup               */
        /* ---------------------------------------------------------------------------------------- */
        case INTEL_SET_LINK_SM_IDLE:
        {
            handle->setLinkSM = INTEL_SET_LINK_SM_SETUP;
            /* only leave idle state, fall through next */
        }
        //lint -fallthrough

        /* ---------------------------------------------------------------------------------------- */
        /* Setup link change                                                                        */
        /* ---------------------------------------------------------------------------------------- */
        case INTEL_SET_LINK_SM_SETUP:
        {
            if(intel_set_link_setup(handle))
            {
                if(I210SFP != handle->params.DeviceID)
                {
                    /* next step for copper phy */
                    handle->setLinkSM = INTEL_SET_LINK_SM_STEP1;
                }else
                {
                    /* to speed up the link setup phase for SFP,
                     * phy power is set here directly (INTEL_SET_LINK_SM_STEP1/2 are skipped).
                     */
                    LSA_UINT32 ctrl = intel_reg32(INTEL_CTRL);
                    LSA_UINT32 pcs_cfg = intel_reg32(INTEL_PCS_CFG);

                    switch (handle->newPHYPower)
                    {
                        case EDDS_PHY_POWER_ON:
                            INTEL_SETBIT(ctrl,INTEL_CTRL_SLU);
                            INTEL_SETBIT(pcs_cfg, INTEL_PCS_CFG_ENABLE);
                            break;
                        case EDDS_PHY_POWER_OFF:
                            INTEL_CLRBIT(ctrl,INTEL_CTRL_SLU);           /* Set LinkDown */
                            INTEL_CLRBIT(pcs_cfg, INTEL_PCS_CFG_ENABLE); /* disable transceiver - use this to force link down at the far end */
                            break;
                        case EDDS_PHY_POWER_UNCHANGED:
                        default:
                            break;
                    }
                    intel_reg32(INTEL_CTRL) = ctrl;
                    intel_reg32(INTEL_PCS_CFG) = pcs_cfg;

                    /* next step for SFP */
                    handle->setLinkSM = INTEL_SET_LINK_SM_FINISH;
                }
            }
        }
        break;

        /* ---------------------------------------------------------------------------------------- */
        /* First step of setting link                                                               */
        /* ---------------------------------------------------------------------------------------- */
        case INTEL_SET_LINK_SM_STEP1:
        {
            intel_set_link_step1(handle);
            handle->setLinkSM = INTEL_SET_LINK_SM_STEP2;
        }
        break;

        /* ---------------------------------------------------------------------------------------- */
        /* Second step of setting link                                                              */
        /* ---------------------------------------------------------------------------------------- */
        case INTEL_SET_LINK_SM_STEP2:
        {
            if(intel_set_link_step2(handle))
            {
                handle->setLinkSM = INTEL_SET_LINK_SM_FINISH;
            }
        }
        break;

        /* ---------------------------------------------------------------------------------------- */
        /* Finish setting the link                                                                  */
        /* ---------------------------------------------------------------------------------------- */
        case INTEL_SET_LINK_SM_FINISH:
        {
            if(intel_set_link_finish(handle))
            {
                handle->isResetPending = LSA_FALSE;
                handle->setLinkSM = INTEL_SET_LINK_SM_IDLE;
                /* state machine is finished */
                status = EDD_STS_OK;
            }
        }
        break;

        default:
        {
            handle->isResetPending = LSA_FALSE;
            handle->setLinkSM = INTEL_SET_LINK_SM_IDLE;
            /* state machine is finished */
            status = EDD_STS_OK;
        }
        break;
    }

    return status;
}


/**  \brief     Save volatile hardware registers.
 *
 *  \details    This function saves ethernet controller registers that are modified directly
 *              by hardware during normal operation.
 *  \author     am2219
 *  \date       2015-10-14
 */
static LSA_VOID intel_save_volatile_registers(
        INTEL_LL_HANDLE_TYPE* const handle)
{
    handle->lastIMS = intel_reg32(INTEL_IMS);    
    if(IG82574L == handle->params.DeviceID)
    {
        /* save ivar register for mdix restore (Hartwell only) */
        handle->savedIVAR = intel_reg32(INTEL_IVAR);
    }
}

/** \brief Disable part of MTA.
 *
 * \details     This is a helper function, to satisfy the DRY principle.
 *
 * \param index     start index to clear
 * \param number    number of entries to clear
 *
 */
static LSA_VOID intel_clear_mta(INTEL_LL_HANDLE_TYPE const * const handle, LSA_UINT8 index, LSA_INT8 number)
{
    for (; 0 < number; --number,++index) /* disable part of Multicast Table Array [index..index+number[ */
    {
        intel_reg32(INTEL_MTA_0_127(index)) = 0;
    }
}

/** \brief      setup all registers after a reset.
 *
 *  \details    This function is called after a reset to setup all
 *              used registers.
 *
 *  \return     EDD_STS_OK_PENDING, if a step of register setup was successfully executed, but more states
 *              are pending.
 *              EDD_STS_OK, if state machine is finished.
 *              EDD_STS_ERR_HW, if PHY setup could not be finished (error on PHY access).
 *
 *  \author     am2219
 *  \date       2015-10-20
 */
static LSA_RESULT intel_setup_registers_sm(
        INTEL_LL_HANDLE_TYPE* const handle)
{
    LSA_RESULT Status;

    Status = EDD_STS_OK_PENDING;

    switch(handle->regSetupSM)
    {
        case INTEL_REG_SETUP_SM_IDLE:
        {
            LSA_UINT c;

            handle->TXDescriptorsUsed = 0;
            handle->RXDescriptorIndex = 0;
            handle->TXDescriptorIndex = 0;
            handle->TXDescriptorIndexEnd = 0;
            /* make all RDESC available (one space descriptor left) */
            handle->RXDescriptorIndexEnd = handle->RxFrameBufferCount - 1;

            intel_reg32(INTEL_LEDCTL) = handle->savedLEDCTL;

            /* set mac address */
            {

                if ((0 != handle->InterfaceMAC.MacAdr[0]) || (0 != handle->InterfaceMAC.MacAdr[1]) || (0 != handle->InterfaceMAC.MacAdr[2]) || (0 != handle->InterfaceMAC.MacAdr[3]) || (0 != handle->InterfaceMAC.MacAdr[4]) || (0 != handle->InterfaceMAC.MacAdr[5]))
                {
                    intel_reg32(INTEL_RAL_0_15(0)) =
                            (((LSA_UINT32) handle->InterfaceMAC.MacAdr[3]) << 24) | (((LSA_UINT32) handle->InterfaceMAC.MacAdr[2]) << 16) | (((LSA_UINT32) handle->InterfaceMAC.MacAdr[1]) << 8) | (((LSA_UINT32) handle->InterfaceMAC.MacAdr[0])/*<<0*/); //lint !e845 JB 12/11/2014 null within INTEL_RAL_0_15 on purpose
                    intel_reg32(INTEL_RAH_0_15(0)) =
                            INTEL_BIT(INTEL_RAH_AV,1) | /* enable MAC address */
                            (((LSA_UINT32) handle->InterfaceMAC.MacAdr[5]) << 8) | (((LSA_UINT32) handle->InterfaceMAC.MacAdr[4])/*<<0*/); //lint !e845 JB 12/11/2014 null within INTEL_RAH_0_15 on purpose
                }
                else /* write eeprom MAC address back to setup structure */
                {
                    LSA_UINT32 tmp;
                    tmp = intel_reg32(INTEL_RAH_0_15(0)); //lint !e845 JB 12/11/2014 null within INTEL_RAH_0_15 on purpose
                    if (0 != INTEL_GETBIT(INTEL_RAH_AV,tmp) ) /* interface mac is present in first receive address (RAH/L[0])... */ //1 would be int, not unsiged int
                    {
                        handle->InterfaceMAC.MacAdr[4] = (tmp /*>> 0*/) & 0xFF;
                        handle->InterfaceMAC.MacAdr[5] = (tmp >> 8) & 0xFF;
                        tmp = intel_reg32(INTEL_RAL_0_15(0)); //lint !e845 JB 12/11/2014 null within INTEL_RAL_0_15 on purpose
                        handle->InterfaceMAC.MacAdr[0] = (tmp /*>> 0*/) & 0xFF;
                        handle->InterfaceMAC.MacAdr[1] = (tmp >> 8) & 0xFF;
                        handle->InterfaceMAC.MacAdr[2] = (tmp >> 16) & 0xFF;
                        handle->InterfaceMAC.MacAdr[3] = (tmp >> 24) & 0xFF;
                    }
                    else /* no interface mac address stored in eeprom, no mac address from DSB...*/
                    {
                        EDDS_MEMSET_LOCAL(handle->InterfaceMAC.MacAdr, 0,
                                sizeof(handle->InterfaceMAC.MacAdr));
                    }
                }
            }

            /* write port mac addresses to mac table */
            /* first is interface mac, skip it */
            for(c = 1; c < EDDS_INTEL_MAC_TABLE_START_OFFSET_MC; ++c)
            {
                intel_reg32(INTEL_RAL_0_15(c)) =
                        (((LSA_UINT32) handle->PortMAC[c-1].MacAdr[3]) << 24) |
                        (((LSA_UINT32) handle->PortMAC[c-1].MacAdr[2]) << 16) |
                        (((LSA_UINT32) handle->PortMAC[c-1].MacAdr[1]) << 8)  |
                        (((LSA_UINT32) handle->PortMAC[c-1].MacAdr[0])/*<<0*/); //lint !e845 JB 12/11/2014 null within INTEL_RAL_0_15 on purpose
                intel_reg32(INTEL_RAH_0_15(c)) =
                        INTEL_BIT(INTEL_RAH_AV,1) | /* enable MAC address */
                        (((LSA_UINT32) handle->PortMAC[c-1].MacAdr[5]) << 8) |
                        (((LSA_UINT32) handle->PortMAC[c-1].MacAdr[4])/*<<0*/); //lint !e845 JB 12/11/2014 null within INTEL_RAH_0_15 on purpose
            }

            for (c = EDDS_INTEL_MAC_TABLE_START_OFFSET_MC; c < 16; ++c) /* first receive address is interface MAC, skip it */
            {
                intel_reg32(INTEL_RAL_0_15(c)) = handle->MCRegL[c];
                intel_reg32(INTEL_RAH_0_15(c)) = handle->MCRegH[c];
            }

            /* next state */
            handle->regSetupSM = INTEL_REG_SETUP_SM_STEP1;
        }
        break;

        /* These states are required to timeslice writing to the pcie hardware... */

        case INTEL_REG_SETUP_SM_STEP1:
        {
            intel_clear_mta(handle,0,32);
            /* next state */
            handle->regSetupSM = INTEL_REG_SETUP_SM_STEP2;
        }
        break;

        case INTEL_REG_SETUP_SM_STEP2:
        {
            intel_clear_mta(handle,32,32);
            /* next state */
            handle->regSetupSM = INTEL_REG_SETUP_SM_STEP3;
        }
        break;

        case INTEL_REG_SETUP_SM_STEP3:
        {
            intel_clear_mta(handle,64,32);
            /* next state */
            handle->regSetupSM = INTEL_REG_SETUP_SM_STEP4;
        }
        break;

        case INTEL_REG_SETUP_SM_STEP4:
        {
            intel_clear_mta(handle,96,32);
            /* next state */
            handle->regSetupSM = INTEL_REG_SETUP_SM_STEP5;
        }
        break;

        case INTEL_REG_SETUP_SM_STEP5:
        {
            LSA_UINT64 physicalAddr;

            /* setup Transmit Descriptor Ring Structure */
            EDDS_PTR_TO_ADDR64_DEV(handle->hSysDev, handle->pTxDescriptors,
                    &physicalAddr);
            intel_reg64(INTEL_TDBAL) = physicalAddr;
            intel_reg32(INTEL_TDLEN) =
                    handle->TxFrameBufferCount * sizeof(*(handle->pTxDescriptors));

            /* setup Receive Descriptor Ring Structure */
            EDDS_PTR_TO_ADDR64_DEV(handle->hSysDev, handle->pRxDescriptors,
                    &physicalAddr);
            intel_reg64(INTEL_RDBAL) = physicalAddr;
            intel_reg32(INTEL_RDLEN) =
                    handle->RxFrameBufferCount * sizeof(*(handle->pRxDescriptors));

            /* first setup empty; all values should be zero */
            //EDDS_ASSERT(0 == handle->TXDescriptorIndex);
            //EDDS_ASSERT(0 == handle->TXDescriptorIndexEnd);
            //EDDS_ASSERT(0 == handle->RXDescriptorIndex);
            //EDDS_ASSERT(0 == handle->RXDescriptorIndexEnd);
            intel_reg32(INTEL_TDH) = handle->TXDescriptorIndex;
            intel_reg32(INTEL_TDT) = handle->TXDescriptorIndexEnd;
            intel_reg32(INTEL_RDH) = handle->RXDescriptorIndex;
            intel_reg32(INTEL_RDT) = handle->RXDescriptorIndex; /* first setup: make tail same as head */

            /* buffer size is 2048 after reset (@see RCTL.BSIZE + RCTL.BSEX) */
            //val = INTEL_BITS(INTEL_RXDCTL_WTHRESH,0x0) | INTEL_BIT(INTEL_RXDCTL_ENABLE,1);
            //intel_reg32(INTEL_RXDCTL) = val;
            //IG82574L doesn't have those bits
            if(IG82574L != handle->params.DeviceID)
            {
                INTEL_SETBIT(intel_reg32(INTEL_RXDCTL), INTEL_RXDCTL_ENABLE);
                INTEL_SETBIT(intel_reg32(INTEL_TXDCTL), INTEL_TXDCTL_ENABLE);
            }
            
            handle->regSetupSM = INTEL_REG_SETUP_SM_FINISH;
        }
        break;

        case INTEL_REG_SETUP_SM_FINISH:
        {
            LSA_UINT32 rxdctl;
            LSA_UINT32 txdctl;
            LSA_UINT32 tctl;
            
            /* not valid for IG82574L */
            if(IG82574L != handle->params.DeviceID)
            {
                rxdctl = intel_reg32(INTEL_RXDCTL);
    
                if(! INTEL_GETBIT(INTEL_RXDCTL_ENABLE,rxdctl))
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                        "intel_setup_registers_sm: Bit INTEL_RXDCTL_ENABLE not set in INTEL_RXDCTL.");
                    break;
                }
    
                txdctl = intel_reg32(INTEL_TXDCTL);
    
                if(! INTEL_GETBIT(INTEL_TXDCTL_ENABLE,txdctl))
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                        "intel_setup_registers_sm: Bit INTEL_TXDCTL_ENABLE not set in INTEL_TXDCTL.");
                    break;
                }
            }
            
            /* we use Software workaround for "send on LinkDown" */
            intel_reg32(INTEL_RCTL) = INTEL_BIT(INTEL_RCTL_EN,1) /* enable receive unit */
            | INTEL_BIT(INTEL_RCTL_UPE,0) /* unicast promiscuous disable */
            | INTEL_BIT(INTEL_RCTL_MPE,0) /* multicast promiscuous disable */
            | INTEL_BIT(INTEL_RCTL_VFE,0) /* do not use VFTA table! */
            | INTEL_BIT(INTEL_RCTL_BAM,1) /* broadcast accept mode */
            | INTEL_BIT(INTEL_RCTL_SECRC,1) /* strip ethernet crc from received packets */
            ;

            tctl = intel_reg32(INTEL_TCTL);
            INTEL_SETBIT(tctl, INTEL_TCTL_EN);
            /* enable transmit unit */
            INTEL_SETBIT(tctl, INTEL_TCTL_PSP);
            /* pad short packets */
            INTEL_SETBITS(tctl, INTEL_TCTL_CT, 0xF);
            /* back-off algorythm: collision threshold 0fh */
            INTEL_SETBITS(tctl, INTEL_TCTL_COLD, 0x40);
            /* collision distance: 64 byte time */
            /**
             *     0x200 (512 byte time) for half-duplex
             *     0x40  (64 byte time) for full -duplex
             */
            intel_reg32(INTEL_TCTL) = tctl;

            /* second setup: now write to receive tail to make RDESC available for receive */
            intel_reg32(INTEL_RDT) = handle->RXDescriptorIndexEnd;

            /* acknowledge all pending interrupts :
             * a read acknowledges all pending interrupts
             * writing a 1 to a bit clears the pending interrupt
             */
            intel_reg32(INTEL_ICR) = 0xFFFFFFFF;

            /* set "driver loaded" indication bit */
            INTEL_SETBIT(intel_reg32(INTEL_CTRL_EXT), INTEL_CTRL_EXT_DRV_LOAD);
            Status = EDD_STS_OK;    /* were done */
            handle->regSetupSM = INTEL_REG_SETUP_SM_IDLE;
        }
    }

    return Status;
}


static LSA_VOID intel_reset(
        INTEL_LL_HANDLE_TYPE const * const handle,
        LSA_BOOL deviceReset)
{
    LSA_UINT32 ctrl;

    intel_reg32(INTEL_IMC) = 0xFFFFFFFF; /* disable all interrupts */
    intel_reg32(INTEL_ICR) = 0xFFFFFFFF; /* clear all interrupts */

    ctrl = intel_reg32(INTEL_CTRL);
    INTEL_SETBIT(ctrl, INTEL_CTRL_RST);
    INTEL_SETBIT(ctrl, INTEL_CTRL_LRST);
    if (!deviceReset)
    {
        INTEL_CLRBIT(ctrl, INTEL_CTRL_DEV_RST);
    }
    else
    {
        INTEL_SETBIT(ctrl, INTEL_CTRL_DEV_RST);
    }

    intel_reg32(INTEL_CTRL) = ctrl; //lint !e845 zero needs to be given (macro handling) (JB 04/2016)
}

/** \brief      Try to finish the pending reset.
 *
 *  \details    Finish software reset, that was initiated with #intel_reset
 *  \note       Caller must guarantee that this function is called at least 3ms (or more) after
 *              #intel_reset was successfully executed.
 *
 *  \param      handle
 *  \return     LSA_TRUE if reset is finished,
 *              LSA_FALSE if software should try to finish the reset later.
 *  \author     am2219
 *  \date       2015-10-14
 */
static LSA_BOOL intel_reset_finish(INTEL_LL_HANDLE_TYPE const * const handle)
{
    LSA_UINT32 eec;
    LSA_UINT32 ctrl;

    eec = intel_reg32(INTEL_EECD);

    if( ! INTEL_GETBIT(INTEL_EDCD_AUTO_RD,eec))
    {
        /* reset not finished */
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
            "intel_reset_finish: Bit INTEL_EDCD_AUTO_RD not set in INTEL_EECD.");
        return LSA_FALSE;
    }

    if( (I210 == handle->params.DeviceID)||(I210SFP == handle->params.DeviceID))
    {
        LSA_UINT32 status;

        status = intel_reg32(INTEL_STATUS);

        if( ! INTEL_GETBIT(INTEL_STATUS_PF_RST_DONE,status))
        {
            /* reset not finished */
            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                "intel_reset_finish: Bit INTEL_STATUS_PF_RST_DONE not set in INTEL_STATUS.");
            return LSA_FALSE;
        }
    }

    ctrl = intel_reg32(INTEL_CTRL);
    if(INTEL_GETBIT(INTEL_CTRL_RST,ctrl))
    {
        /* reset not finished */
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
            "intel_reset_finish: Bit INTEL_CTRL_RST not set in INTEL_CTRL.");
        return LSA_FALSE;
    }

    intel_reg32(INTEL_IMC) = 0xFFFFFFFF; /* disable all interrupts again */
    if(I210SFP != handle->params.DeviceID)
    {
        /* use internal PHY - Invert Loss-of-Signal (LOS/LINK) Signal should be set to zero */
        INTEL_CLRBIT(ctrl, INTEL_CTRL_ILOS);
        INTEL_SETBIT(ctrl, INTEL_CTRL_SLU);
    }else
    {
        LSA_UINT32 pcs_cfg = intel_reg32(INTEL_PCS_CFG);

        /* with SerDes/SFP , use ILOS settings from EEPROM Initialization Control 3 (word 0x24:bit13) */
        /* disable Link recognition after reset ("PHY"/SERDES has power off)*/
        INTEL_CLRBIT(ctrl, INTEL_CTRL_SLU);

        /* disable transceiver - use this to force link down at the far end */
        INTEL_CLRBIT(pcs_cfg, INTEL_PCS_CFG_ENABLE);
        intel_reg32(INTEL_PCS_CFG) = pcs_cfg;
    }
    
    INTEL_CLRBIT(ctrl, INTEL_CTRL_RFCE);
    INTEL_CLRBIT(ctrl, INTEL_CTRL_TFCE);
    INTEL_CLRBIT(ctrl, INTEL_CTRL_FRCSPD);
    INTEL_CLRBIT(ctrl, INTEL_CTRL_FRCDPLX);
    intel_reg32(INTEL_CTRL) = ctrl;  //lint !e845 zero needs to be given (macro handling) (JB 04/2016)
    intel_reg32(INTEL_IMS) = handle->lastIMS; /* restore interrupt mask */
    if(IG82574L == handle->params.DeviceID)
    {
        /* restore ivar register for mdix restore */
        intel_reg32(INTEL_IVAR) = handle->savedIVAR;
    }
    
    return LSA_TRUE;
}

/** \brief      First step of phy reset.
 *
 *  \details    This function is called to begin the reset of the phy.
 *  \return     LSA_TRUE if this step of phy reset could be successfully be executed,
 *              LSA_FALSE if software should try it again later.
 *  \note       After successful execution, software must wait at least 300us, before executing the next step.
 *  \author     am2219
 *  \date       2015-10-14
 */
static LSA_BOOL intel_reset_phy_step1(INTEL_LL_HANDLE_TYPE* const handle)
{
    LSA_UINT32 ctrl;

    /* Acquire ownership of phy */
    if (!intel_try_acquire_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
            "intel_reset_phy_step1: could not aquire ownership of phy.");
        return LSA_FALSE;
    }

    /* Reset PHY.
     * @see CTRL register description, PHY_RST bit
     * 8254x_GBe_SDM.pdf (317453006EN.PDF), Table 13-3.
     *
     * The technique is to set the bit, wait approximately 3us, then
     * clear the bit.
     *
     * @note For the 82546GB, when resetting the PHY through the
     * MAC, the PHY should be held in reset for a minimum of 10 ms
     * before releasing the reset signal.
     * Actually we will not support the 82546GB; to support it,
     * we should finish the phy reset in setup, after a minimum time period
     * of 10 ms between set/unset of PHY_RST bit.
     */
    /**@FIXME SPRINGVILLE 4.3.1.4 PHY Reset */

    if(I210SFP != handle->params.DeviceID)
    {
        ctrl = intel_reg32(INTEL_CTRL);
        INTEL_SETBIT(ctrl, INTEL_CTRL_PHY_RST);
        INTEL_CLRBIT(intel_reg32(INTEL_CTRL_EXT), INTEL_CTRL_EXT_SPD_BYPS);
    
        intel_reg32(INTEL_CTRL) = ctrl;  //lint !e845 zero needs to be given (macro handling) (JB 04/2016)
    }

    return LSA_TRUE;
}


/** \brief      Second step of phy reset.
 *
 *  \details    This function is called as second step to reset the phy.
 *  \note       Caller must guarantee that this function is called at least 300us (or more) after
 *              step1 was successfully executed.
 *  \author     am2219
 *  \date       2015-10-14
 */
static LSA_VOID intel_reset_phy_step2(INTEL_LL_HANDLE_TYPE const * const handle)
{
    LSA_UINT32 ctrl;

    if( I210SFP != handle->params.DeviceID)
    {
        /* reset internal PHY */
        ctrl = intel_reg32(INTEL_CTRL);
        INTEL_CLRBIT(ctrl, INTEL_CTRL_PHY_RST);
        intel_reg32(INTEL_CTRL) = ctrl;  //lint !e845 zero needs to be given (macro handling) (JB 04/2016)
    }
}



/** \brief      Third step of phy reset.
 *
 *  \details    This function is called as third step to reset the phy.
 *  \return     LSA_TRUE if this step of phy reset could be successfully be executed,
 *              LSA_FALSE if software should try it again later.
 *  \author     am2219
 *  \date       2015-10-14
 */
static LSA_BOOL intel_reset_phy_step3(INTEL_LL_HANDLE_TYPE* const handle)
{
    /* Release ownership of phy, regardless of Status */
    return intel_try_release_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM);
}

/** \brief      Begin of PHY setup.
 *
 *  \details    This function is called after a successful phy reset.
 *
 *  \return     LSA_TRUE, if a step of phy setup was successfully executed.
 *              LSA_FALSE, if software should try it again later.
 *
 *  \author     am2219
 *  \date       2015-10-19
 */
static LSA_BOOL intel_setup_phy_begin(INTEL_LL_HANDLE_TYPE* const handle)
{
    if(I210 == handle->params.DeviceID) /* phy config done only with internal phy, not SFP */
    {
        if(! INTEL_GETBIT(INTEL_EEMNGCTL_CFG_DONE,intel_reg32(INTEL_EEMNGCTL)))
        {
            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                "intel_reset_finish: Bit INTEL_EEMNGCTL_CFG_DONE not set in INTEL_EEMNGCTL.");
            return LSA_FALSE;
        }
    }

    /* reset phy setup state machine */
    handle->phySetupSM = INTEL_PHY_SETUP_SM_IDLE;

    return LSA_TRUE;
}


/** \brief      Internal PHY setup state machine.
 *
 *  \details    This function is called after a successful phy reset. It must be called several times,
 *              until state machine is finished (return value != EDD_STS_OK_PENDING).
 *
 *  \return     EDD_STS_OK_PENDING, if a step of phy setup was successfully executed, but more states
 *              are pending.
 *              EDD_STS_OK, if state machine is finished.
 *              EDD_STS_ERR_HW, if PHY setup could not be finished (error on PHY access).
 *
 *  \author     am2219
 *  \date       2015-10-19
 */
static LSA_RESULT intel_setup_phy_sm(INTEL_LL_HANDLE_TYPE* const handle)
{
    LSA_RESULT status;

    status = EDD_STS_OK_PENDING;        /* Default: OK, state machine not finished */

    switch(handle->phySetupSM)
    {
        case INTEL_PHY_SETUP_SM_IDLE:
        {
            /* begin setup of phy */
            handle->AutoNegCapAdvertised = EDD_AUTONEG_CAP_NONE;
            
            if(I210SFP != handle->params.DeviceID)
            {
                if(intel_try_acquire_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
                {
                    /* OK, next state */
                    handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP1;
                }
                else
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                        "intel_setup_phy_sm: could not aquire ownership of phy.");
                }
            }else
            {
                /* with SerDes/SFP, we are done. */
                handle->AutoNegCapAdvertised = EDD_AUTONEG_CAP_OTHER;
                status = EDD_STS_OK;
            }
        }
        break;

        /* ----------------------------------------------------- */
        /* ----- read all PHY registers for modifying them ----- */
        /* ----------------------------------------------------- */

        /* Step 1+2: Read GCON PHY Register */
        case INTEL_PHY_SETUP_SM_STEP1:
        {
            intel_beginReadPhyReg(handle,INTEL_PHY_GCON);
            /* OK, next state */
            handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP2;
        }
        break;

        case INTEL_PHY_SETUP_SM_STEP2:
        {
            if(intel_endReadPhyReg(handle,&handle->phyRegs.gcon))
            {
                // ---------------- GBit support is disabled ----------------
                if (handle->params.Disable1000MBitSupport)
                {
                    // ---------------- GBit shall not be advertised on AutoNeg ----------------
                    INTEL_CLRBIT(handle->phyRegs.gcon, INTEL_PHY_GCON_1000BASETHD);
                    INTEL_CLRBIT(handle->phyRegs.gcon, INTEL_PHY_GCON_1000BASETFD);
                }

                /* OK, next state */
                handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP3;
            }
        }
        break;

        /* Step 3+4: Read PSCON PHY Register */
        case INTEL_PHY_SETUP_SM_STEP3:
        {
            intel_beginReadPhyReg(handle,INTEL_PHY_PSCON);
            /* OK, next state */
            handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP4;
        }
        break;

        case INTEL_PHY_SETUP_SM_STEP4:
        {
            if(intel_endReadPhyReg(handle,&handle->phyRegs.pscon))
            {
                if (!handle->params.Disable1000MBitSupport)
                {
                    // ---------------- GBit support is not disabled ----------------
                    // ---------------- enable down shift ----------------
                    // SmartSpeed is an enhancement to auto-negotiation that enables the PHY to react intelligently to
                    // network conditions that prohibit establishment of a 1000BASE-T link, such as cable problems. Such
                    // problems might allow auto-negotiation to complete, but then inhibit completion of the training phase.
                    // Normally, if a 1000BASE-T link fails, the PHY returns to the auto-negotiation state with the same speed
                    // settings indefinitely. With SmartSpeed enabled by setting the Downshift Enable field, after a
                    // configurable number of failed attempts, as configured in the Downshift counter field the PHY
                    // automatically downgrades the highest ability it advertises to the next lower speed: from 1000 to
                    // 100 to 10 Mb/s.
                    // enable down shift and set counter to 1 --> 2 attempts
                    INTEL_SETBITI(handle->phyRegs.pscon, INTEL_PHY_PSCON_DOWNSHIFT_ENABLE);
                    INTEL_SETBITSI(handle->phyRegs.pscon, INTEL_PHY_PSCON_DOWNSHIFT_COUNTER_s,
                            INTEL_PHY_PSCON_DOWNSHIFT_COUNTER_e, 1);
                }
                /* OK, next state */
                handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP5;
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "intel_setup_phy_sm: Reading Phy Register INTEL_PHY_PSCON not finished yet");
            }
        }
        break;

        /* Step 5+6: Read PCTRL PHY Register */
        case INTEL_PHY_SETUP_SM_STEP5:
        {
            intel_beginReadPhyReg(handle,INTEL_PHY_PCTRL);
            /* OK, next state */
            handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP6;
        }
        break;

        case INTEL_PHY_SETUP_SM_STEP6:
        {
            if(intel_endReadPhyReg(handle,&handle->phyRegs.pctrl))
            {
                // ---------------- set phy power off ----------------
                INTEL_SETBITI(handle->phyRegs.pctrl, 11);

                /* OK, next state */
                handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP7;
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "intel_setup_phy_sm: Reading Phy Register INTEL_PHY_PCTRL not finished yet");
            }
        }
        break;

        /* Step 7+8: Read ANA PHY Register */
        case INTEL_PHY_SETUP_SM_STEP7:
        {
            intel_beginReadPhyReg(handle,INTEL_PHY_ANA);
            /* OK, next state */
            handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP8;
        }
        break;

        case INTEL_PHY_SETUP_SM_STEP8:
        {
            if(intel_endReadPhyReg(handle,&handle->phyRegs.ana))
            {
                // ---------------- get advertised autoneg capability ----------------
                if (1 != INTEL_GETBITS(INTEL_PHY_ANA_Selector,handle->phyRegs.ana))
                {
                    handle->AutoNegCapAdvertised = EDD_AUTONEG_CAP_OTHER;
                }
                else
                {
                    handle->AutoNegCapAdvertised |=
                            INTEL_GETBIT(INTEL_PHY_ANA_10BASETXHD,handle->phyRegs.ana) ? EDD_AUTONEG_CAP_10BASET :
                                    0;
                    handle->AutoNegCapAdvertised |=
                            INTEL_GETBIT(INTEL_PHY_ANA_10BASETXFD,handle->phyRegs.ana) ? EDD_AUTONEG_CAP_10BASETFD :
                                    0;
                    handle->AutoNegCapAdvertised |=
                            INTEL_GETBIT(INTEL_PHY_ANA_100BASETXHD,handle->phyRegs.ana) ? EDD_AUTONEG_CAP_100BASETX :
                                    0;
                    handle->AutoNegCapAdvertised |=
                            INTEL_GETBIT(INTEL_PHY_ANA_100BASETXFD,handle->phyRegs.ana) ? EDD_AUTONEG_CAP_100BASETXFD :
                                    0;

                    if (!handle->params.Disable1000MBitSupport)
                    {
                        handle->AutoNegCapAdvertised |=
                                INTEL_GETBIT(INTEL_PHY_GCON_1000BASETHD,handle->phyRegs.gcon) ? EDD_AUTONEG_CAP_1000BASET :
                                        0;
                        handle->AutoNegCapAdvertised |=
                                INTEL_GETBIT(INTEL_PHY_GCON_1000BASETFD,handle->phyRegs.gcon) ? EDD_AUTONEG_CAP_1000BASETFD :
                                        0;
                    }
                }

                /* OK, next state */
                handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP9;
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "intel_setup_phy_sm: Reading Phy Register INTEL_PHY_ANA not finished yet");
            }
        }
        break;

        /* ------------------------------------------------- */
        /* ----- write back all modified PHY registers ----- */
        /* ------------------------------------------------- */

        /* Step 9+10: Write GCON PHY Register */
        case INTEL_PHY_SETUP_SM_STEP9:
        {
            intel_beginWritePhyReg(handle,INTEL_PHY_GCON,handle->phyRegs.gcon);
            /* OK, next state */
            handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP10;
        }
        break;

        case INTEL_PHY_SETUP_SM_STEP10:
        {
            if(intel_endWritePhyReg(handle))
            {
                /* OK, next state */
                handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP11;
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "intel_setup_phy_sm: Reading Phy Register INTEL_PHY_GCON not finished yet");
            }
        }
        break;

        /* Step 11+12: Write PSCON  PHY Register */
        case INTEL_PHY_SETUP_SM_STEP11:
        {
            intel_beginWritePhyReg(handle,INTEL_PHY_PSCON,handle->phyRegs.pscon);
            /* OK, next state */
            handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP12;
        }
        break;

        case INTEL_PHY_SETUP_SM_STEP12:
        {
            if(intel_endWritePhyReg(handle))
            {
                /* OK, next state */
                handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP13;
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "intel_setup_phy_sm: Reading Phy Register INTEL_PHY_PSCON not finished yet");
            }
        }
        break;

        /* Step 13+14: Read PCTRL PHY Register */
        case INTEL_PHY_SETUP_SM_STEP13:
        {
            intel_beginWritePhyReg(handle,INTEL_PHY_PCTRL,handle->phyRegs.pctrl);
            /* OK, next state */
            handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP14;
        }
        break;

        case INTEL_PHY_SETUP_SM_STEP14:
        {
            if(intel_endWritePhyReg(handle))
            {
                /* OK, next state */
                handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP15;
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "intel_setup_phy_sm: Reading Phy Register INTEL_PHY_PCTRL not finished yet");
            }
        }
        break;

        /* Step 15+16: Read ANA PHY Register */
        case INTEL_PHY_SETUP_SM_STEP15:
        {
            intel_beginWritePhyReg(handle,INTEL_PHY_ANA,handle->phyRegs.ana);
            /* OK, next state */
            handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP16;
        }
        break;

        case INTEL_PHY_SETUP_SM_STEP16:
        {
            if(intel_endWritePhyReg(handle))
            {
                handle->phySetupSM = INTEL_PHY_SETUP_SM_STEP17;
            }
            else
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                    "intel_setup_phy_sm: Reading Phy Register INTEL_PHY_ANA not finished yet");
            }
        }
        break;

        /* ---------------------------- */
        /* ----- finish PHY setup ----- */
        /* ---------------------------- */

        case INTEL_PHY_SETUP_SM_STEP17:
        {
            /* NOTE: only for I210 */
            if((I210 == handle->params.DeviceID)||(I210SFP == handle->params.DeviceID))
            {
                LSA_UINT32 ipcnfg;
                LSA_UINT32 phpm;

                /* disable EEE --> enabled on default */
                ipcnfg = intel_reg32(INTEL_IPCNFG);
                INTEL_CLRBIT(ipcnfg, INTEL_IPCNFG_EEE_100M_AN);
                INTEL_CLRBIT(ipcnfg, INTEL_IPCNFG_EEE_1G_AN);
                intel_reg32(INTEL_IPCNFG) = ipcnfg;

                /* disable SPD (smart power down) --> enabled on default */
                /* disable Go Link disconnect --> might be set in NVM to start without link */
                phpm = intel_reg32(INTEL_PHPM);
                INTEL_CLRBIT(phpm, INTEL_PHPM_SPD_EN);
                INTEL_CLRBIT(phpm, INTEL_PHPM_GO_LINK_DISCONNECT);
                intel_reg32(INTEL_PHPM) = phpm;
            }
            handle->phySetupSM = INTEL_PHY_SETUP_SM_FINISH;
            /* fall through to release semaphore */
        }
        //lint -fallthrough

        case INTEL_PHY_SETUP_SM_FINISH:
        {
            if(intel_try_release_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
            {
                /* this was last step, return OK. */
                status = EDD_STS_OK;
                handle->phySetupSM = INTEL_PHY_SETUP_SM_IDLE; /* reset state machine */
            }
        }
        break;

        default:
        {
            EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "intel_setup_phy_sm: invalid state for handle->phySetupSM(%d)", handle->phySetupSM);
            status = EDD_STS_ERR_SEQUENCE;
        }
        break;
    }

    return status;
}


static LSA_RESULT intel_reset_phy(INTEL_LL_HANDLE_TYPE* const handle)
{
    LSA_RESULT Status;

    /* step 1, 2 and 3 are resetting the phy */
    while(!intel_reset_phy_step1(handle))
    {
        EDDS_WAIT_US(10);
    }

    EDDS_WAIT_US(300);

    intel_reset_phy_step2(handle);

    while(!intel_reset_phy_step3(handle))
    {
        EDDS_WAIT_US(10);
    }

    /* Intel i210 Data sheet:
     * Note: After a PHY reset, access through the MDIC register should not be attempted for 300us.
     */
    EDDS_WAIT_US(300);


    while(!intel_setup_phy_begin(handle))
    {
        EDDS_WAIT_US(10);
    }

    while(EDD_STS_OK_PENDING == (Status = intel_setup_phy_sm(handle)))
    {
        EDDS_WAIT_US(10);
    }

    return Status;
}


//JB 05/12/2014 we will check this within a MACRO
static LSA_BOOL intel_rdesc_done(INTEL_RDESC const * const currentRD)
{
    return (1 == INTEL_GETBITI(INTEL_RDESC_STATUS_DD, currentRD->Status)) ? LSA_TRUE :
            LSA_FALSE;
}

// helper function - allocates aligned memory, depending on deviceID
static LSA_RESULT intel_alloc_TxRxDescr(INTEL_LL_HANDLE_TYPE* const handle)
{
    LSA_RESULT Status = EDD_STS_OK;
    LSA_UINT16 deviceID;
    LSA_UINT32 alignment; // valid 2,4,8,16,32,64,128 ...
    LSA_UINT64 physicalAddrTx;
    LSA_UINT64 physicalAddrRx;
    LSA_UINT64 offsetTx;
    LSA_UINT64 offsetRx;

    deviceID = handle->params.DeviceID;

    // get alignment depending on deviceID
    switch ((enum INTEL_DEVICE_ID) (deviceID))
    {
        case I210:
        case I210SFP:
            alignment = 128;
            break;
        case IG82574L:
            alignment = 16;
            break;
        default:
            // can not happen, is checked in open before --> sanity
            alignment = 16;
            break;
    }

    // allocate descriptor for Rx and Tx
    EDDS_ALLOC_DEV_BUFFER_MEM(handle->hSysDev,
            (EDD_UPPER_MEM_PTR_TYPE*) &handle->pTxDescrAlloc,
            (handle->TxFrameBufferCount * sizeof(INTEL_TDESC) + alignment));
    if(0 == handle->pTxDescrAlloc)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "intel_alloc_TxRxDescr: failed to allocate memory for handle->pTxDescrAlloc");
        Status = EDD_STS_ERR_RESOURCE;
    }
    else
    {
        EDDS_ALLOC_DEV_BUFFER_MEM(handle->hSysDev,
            (EDD_UPPER_MEM_PTR_TYPE*) &handle->pRxDescrAlloc,
            (handle->RxFrameBufferCount * sizeof(INTEL_RDESC) + alignment));

        if(0 == handle->pRxDescrAlloc)
        {
            LSA_UINT16 retval;

            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "intel_alloc_TxRxDescr: failed to allocate memory for handle->pRxDescrAlloc");

            EDDS_FREE_DEV_BUFFER_MEM(handle->hSysDev, &retval, (EDD_UPPER_MEM_PTR_TYPE) handle->pTxDescrAlloc);
            if(EDD_STS_OK != retval)
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "intel_alloc_TxRxDescr: failed to free memory for handle->pTxDescrAlloc");
            }

            Status = EDD_STS_ERR_RESOURCE;
        }
        else
        {


            // get physical address as LSA_UINT64 from virtual address
            EDDS_PTR_TO_ADDR64_DEV(handle->hSysDev, handle->pTxDescrAlloc,
                    &physicalAddrTx);
            EDDS_PTR_TO_ADDR64_DEV(handle->hSysDev, handle->pRxDescrAlloc,
                    &physicalAddrRx);

            // calculate offset to aligned address with physical address
            offsetTx = physicalAddrTx + (alignment-1);
            offsetTx &= ~((LSA_UINT64)(alignment-1));
            offsetTx -= physicalAddrTx;

            offsetRx = physicalAddrRx + (alignment-1);
            offsetRx &= ~((LSA_UINT64)(alignment-1));
            offsetRx -= physicalAddrRx;

            // add offset to virtual address
            handle->pTxDescriptors =
                    (EDD_UPPER_MEM_PTR_TYPE) (((LSA_UINT8*) handle->pTxDescrAlloc) + offsetTx);
            handle->pRxDescriptors =
                    (EDD_UPPER_MEM_PTR_TYPE) (((LSA_UINT8*) handle->pRxDescrAlloc) + offsetRx);
        }
    }

    return Status;
}

// helper function - free's aligned memory
static LSA_RESULT intel_free_TxRxDescr(INTEL_LL_HANDLE_TYPE const * const handle)
{
    LSA_RESULT Status = EDD_STS_OK;

    // it is assured that descriptor memory for rx and tx is allocated, so it is not necessary to check it here
    EDDS_FREE_DEV_BUFFER_MEM(handle->hSysDev, &Status, (EDD_UPPER_MEM_PTR_TYPE) handle->pTxDescrAlloc);
    if (EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "intel_free_TxRxDescr: failed to free memory for handle->pTxDescrAlloc.");
    }

    EDDS_FREE_DEV_BUFFER_MEM(handle->hSysDev, &Status, (EDD_UPPER_MEM_PTR_TYPE) handle->pRxDescrAlloc);
    if(EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "intel_free_TxRxDescr: failed to free memory for handle->pRxDescrAlloc.");
    }

    return Status;
}

static LSA_BOOL intel_is_reset_time_elapsed(
        INTEL_LL_HANDLE_TYPE const * const handle)
{
    LSA_UINT64 CurrentSysTim;

    CurrentSysTim = EDDS_GET_SYSTEM_TIME_NS(handle->hSysDev);

    if(handle->ResetTargetSysTim <= CurrentSysTim)
    {
        return LSA_TRUE;
    }
    else
    {
        return LSA_FALSE;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   intel_link_reset_state_machine               +*/
/*+                                                                         +*/
/*+  Input/Output          :   INTEL_LL_HANDLE_TYPE*           handle       +*/
/*+                                                                         +*/
/*+  Result                :   LSA_VOID                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  handle      : LL instance management pointer                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Description: Handle link reset state machine of LL.                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID EDDS_LOCAL_FCT_ATTR intel_link_reset_state_machine(
        INTEL_LL_HANDLE_TYPE* handle, LSA_BOOL hwTimeSlicing)
{
    LSA_UINT16 timeDelayTillNextStateUS;
    LSA_BOOL moreStates;    /* try to execute another state */
    INTEL_LINK_RESET_STATE_SM_STATES_TYPE CurrentState;
    LSA_RESULT Status;

    Status = EDD_STS_OK;
    moreStates = !hwTimeSlicing;

    /* possible while loop is an equivalent of re-calling this function */
    do
    {
        CurrentState = handle->ResetState;
        EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
                "IN : intel_link_reset_state_machine(): CurrentState=0x%X",
                CurrentState);

        timeDelayTillNextStateUS = 0;     /* per default, no minimum delay between this and next state is present */

        if(intel_is_reset_time_elapsed(handle))
        {
        switch (CurrentState) /* CurrentState is from type INTEL_LINK_RESET_STATE_SM_STATES_TYPE */
        {
            /* ----------------------------------------------------------------------------------------
             * CurrentState: INTEL_RESET_STATE_IDLE
             * ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_IDLE:
            {
                if (!handle->isLinkUp)
                {
                    /* leave idle state, fall through! */
                    handle->ResetState = INTEL_RESET_STATE_RXTX_QUEUE_SHUTDOWN;
                }
            }    //lint -fallthrough


            /* ----------------------------------------------------------------------------------------
             * Disable RX/TX queues
             * ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_RXTX_QUEUE_SHUTDOWN:
            {
                LSA_UINT32 idx, count;
                LSA_UINT32 txdctl = intel_reg32(INTEL_TXDCTL);
                LSA_UINT32 tctl = intel_reg32(INTEL_TCTL);

                INTEL_CLRBIT(txdctl, INTEL_TXDCTL_ENABLE);
                /* disable transmit queue */intel_reg32(INTEL_TXDCTL) =
                        txdctl;
                INTEL_CLRBIT(tctl, INTEL_TCTL_EN);
                /* disable transmit unit */intel_reg32(INTEL_TCTL) = tctl;

                INTEL_CLRBIT(intel_reg32(INTEL_RXDCTL),
                        INTEL_RXDCTL_ENABLE);
                /* disable receive queue */
                INTEL_CLRBIT(intel_reg32(INTEL_RCTL), INTEL_RCTL_EN);
                /* disable receive unit */

                /* set all enqueued descriptors to DD */
                idx = handle->TXDescriptorIndex;
                for (count = handle->TXDescriptorsUsed; count > 0; --count)
                {
                    INTEL_TDESC* tdesc = &handle->pTxDescriptors[idx];

                    /**@todo  honor cache line size and alignment to reduce number of EDDS_DEV_MEM_DMA_SYNC_FROM_DEVICE calls!
                     */
                    EDDS_DEV_MEM_DMA_SYNC_FROM_DEVICE(handle->hSysDev,tdesc,sizeof(*tdesc));

                    INTEL_SETBIT(tdesc->legacy.STA_RSV,
                            INTEL_TDESC_STA_RSV_DD);

                    /* we didn't need to do a EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE(handle->hSysDev,tdesc,sizeof(*tdesc))
                     * since the transmit unit is disabled!
                     * this is done on normal send operation in EDDS_LL_SEND.
                     */
                    ++idx;
                    if(handle->TxFrameBufferCount <= idx)
                    {
                        idx = 0;
                    }
                }

                /* wait for next reset step */
                handle->ResetState = INTEL_RESET_STATE_RESET_CHIP_ON;
                timeDelayTillNextStateUS = INTEL_TIME_RXTX_QUEUE_SHUTDOWN;
            }
            break; /* INTEL_RESET_STATE_IDLE */

            /* ----------------------------------------------------------------------------------------
             * Save registers needed to recover actual configuration of MAC after reset
             * Initiate Chip reset.
             * ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_RESET_CHIP_ON:
            {
                /* save registers */
                intel_save_volatile_registers(handle);
                intel_reset(handle, LSA_FALSE);

                /* wait for next reset step */
                handle->ResetState = INTEL_RESET_STATE_WF_SW_QUEUE_FLUSH;
                timeDelayTillNextStateUS = INTEL_TIME_RESET_REGISTER_ACCESS;
            }
            break; /* INTEL_RESET_STATE_WF_RESET_STEP_1 */

            /* ----------------------------------------------------------------------------------------
             * Wait until software has flushed the disabled tx queues
             * ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_WF_SW_QUEUE_FLUSH:
            {
                if (0 != handle->TXDescriptorsUsed)
                {
                    /* wait at least one cylce (>=1ms) (until no more TX descriptors are in use) */
                    timeDelayTillNextStateUS = INTEL_TIME_TX_QUEUE_SW_FLUSH;

                    break; /* break switch case; we do this case again in at least 1ms */
                }

                /* go on with next step, no wait needed */
                handle->ResetState = INTEL_RESET_STATE_RESET_CHIP_OFF;
            }
            break; /* INTEL_RESET_STATE_WF_RESET_STEP_2 */

            /* ---------------------------------------------------------------------------------------- */
            /* CurrentState: INTEL_RESET_STATE_WF_RESET_STEP_3                                          */
            /* ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_RESET_CHIP_OFF:
            {
                if( intel_reset_finish(handle) )
                {
                    handle->ResetState = INTEL_RESET_STATE_RESET_PHY_ON;
                }
                else
                {
                    /* retry reset finish next trigger */
                }
            }
            break; /* INTEL_RESET_STATE_WF_RESET_STEP_3 */


            /* ---------------------------------------------------------------------------------------- */
            /* CurrentState: INTEL_RESET_STATE_WF_RESET_PHY_STEP_1                                      */
            /* ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_RESET_PHY_ON:
            {
                if(intel_reset_phy_step1(handle))
                {
                    handle->ResetState = INTEL_RESET_STATE_RESET_PHY_OFF;
                    timeDelayTillNextStateUS = INTEL_TIME_RESET_PHY_ACTIVE;
                }
            }
            break;  /* INTEL_RESET_STATE_WF_RESET_PHY_STEP1 */

            /* ---------------------------------------------------------------------------------------- */
            /* CurrentState: INTEL_RESET_STATE_WF_RESET_PHY_STEP_2                                      */
            /* ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_RESET_PHY_OFF:
            {
                intel_reset_phy_step2(handle);
                handle->ResetState = INTEL_RESET_STATE_RESET_PHY_FINISH;
            }
            /* INTEL_RESET_STATE_WF_RESET_PHY_STEP2 */
            /* fall through, no delaytime!  */
            //lint -fallthrough

            /* ---------------------------------------------------------------------------------------- */
            /* CurrentState: INTEL_RESET_STATE_WF_RESET_PHY_STEP_3                                      */
            /* ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_RESET_PHY_FINISH:
            {
                if(intel_reset_phy_step3(handle))
                {
                    handle->ResetState = INTEL_RESET_STATE_WF_EEPROM;
                    timeDelayTillNextStateUS = INTEL_TIME_RESET_PHY_ACCESS;
                }else
                {
                    /* retry after delay */
                    timeDelayTillNextStateUS = INTEL_TIME_SEMAPHORE;
                }
            }
            break;  /* INTEL_RESET_STATE_WF_RESET_PHY_STEP3 */

            /* ---------------------------------------------------------------------------------------- */
            /* CurrentState: INTEL_RESET_STATE_WF_RESET_PHY_STEP_4                                      */
            /* ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_WF_EEPROM:
            {
                if(intel_setup_phy_begin(handle))
                {
                    handle->ResetState = INTEL_RESET_STATE_WF_SETUP_PHY_SM;
                }else
                {
                    /* retry after delay */
                    timeDelayTillNextStateUS = INTEL_TIME_REGISTER_ACCESS;
                }
            }
            break;  /* INTEL_RESET_STATE_WF_RESET_PHY_SETUP */

            /* ---------------------------------------------------------------------------------------- */
            /* CurrentState: INTEL_RESET_STATE_WF_RESET_PHY_FINISH                                      */
            /* ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_WF_SETUP_PHY_SM:
            {
                Status = intel_setup_phy_sm(handle);
                if(EDD_STS_OK_PENDING == Status)
                {
                    /* retry after delay */
                    timeDelayTillNextStateUS = INTEL_TIME_PHY_REGISTER_ACCESS;
                }else
                {
                    if (EDD_STS_OK == Status)
                    {
                        handle->regSetupSM = INTEL_REG_SETUP_SM_IDLE;
                        handle->ResetState = INTEL_RESET_STATE_WF_REGISTER_SETUP_SM;
                    }
                    else
                    {
                        /* try to reset whole chip again */
                        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_WARN,
                            "intel_link_reset_state_machine: INTEL_RESET_STATE_WF_SETUP_PHY_SM failed. restart state machine.");
                        handle->ResetState = INTEL_RESET_STATE_RESET_CHIP_ON;
                    }
                }
            }
            break;  /* INTEL_RESET_STATE_WF_RESET_PHY_SETUP */

            /* ---------------------------------------------------------------------------------------- */
            /* CurrentState: INTEL_RESET_STATE_WF_REGISTER_SETUP                                        */
            /* ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_WF_REGISTER_SETUP_SM:
            {
                Status = intel_setup_registers_sm(handle);

                if(EDD_STS_OK_PENDING == Status)
                {
                    /* retry after delay */
                    timeDelayTillNextStateUS = INTEL_TIME_REGISTER_ACCESS;
                }else
                {
                    if (EDD_STS_OK == Status)
                    {
                        handle->ResetState = INTEL_RESET_STATE_WF_SET_LINK_SM;
                    }
                    else
                    {
                        /* try to reset whole chip again */
                        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_WARN,
                            "intel_link_reset_state_machine: INTEL_RESET_STATE_WF_REGISTER_SETUP_SM failed. restart state machine.");
                        handle->ResetState = INTEL_RESET_STATE_RESET_CHIP_ON;
                    }
                }
            }
            break;  /* INTEL_RESET_STATE_WF_REGISTER_SETUP */


            /* ---------------------------------------------------------------------------------------- */
            /* CurrentState: INTEL_RESET_STATE_WF_SET_LINK_SETUP                                        */
            /* ---------------------------------------------------------------------------------------- */
            case INTEL_RESET_STATE_WF_SET_LINK_SM:
            {
                Status = intel_set_link_sm(handle);
                if(EDD_STS_OK_PENDING != Status)
                {
                    handle->isResetPending = LSA_FALSE;
                    handle->ResetState = INTEL_RESET_STATE_IDLE;
                }
            }
            break;


            default:
            {
                EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_WARN,
                        "intel_link_reset_state_machine(): FATAL -> unknown current reset state (0x%X)!",
                        CurrentState);
            } /* default */
        } /* end switch current state */
        } /* end if(intel_is_reset_time_elapsed(handle)) */

        if(    (INTEL_RESET_STATE_IDLE == handle->ResetState)
            || (INTEL_RESET_STATE_WF_SW_QUEUE_FLUSH == handle->ResetState)
          )
        {
            moreStates = LSA_FALSE;
        }

        /* if time to wait until next step may be executed is set (<>0),
         * set new handle->ResetTargetSysTim
         */
        if(timeDelayTillNextStateUS)
        {
            handle->ResetTargetSysTim = EDDS_GET_SYSTEM_TIME_NS(handle->hSysDev) + (1000 * timeDelayTillNextStateUS);
            if(moreStates)
            {
                EDDS_WAIT_US(timeDelayTillNextStateUS);
            }
        }

        /* possible while loop is an equivalent of re-calling this function */
        EDDS_LL_TRACE_02(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
                "OUT: intel_link_reset_state_machine(): NextState=0x%X, Status=0x%X", handle->ResetState,Status);
    } while(moreStates);


    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   intel_read_statistics_state_machine          +*/
/*+                                                                         +*/
/*+  Input/Output          :   INTEL_LL_HANDLE_TYPE*           handle       +*/
/*+                                                                         +*/
/*+  Result                :   LSA_VOID                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  handle      : LL instance management pointer                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Description: Handle reading of statistics state machine of LL.         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_VOID EDDS_LOCAL_FCT_ATTR intel_read_statistics_state_machine(
        INTEL_LL_HANDLE_TYPE* handle,LSA_BOOL hwTimeSlicing)
{
	INTEL_READ_STATS_SM_STATES_TYPE CurrentState;

    CurrentState = handle->ReadStatisticsState;

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN : intel_read_statistics_state_machine(): CurrentState=0x%X",
            CurrentState);

    switch (CurrentState) /* CurrentState is from type INTEL_READ_STATS_SM_STATES_TYPE */
    {
        /* ---------------------------------------------------------------------------------------- */
        /* CurrentState: INTEL_READ_STATS_IN                                                        */
        /* ---------------------------------------------------------------------------------------- */
        case INTEL_READ_IN_STATISTICS1:
        {
            /* Good Packets Received Count      - GPRC    (0x04074) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_GPRC] += intel_reg32(INTEL_GPRC);
            handle->ReadStatisticsState = INTEL_READ_IN_STATISTICS2;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_IN_STATISTICS2:
        {
            /* Broadcast Packets Received Count - BPRC    (0x04078) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_BPRC] += intel_reg32(INTEL_BPRC);
            handle->ReadStatisticsState = INTEL_READ_IN_STATISTICS3;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_IN_STATISTICS3:
        {
            /* Multicast Packets Received Count - MPRC    (0x0407C) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_MPRC] += intel_reg32(INTEL_MPRC);
            handle->ReadStatisticsState = INTEL_READ_IN_STATISTICS4;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_IN_STATISTICS4:
        {
            LSA_UINT64 tmp, GPRC;
             /* Good Octets Received Count       - GORCL/H (0x04088/0x0408C) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_GORC] += intel_reg32(INTEL_GORCL);
            handle->StatsRegShadow[INTEL_STATS_INDEX_GORC] += ((LSA_UINT64)intel_reg32(INTEL_GORCH)) << 32;

            tmp = handle->StatsRegShadow[INTEL_STATS_INDEX_BPRC] + handle->StatsRegShadow[INTEL_STATS_INDEX_MPRC];
            handle->Statistics.InOctets = (LSA_UINT32) handle->StatsRegShadow[INTEL_STATS_INDEX_GORC];
            handle->Statistics.InNUcastPkts = (LSA_UINT32) tmp;
            /* RFC2863_COUNTER */
            handle->Statistics.InHCOctets = handle->StatsRegShadow[INTEL_STATS_INDEX_GORC];
            handle->Statistics.InMulticastPkts = (LSA_UINT32) handle->StatsRegShadow[INTEL_STATS_INDEX_MPRC];
            handle->Statistics.InBroadcastPkts = (LSA_UINT32) handle->StatsRegShadow[INTEL_STATS_INDEX_BPRC];
            handle->Statistics.InHCMulticastPkts = handle->StatsRegShadow[INTEL_STATS_INDEX_MPRC];
            handle->Statistics.InHCBroadcastPkts = handle->StatsRegShadow[INTEL_STATS_INDEX_BPRC];
            /* RFC2863_COUNTER - End */
            GPRC = handle->StatsRegShadow[INTEL_STATS_INDEX_GPRC];

            /* Determine amount of received unicast frames. */
            /* NOTE: The statistic registers can not be read consistently and incoming frames may alter some */
            /*       counters in between. This might lead to temporary wrong counter values. Therefore the   */
            /*       last counter is saved in a shadow variable. In addition LSA_UINT64 counter values are   */
            /*       used so that we dont have to care about wrap arounds.                                   */
            if ((handle->InUcastPktsShadow + tmp) > GPRC)
            {
                handle->Statistics.InUcastPkts = (LSA_UINT32) handle->InUcastPktsShadow;
                handle->Statistics.InHCUcastPkts = handle->InUcastPktsShadow;
            }
            else
            {
                handle->InUcastPktsShadow = (LSA_UINT64) (GPRC - tmp); /* save current unicast frame counter in shadow */
                handle->Statistics.InUcastPkts = (LSA_UINT32) handle->InUcastPktsShadow; /* unicast = GPRC - MPRC - BPRC */
                handle->Statistics.InHCUcastPkts = handle->InUcastPktsShadow;
            }

            handle->ReadStatisticsState = INTEL_READ_OUT_STATISTICS1;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        /* ---------------------------------------------------------------------------------------- */
        /* CurrentState: INTEL_READ_STATS_OUT                                                       */
        /* ---------------------------------------------------------------------------------------- */
        case INTEL_READ_OUT_STATISTICS1:
        {
            /* Good Packets Transmitted Count       - GPTC    (0x04080) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_GPTC] += intel_reg32(INTEL_GPTC);
            handle->ReadStatisticsState = INTEL_READ_OUT_STATISTICS2;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_OUT_STATISTICS2:
        {
            /* Management Packets Transmitted Count - MPTC    (0x040BC) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_MPTC] += intel_reg32(INTEL_MPTC);
            handle->ReadStatisticsState = INTEL_READ_OUT_STATISTICS3;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_OUT_STATISTICS3:
        {
            /* Broadcast Packets Transmitted Count  - BPTC    (0x040F4) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_BPTC] += intel_reg32(INTEL_BPTC);
            handle->ReadStatisticsState = INTEL_READ_OUT_STATISTICS4;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_OUT_STATISTICS4:
        {
            LSA_UINT64 tmp, GPTC;
            /* Good Octets Transmitted Count        - GOTCL/H (0x04090/0x04094) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_GOTC] += intel_reg32(INTEL_GOTCL);
            handle->StatsRegShadow[INTEL_STATS_INDEX_GOTC] += ((LSA_UINT64)intel_reg32(INTEL_GOTCH)) << 32;

            tmp = handle->StatsRegShadow[INTEL_STATS_INDEX_MPTC] + handle->StatsRegShadow[INTEL_STATS_INDEX_BPTC];
            handle->Statistics.OutOctets = (LSA_UINT32) handle->StatsRegShadow[INTEL_STATS_INDEX_GOTC];
            handle->Statistics.OutNUcastPkts = (LSA_UINT32) tmp;
            /* RFC2863_COUNTER */
            handle->Statistics.OutHCOctets = handle->StatsRegShadow[INTEL_STATS_INDEX_GOTC];
            handle->Statistics.OutMulticastPkts = (LSA_UINT32) handle->StatsRegShadow[INTEL_STATS_INDEX_MPTC];
            handle->Statistics.OutBroadcastPkts = (LSA_UINT32) handle->StatsRegShadow[INTEL_STATS_INDEX_BPTC];
            handle->Statistics.OutHCMulticastPkts = handle->StatsRegShadow[INTEL_STATS_INDEX_MPTC];
            handle->Statistics.OutHCBroadcastPkts = handle->StatsRegShadow[INTEL_STATS_INDEX_BPTC];
            /* RFC2863_COUNTER - End */
            GPTC = handle->StatsRegShadow[INTEL_STATS_INDEX_GPTC];

            /* determine amount of sent unicast frames */
            /* NOTE: The statistic registers can not be read consistently and outgoing frames may alter some */
            /*       counters in between. This might lead to temporary wrong counter values. Therefore the   */
            /*       last counter is saved in a shadow variable. In addition LSA_UINT64 counter values are   */
            /*       used so that we dont have to care about wrap arounds.                                   */
            if ((handle->OutUcastPktsShadow + tmp) > GPTC)
            {
                handle->Statistics.OutUcastPkts = (LSA_UINT32) handle->OutUcastPktsShadow;
                handle->Statistics.OutHCUcastPkts = handle->OutUcastPktsShadow;
            }
            else
            {
                handle->OutUcastPktsShadow = (LSA_UINT64) (GPTC - tmp); /* save current unicast frame counter in shadow */
                handle->Statistics.OutUcastPkts = (LSA_UINT32) handle->OutUcastPktsShadow; /* unicast = GPRC - MPRC - BPRC */
                handle->Statistics.OutHCUcastPkts = handle->OutUcastPktsShadow; /* unicast = GPRC - MPRC - BPRC */
            }

            handle->ReadStatisticsState = INTEL_READ_IN_ERROR_STATISTICS1;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        /* ---------------------------------------------------------------------------------------- */
        /* CurrentState: INTEL_READ_STATS_IN_ERROR                                                  */
        /* ---------------------------------------------------------------------------------------- */
        case INTEL_READ_IN_ERROR_STATISTICS1:
        {
            /* CRC Error Count            - CRCERRS (0x04000) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_CRCERRS] += intel_reg32(INTEL_CRCERRS);
            handle->ReadStatisticsState = INTEL_READ_IN_ERROR_STATISTICS2;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_IN_ERROR_STATISTICS2:
        {
            /* Missed Packets Count       - MPC     (0x04010) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_MPC]     += intel_reg32(INTEL_MPC);
            handle->ReadStatisticsState = INTEL_READ_IN_ERROR_STATISTICS3;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_IN_ERROR_STATISTICS3:
        {
            /* RX Error Count             - RXERRC  (0x0400C) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_RXERRC]  += intel_reg32(INTEL_RXERRC);
            handle->ReadStatisticsState = INTEL_READ_IN_ERROR_STATISTICS4;
        }
        if(hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_IN_ERROR_STATISTICS4:
        {
            handle->ReadStatisticsState = INTEL_READ_IN_STATISTICS1;
            /* Receive Length Error Count - RLEC    (0x04040) */
            handle->StatsRegShadow[INTEL_STATS_INDEX_RLEC] += intel_reg32(INTEL_RLEC);

            handle->Statistics.InDiscards = (LSA_UINT32)handle->StatsRegShadow[INTEL_STATS_INDEX_MPC];
            handle->Statistics.InErrors = (LSA_UINT32)(handle->StatsRegShadow[INTEL_STATS_INDEX_CRCERRS] +
            handle->StatsRegShadow[INTEL_STATS_INDEX_RXERRC] +
            handle->StatsRegShadow[INTEL_STATS_INDEX_RLEC]); //lint !e845 !e778 JB 08/12/2014 readability
            handle->ReadStatisticsState = INTEL_READ_OUT_ERROR_STATISTICS1;
        }
        if (hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_OUT_ERROR_STATISTICS1:
        {
            /* Excessive Collision count */
            handle->StatsRegShadow[INTEL_STATS_INDEX_ECOL] += intel_reg32(INTEL_ECOL);
            handle->ReadStatisticsState = INTEL_READ_OUT_ERROR_STATISTICS2;
        }
        if (hwTimeSlicing) break; /* else fall through */
        //lint -fallthrough

        case INTEL_READ_OUT_ERROR_STATISTICS2:
        {
            /* Late Collision count */
            handle->StatsRegShadow[INTEL_STATS_INDEX_LATECOL] += intel_reg32(INTEL_LATECOL);
            handle->Statistics.OutErrors = (LSA_UINT32)(handle->StatsRegShadow[INTEL_STATS_INDEX_ECOL] + handle->StatsRegShadow[INTEL_STATS_INDEX_LATECOL]);
            handle->ReadStatisticsState = INTEL_READ_IN_STATISTICS1;
        }
        if(hwTimeSlicing) break; /* else fall through */

        break;  /* last case always breaks */

        default:
        {
            EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                    "intel_read_statistics_state_machine(): UNEXP -> unknown current reset state (0x%X)!",
                    CurrentState);
        } /* default */
    } /* end switch current state */

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT: intel_read_statistics_state_machine(): NextState=0x%X", handle->ReadStatisticsState);

    return;
}



/** \brief      Check and set link state machine.
 *
 *  \details    Check actual link and probably set pending link change.
 *
 *  \param      handle      Handle to EDDS instance.
 *
 *  \return     LSA_TRUE, if link was changed,
 *              LSA_FALE, if no link change happened.
 *
 *  \author     am2219
 *  \date       2015-10-23
 */
static LSA_BOOL EDDS_LOCAL_FCT_ATTR intel_link_check_and_set_state_machine(
        INTEL_LL_HANDLE_TYPE* handle,LSA_BOOL hwTimeSlicing)
{
    /* read link - do not evalute ICR.LSC, because LLIF does not handle interrupts (this may be done in system adaption) */
    /* read STATUS register for Status, Speed and Mode */
    LSA_BOOL isLinkUp;
    LSA_UINT32 value;
    LSA_BOOL linkChanged;

    linkChanged = LSA_FALSE;

    /* write before read */
    /* linkstat */
    if(handle->isLinkStateChanged)
    {
        LSA_BOOL moreStates;
        moreStates = !hwTimeSlicing;

        do
        {
            if(EDD_STS_OK_PENDING != intel_set_link_sm(handle))
            {
                moreStates = LSA_FALSE;
                handle->isLinkStateChanged = LSA_FALSE;
            }
        } while (moreStates);
    }else
    {
        value = intel_reg32(INTEL_STATUS);

        /* is link down or up? */
        isLinkUp = (0 != INTEL_GETBIT(INTEL_STATUS_LU, value) ? LSA_TRUE : LSA_FALSE);

        if(isLinkUp != handle->isLinkUp)
        {
            /* link has changed! */
            linkChanged = LSA_TRUE;
            handle->isLinkUp = isLinkUp;
            /* link status changed to UP */
            if (handle->isLinkUp)
            {
                /* set status and mode shadow */
                handle->StatusShadow = EDD_LINK_UP;
                handle->ModeShadow   = (0 == INTEL_GETBIT(INTEL_STATUS_FD, value)) ? EDD_LINK_MODE_HALF : EDD_LINK_MODE_FULL;

                /* set speed shadow */
                switch (INTEL_GETBITS(INTEL_STATUS_SPEED, value))
                {
                    case 0:
                        handle->SpeedShadow = EDD_LINK_SPEED_10;
                        break;
                    case 1:
                        handle->SpeedShadow = EDD_LINK_SPEED_100;
                        break;
                    case 2:
                    case 3:
                        handle->SpeedShadow = EDD_LINK_SPEED_1000;
                        break;
                    default:
                        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_UNEXP,
                            "intel_link_check_and_set_state_machine: value for Speed is not known.");
                        /* not possible */
                        break;
                }
            }
            /* link status changed to DOWN */
            else
            {
                /* no link up */
                handle->StatusShadow = EDD_LINK_DOWN;
                handle->ModeShadow   = EDD_LINK_UNKNOWN;
                handle->SpeedShadow  = EDD_LINK_UNKNOWN;

                /* link status changed and no link up, run reset */
                handle->isResetPending = LSA_TRUE;
            }
        }
    }

    return linkChanged;
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   intel_get_startup_link_param                 +*/
/*+                                                                         +*/
/*+  Input/Output          :   INTEL_LL_HANDLE_TYPE*                        +*/
/*+                            EDDS_LOCAL_LL_STARTUP_LINK_PARAM_PTR_TYPE    +*/
/*+                                                                         +*/
/*+  Result                :   LSA_RESULT                                   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  handle            : LL instance management pointer                     +*/
/*+  pHwParam          : pointer to link param structure to be filled       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Description: Return the initial phy power status and configured link   +*/
/*+               speed mode.                                               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_RESULT intel_get_startup_link_param(
        INTEL_LL_HANDLE_TYPE const * const handle,
        EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE pHwParam)
{
    LSA_RESULT Status = EDD_STS_OK;

    #if (EDDS_CFG_TRACE_MODE == 0)
        LSA_UNUSED_ARG(handle);
    #endif

    if( 1 > pHwParam->PortCnt )
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "INTEL_LL_SETUP: PortCnt to small!");
        Status = EDD_STS_ERR_PARAM;
    }

    if( EDD_STS_OK == Status )
    {
        pHwParam->PHYPower[0]      = EDDS_PHY_POWER_OFF;
        pHwParam->LinkSpeedMode[0] = EDD_LINK_AUTONEG;
        pHwParam->IsWireless[0]    = EDD_PORT_IS_NOT_WIRELESS;
    }

    return (Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_OPEN                               +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_HANDLE                   hDDB          +*/
/*+                             EDDS_UPPER_DPB_PTR_TYPE       pDPB          +*/
/*+                             LSA_UINT32                    TraceIdx      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  hDDB         : Handle to Device Description Block (Device-management)  +*/
/*+  pDPB         : Pointer to Device Parameter Block                       +*/
/*+  TraceIdx     : Trace index to use with this EDDS LLIF instance         +*/
/*+                                                                         +*/
/*+  Result       :         EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDD_STS_ERR_ABORT                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initialize LL Ethernetcontroller using DPB parameters     +*/
/*+                                                                         +*/
/*+               See DPB documentation.                                    +*/
/*+                                                                         +*/
/*+               global:                                                   +*/
/*+                                                                         +*/
/*+               pDPB->RxBufferCnt     : number of rx-buffers/descriptors  +*/
/*+               pDPB->TxBufferCnt     : number of tx-buffers/descriptors  +*/
/*+               pDPB->RxBufferSize    : rx-buffers size                   +*/
/*+               pDPB->RxBufferOffset  : rx-buffer offset                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_OPEN(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDDS_HANDLE hDDB,
        EDDS_UPPER_DPB_PTR_TO_CONST_TYPE pDPB,
        LSA_UINT32 TraceIdx,
        EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE pCaps)
{
    enum INTEL_DEVICE_ID deviceID;
    LSA_RESULT Status;
    LSA_UINT16 idx;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT, "IN :INTEL_LL_OPEN()");

    handle->params = *((INTEL_LL_PARAMETER_TYPE*) pDPB->pLLParameter);
    handle->hDDB = hDDB;
    handle->hSysDev = pDPB->hSysDev;

    EDDS_INTERNAL_STATISTICS_INIT(handle->hSysDev,handle->perf_as);
    EDDS_INTERNAL_STATISTICS_INIT(handle->hSysDev,handle->perf_rs);
    EDDS_INTERNAL_STATISTICS_INIT(handle->hSysDev,handle->perf_tas);
    EDDS_INTERNAL_STATISTICS_INIT(handle->hSysDev,handle->perf_trs);

    handle->TraceIdx = TraceIdx;
    handle->isLinkUp = LSA_FALSE;
    handle->lastIMS = 0;
    handle->savedIVAR = 0;

    handle->ReadStatisticsState = INTEL_READ_IN_STATISTICS1;
    handle->InUcastPktsShadow = 0;
    handle->OutUcastPktsShadow = 0;
    EDDS_MEMSET_LOCAL(&handle->Statistics,0,sizeof(handle->Statistics));

    for(idx = 0 ; idx < INTEL_STATS_INDEX_MAX ; idx++)
    {
        handle->StatsRegShadow[idx] = 0;
    }

    handle->ResetState = INTEL_RESET_STATE_IDLE;
    handle->isResetPending = LSA_FALSE;
    handle->ResetTargetSysTim = 0;

    handle->fakeSendCounter = 0;
    
    /* get caps */
    pCaps->HWFeatures = EDDS_LL_CAPS_HWF_NONE
            | EDDS_LL_CAPS_HWF_CRC_GENERATE
            | EDDS_LL_CAPS_HWF_EXACT_MAC_FILTER //lint !e835 JB 13/11/2014 define-based behaviour
            | EDDS_LL_CAPS_HWF_PHY_POWER_OFF
            | EDDS_LL_CAPS_HWF_CLUSTER_IP_SUPPORT;  /* ClusterIp can be supported for every hardware of this lower layer */
    pCaps->PortCnt = 1;
    pCaps->MaxFrameSize = 1536;
    
    deviceID = (enum INTEL_DEVICE_ID) (handle->params.DeviceID);

    switch (deviceID)
    {
        case I210:
            /* ok, supported */
            pCaps->HWFeatures |= EDDS_LL_CAPS_HWF_HSYNC_APPLICATION; /* i210 with SFP module can be used as Hsync app */
            Status = EDD_STS_OK;
            break;
        case I210SFP:
            if(handle->params.Disable1000MBitSupport)
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                "INTEL_LL_OPEN: 1GBit could not be disabled in SFP device");
                /* not supported with 1GBit disabled! */
                Status = EDD_STS_ERR_PARAM;
            }else
            {
                Status = EDD_STS_OK;
                pCaps->HWFeatures |= EDDS_LL_CAPS_HWF_HSYNC_APPLICATION; /* i210 with SFP module can be used as Hsync app */
            }
            break;            
        case IG82574L:
            /* ok, supported */
            Status = EDD_STS_OK;
            break;
        default:
            EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "INTEL_LL_OPEN: Invalid deviceID(%d).", deviceID);
            Status = EDD_STS_ERR_PARAM;
    }

    /* intel needs an receive framebuffer with minimum of 2048 byte
     * to receive a maximum frame with 1518+4+4 (frame with vlan tag and crc)
     */
    if (EDD_FRAME_BUFFER_LENGTH < sizeof(INTEL_RX_FRAMEBUFFER)) //lint !e774 !e506 JB 13/11/2014 defined based behaviour
    //fixme: compile time verification "plausability check!!!"
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "INTEL_LL_OPEN: ivalid Framebufferlength");
        return EDD_STS_ERR_ABORT;
    }

    if (EDD_STS_OK == Status)
    {
        /* only n*8 transmit/receive descriptors allowed. */
        handle->TxFrameBufferCount =
                (((LSA_UINT32) pDPB->TxBufferCnt) + INTEL_TDESC_ARRAY_GRANULARITY) & ~(((LSA_UINT32) INTEL_TDESC_ARRAY_GRANULARITY) - 1);
        handle->RxFrameBufferCount =
                (((LSA_UINT32) pDPB->RxBufferCnt) + INTEL_RDESC_ARRAY_GRANULARITY) & ~(((LSA_UINT32) INTEL_RDESC_ARRAY_GRANULARITY) - 1);

        /**@note intel descriptor rings need an extra "space" descriptor to work correctly!
         * pDPB->TxBufferCnt : allocate pDPB->TxBufferCnt+1 descriptors
         * pDPB->RxBufferCnt : allocate pDPB->RxBufferCnt+1 descriptors
         */
        if ((INTEL_MAX_TX_DESCRIPTORS < handle->TxFrameBufferCount) || (INTEL_MAX_RX_DESCRIPTORS < handle->RxFrameBufferCount))
        {
            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "INTEL_LL_OPEN: not enough TX or RX descriptor rings available.");
            Status = EDD_STS_ERR_PARAM;
        }
        else
        {
            LSA_UINT32 count;

            // alloc tx and rx descriptors
            // NOTE: Receive/Transmit Descriptor Base Address (xDBAL) must point to a ...
            //       I82574 --> 16-byte
            //       I210   --> 128-byte
            //       ... aligned block of data
            Status = intel_alloc_TxRxDescr(handle);

            if(EDD_STS_OK == Status)
            {
                for (count = 0; count < handle->TxFrameBufferCount; ++count)
                {
                    handle->pTxDescriptors[count].legacy.BufferAddress = 0;
                    handle->pTxDescriptors[count].legacy.Length = 0;
                    handle->pTxDescriptors[count].legacy.CSO = 0;
                    handle->pTxDescriptors[count].legacy.CSS = 0;
                    handle->pTxDescriptors[count].legacy.CMD = INTEL_BITI(5,0) /* DEXT = 0b (legacy TDESC type) */
                    | INTEL_BITI(3,1) | INTEL_BITI(1,1) | INTEL_BITI(0,1) /* RS + IFCS + EOP */
                    ;
                    handle->pTxDescriptors[count].legacy.STA_RSV = INTEL_BITI(0,1); /* clear status, but set DD */
                    /* (==ready for software, used by INTEL_LL_SEND!) */
                    handle->pTxDescriptors[count].legacy.Special = 0; /* clear special */
                }

                /**@todo    honor cache line size and alignment to reduce number of EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE calls!
                 */
                EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE(handle->hSysDev,handle->pTxDescriptors,handle->TxFrameBufferCount*sizeof(handle->pTxDescriptors[0]));

                /* allocate array of RX Framebuffer Pointers */
                EDDS_ALLOC_LOCAL_MEM(((LSA_VOID**)&handle->RxFrameBufferPtr),
                    sizeof(handle->RxFrameBufferPtr[0])*handle->RxFrameBufferCount);
                if (0 == handle->RxFrameBufferPtr)
                {
                    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                        "INTEL_LL_OPEN: could not allocate handle->RxFrameBufferPtr");

                    Status = intel_free_TxRxDescr(handle);
                    if( EDD_STS_OK != Status )
                    {
                        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                             "INTEL_LL_OPEN: could not free memory for tx and rx descriptors");
                    }

                    return EDD_STS_ERR_RESOURCE;
                }

                for (count = 0; count < handle->RxFrameBufferCount; ++count)
                {
                    EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(handle->hSysDev,
                            (LSA_VOID**) &handle->RxFrameBufferPtr[count],
                            sizeof(INTEL_RX_FRAMEBUFFER));
                    if(0 == handle->RxFrameBufferPtr[count])
                    {
                        LSA_UINT16 retval;
                        LSA_UINT32 clean_count;

                        EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                            "INTEL_LL_OPEN: could not allocate handle->RxFrameBufferPtr[%d]", count);

                        for(clean_count = count; clean_count > 0; --clean_count)
                        {
                            EDDS_FREE_RX_TRANSFER_BUFFER_MEM(handle->hSysDev,&retval,handle->RxFrameBufferPtr[clean_count-1]);
                            if (EDD_STS_OK != retval)
                            {
                                EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                    "INTEL_LL_OPEN: could not free memory for handle->RxFrameBufferPtr[%d]", clean_count-1);
                            }
                        }

                        EDDS_FREE_LOCAL_MEM(&retval, handle->RxFrameBufferPtr);
                        if (EDD_STS_OK != retval)
                        {
                            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                "INTEL_LL_OPEN: could not free memory for handle->RxFrameBufferPtr");
                        }

                        
                        Status = intel_free_TxRxDescr(handle);
                        if( EDD_STS_OK != Status )
                        {
                            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                                 "INTEL_LL_OPEN: could not free memory for tx and rx descriptors");
                        }
                        
                        return EDD_STS_ERR_RESOURCE;
                    }
                    EDDS_PTR_TO_ADDR64_RX(handle->hSysDev,
                                          handle->RxFrameBufferPtr[count],
                                          &handle->pRxDescriptors[count].BufferAddress);
                    handle->pRxDescriptors[count].Length = 0; /* clear received frame size */
                    /** @note buffer size is 2048 after reset (@see RCTL.BSIZE + RCTL.BSEX) */
                    handle->pRxDescriptors[count].packetChecksum = 0; /* clear checksum */
                    handle->pRxDescriptors[count].Status = 0; /* clear status */
                    handle->pRxDescriptors[count].Errors = 0; /* clear errors */
                    handle->pRxDescriptors[count].Special = 0; /* clear special */
                }

                /**@todo    honor cache line size and alignment to reduce number of EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE calls!
                 */
                EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE(handle->hSysDev,handle->pRxDescriptors,handle->RxFrameBufferCount*sizeof(handle->pRxDescriptors[0]));
            }
        }

    }

    EDDS_LL_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_CHAT, "OUT:INTEL_LL_OPEN()");

    return (Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SETUP                              +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_UPPER_DSB_PTR_TYPE       pDSB          +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pDSB       : Pointer to Device Setup Block                             +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_HW                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup LL Ethernetcontroller and start it.                 +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SETUP(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDDS_UPPER_DSB_PTR_TYPE pDSB,
        EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE pHwParam)

{
    LSA_RESULT Status;
    LSA_UINT32 c;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_SETUP()");

    /* initialize saved Link State variables */
    handle->setLinkSM = INTEL_SET_LINK_SM_IDLE;
    handle->regSetupSM = INTEL_REG_SETUP_SM_IDLE;
    handle->phySetupSM = INTEL_PHY_SETUP_SM_IDLE;
    handle->savedLinkStat = EDD_LINK_AUTONEG;
    handle->savedPHYPower = EDDS_PHY_POWER_OFF;
    handle->isLinkStateChanged = LSA_FALSE;
    handle->newLinkStat = EDD_LINK_UNCHANGED;
    handle->newPHYPower = EDDS_PHY_POWER_UNCHANGED;
    for(c = 0; c < EDDS_INTEL_SUPPORTED_PORTS; c++)
    {
    	handle->PortStatus[c] = EDD_PORT_PRESENT;
    }

    EDDS_MEMSET_LOCAL(&handle->phyRegs,0,sizeof(handle->phyRegs));

    intel_reset(handle, LSA_TRUE);
    // wait 3ms before accessing the register again
    EDDS_WAIT_US(3000);

    Status = intel_reset_finish(handle) ? EDD_STS_OK : EDD_STS_ERR_HW;

    if(EDD_STS_OK != Status)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "INTEL_LL_SETUP: could not finish reset.");
    }
    else
    {
        /* setup all port mac addresses */
        for (c = 0; c < EDDS_INTEL_SUPPORTED_PORTS; ++c)
        {
            if ((0 != pDSB->MACAddress[c + 1].MacAdr[0]) || (0 != pDSB->MACAddress[c + 1].MacAdr[1]) || (0 != pDSB->MACAddress[c + 1].MacAdr[2]) || (0 != pDSB->MACAddress[c + 1].MacAdr[3]) || (0 != pDSB->MACAddress[c + 1].MacAdr[4]) || (0 != pDSB->MACAddress[c + 1].MacAdr[5]))
            {
                handle->PortMAC[c] = pDSB->MACAddress[c + 1];
            }
            else /* write eeprom MAC address back to setup structure */
            {
                EDDS_MEMSET_UPPER(pDSB->MACAddress[c+1].MacAdr, 0,
                        sizeof(pDSB->MACAddress[c+1].MacAdr));
                handle->PortMAC[c] = pDSB->MACAddress[c + 1];
            }

            /* write PortMac address to return values in EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE */
            pHwParam->MACAddressPort[c] = handle->PortMAC[c];
        }

        for (c = 0; c < 16; ++c) /* first receive address is interface MAC */
        {
            handle->MCRegL[c] = intel_reg32(INTEL_RAL_0_15(c));
            handle->MCRegH[c] = intel_reg32(INTEL_RAH_0_15(c));
        }

        /* THis is done in intel_setup_registers:
         handle->TXDescriptorsUsed = 0;
         handle->RXDescriptorIndex = 0;
         handle->TXDescriptorIndex = 0;
         handle->TXDescriptorIndexEnd = 0;
         */
        /* make all RDESC available (one space descriptor left) */
        handle->RXDescriptorIndexEnd = handle->RxFrameBufferCount - 1;
        /* save LEDCTL for first time */
        handle->backupLEDCTL = intel_reg32(INTEL_LEDCTL);
        handle->savedLEDCTL = handle->backupLEDCTL;

        /* setup initial configured link speed mode */
        handle->LinkSpeedModeConfigured = EDD_LINK_AUTONEG;

        /* setup shadow status, mode and speed */
        handle->StatusShadow = EDD_LINK_DOWN;
        handle->ModeShadow   = EDD_LINK_UNKNOWN;
        handle->SpeedShadow  = EDD_LINK_UNKNOWN;

        handle->InterfaceMAC = pDSB->MACAddress[0];
        Status = intel_reset_phy(handle);
        if(EDD_STS_OK != Status)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "INTEL_LL_SETUP: could not reset phy.");
        }
        else
        {
            do
            {
                Status = intel_setup_registers_sm(handle);
            } while(EDD_STS_OK_PENDING == Status);
            if(EDD_STS_OK != Status)
            {
                EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "INTEL_LL_SETUP: could not setup registers.");
            }
            else
            {

                /* give Interface MAC address back for application in DSB */
                pDSB->MACAddress[0] = handle->InterfaceMAC;

                /* give Interface MAC address back for EDDS in return structure*/
                pHwParam->MACAddress = handle->InterfaceMAC;
            }
        }
    }
    if (EDD_STS_OK == Status)
    {
        /* get initial phy power status and configured link speed mode */
        Status = intel_get_startup_link_param(handle, pHwParam);
        if(EDD_STS_OK != Status)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "INTEL_LL_SETUP: could not get startup link param.");
        }
    }

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_SETUP()");

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SHUTDOWN                           +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE           pLLManagement   +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Shutdown LL Ethernetcontroller. Stops it                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SHUTDOWN(
        LSA_VOID_PTR_TYPE pLLManagement)
{
    LSA_UINT countdown;
    LSA_RESULT Status;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_SHUTDOWN()");

    Status = EDD_STS_OK;

    intel_reg32(INTEL_IMC) = 0xFFFFFFFF; /* disable all interrupts */

    INTEL_CLRBIT(intel_reg32(INTEL_TCTL), INTEL_TCTL_EN);
    /* disable transmit unit */
    INTEL_CLRBIT(intel_reg32(INTEL_RCTL), INTEL_RCTL_EN);
    /* disable receive unit */

    /* Acquire ownership of phy */
    countdown = 10;
    while((--countdown) && (!intel_try_acquire_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM)))
    {
        EDDS_WAIT_US(10);
    }
    if(!countdown)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "INTEL_LL_SHUTDOWN: could not aquire semaphore.");
        Status = EDD_STS_ERR_HW;
    }

    if ((EDD_STS_OK == Status)&&(I210SFP != handle->params.DeviceID))
    {
        /* set PHY power off (use shadow pctrl PHY register) */
        INTEL_SETBITI(handle->phyRegs.pctrl, 11);

        intel_beginWritePhyReg(handle,INTEL_PHY_PCTRL, handle->phyRegs.pctrl);
        countdown = 20;   /** @note Hartwell phy write needs up to 180us! */
        while((--countdown) && (! intel_endWritePhyReg(handle)))
        {
            EDDS_WAIT_US(10);
        }
        if(!countdown)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "INTEL_LL_SHUTDOWN: could not write PHY Register INTEL_PHY_PCTRL.");
            Status = EDD_STS_ERR_HW;
        }

        countdown = 10;
        /* Release ownership of phy, regardless of Status */
        while((--countdown) && (!intel_try_release_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM)))
        {
            EDDS_WAIT_US(10);
        }
        if(!countdown)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "INTEL_LL_SHUTDOWN: could not release semaphore.");
            Status = EDD_STS_ERR_HW;
        }
    }
    else if ((EDD_STS_OK == Status) && (I210SFP == handle->params.DeviceID))
    {
        LSA_UINT32 ctrl = intel_reg32(INTEL_CTRL);
        LSA_UINT32 pcs_cfg = intel_reg32(INTEL_PCS_CFG);

        INTEL_CLRBIT(ctrl, INTEL_CTRL_SLU);           /* Set LinkDown */
        INTEL_CLRBIT(pcs_cfg, INTEL_PCS_CFG_ENABLE);  /* disable transceiver - use this to force link down at the far end */

        intel_reg32(INTEL_CTRL) = ctrl;
        intel_reg32(INTEL_PCS_CFG) = pcs_cfg;
    }

    intel_reg32(INTEL_LEDCTL) = handle->backupLEDCTL; /* restore LEDCTL*/

    INTEL_CLRBIT(intel_reg32(INTEL_CTRL_EXT), INTEL_CTRL_EXT_DRV_LOAD);
    /* clear "driver loaded" indication bit */
    intel_reg32(INTEL_CTRL_EXT);
    /* force pci posted write to finish. */

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_SHUTDOWN()");

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_CLOSE                              +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE          pLLManagement    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes LL Ethernetcontroller. Stops it                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(550,retval)
//JB 05/12/2014 system adaption - fix ouput macro
//@fixme retval redundant, fix if possible #MA_REF
LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_CLOSE(LSA_VOID_PTR_TYPE pLLManagement)
{
    LSA_RESULT Status = EDD_STS_OK;
    LSA_UINT32 count;
    LSA_UINT16 retval;

    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_CLOSE()");

    for (count = 0; count < handle->RxFrameBufferCount; ++count)
    {
        INTEL_RX_FRAMEBUFFER* ptr = handle->RxFrameBufferPtr[count];

        if (0 != ptr)
        {
            EDDS_FREE_RX_TRANSFER_BUFFER_MEM(handle->hSysDev, &retval, ptr); //lint !e774 JB 09/12/2014 system adaption
            if(EDD_STS_OK != retval)
            {
                EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                    "INTEL_LL_CLOSE: could free memory for handle->RxFrameBufferPtr[%d]", count);
                Status = EDD_STS_ERR_RESOURCE;
            }
        }
    }

    if(EDD_STS_OK == Status)
    {
        EDDS_FREE_LOCAL_MEM(&retval, handle->RxFrameBufferPtr);
        if(EDD_STS_OK != retval)
        {
            EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "INTEL_LL_CLOSE: could free memory for handle->RxFrameBufferPtr");
            Status = EDD_STS_ERR_RESOURCE;
        }
        else
        {
            Status = intel_free_TxRxDescr(handle);
        }
    }
    EDDS_INTERNAL_STATISTICS_DEINIT(handle->hSysDev,handle->perf_as);
    EDDS_INTERNAL_STATISTICS_DEINIT(handle->hSysDev,handle->perf_rs);
    EDDS_INTERNAL_STATISTICS_DEINIT(handle->hSysDev,handle->perf_tas);
    EDDS_INTERNAL_STATISTICS_DEINIT(handle->hSysDev,handle->perf_trs);

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_CLOSE()");

    return (Status);

}
//lint +esym(550,retval)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :  INTEL_LL_RECV                                         +*/
/*+  Input/Output  :  LSA_VOID_PTR_TYPE                       pLLManagement +*/
/*+                   EDDS_UPPER_MEM_PTR EDDS_LOCAL_MEM_ATTR* pBufferAddr   +*/
/*+                   EDDS_LOCAL_MEM_U32_PTR_TYPE             pLength       +*/
/*+                   EDDS_LOCAL_MEM_U32_PTR_TYPE             pPortID       +*/
/*+  Result        :  LSA_RESULT                                            +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to pointer to buffer with Frame-netto-data       +*/
/*+                received if EDD_STS_OK.                                  +*/
/*+  pLength    :  Pointer to address with length of frame received.        +*/
/*+  pPortID    :  Pointer to address with PortID of frame received.        +*/
/*+                                                                         +*/
/*+  Result     :  EDD_STS_OK                                               +*/
/*+                EDD_STS_OK_NO_DATA                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions checks if a frame was received. If not it  +*/
/*+               returns EDD_STS_OK_NO_DATA. If a frame was received, the  +*/
/*+               functions returns pBufferAddr and pLength and sets        +*/
/*+               EDD_STS_OK or EDD_STS_ERR_RX. The buffer returned must    +*/
/*+               not be accessed anymore by the LLIF! The EDDS calls       +*/
/*+               INTEL_LL_RECV_PROVIDE() to provide a new receive buffer   +*/
/*+               to the LL. EDD_STS_ERR_RX is set, if the frame has a      +*/
/*+               error (e.g. wrong CRC). The EDDS discards this frame      +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 17/11/2014 params set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_RECV(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR *pBufferAddr,
        EDDS_LOCAL_MEM_U32_PTR_TYPE pLength,
        EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID)
{

    INTEL_RDESC* currentRD;
    LSA_UINT32 RxDescIdx;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;
    LSA_RESULT Status;

    EDDS_LL_TRACE_02(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_RECV(pBufferAddr: 0x%X, pLength: 0x%X)", pBufferAddr,
            pLength);


    // get current receive descriptor index, this descriptor contains received frame
    RxDescIdx = handle->RXDescriptorIndex;
    currentRD = &(handle->pRxDescriptors[RxDescIdx]);

    /**@todo	honor cache line size and alignment to reduce number of EDDS_DEV_MEM_DMA_SYNC_FROM_DEVICE calls!
     */
    EDDS_DEV_MEM_DMA_SYNC_FROM_DEVICE(handle->hSysDev,currentRD,sizeof(*currentRD));

    /* check DD for current receive dscr */
    if (intel_rdesc_done(currentRD))
    {
        Status = EDD_STS_OK;

        /* clear DD bit */
        INTEL_CLRBITI(currentRD->Status, INTEL_RDESC_STATUS_DD);

        /* set length of received frame and virtual buffer address */
        *pLength = currentRD->Length;
        *pPortID = 1;
        *pBufferAddr = handle->RxFrameBufferPtr[RxDescIdx];

        /* reset physical buffer address and length */
        currentRD->BufferAddress = 0;
        currentRD->Length = 0;

        /* set virtual buffer address to NULL */
        handle->RxFrameBufferPtr[RxDescIdx] = 0;

        /**@warning	to use EDDS_RX_MEM_DMA_SYNC_FROM_DEVICE,
         * 			the pointer "*pBufferAddr" must be cache line aligned to prevent destroying of other data!
         */
        EDDS_RX_MEM_DMA_SYNC_FROM_DEVICE(handle->hSysDev,*pBufferAddr,*pLength);

        // increase current receive descriptor index (regarding descriptor ring size)
        ++handle->RXDescriptorIndex;
        if (handle->RxFrameBufferCount <= handle->RXDescriptorIndex)
        {
            handle->RXDescriptorIndex = 0;
        }
    }
    else
    {
        Status = EDD_STS_OK_NO_DATA;
    }

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_RECV(), Status: 0x%X", Status);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    INTEL_LL_RECV_PROVIDE                               +*/
/*+  Input/Output  :    LSA_VOID_PTR_TYPE             pLLManagement         +*/
/*+                     EDD_UPPER_MEM_PTR_TYPE        pBufferAddr           +*/
/*+  Result        :    LSA_RESULT                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to receive buffer. The receive buffer is         +*/
/*+                EDD_FRAME_BUFFER_LENGTH in size.                         +*/
/*+                                                                         +*/
/*+  Result     :  EDD_STS_OK                                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions provides a new receive buffer to the LL.   +*/
/*+               This function is called after the LL returned a buffer    +*/
/*+               with INTEL_LL_RECV() to the EDDS.                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_RECV_PROVIDE(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE pBufferAddr)
{

    LSA_RESULT Status;
    INTEL_RDESC* lastRD;

    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_RECV_PROVIDE(pBufferAddr: 0x%X)", pBufferAddr);

    /* get tail receive descriptor; it identifies the location beyond the last descriptor            */
    /* hardware can progress; this is the location where software writes the first new descriptor    */
    /* NOTE: there is always one descriptor that is not usable by hardware or software, all other    */
    /*       descriptors belong to hardware; this is the descriptor where the tail index points to;  */
    /*       when a receiption is signaled a descriptor temporarily belongs to software              */
    /*       for buffer exchange; the tail descriptor is incremented, so that it always points to    */
    /*       the cleared descriptor from the previous call to LL_RECV; hardware would assume that    */
    /*       there is no free descriptor when head would be equal to tail --> this is not permitted; */
    ++handle->RXDescriptorIndexEnd;

    if (handle->RxFrameBufferCount <= handle->RXDescriptorIndexEnd)
    {
        handle->RXDescriptorIndexEnd = 0;
    }

    lastRD = &handle->pRxDescriptors[handle->RXDescriptorIndexEnd];
    handle->RxFrameBufferPtr[handle->RXDescriptorIndexEnd] = pBufferAddr;
    EDDS_PTR_TO_ADDR64_RX(handle->hSysDev, pBufferAddr,
            &lastRD->BufferAddress); /* set new ethernet frame buffer */
    EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE(handle->hSysDev,lastRD,sizeof(*lastRD));

    Status = EDD_STS_OK;

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_RECV_PROVIDE(), Status: 0x%X", Status);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    INTEL_LL_RECV_TRIGGER                               +*/
/*+  Input/Output  :    LSA_VOID_PTR_TYPE             pLLManagement         +*/
/*+                     EDD_UPPER_MEM_PTR_TYPE        pBufferAddr           +*/
/*+  Result        :    LSA_RESULT                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions (re-)triggers the EDDS hw to receive       +*/
/*+               frames. It's implemented to reduce register accesses      +*/
/*+               within the receive mechanism of the EDDS.                 +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_RECV_TRIGGER(
                LSA_VOID_PTR_TYPE pLLManagement)
{
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_RECV_TRIGGER()");
    
    /* (re-)trigger hardware receive: set end of (hw) descriptor list */
    intel_reg32(INTEL_RDT) = handle->RXDescriptorIndexEnd;
    
    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
                    "OUT:INTEL_LL_RECV_TRIGGER()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SEND                               +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE      pLLManagement        +*/
/*+                             EDDS_UPPER_MEM_PTR     pBufferAddr          +*/
/*+                             LSA_UINT32             Length               +*/
/*+                             LSA_UINT32             PortID               +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to buffer with Frame-netto-data to send          +*/
/*+                This address will be used and put to the controller!     +*/
/*+                (The data is not copied!) So the buffer must not be      +*/
/*+                accessed until it is released!                           +*/
/*+  Length     :  Number of Bytes pBuffer points to  (max. 1514/1518 Byte) +*/
/*+  PortID     :  PortID to send                                           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDDS_STS_ERR_RESOURCES                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Start sending a Frame. The frame (pBufferAddr) is given   +*/
/*+               to  the low-level function for further handling. Length   +*/
/*+               is the length of the frame. The frame has to include      +*/
/*+               mac-addresses, type/len, and data. FCS is added be low-   +*/
/*+               level function (normally be controller)                   +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE pBufferAddr,
        LSA_UINT32 Length,
        LSA_UINT32 PortID)

{
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;
    INTEL_TDESC* currentTD;
    LSA_RESULT Status;
    LSA_UINT32 tail;

    EDDS_LL_TRACE_03(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_SEND(pBufferAddr: 0x%X, Length: 0x%X, PortID: %d)",
            pBufferAddr, Length, PortID);

    /*-------------------------------------------------------------------------*/
    /* check for valid length                                                  */
    /*-------------------------------------------------------------------------*/

    if (Length > EDD_IEEE_FRAME_LENGTH || 1 < PortID)
        return (EDD_STS_ERR_PARAM);

    if ((handle->isLinkUp) && (!handle->isResetPending))
    {
        tail = handle->TXDescriptorIndexEnd;

        currentTD = &(handle->pTxDescriptors[tail]);

        /* only a maximum of handle->TxFrameBufferCount-1 frames may be queued at a time,
         * since intel descriptor rings need an "space" descriptor!
         */
        if (handle->TXDescriptorsUsed < (handle->TxFrameBufferCount - 1)) /* extra test; should always be true */
        {
            EDDS_PTR_TO_ADDR64_TX(handle->hSysDev, pBufferAddr,
                    &currentTD->legacy.BufferAddress);
            currentTD->legacy.Length = (LSA_UINT16) Length;
            INTEL_CLRBITI(currentTD->legacy.STA_RSV, 0);
            /* clear DD */
            ++handle->TXDescriptorsUsed;
            ++tail;
            if (handle->TxFrameBufferCount <= tail)
            {
                tail = 0;
            }

            /**@todo	honor cache line size and alignment to reduce number of EDDS_XXX_MEM_DMA_SYNC_TO_DEVICE calls!
             */
            EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE(handle->hSysDev,currentTD,sizeof(*currentTD));
            EDDS_TX_MEM_DMA_SYNC_TO_DEVICE(handle->hSysDev,pBufferAddr,Length);

            handle->TXDescriptorIndexEnd = tail;
            EDDS_LL_TRACE_03(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                    "INTEL_LL_SEND(0x%08X): BufferAddress: 0x%X, ++TXDescriptorsUsed: %d",
                    handle, currentTD->legacy.BufferAddress,
                    handle->TXDescriptorsUsed);

            Status = EDD_STS_OK;
        }
        else
        {
            EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "INTEL_LL_SEND: reached maximum number of desriptor rings(%d)", handle->TxFrameBufferCount - 1);
            Status = EDD_STS_ERR_RESOURCE;
        }

    }
    else
    {
        ++handle->fakeSendCounter;

        Status = EDD_STS_OK;
    }
    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_SEND(), Status: 0x%X", Status);

    return (Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SEND_TRIGGER                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE          pLLManagement    +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts transmission of frames applied by INTEL_LL_SEND.   +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND_TRIGGER(
        LSA_VOID_PTR_TYPE pLLManagement)
{
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_SEND_TRIGGER()");

    intel_reg32(INTEL_TDT) = handle->TXDescriptorIndexEnd;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_SEND()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SEND_STS                           +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE          pLLManagement    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK           : frame successul transm.  +*/
/*+                         EDD_STS_OK_NO_DATA   : no frame transmitted     +*/
/*+                         ... transmit errors                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for finished transmissions. If a transmission was  +*/
/*+               finished, EDD_STS_OK is returned. If no (more) finished   +*/
/*+               EDD_STS_OK_NO_DATA is returned.                           +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND_STS(
        LSA_VOID_PTR_TYPE pLLManagement)
{
    LSA_RESULT Status;
    INTEL_TDESC* lastTD;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_SEND_STS()");

    Status = EDD_STS_OK;

    if (0 != handle->TXDescriptorsUsed)
    {
        lastTD = &handle->pTxDescriptors[handle->TXDescriptorIndex];

        /**@warning	to use EDDS_DEV_MEM_DMA_SYNC_FROM_DEVICE,
         * 			the pointer "lastTD" must be cache line aligned to prevent destroying of other data.
         */
        EDDS_DEV_MEM_DMA_SYNC_FROM_DEVICE(handle->hSysDev,lastTD,sizeof(*lastTD));

        if (1 == INTEL_GETBITI(0,lastTD->legacy.STA_RSV) /* DD == 1; Descriptor Done */
        && (handle->TXDescriptorIndex != handle->TXDescriptorIndexEnd) && (0 < handle->TXDescriptorsUsed))
        {
            Status = EDD_STS_OK;

            ++handle->TXDescriptorIndex;
            --handle->TXDescriptorsUsed;
            if (handle->TxFrameBufferCount <= handle->TXDescriptorIndex)
            {
                handle->TXDescriptorIndex = 0;
            }
            EDDS_LL_TRACE_03(handle->TraceIdx, LSA_TRACE_LEVEL_NOTE,
                    "INTEL_LL_SEND_STS(0x%08X): pBufferAddr: 0x%X, --TXDescriptorsUsed: %d",
                    handle, lastTD->legacy.BufferAddress,
                    handle->TXDescriptorsUsed);

            lastTD->legacy.BufferAddress = 0;
            /**@todo  honor cache line size and alignment to reduce number of EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE calls!
             */
            EDDS_DEV_MEM_DMA_SYNC_TO_DEVICE(handle->hSysDev,lastTD,sizeof(*lastTD));
        }
        else
        {
            Status = EDD_STS_OK_NO_DATA;
        }

    }
    else if (0 != handle->fakeSendCounter)
    {
        --handle->fakeSendCounter;
        Status = EDD_STS_OK;
    }
    else
    {
        Status = EDD_STS_OK_NO_DATA;
    }

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_SEND_STS(), Status: 0x%X", Status);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   INTEL_LL_GET_STATS                           +*/
/*+  Input/Output          :   LSA_VOID_PTR_TYPE              pLLManagement +*/
/*+                            LSA_UINT32                     PortID        +*/
/*+                            EDDS_LOCAL_STATISTICS_PTR_TYPE pStats        +*/
/*+  Result                :   LSA_RESULT                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : PortID to read statistics for                             +*/
/*+               0: local interface                                        +*/
/*+               1..x: Port                                                +*/
/*+                                                                         +*/
/*+               Note: if only one port present, local interface is equal  +*/
/*+                     to the port statistic                               +*/
/*+                                                                         +*/
/*+  pStats     : Pointer to Statistics Structure to be filled              +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets statistics from Low-Level-functions                   */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_STATS(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_UINT32 PortID,
        EDDS_LOCAL_STATISTICS_PTR_TYPE pStats)
{
    LSA_RESULT Status;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_02(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_GET_STATS(PortID: 0x%X, pStats: 0x%X)", PortID,
            pStats);

    if ((1 < PortID) || (0 == pStats))
    {
        /* PortID not expected */
        EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "INTEL_LL_GET_STATS: Invalid PortID(%d)", PortID);
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        if (PortID == 0)
        {
            pStats->SupportedCounters = INTEL_MIB2_SUPPORTED_COUNTERS_IF;
        }
        else
        {
            pStats->SupportedCounters = INTEL_MIB2_SUPPORTED_COUNTERS_PORT;
        }
        if (PortID == 0)
        {
            pStats->InDiscards = 0;
            pStats->InErrors = 0;
        }
        else
        {
            pStats->InDiscards = handle->Statistics.InDiscards;
            pStats->InErrors = handle->Statistics.InErrors;
        }
        pStats->InUnknownProtos = 0;
        pStats->InNUcastPkts    = handle->Statistics.InNUcastPkts;
        pStats->InUcastPkts     = handle->Statistics.InUcastPkts;
        pStats->InOctets        = handle->Statistics.InOctets;
        pStats->OutDiscards     = 0;
        if (PortID == 0)
        {
            pStats->OutErrors = 0;
        }
        else
        {
            pStats->OutErrors = handle->Statistics.OutErrors;
        }
        pStats->OutQLen         = 0;
        pStats->OutNUcastPkts   = handle->Statistics.OutNUcastPkts;
        pStats->OutUcastPkts    = handle->Statistics.OutUcastPkts;
        pStats->OutOctets       = handle->Statistics.OutOctets;
        /* RFC2863_COUNTER */
        pStats->InMulticastPkts = handle->Statistics.InMulticastPkts;
        pStats->InBroadcastPkts = handle->Statistics.InBroadcastPkts;
        pStats->OutMulticastPkts = handle->Statistics.OutMulticastPkts;
        pStats->OutBroadcastPkts = handle->Statistics.OutBroadcastPkts;
        pStats->InHCOctets      = handle->Statistics.InHCOctets;
        pStats->InHCUcastPkts   = handle->Statistics.InHCUcastPkts;
        pStats->InHCMulticastPkts = handle->Statistics.InHCMulticastPkts;
        pStats->InHCBroadcastPkts = handle->Statistics.InHCBroadcastPkts;
        pStats->OutHCOctets     = handle->Statistics.OutHCOctets;
        pStats->OutHCUcastPkts  = handle->Statistics.OutHCUcastPkts;
        pStats->OutHCMulticastPkts = handle->Statistics.OutHCMulticastPkts;
        pStats->OutHCBroadcastPkts = handle->Statistics.OutHCBroadcastPkts;
        /* RFC2863_COUNTER - End */

        Status = EDD_STS_OK;
    }
    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_GET_STATS(), Status: 0x%X", Status);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+ Functionname: INTEL_LL_GET_LINK_STATE                                   +*/
/*+ Input/Output: LSA_VOID_PTR_TYPE                  pLLManagement          +*/
/*+               EDD_UPPER_GET_LINK_STATUS_PTR_TYPE pLinkStat,             +*/
/*+               EDDS_LOCAL_MEM_U16_PTR_TYPE        pMAUType,              +*/
/*+               EDDS_LOCAL_MEM_U8_PTR_TYPE         pMediaType,            +*/
/*+               EDDS_LOCAL_MEM_U8_PTR_TYPE         pIsPOF,                +*/
/*+               EDDS_LOCAL_MEM_U32_PTR_TYPE        pPortStatus,           +*/
/*+               EDDS_LOCAL_MEM_U32_PTR_TYPE        pAutonegCapAdvertised  +*/
/*+               EDDS_LOCAL_MEM_U8_PTR_TYPE         pLinkSpeedModeConfigured+*/
/*+                                                                         +*/
/*+ Result      : LSA_RESULT                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to get link status from  (1..x)                      +*/
/*+  pLinkStat  : Pointer to LinkStatus to be filled                        +*/
/*+                                                                         +*/
/*+  pMAUType   : Pointer to address for actual MAUType                     +*/
/*+               EDD_MAUTYPE_UNKNOWN (=0) if unknown. Else IEC MAUType     +*/
/*+                                                                         +*/
/*+  pMediaType : Pointer to address for actual MediaType                   +*/
/*+               EDD_MEDIATYPE_UNKNOWN (=0) if unknown. Else IEC MediaType +*/
/*+                                                                         +*/
/*+  pIsPOF     : is it a real POF port                                     +*/
/*+                                                                         +*/
/*+  pPortStatus: Pointer to address for actual PortStatus (see edd_usr.h)  +*/
/*+                                                                         +*/
/*+  pAutonegCapAdvertised:    Pointer to address for actual autoneg        +*/
/*+                            advertised capabilities.                     +*/
/*+                            Note: 0 if no autoneg supported.             +*/
/*+                            EDDS_AUTONEG_CAP_xxx see edd_usr.h           +*/
/*+                                                                         +*/
/*+  pLinkSpeedModeConfigured: Configured LinkSpeedMode, needed on handling +*/
/*+                            for FO ports.                                +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+             :           EDD_STS_ERR_PARAM                               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets LinkStatus from Low-Level-functions                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//JB 17/11/2014 params will be set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_LINK_STATE(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_UINT32 PortID,
        EDD_UPPER_GET_LINK_STATUS_PTR_TYPE pLinkStat,
        EDDS_LOCAL_MEM_U16_PTR_TYPE pMAUType,
        EDDS_LOCAL_MEM_U8_PTR_TYPE pMediaType,
        EDDS_LOCAL_MEM_U8_PTR_TYPE pIsPOF,
        EDDS_LOCAL_MEM_U32_PTR_TYPE pPortStatus,
        EDDS_LOCAL_MEM_U32_PTR_TYPE pAutonegCapAdvertised,
        EDDS_LOCAL_MEM_U8_PTR_TYPE pLinkSpeedModeConfigured)
{
    LSA_RESULT Status = EDD_STS_OK;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_GET_LINK_STATE(PortID: %d)", PortID);

    if (1 != PortID)
    {
        EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "INTEL_LL_GET_STATS: Invalid PortID(%d)", PortID);
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        *pAutonegCapAdvertised    = handle->AutoNegCapAdvertised;    /* AutoNegCapAdvertised is set on PHY reset*/
        *pLinkSpeedModeConfigured = handle->LinkSpeedModeConfigured; /* configured LinkSpeedMode */
        *pMAUType                 = EDD_MAUTYPE_UNKNOWN;             /* MAUType determined on EDDS_GET_PHY_PARAMS */
        
        /* we always know the media type... */
        if(I210SFP == handle->params.DeviceID)
        {
            *pMediaType               = EDD_MEDIATYPE_FIBER_OPTIC_CABLE;
        }else
        {
            *pMediaType               = EDD_MEDIATYPE_COPPER_CABLE;
        }
        *pIsPOF                   = EDD_PORT_OPTICALTYPE_ISNONPOF;                               /* IsPOF determined on EDDS_GET_PHY_PARAMS */
        *pPortStatus              = handle->PortStatus[PortID-1];    /* PortStatus is set by Switch_change_port_state */
        pLinkStat->PortID         = (LSA_UINT16) PortID;             /* set PortID */

        /* those values are determined on INTEL_LL_RECURRING_TASK */
        /* whenever the link status is changing */
        pLinkStat->Status         = handle->StatusShadow;
        pLinkStat->Mode           = handle->ModeShadow;
        pLinkStat->Speed          = handle->SpeedShadow;

        /* retrieve MAUType and MediaType */
        EDDS_GET_PHY_PARAMS(handle->hSysDev, handle->hDDB, PortID,
                pLinkStat->Speed, pLinkStat->Mode, pMAUType, pMediaType, pIsPOF);
    }

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_GET_LINK_STATE(), Status: 0x%X", Status);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SET_LINK_STATE                     +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                    PortID        +*/
/*+                             LSA_UINT8                     LinkStat      +*/
/*+                             LSA_UINT8                     PHYPower      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set Link for                                      +*/
/*+  pLinkStat  : LinkStatus to be setup       [IN/OUT]                     +*/
/*+               EDD_LINK_AUTONEG                                          +*/
/*+               EDD_LINK_100MBIT_HALF                                     +*/
/*+               EDD_LINK_100MBIT_FULL                                     +*/
/*+               EDD_LINK_10MBIT_HALF                                      +*/
/*+               EDD_LINK_10MBIT_FULL                                      +*/
/*+               EDD_LINK_UNCHANGED     (do not change LinkStat)           +*/
/*+  pPHYPower  : Power of PHY                 [IN/OUT]                     +*/
/*+               EDDS_PHY_POWER_ON                                         +*/
/*+               EDDS_PHY_POWER_OFF                                        +*/
/*+               EDDS_PHY_POWER_UNCHANGED(do not change power)             +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set Link to given values (only if hardware is setup)      +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+               The actual LinkState and PHYPower is returned.            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SET_LINK_STATE(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_UINT32 PortID,
        LSA_UINT8 * const pLinkStat,
        LSA_UINT8 * const pPHYPower)
{
    LSA_RESULT Status = EDD_STS_OK;
    LSA_UINT8 LinkStat;
    LSA_UINT8 PHYPower;
    LSA_BOOL Disabled1000MBitSupport;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    LinkStat = *pLinkStat;
    PHYPower = *pPHYPower;

    EDDS_LL_TRACE_03(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
        "IN :INTEL_LL_SET_LINK_STATE(PortID: %d, LinkStat: 0x%X, Power: 0x%X)",
        PortID, LinkStat, PHYPower);

    Disabled1000MBitSupport =
            handle->params.Disable1000MBitSupport && (EDD_LINK_1GBIT_FULL == LinkStat || EDD_LINK_1GBIT_HALF == LinkStat);

    if ( ((1 != PortID) || Disabled1000MBitSupport)
            || (
                    (EDD_LINK_UNCHANGED != LinkStat) &&
                    /* SFP supports only Autoneg (or explicit set 1000MbitFD) */
                    (
                            (I210SFP == handle->params.DeviceID)&&
                            (EDD_LINK_1GBIT_FULL != LinkStat)&&
                            (EDD_LINK_AUTONEG != LinkStat)
                    )
               )
       )
    {
        EDDS_LL_TRACE_03(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "IN :INTEL_LL_SET_LINK_STATE(PortID: %d, LinkStat: 0x%X, Power: 0x%X) Disabled1000MBitSupport or PortID != 1",
            PortID, LinkStat, PHYPower);
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        /* we should support only 100MBitFD, Autoneg and Unchanged */
        if( (EDD_LINK_100MBIT_FULL == LinkStat)
          ||(EDD_LINK_AUTONEG == LinkStat)
          ||(EDD_LINK_UNCHANGED == LinkStat)
          )
        {
            /* save link parameters to be set, see LL_RECURRING_TASK */
            handle->newLinkStat = LinkStat;
            handle->newPHYPower = PHYPower;
            handle->isLinkStateChanged = LSA_TRUE;
        }
        else
        {
            EDDS_LL_TRACE_03(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                "IN :INTEL_LL_SET_LINK_STATE(PortID: %d, LinkStat: 0x%X, Power: 0x%X) LinkStat not supported",
                PortID, LinkStat, PHYPower);
            Status = EDD_STS_ERR_PARAM;
        }

        if(EDD_LINK_UNCHANGED == LinkStat)
        {
            *pLinkStat = handle->savedLinkStat;
        }

        if(EDDS_PHY_POWER_UNCHANGED == PHYPower)
        {
            *pPHYPower = handle->savedPHYPower;
        }

    }

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_SET_LINK_STATE(), Status: 0x%X", Status);

    return (Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname     :  INTEL_LL_MC_ENABLE                                 +*/
/*+  Input/Output     :  LSA_VOID_PTR_TYPE                    pLLManagement +*/
/*+                      EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR* pMCAddr       +*/
/*+  Result           :  LSA_RESULT                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pMCAddr    : Pointer to 6 Byte Multicastaddress to Enable              +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE      : no more free MC entrys        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Enables Multicastaddress                                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_MC_ENABLE(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr)
{
    LSA_RESULT Status = EDD_STS_OK;
    LSA_UINT i;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_MC_ENABLE(pMCAddr: 0x%X)", pMCAddr);

    for (i = EDDS_INTEL_MAC_TABLE_START_OFFSET_MC; i < 16; ++i) /* first receive addresses are interface and port MAC, skip it */
    {
        LSA_UINT32 tmp;

        tmp = handle->MCRegH[i];
        if (0 == INTEL_GETBIT(INTEL_RAH_AV,tmp)) /* extra port MAC address not enabled*/
        {
            handle->MCRegL[i] =
                    (((LSA_UINT32) pMCAddr->MacAdr[3]) << 24) | (((LSA_UINT32) pMCAddr->MacAdr[2]) << 16) | (((LSA_UINT32) pMCAddr->MacAdr[1]) << 8) | (((LSA_UINT32) pMCAddr->MacAdr[0])/*<<0*/);
            handle->MCRegH[i] =
                    INTEL_BIT(INTEL_RAH_AV,1) | /* enable MAC address */
                    (((LSA_UINT32) pMCAddr->MacAdr[5]) << 8) | (((LSA_UINT32) pMCAddr->MacAdr[4])/*<<0*/);

            intel_reg32(INTEL_RAL_0_15(i)) = handle->MCRegL[i];
            intel_reg32(INTEL_RAH_0_15(i)) = handle->MCRegH[i];
            break;
        }

    }
    if(16 == i)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "INTEL_LL_MC_ENABLE: no empty entry found in MAC Table.");
        Status = EDD_STS_ERR_RESOURCE;
    }

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_MC_ENABLE(), Status: 0x%X", Status);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname     :  INTEL_LL_MC_DISABLE                                +*/
/*+  Input/Output     :  LSA_VOID_PTR_TYPE                    pLLManagement +*/
/*+                      LSA_BOOL                             DisableAll    +*/
/*+                      EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR* pMCAddr       +*/
/*+  Result           :  LSA_RESULT                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  DisableAll  :LSA_TRUE: frees all Multicast addresses. pMCAddr          +*/
/*+                         not used.                                       +*/
/*+               LSA_FALSE:free pMACAddr only                              +*/
/*+  pMCAddr    : Pointer to 6 Byte Multicastaddress to Disable if          +*/
/*+               DisableAll = LSA_FALSE                                    +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Disables Multicastaddress                                 +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_MC_DISABLE(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_BOOL DisableAll,
        EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr)
{
    LSA_RESULT Status = EDD_STS_OK;
    LSA_UINT32 i;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_02(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_MC_DISABLE(DisableAll: 0x%X, pMCAddr: 0x%X)",
            DisableAll, pMCAddr);

    for (i = EDDS_INTEL_MAC_TABLE_START_OFFSET_MC; i < 16; ++i) /* first receive addresses is interface and port MAC, skip it */
    {
        LSA_UINT32 tmp;

        tmp = handle->MCRegH[i];
        if (!DisableAll)
        {
            /* check if last 2 byte are the same and MAC address is enabled */
            if ((0x8000FFFF & tmp) == (INTEL_BIT(INTEL_RAH_AV,1) | /* enable MAC address */
            (((LSA_UINT32) pMCAddr->MacAdr[5]) << 8) | (((LSA_UINT32) pMCAddr->MacAdr[4])/*<<0*/)))
            {
                LSA_UINT32 tmp2 = handle->MCRegL[i];
                /* check if first 4 byte are the same */
                if (tmp2 == ((((LSA_UINT32) pMCAddr->MacAdr[3]) << 24) | (((LSA_UINT32) pMCAddr->MacAdr[2]) << 16) | (((LSA_UINT32) pMCAddr->MacAdr[1]) << 8) | (((LSA_UINT32) pMCAddr->MacAdr[0])/*<<0*/)))
                {
                    /* disable MAC address */
                    handle->MCRegH[i] = 0;
                    handle->MCRegL[i] = 0;
                    intel_reg32(INTEL_RAH_0_15(i)) = handle->MCRegH[i];
                    intel_reg32(INTEL_RAL_0_15(i)) = handle->MCRegL[i];
                    break;
                }
            }
        }
        else /* disable all*/
        {
            handle->MCRegH[i] = 0;
            handle->MCRegL[i] = 0;
            intel_reg32(INTEL_RAH_0_15(i)) = handle->MCRegH[i];
            intel_reg32(INTEL_RAL_0_15(i)) = handle->MCRegL[i];
        }
    }
    if( (16 == i) && (!DisableAll) )
    {
        EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "INTEL_LL_MC_DISABLE: no entry found in MAC Table.");
        Status = EDD_STS_ERR_RESOURCE;
    }

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_MC_DISABLE(), Status: 0x%X", Status);

    return (Status);

}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_LED_BACKUP_MODE                    +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Backup "Show Location" LED(s) context in internal         +*/
/*+               management structure.                                     +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_BACKUP_MODE(
        LSA_VOID_PTR_TYPE pLLManagement)
{
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_LED_BACKUP_MODE()");

	if (!handle->isResetPending)
	{
	    handle->backupLEDCTL = intel_reg32(INTEL_LEDCTL);
	}
	else
	{
	    handle->backupLEDCTL = handle->savedLEDCTL;
	}

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_LED_BACKUP_MODE()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_LED_RESTORE_MODE                   +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: RESTORE "Show Location" LED(s) context from internal      +*/
/*+               management structure.                                     +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_RESTORE_MODE(
        LSA_VOID_PTR_TYPE pLLManagement)
{
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_LED_RESTORE_MODE()");

	if (!handle->isResetPending)
	{
	    intel_reg32(INTEL_LEDCTL) = handle->backupLEDCTL;
	}
	else
	{
	    handle->savedLEDCTL = handle->backupLEDCTL;
	}

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_LED_RESTORE_MODE()");

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_LED_SET_MODE                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_BOOL                      LEDOn         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  LEDOn      : Mode of LED to set (LSA_TRUE/LSA_FALSE)                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets "Show Location" LED(s)	ON or OFF 										+*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_SET_MODE(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_BOOL LEDOn)
{
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_LED_SET_MODE() MODE: %d", LEDOn);

	handle->savedLEDCTL =
	        (!LEDOn) ? INTEL_LEDCTL_DEFAULT_LEDS_OFF :
					INTEL_LEDCTL_DEFAULT_LEDS_ON;
	if (!handle->isResetPending)
	{
	    intel_reg32(INTEL_LEDCTL) = handle->savedLEDCTL;
	}

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_LED_SET_MODE()");

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SWITCH_CHANGE_PORT                 +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT16                    PortID        +*/
/*+                             LSA_UINT8                     isPulled      +*/
/*+  Result                :    LSA_UINT32                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement : Pointer to LLIF specific management structure          +*/
/*+  PortID        : Port to set state for                                  +*/
/*+  isPulled      : If EDD_PORTMODULE_IS_PULLED set state to pulled        +*/
/*+                  If EDD_PORTMODULE_IS_PLUGGED set state to plugged      +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:                                                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SWITCH_CHANGE_PORT(
        LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT16 PortID, LSA_UINT8 isPulled)
{
	INTEL_LL_HANDLE_TYPE * handle;
    LSA_RESULT Status;

	handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;
	Status = EDD_STS_OK;

    EDDS_LL_TRACE_02(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_SWITCH_CHANGE_PORT_STATE() isPulled(%d) for PortID(%d)", isPulled, PortID);

    if(PortID < 1 || PortID > EDDS_INTEL_SUPPORTED_PORTS)
    {
        EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
            "INTEL_LL_SWITCH_CHANGE_PORT_STATE: invalid PortID(%d)", PortID);
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        if(EDD_PORTMODULE_IS_PULLED == isPulled)
        {
            handle->PortStatus[PortID-1] = EDD_PORT_NOT_PRESENT;
        }
        else
        {
            handle->PortStatus[PortID-1] = EDD_PORT_PRESENT;
        }
    }

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_SWITCH_CHANGE_PORT_STATE()");

    return Status;
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_RECURRING_TASK                     +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_BOOL                      hwTimeSlicing +*/
/*+  Result                :    LSA_UINT32                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+  Result     : EDDS_LL_RECURRING_TASK_NOTHING or                         +*/
/*+               different EDDS_LL_RECURRING_TASK_* values as a bitmap     +*/
/*+                                                                         +*/
/*+  pLLManagement : Pointer to LLIF specific management structure          +*/
/*+  hwTimeSclicing: If LSA_TRUE, LLIF should timeslice hardware accesses   +*/
/*+                  in this function (function is called cyclically, with  +*/
/*+                  a critical high frequency).                            +*/
/*+                  If LSA_FALSE, all recurring task hw accesses should be +*/
/*+                  done in one action (function is called cyclically,     +*/
/*+                  but with a lower, much less critically frequency).     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Recurring tasks of the LLIF can be processed in this      +*/
/*+               function. It returns a resultBitmap to give EDDS a hint   +*/
/*+               what was done (e.g. update of statistic counter, link has +*/
/*+               changed or a chip reset in progress).                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT32 EDDS_LOCAL_FCT_ATTR INTEL_LL_RECURRING_TASK(
        LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL hwTimeSlicing)
{
    LSA_UINT32 resultBitmap = EDDS_LL_RECURRING_TASK_NOTHING;

    INTEL_LL_HANDLE_TYPE* handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_RECURRING_TASK()");


    /* \note handle->isResetPending may be set/cleared in each state machine! */
    if(!handle->isResetPending)
    {
        if(intel_link_check_and_set_state_machine(handle,hwTimeSlicing))
        {
            resultBitmap |= EDDS_LL_RECURRING_TASK_LINK_CHANGED;
        }
    }

    /* \note handle->isResetPending may be set/cleared in each state machine! */
    /* trigger reset state machine */
    /* isResetPending is set to false after reset is done, see intel_link_reset_state_machine */
    if (handle->isResetPending)
    {
        resultBitmap |= EDDS_LL_RECURRING_TASK_CHIP_RESET;
        intel_link_reset_state_machine(handle,hwTimeSlicing);
    }

    /* \note handle->isResetPending may be set/cleared in each state machine! */
    /* trigger statistics state machine */
    /* only when no reset is currently running */
    if (!handle->isResetPending)
    {
        resultBitmap |= EDDS_LL_RECURRING_TASK_STATISTIC_COUNTER;
        intel_read_statistics_state_machine(handle,hwTimeSlicing);
    }

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_RECURRING_TASK(): resultBitmap == %08X",resultBitmap);
    return resultBitmap;
}

/** @} ***********************************************************************/
/*  end of file intel_edd.c                                                  */
/*****************************************************************************/

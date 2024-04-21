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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_IOBaseUser.cpp                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PnDriver user interface-                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1120
#define PND_MODULE_ID	    1120

#include "pnd_IOBaseCoreCommon.h"
#include "pnd_iodu.h"
#include "pnd_Controller.h"
#include "pnd_IOInterface.h"
#include "pnd_IOSystemReconfig.h"
#include "pnd_Adapter.h"
#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"

#include "pniobase.h"
#include "pniousrx.h"
/* StdLib++ */
#include <vector>

using namespace std;


/*===========================================================================
* FUNCTION : PNIO_controller_open
*----------------------------------------------------------------------------
* PURPOSE  : With this function an application program registers
*           itself with the PROFInet IO as IO controller
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else *
*----------------------------------------------------------------------------
* INPUTS   : CpIndex - Unique identification for the communication module
*           		  (module index in the component configuration)
*           ExtPar - Extended parameter (see earlier definition),
*           		 among others the following parameters can be
*           		 applied via the OR logical operator:PNIO_MODE_CTRL
*           cbf_RecReadConf - Callback function for signaling the results
							   of data set read tasks
*           cbf_RecWriteConf - Callback function for signaling the results
*           				   of data set write tasks
*           cbf_AlarmInd - Callback function for signaling alarms
* OUTPUS   : pApplHandle - Handle, which is assigned to the controller.
*----------------------------------------------------------------------------
* COMMENTS : With this function an application program registers itself
*           with the PROFInet IO as IO Controller. Additionally the
*           task-specific callback functions relevant to the IO controller
*           are also registered. It is possible, to set the same callback
*           function for all call back events. The function pointers must
*           not be NULL, except for Alarm (cbf_AlarmInd)
*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR
PNIO_controller_open(PNIO_UINT32 CpIndex,
	PNIO_UINT32 ExtPar,
	PNIO_CBF cbf_RecReadConf,
	PNIO_CBF cbf_RecWriteConf,
	PNIO_CBF cbf_AlarmInd,
	PNIO_UINT32 * pApplHandle)
{
	PNIO_UINT32 Ret = PNIO_OK;

    CController *pThis = 0;

    if(!pApplHandle)
        return PNIO_ERR_PRM_HND;

    CpIndex--; // Internally CpIndex Starts with 0

    Ret = CController::get_handle_by_index(CpIndex);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_PRM_CP_ID;

    pThis = CController::get_instance(CpIndex);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_SEQUENCE;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_controller_open CpIndex %d", CpIndex);

	PND_IOBC_TRACE_06(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CpIndex= %d, ExtPar=%d cbf_RecReadConf=%d cbf_RecWriteConf=%d cbf_AlarmInd=%d ApplHandle = %d",
		CpIndex, ExtPar, cbf_RecReadConf, cbf_RecWriteConf, cbf_AlarmInd, pApplHandle);

	Ret = CController::controller_open(CpIndex, ExtPar,
		cbf_RecReadConf, cbf_RecWriteConf, cbf_AlarmInd, pApplHandle);

	PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "PNIO_controller_open ret= %d, ApplHandle= %d" ,Ret, pApplHandle);

	PND_IOB_USR_EXIT

	return Ret;
}

/*===========================================================================
* FUNCTION : PNIO_controller_close
*----------------------------------------------------------------------------
* PURPOSE  : close Controller from handle
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_WRONG_HND, PNIO_ERR_INTERNAL
*----------------------------------------------------------------------------
* INPUTS   : - ApplHandle -  Handle Handle from PNIO_controller_open
* OUTPUS   : -
*----------------------------------------------------------------------------
* COMMENTS : With this function the application program deregisters
* an IO-Controller with PROFInet, which had been previously registered with
* PNIO_controller_open
*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR PNIO_controller_close(PNIO_UINT32 ApplHandle)
{
	PNIO_UINT32 Ret = PNIO_OK;

    CController *pThis = 0;

    Ret = CController::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND;

    pThis = CController::get_instance(ApplHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_WRONG_HND;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_controller_close");

	Ret = CController::controller_close(ApplHandle);

	PND_IOB_USR_EXIT

	return Ret;
}



/*===========================================================================
* FUNCTION : PNIO_set_mode
*----------------------------------------------------------------------------
* PURPOSE  : set new mode for Controller from handle
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_WRONG_HND, PNIO_ERR_INTERNAL
*----------------------------------------------------------------------------
* INPUTS   : - ApplHandle -  Handle of controler
*           - Mode - new mode
* OUTPUS   :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR PNIO_set_mode(PNIO_UINT32 ApplHandle,
	PNIO_MODE_TYPE Mode)
{
	PNIO_UINT32 Ret;

    CController *pThis = 0;

    Ret = CController::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND;

    pThis = CController::get_instance(ApplHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_SEQUENCE;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_set_mode");

	PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "ApplHandle= %d, Mode= %d",ApplHandle, Mode);

    if(pThis) 
    {
        Ret = pThis->set_mode(Mode);
	} else {
		Ret = PNIO_ERR_WRONG_HND;
	}

	PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_set_mode, ret= %d" ,Ret);

	PND_IOB_USR_EXIT

	return Ret;
}

/*===========================================================================
* FUNCTION : PNIO_ctrl_diag_req
*----------------------------------------------------------------------------
* PURPOSE  : 
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_WRONG_HND, PNIO_ERR_SEQUENCE
*----------------------------------------------------------------------------
* INPUTS   : - ApplHandle -  Handle of controller
*            - pDiagReq - Pointer of Diagnostic Request
* OUTPUS   :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR PNIO_ctrl_diag_req(
	PNIO_UINT32    ApplHandle,
	PNIO_CTRL_DIAG * pDiagReq)
{
	PNIO_UINT32 Ret;

	CController *pThis = 0;

	Ret = CController::get_handle_by_index(ApplHandle);
	if (Ret == PND_INVALID_HANDLE)
		return PNIO_ERR_WRONG_HND;

	pThis = CController::get_instance(ApplHandle);
	if (pThis == PNIO_NULL)
		return PNIO_ERR_SEQUENCE;


	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_ctrl_diag_req");

	PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "ApplHandle= %d",ApplHandle);

		if(pThis)
		{
		Ret = (PNIO_UINT32)((CController *)pThis)->ctrl_diag_request( (LSA_HANDLE_TYPE) ApplHandle, pDiagReq);
		}
		else
		{
			Ret = PNIO_ERR_WRONG_HND;
		}

	PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_ctrl_diag_req, ret= %d" ,Ret);

	PND_IOB_USR_EXIT

	return Ret;
}

/*===========================================================================
* FUNCTION : PNIO_iosystem_reconfig
*----------------------------------------------------------------------------
* PURPOSE  : 
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success
*----------------------------------------------------------------------------
* INPUTS   : - ApplHandle -  Handle of controller
*            - Step - Indicates the step that should be executed.
*                     PNIO_IOS_RECONFIG_MODE_DEACT: Deactivate all IODevices
*                     PNIO_IOS_RECONFIG_MODE_TAILOR: Start Tailoring and activate appropriate IODevices
*            - DeviceCnt - Array of logical addresses to IODevices which should be tailored.
*                          A listed IODevice has to be an optional IODevice. 
*            - DeviceList  - Number of entries in the DeviceList
*            - PortInterconnectionCnt - Array of a set of two logical addresses 
*                                       which lists each port interconnection 
*                                       as a set of two logical addresses.
*            - PortInterconnectionList - Number of entries in the PortInterconnectionList
* OUTPUS   :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
/*lint -e{818} Pointer parameter 'DeviceList' could be declared as pointing to const*/
PNIO_UINT32 PNIO_CODE_ATTR PNIO_iosystem_reconfig(
        PNIO_UINT32               ApplHandle,                 /* in */
        PNIO_IOS_RECONFIG_MODE    Mode,                       /* in */
		PNIO_UINT32               DeviceCnt,                  /* in */
		PNIO_ADDR*                DeviceList,                 /* in */  
		PNIO_UINT32               PortInterconnectionCnt,     /* in */
		PNIO_ADDR*                PortInterconnectionList)
{
	PNIO_UINT32 Ret;

	CController *pThis = 0;

	Ret = CController::get_handle_by_index(ApplHandle);
	if (Ret == PND_INVALID_HANDLE)
		return PNIO_ERR_WRONG_HND;

	pThis = CController::get_instance(ApplHandle);
	if (pThis == PNIO_NULL)
		return PNIO_ERR_SEQUENCE;


	PND_IOB_USR_ENTER

		PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_iosystem_reconfig");

		PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "ApplHandle= %d, Mode= %d", ApplHandle, Mode);

		if(pThis)
		{

            Ret = (PNIO_UINT32)((CController *)pThis)->iosystem_reconfig(ApplHandle, new CIOSystemReconfig(Mode, DeviceCnt, DeviceList, 
                                                                                                           PortInterconnectionCnt, PortInterconnectionList,
                                                                                                           new CPndAdapter()));

		}
		else
		{
			Ret = PNIO_ERR_WRONG_HND;
		}

		PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_iosystem_reconfig, ret= %d" ,Ret);

	PND_IOB_USR_EXIT

	return Ret;
}

/*===========================================================================
* FUNCTION : PNIO_data_read
*----------------------------------------------------------------------------
* PURPOSE  : With this functions the IO data is read.
*           The caller of the function is to make available
*           the data buffer pBuffer. After return of the function,
*           the IO data is now available  in this data buffer.
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_WRONG_HND, PNIO_ERR_INTERNAL
*----------------------------------------------------------------------------
* INPUTS   : ApplHandle - Handle of controler
*           pAddr - Address of the sub module
*           BufLen - Length of the data buffer (in byte) made available
*           IOlocState - IO local status (consumer status IOCS)
* OUTPUS   : pDataLen - read length of the data buffer (in byte)
*           pBuffer - Data buffer
*           pIOremState - IO remote status (provider status IOPS)
*----------------------------------------------------------------------------
* COMMENTS : pAddr is the address of the remote sub-module.
*
*
*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR
PNIO_data_read(
    PNIO_UINT32     ApplHandle,
	PNIO_ADDR * pAddr,
	PNIO_UINT32 BufLen,
	PNIO_UINT32 * pDataLen,
	PNIO_UINT8 * pBuffer,
	PNIO_IOXS IOlocState,
	PNIO_IOXS * pIOremState)
{
    PNIO_UINT32 Ret;
	/*
	this function is reentrant
	PND_IOB_USR_ENTER
	*/
    Ret = CController::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
    return PNIO_ERR_WRONG_HND;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "-> PNIO_data_read");

    PND_IOBC_TRACE_06(0, LSA_TRACE_LEVEL_NOTE,"ApplHandle=%d pAddr=%d  pDataLen=%d  pBuffer=%d IOlocState=%d  pIOremState=%d ",
		ApplHandle,pAddr, pDataLen, pBuffer, &IOlocState, pIOremState );
	
    CController *pIController = CController::get_instance(ApplHandle);

	PND_ASSERT(pDataLen != PNIO_NULL);

    if(pIController)
    {
        if(!pDataLen) 
        {
            Ret = PNIO_ERR_PRM_LEN;
        } 
        else 
        {
            IIODU *pIODU = CController::get_IODU(ApplHandle);

            if(pIODU) 
            {
                PNIO_BOOL readFromPi = PNIO_TRUE;

#ifdef PND_CFG_ISO_SUPPORT
                if (pIODU->isIsoData(pAddr, PND_ACC_T_READ) && pIController->isIsoDataConsistent())
                {
                    readFromPi = PNIO_FALSE;  //io data has been read from process image within iso sequence,
                                              //write from shadow buffer to user buffer only.
                }
#endif

                Ret = pIODU->data_io_rw(pAddr, PND_ACC_T_READ, &IOlocState, pIOremState, &BufLen, pBuffer, readFromPi);
                *pDataLen = BufLen;

#ifdef PND_CFG_ISO_SUPPORT
                if(Ret == PNIO_OK)
                {
                    if (pIODU->isIsoData(pAddr, PND_ACC_T_READ) && !pIController->isIsoDataConsistent())
                    {
                        Ret = PNIO_WARN_IRT_INCONSISTENT;
                    }
                }
#endif
            } 
            else 
            {
                Ret = PNIO_ERR_WRONG_HND;
            }
        }
    }
    else
    {
        Ret = PNIO_ERR_WRONG_HND;
    }


    if((Ret == PNIO_OK || Ret == PNIO_WARN_IRT_INCONSISTENT) && pBuffer)
    {
        PND_IOBC_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<- PNIO_data_read, ret= %d, pBuffer=0x%08x, *pDataLen=%d" ,Ret , pBuffer, *pDataLen);
    } 
    else 
    {
	    PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_data_read, ret= %d" ,Ret);
    }


    /*
    this function is reentrant
    PND_IOB_USR_EXIT
    */

    return Ret;
}




/*===========================================================================
* FUNCTION : PNIO_data_write
*----------------------------------------------------------------------------
* PURPOSE  : With this function the IO data is written and
*           the corresponding IOlocState is set.
*           The data buffer pBuffer contains the IO data to be written.
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_WRONG_HND, PNIO_ERR_INTERNAL
*----------------------------------------------------------------------------
* INPUTS   : ApplHandle - Handle of controler
*           pAddr - Address of the sub module
*           BufLen - Length of the data buffer (in byte) made available
*           pBuffer - Data buffer
*           IOlocState - IO local status (consumer status IOCS)
* OUTPUS   :
*           pIOremState - IO remote status (provider status IOPS)
*----------------------------------------------------------------------------
* COMMENTS : pAddr is the address of the remote sub-module.
*           This function is used for writing output data.
*
*
*==========================================================================*/
/*lint -e{818} Pointer parameter 'pAddr' could be declared as pointing to const*/
PNIO_UINT32 PNIO_CODE_ATTR
PNIO_data_write(PNIO_UINT32 ApplHandle, PNIO_ADDR * pAddr, PNIO_UINT32 BufLen,
	PNIO_UINT8 * pBuffer, PNIO_IOXS IOlocState,	PNIO_IOXS * pIOremState)
{
    PNIO_UINT32 Ret;

	/*
	this function is reentrant
	PND_IOB_USR_ENTER
	*/

    Ret = CController::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
    return PNIO_ERR_WRONG_HND;

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_data_write");

	
	if(pBuffer)
	{
		PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH,"pBuffer, BufLen %d, %d", pBuffer, BufLen);
	}
	else 
	{
        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "pBuffer == NULL");
	}

    CController *pIController = CController::get_instance(ApplHandle);

    if (pIController)
    {
        IIODU *pIODU = CController::get_IODU(ApplHandle);

        if(pIODU) 
        {
            PNIO_BOOL writeToPi = PNIO_TRUE;

#ifdef PND_CFG_ISO_SUPPORT
            if (pIODU->isIsoData(pAddr, PND_ACC_T_WRITE) && pIController->isIsoDataConsistent())
            {
                writeToPi = PNIO_FALSE;  //io data will be written to process image within iso sequence, 
                                         //write from user buffer to shadow buffer only.
            }
#endif

            Ret = pIODU->data_io_rw(pAddr, PND_ACC_T_WRITE, &IOlocState, pIOremState, &BufLen, pBuffer, writeToPi);

#ifdef PND_CFG_ISO_SUPPORT
            if (Ret == PNIO_OK)
            {
                if (pIODU->isIsoData(pAddr, PND_ACC_T_WRITE) && !pIController->isIsoDataConsistent())
                {
                    Ret = PNIO_WARN_IRT_INCONSISTENT;
                }
            }
#endif
        } 
        else
        {
            Ret = PNIO_ERR_WRONG_HND;
        }
    }
    else
    {
        Ret = PNIO_ERR_WRONG_HND;
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, " <- PNIO_data_write");
	/*
	PND_IOB_USR_EXIT
	*/

	return Ret;
}


/*===========================================================================
* FUNCTION : PNIO_register_cbf
*----------------------------------------------------------------------------
* PURPOSE  : With this function a callback function for the event
*           type CbeType is registered. The function pointer must not be NULL
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_WRONG_HND, PNIO_ERR_INTERNAL
*----------------------------------------------------------------------------
* INPUTS   : ApplHandle - Handle of controler
*           CbeType - Callback event type
*           Cbf - Callback function
* OUTPUS   : -
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR
PNIO_register_cbf(PNIO_UINT32 ApplHandle,
	PNIO_CBE_TYPE CbeType,
	PNIO_CBF Cbf)
{
    PNIO_UINT32 Ret;

    CController *pThis;

    Ret = CController::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND;

    pThis = CController::get_instance(ApplHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_SEQUENCE;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_register_cbf");

    if(pThis) {
        Ret = ((CController *) pThis)->register_cbf(CbeType, Cbf);
	} else {
		Ret = PNIO_ERR_WRONG_HND;
	}

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_register_cbf");

	PND_IOB_USR_EXIT

	return Ret;
}

/*===========================================================================
* FUNCTION : PNIO_rec_write_req
*----------------------------------------------------------------------------
* PURPOSE  : With this function the controller sends a write data record
*           the result of this job is signaled by the callback event
*           PNIO_CBE_REC_WRITE_CONF
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_WRONG_HND, PNIO_ERR_INTERNAL
*----------------------------------------------------------------------------
* INPUTS   : ApplHandle - Handle of controler
*           pAddr - Address of the sub module
*           RecordIndex- data record number
*           ReqRef - Reference assigned by the IO base user program
*           Length - Length of the data buffer (in byte) made available
*           pBuffer - IO local status (consumer status IOCS)
* OUTPUS   : -
*----------------------------------------------------------------------------
* COMMENTS : -
*==========================================================================*/
/*lint -e{818} Pointer parameter 'pBuffer' could be declared as pointing to const*/
PNIO_UINT32 PNIO_CODE_ATTR
PNIO_rec_write_req(PNIO_UINT32 ApplHandle, PNIO_ADDR * pAddr, PNIO_UINT32 RecordIndex,
	PNIO_REF ReqRef, PNIO_UINT32 Length, PNIO_UINT8 * pBuffer)
{
    PNIO_UINT32 Ret;

    CController *pThis;

    Ret = CController::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND;

    pThis = CController::get_instance(ApplHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_SEQUENCE;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_rec_write_req");

    if(pThis) {
        Ret = pThis->record_write(pAddr, ReqRef, RecordIndex, Length, pBuffer);
    } else {
        Ret = PNIO_ERR_WRONG_HND;
    }

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_rec_write_req");

	PND_IOB_USR_EXIT

	return Ret;
}

/*===========================================================================
* FUNCTION : PNIO_rec_read_req
*----------------------------------------------------------------------------
* PURPOSE  : With this function a controller sends a read data record job.
*           the result of the job is signaled by callback event
*           PNIO_CBE_REC_READ_CONF
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_WRONG_HND, PNIO_ERR_INTERNAL
*----------------------------------------------------------------------------
* INPUTS   : ApplHandle - Handle of controler
*           pAddr - Address of the sub module
*           RecordIndex - Data record number
*           ReqRef - reference assigned by IO base user program
*           Length - Length of data buffer
* OUTPUS   :
*
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR
PNIO_rec_read_req(PNIO_UINT32 ApplHandle,
	PNIO_ADDR * pAddr,
	PNIO_UINT32 RecordIndex,
	PNIO_REF ReqRef,
	PNIO_UINT32 Length)
{
    PNIO_UINT32 Ret;

    CController *pThis;

    Ret = CController::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND;

    pThis = CController::get_instance(ApplHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_SEQUENCE;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_rec_read_req");

    if(pThis) {
        Ret = pThis->record_read(pAddr, ReqRef, RecordIndex, Length);
	} else {
		Ret = PNIO_ERR_WRONG_HND;
	}

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_rec_read_req");

	PND_IOB_USR_EXIT

	return Ret;
}

/*===========================================================================
* FUNCTION : PNIO_device_activate
*----------------------------------------------------------------------------
* PURPOSE  : With this function an IO device is activated or deactivated
*           by the IO controller
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_WRONG_HND, PNIO_ERR_INTERNAL
*----------------------------------------------------------------------------
* INPUTS   : ApplHandle - Handle of controler
*           pAddr - logical address of the device to which the task is sent
*           DeviceMode - PNIO_DA_FALSE deactivates the device;
*           			 PNIO_DA_TRUE activates the device
* OUTPUS   : -
*----------------------------------------------------------------------------
* COMMENTS : Any logical address of the IO device, for which the activation
*           is intended, is passed to pAddr. In the controller mode OFFLINE
*           neither activation nor deactivation of devices is possible
*           After the initial start-up all devices are activated and
*           are in the mode OPERATE. The application status of the controller
*           is CLEAR and the user now is to deactivate the required devices.
*           A subsequent mode change of the controller from CLEAR to OPERATE
*           sets the IOxS of the data from/to the activated device to GOOD.
*==========================================================================*/
/*lint -e{818} Pointer parameter 'pAddr' could be declared as pointing to const*/
PNIO_UINT32 PNIO_CODE_ATTR PNIO_device_activate(PNIO_UINT32 ApplHandle, 
	PNIO_ADDR * pAddr, 	PNIO_DEV_ACT_TYPE DeviceMode)
{
    PNIO_UINT32 Ret;

    CController *pThis;

    Ret = CController::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND;

    pThis = CController::get_instance(ApplHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_WRONG_HND;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_device_activate");

    if(pThis) {
        Ret = ((CController *) pThis)->device_activate(pAddr, DeviceMode);
	} else {
		Ret = PNIO_ERR_WRONG_HND;
	}

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_device_activate");

	PND_IOB_USR_EXIT

	return Ret;
}

/*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR PNIO_interface_open(PNIO_UINT32 CpIndex,
	PNIO_CBF cbf_RecReadConf,
	PNIO_CBF cbf_AlarmInd,
	PNIO_UINT32 * pApplHandle)
{
	PNIO_UINT32 Ret = PNIO_OK;

	CIOInterface *pThis = 0;

    if(!pApplHandle)
        return PNIO_ERR_PRM_HND;

    CpIndex--; // Internally CpIndex Starts with 0

    Ret = CIOInterface::get_handle_by_index(CpIndex);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_PRM_CP_ID;

    pThis = CIOInterface::get_instance(CpIndex);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_SEQUENCE;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_interface_open CpIndex %d", CpIndex);

	PND_IOBC_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CpIndex= %d, cbf_RecReadConf=%d cbf_AlarmInd=%d ApplHandle = %d",
		CpIndex, cbf_RecReadConf, cbf_AlarmInd, pApplHandle);

	Ret = CIOInterface::interface_open(CpIndex,
    cbf_RecReadConf,
    cbf_AlarmInd,
    pApplHandle);

	PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "PNIO_interface_open ret= %d, ApplHandle= %d" ,Ret, pApplHandle);

	PND_IOB_USR_EXIT

	return Ret;
}

/*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR PNIO_interface_close(PNIO_UINT32 ApplHandle)
{
	PNIO_UINT32 Ret = PNIO_OK;

	CIOInterface *pThis = 0;

    Ret = CIOInterface::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND;

    pThis = CIOInterface::get_instance(ApplHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_WRONG_HND;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_interface_close");

	Ret = CIOInterface::interface_close(ApplHandle);

	PND_IOB_USR_EXIT

	return Ret;
}

/*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR
    PNIO_interface_register_cbf(PNIO_UINT32 ApplHandle,
	PNIO_CBE_TYPE CbeType,
	PNIO_CBF Cbf)
{
    PNIO_UINT32 Ret;

	CIOInterface *pThis;

    Ret = CIOInterface::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND;

    pThis = CIOInterface::get_instance(ApplHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_SEQUENCE;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_interface_register_cbf");

    if(pThis) {
        Ret = ((CIOInterface *) pThis)->register_cbf(CbeType, Cbf);
	} else {
		Ret = PNIO_ERR_WRONG_HND;
	}

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_interface_register_cbf");

	PND_IOB_USR_EXIT

	return Ret;
}

/*==========================================================================*/
/*lint -e{1746} parameter could be made const reference*/
PNIO_UINT32 PNIO_CODE_ATTR PNIO_interface_set_ip_and_nos(
	    PNIO_UINT32               ApplHandle,
        PNIO_SET_IP_NOS_MODE_TYPE Mode,
		PNIO_IPv4                 IPv4,
		PNIO_NOS                  NoS)
{
	CIOInterface *pThis = PNIO_NULL;

	PNIO_UINT32 Ret = CIOInterface::get_handle_by_index(ApplHandle);
	if (Ret == PND_INVALID_HANDLE)
		return PNIO_ERR_WRONG_HND;

	pThis = CIOInterface::get_instance(ApplHandle);
	if (pThis == PNIO_NULL)
		return PNIO_ERR_SEQUENCE;


	PND_IOB_USR_ENTER

		if(pThis)
		{
			Ret = (PNIO_UINT32)((CIOInterface *)pThis)->set_ip_and_nos(ApplHandle, Mode, IPv4, NoS);
		}
		else
		{
			Ret = PNIO_ERR_WRONG_HND;
		}

	PND_IOB_USR_EXIT

	return Ret;
}

/*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR
	PNIO_interface_rec_read_req(PNIO_UINT32 ApplHandle,
	PNIO_ADDR * pAddr,
	PNIO_UINT32 RecordIndex,
	PNIO_REF ReqRef,
	PNIO_UINT32 Length)
{
	CIOInterface *pThis = PNIO_NULL;

    PNIO_UINT32 Ret = CIOInterface::get_handle_by_index(ApplHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND;

    pThis = CIOInterface::get_instance(ApplHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_SEQUENCE;

	PND_IOB_USR_ENTER

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_interface_rec_read_req");

    if(pThis) {
        Ret = pThis->record_read(pAddr, ReqRef, RecordIndex, Length);
	} else {
		Ret = PNIO_ERR_WRONG_HND;
	}

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_interface_rec_read_req");

	PND_IOB_USR_EXIT

	return Ret;
}

#ifdef PND_CFG_ISO_SUPPORT
/*===========================================================================
* FUNCTION : PNIO_CP_register_cbf
*----------------------------------------------------------------------------
* PURPOSE  : With this function a callback function for the event
*            type PNIO_CP_CBE_TYPE is registered. The function pointer must not be NULL
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_ALLREADY_DONE, PNIO_ERR_INTERNAL, PNIO_ERR_INVALID_CONFIG
*            PNIO_ERR_PRM_CALLBACK, PNIO_ERR_PRM_TYPE, PNIO_ERR_SEQUENCE, PNIO_ERR_WRONG_HND
*----------------------------------------------------------------------------
* INPUTS   : ApplHandle - Handle of controler
*            CbeType - Callback event type
*            Cbf - Callback function
* OUTPUS   : -
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR PNIO_CP_register_cbf(
        PNIO_UINT32              AppHandle,
        PNIO_CP_CBE_TYPE         CbeType,
        PNIO_CP_CBF              Cbf)
{
    PNIO_UINT32 Ret;

    CController *pThis;

    Ret = CController::get_handle_by_index(AppHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND;

    pThis = CController::get_instance(AppHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_SEQUENCE;

    PND_IOB_USR_ENTER

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_CP_register_cbf");

    if(pThis) {
        Ret = ((CController *) pThis)->register_cp_cbf(CbeType, Cbf);
    } else {
        Ret = PNIO_ERR_WRONG_HND;
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_CP_register_cbf");

    PND_IOB_USR_EXIT

    return Ret;    
}

/*===========================================================================
* FUNCTION : PNIO_CP_set_opdone
*----------------------------------------------------------------------------
* PURPOSE  : With this function, the IO-Base user program signals the IO-Base interface
*            that it has completed isochronous real-time processing of the IRT IO data. The IO-Base
*            interface than initiates transmission of the IRT output data from the host memory to the 
*            process image by DMA.
*----------------------------------------------------------------------------
* RETURNS  : PNIO_OK on success; else PNIO_ERR_INTERNAL, PNIO_ERR_NO_FW_COMMUNICATION, PNIO_ERR_WRONG_HND
*----------------------------------------------------------------------------
* INPUTS   : ApplHandle - Handle of controler
* OUTPUS   : Information on the current cycle in which this call was executed. 
*            If the pointer was 0, nothing is returned.
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
PNIO_UINT32 PNIO_CODE_ATTR PNIO_CP_set_opdone(
        PNIO_UINT32              AppHandle,
        PNIO_CYCLE_INFO        * pCycleInfo
        )
{
    PNIO_UINT32 Ret;

    CController *pThis;

    Ret = CController::get_handle_by_index(AppHandle);
    if (Ret == PND_INVALID_HANDLE)
        return PNIO_ERR_WRONG_HND; 

    pThis = CController::get_instance(AppHandle);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_NO_FW_COMMUNICATION;  //TO-DO: ???

    /*
    This function is called within iso sequence and shouldn't be interrupted 
    PND_IOB_USR_ENTER
    */

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "->PNIO_CP_set_opdone");

    if(pThis) {
        Ret = ((CController *) pThis)->setOpDone(pCycleInfo);
    } else {
        Ret = PNIO_ERR_WRONG_HND;
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-PNIO_CP_set_opdone");

    /*
    PND_IOB_USR_EXIT
    */

    return Ret; 
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

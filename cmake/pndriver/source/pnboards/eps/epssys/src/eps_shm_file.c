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
/*  F i l e               &F: eps_shm_file.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Shared memory file transfer module                                   */
/*                                                                           */
/*****************************************************************************/

#include <eps_sys.h>         /* Types / Prototypes / Fucns        */ 
#include <eps_trc.h>         /* Tracing                           */
#include <eps_locks.h>       /* EPS Locks                         */
#include <eps_shm_file.h>    /* EPS Shared Mem File handling      */
#include <eps_tasks.h>       /* Thread Sleep                      */
#include <eps_register.h>
#include <eps_mem.h>
#define LTRC_ACT_MODUL_ID 20039

typedef struct
{
	LSA_BOOL bInit;                                 /**< Module is initialized      */
	LSA_UINT16 hEnterExit;                          /**< Enter/Exit handle          */
	EPS_SHM_FILE_TRANSFER_PTR_TYPE pFileTransfer;   /**< File transfer pointer      */
} EPS_SHM_FILE_GLOB_TYPE, *EPS_SHM_FILE_GLOB_PTR_TYPE;


static EPS_SHM_FILE_GLOB_PTR_TYPE g_pEpsShmFile = LSA_NULL;
static EPS_SHM_FILE_GLOB_TYPE g_EpsShmFile = { 0 };

/**
 * Undo initialize crtitcal section handle. @see eps_free_critical_section
 */
static LSA_VOID eps_shm_file_undo_init_critical_section(LSA_VOID)
{
	LSA_RESPONSE_TYPE retVal;
	retVal = eps_free_critical_section(g_pEpsShmFile->hEnterExit);
	EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
 * Initialize critical section handle. @see eps_alloc_critical_section
 */
static LSA_VOID eps_shm_file_init_critical_section(LSA_VOID)
{
    LSA_RESPONSE_TYPE retVal;
    retVal = eps_alloc_critical_section(&g_pEpsShmFile->hEnterExit, LSA_FALSE);
    EPS_ASSERT(LSA_RET_OK == retVal);
}

/**
 * Enter critical section. @see eps_enter_critical_section
 */
static LSA_VOID eps_shm_file_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsShmFile->hEnterExit);
}

/**
 * Exit critcal section. @see eps_exit_critical_section
 */
static LSA_VOID eps_shm_file_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsShmFile->hEnterExit);
}

/**
 * Initialize shared memory file transfer module
 * @param pBase [in] Base address of shared memory used for file transfer
 */
LSA_VOID eps_shm_file_init(LSA_UINT8* pBase)
{
	EPS_ASSERT(g_pEpsShmFile == LSA_NULL);
	EPS_ASSERT(pBase != LSA_NULL);

	g_pEpsShmFile = &g_EpsShmFile;
	EPS_ASSERT(g_pEpsShmFile->bInit == LSA_FALSE);

	eps_memset(g_pEpsShmFile, 0, sizeof(*g_pEpsShmFile));

	eps_shm_file_init_critical_section();

	eps_shm_file_enter();

	g_pEpsShmFile->pFileTransfer = (EPS_SHM_FILE_TRANSFER_PTR_TYPE)EPS_CAST_TO_VOID_PTR(pBase);
	g_pEpsShmFile->pFileTransfer->req.bReqStart = 0;
	g_pEpsShmFile->pFileTransfer->req.uReqPacketRead = 0;

	g_pEpsShmFile->bInit = LSA_TRUE;

	eps_shm_file_exit();
}

/** 
 * Undo initialization of shared memory file module
 */
LSA_VOID eps_shm_file_undo_init(LSA_VOID)
{
	EPS_ASSERT(g_pEpsShmFile != LSA_NULL);

	eps_shm_file_undo_init_critical_section();
	g_pEpsShmFile->bInit = LSA_FALSE;
	g_pEpsShmFile = LSA_NULL;
}

/**
 * Retrieve the firmware for a processor via shared memory. The memory for the firmware is allocated and 
 * the base pointer of the firmware file is stored in pFile.
 * 
 * @param pFile [in/out] Firmware options, e.g. Board type, Firmware type
 */
LSA_VOID eps_shm_file_alloc(EPS_APP_FILE_TAG_PTR_TYPE pFile)
{
	LSA_UINT32 r = 0;
	LSA_UINT32 uActOffset = 0;
	LSA_BOOL bDone = LSA_FALSE;
	LSA_BOOL bFirstPacket = LSA_TRUE;
	volatile EPS_SHM_FILE_TRANSFER_PTR_TYPE pFT;
    void* dst;

	EPS_ASSERT(g_pEpsShmFile != LSA_NULL);
	EPS_ASSERT(g_pEpsShmFile->bInit == LSA_TRUE);

	eps_shm_file_enter();

	pFT = g_pEpsShmFile->pFileTransfer;

    pFT->req.isHD = pFile->isHD;
    pFT->req.pndevBoard = pFile->pndevBoard;
    pFT->req.pndevBoardDetail = pFile->pndevBoardDetail;
    pFT->req.FileType = pFile->FileType;

    //Note: Use pFile-> to trace values to avoid LINT error 564. Volatile variables should not used be used multiple times in an expression (although they are only read and not modified here).
    EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE, "eps_shm_file_alloc() - requesting File Transfer. Board %d, Board Detail %d, FileType %d", pFile->pndevBoard, pFile->pndevBoardDetail, pFile->FileType);

	pFT->req.uReqPacketRead = r;
	pFT->rsp.bRspDone = 0;
	pFT->req.bReqStart = 1;
	pFT->rsp.bTransferDone = 0;
	pFT->rsp.bTransferDoneCnf = 0;

	while(!bDone)
	{
		if(pFT->rsp.bRspDone == 1)
		{
			if(pFT->rsp.bRspErr == 1)
			{
			    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_shm_file_alloc() - Transfer Failed");

				eps_shm_file_free(pFile);
				break;
			}
			else
			{
				if(bFirstPacket == LSA_TRUE)
				{
					pFile->uFileSize = pFT->rsp.uFileSize;
					pFile->pFileData = (LSA_UINT8*)eps_mem_alloc(pFile->uFileSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
					if(pFile->pFileData == LSA_NULL)
					{
					    EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_ERROR, "eps_shm_file_alloc() - Memory allocation failed");
						eps_shm_file_free(pFile);
						break;
					}

					EPS_ASSERT( pFT->rsp.uActPacket == r );
					EPS_ASSERT( (uActOffset + pFT->rsp.uActPacketSize) <= pFile->uFileSize );
                    //lint --e(564) variable 'pFT' depends on order of evaluation - uData and uActPacketSize are only read here
					dst = eps_memcpy((LSA_VOID*)(pFile->pFileData + uActOffset), (LSA_VOID*)pFT->rsp.uData, pFT->rsp.uActPacketSize);
                    LSA_UNUSED_ARG(dst);

					uActOffset += pFT->rsp.uActPacketSize;
	
					bFirstPacket = LSA_FALSE;
				}
				else
				{
					EPS_ASSERT( pFT->rsp.uActPacket == r );
					EPS_ASSERT( (uActOffset + pFT->rsp.uActPacketSize) <= pFile->uFileSize );
                    //lint --e(564) variable 'pFT' depends on order of evaluation - uData and uActPacketSize are only read here
                    dst = eps_memcpy((LSA_VOID*)(pFile->pFileData + uActOffset), (LSA_VOID*)pFT->rsp.uData, pFT->rsp.uActPacketSize);
                    LSA_UNUSED_ARG(dst);
					uActOffset += pFT->rsp.uActPacketSize;
				}

				if(uActOffset >= pFile->uFileSize)
				{
					bDone = LSA_TRUE;
				}
				else
				{
					r++;
					pFT->req.uReqPacketRead = r;
					pFT->rsp.bRspDone = 0;
					pFT->req.bReqStart = 1;
				}
			}
		}
		else
		{
			eps_tasks_sleep(1);
		}
	}

	pFT->rsp.bRspDone  = 0;
	pFT->req.bReqStart = 0;
	pFT->rsp.bTransferDone = 1;
	
	//wait for transfer done confirmation
	while (pFT->rsp.bTransferDoneCnf == 0)
	{
	    //do nothing
	    eps_tasks_sleep(1);
	}

    EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "eps_shm_file_alloc() - Transfer Success");

	eps_shm_file_exit();
}

/**
 * Free the formerly allocated memory which holds the firmware
 * 
 * @param pFile [in] Firmware information including pointer to firmware memory
 */
LSA_VOID eps_shm_file_free(EPS_APP_FILE_TAG_PTR_TYPE pFile)
{
    EPS_ASSERT(pFile != LSA_NULL);

    if(pFile->pFileData != LSA_NULL)
    {
        LSA_UINT16 retVal;
        retVal = eps_mem_free(pFile->pFileData, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
        EPS_ASSERT(LSA_RET_OK == retVal);
    }

    pFile->pFileData = LSA_NULL;
    pFile->uFileSize = 0;
    pFile->isHD      = 0;
    pFile->FileType  = EPS_APP_FILE_INVALID;

    pFile->pndevBoard       = EPS_PNDEV_BOARD_INVALID;
    pFile->pndevBoardDetail = EPS_EDDP_BOARD_TYPE_UNKNOWN;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

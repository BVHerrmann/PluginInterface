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
/*  F i l e               &F: pnd_eps_app.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNDriver Platform Adaption                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  20051
#define PND_MODULE_ID      20051

#include <stdio.h>

#include <eps_sys.h>           /* Types / Prototypes / Fucns               */
#include <pntrc_inc.h>         /* Tracing                                  */
#include <eps_cp_hw.h>         /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>     /* Pn Device Drv Interface                  */

#if defined (EPS_CFG_USE_PNDEVDRV) && !defined (PSI_EDDS_CFG_HW_LPCAP)
#include <eps_pndevdrv.h>      /* EPSPnDev                                 */
#elif (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <eps_wpcapdrv.h>      /* WPCAP Support                            */
#endif

#if defined (PSI_EDDS_CFG_HW_LPCAP)
#include <eps_lpcapdrv.h>
#endif

#if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86)) && defined(PND_CFG_BASIC_VARIANT)
#include <eps_ipc_pndevdrv_drv.h>
#include <eps_hif_universal_drv.h>
#include <eps_hif_ipcsiedma_drv.h>
#endif

#include <eps_shm_if.h>        /* Shared Mem Drv Interface                 */
#include <eps_noshmdrv.h>      /* Shortcut Drv                             */
#include <eps_hif_short_drv.h> /* HIF Shortcut Drv                         */
#include <eps_app.h>           /* Own Header                               */
#include <eps_rtos.h>          /* RTOS Interface                           */
#include <servusrx.h>          /* DebugSetting                             */

#if ((EPS_PLF == EPS_PLF_LINUX_SOC1) && (EPS_CFG_USE_HIF_LD == 1))
#include <eps_hif_universal_drv.h>
#include <eps_ipc_linux_soc_drv.h>
#include <eps_hif_ipcsiedma_drv.h>
#include <eps_pncore.h>
#include <eps_hif_pncorestd_drv.h>
#endif

// Standalone CP1625
#if ((EPS_PLF == EPS_PLF_LINUX_SOC1) && (EPS_CFG_USE_HIF_LD == 0))
#include "pnd_int.h"

LSA_UINT32 pnd_eps_app_Calc_CRC32(const LSA_UINT8*, LSA_UINT32);
#endif

#if (EPS_PLF == EPS_PLF_LINUX_SOC1)
#include <eps_pn_soc1_drv.h>        /* SOC1 support*/
#include "signal.h"

typedef void (*sighandler_t)(int);

//TODO z00301au dirty, but EPS doesn't solve this better. 
static EPS_SHM_HW_PTR_TYPE g_pEpsPlfShmHw = LSA_NULL;

LSA_VOID eps_store_fatal_info(EPS_SHM_HW_PTR_TYPE pEpsShmHw, EPS_SHM_FATAL_PAR_PTR_TYPE pFatal);
#endif

#if (EPS_PLF == EPS_PLF_LINUX_X86)

//TODO z00301au dirty, but EPS doesn't solve this better. 
static EPS_SHM_HW_PTR_TYPE g_pEpsPlfShmHw = LSA_NULL;

LSA_VOID eps_store_fatal_info( EPS_SHM_HW_PTR_TYPE pEpsShmHw, EPS_SHM_FATAL_PAR_PTR_TYPE pFatal );
#endif
//---------------------------------------------------------------------------------------------------
static LSA_UINT8 InitLogLevels[TRACE_SUBSYS_NUM];

static PNIO_DEBUG_SETTINGS_TYPE g_DebugSetting;

//----------------------------------------------------------------------------------------------------

void pntrc_rqb_done(LSA_VOID* pRQB0)
{
    LSA_UINT16 rc;
    PNTRC_RQB_PTR_TYPE pRQB;
    EPS_ASSERT( pRQB0 != 0);
    pRQB=(PNTRC_RQB_PTR_TYPE)pRQB0;
    PNTRC_FREE_LOCAL_MEM( &rc, pRQB );
    EPS_ASSERT( rc == LSA_RET_OK );
}

#if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
// z00301au copied from pnboards_eps_app.c
/**
 * platform exception handler
 *
 * @param arg1 EPS/ADN exception value
 * @return
 */
LSA_VOID eps_plf_exc_handler( int signum )
{
    EPS_SHM_FATAL_PAR_TYPE    sFatal;
  
    pntrc_fatal_occurred(); //From now on, no Traces will be recorded!

	if (signum == 0) /* If debug activ, set a breakpoint */
    {
	    EPS_APP_FATAL_HOOK(EPS_EXIT_CODE_EPS_EXCEPTION);
    }

    /* Set Global Fatal Flag */
    g_EpsData.bEpsInFatal = LSA_TRUE;

    /* This function is called while holding a spinlock, blocking function calls are not permitted! */
    g_EpsData.bBlockingPermitted = LSA_FALSE;

    /* Check if Shared Mem If is initialized */
    if (g_pEpsPlfShmHw != LSA_NULL)
    {
        eps_memset(&sFatal, 0, sizeof(sFatal));

        sFatal.exit_code_org = EPS_EXIT_CODE_EPS_EXCEPTION;
        sFatal.pFile         = __FILE__;
        sFatal.uLine         = __LINE__;
        sFatal.pFunc         = __FUNCTION__;
        sFatal.pMsg          = "eps_plf_exc_handler(): unhandled exception detected";

        eps_store_fatal_info(g_pEpsPlfShmHw, &sFatal);

        g_pEpsPlfShmHw->FatalError(g_pEpsPlfShmHw, sFatal);
    }

    EPS_APP_FATAL_HOOK(EPS_EXIT_CODE_EPS_EXCEPTION);
    
    // we need to kill ourselves to get the coredump
    signal(signum, SIG_DFL); 
    kill(getpid(), signum); 
}

// z00301au copied from pnboards_eps_app.c
/**
 * install function of exception handler
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_plf_install_exc_handler( LSA_VOID )
{
	struct sigaction action;
	int i;
	
	/* Registers our personal err handler in Linux */
	action.sa_flags = 0;
	action.sa_handler = eps_plf_exc_handler;
	sigaction(SIGSEGV, &action, (struct sigaction *)0);

	for(i=SIGRTMIN;i<=SIGRTMAX;i++)
	{
	     sigaction(i, &action, (struct sigaction *)0);
	}
}
#endif

static LSA_VOID eps_plf_eps_appready(EPS_SHM_HW_PTR_TYPE pHw, LSA_VOID* pHifHandle)
{
    LSA_UNUSED_ARG(pHifHandle);
    LSA_UNUSED_ARG(pHw);
}


static LSA_VOID eps_plf_eps_appshutdown(EPS_SHM_HW_PTR_TYPE pHw)
{
    LSA_UNUSED_ARG(pHw);
}

static LSA_VOID eps_plf_trace_buffer_full(LSA_VOID_PTR_TYPE hSys, LSA_VOID* hTrace, LSA_UINT32 uBufferId, LSA_UINT8* pBuffer, LSA_UINT32 uLength)
{
    PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSetting = (PNIO_DEBUG_SETTINGS_PTR_TYPE)hSys;
    PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
    LSA_UINT32                   UsedSize;

    LSA_UNUSED_ARG(uLength);

    pBufferHeader=(PNTRC_BUFFER_HEADER_PTR_TYPE)pBuffer;
    UsedSize=pBufferHeader->FirstBlock+pBufferHeader->CurBlockCount*sizeof(PNTRC_BUFFER_ENTRY_TYPE);

    if(LSA_NULL != DebugSetting->CbfPntrcBufferFull) //do nothing if cbf is not set
    {
      // pass current trace buffer to user
      DebugSetting->CbfPntrcBufferFull(pBuffer, UsedSize);
    }

    pntrc_buffer_full_done(hTrace, uBufferId);
}

#if defined (EPS_CFG_USE_PNDEVDRV) && !defined (PSI_EDDS_CFG_HW_LPCAP)
static LSA_VOID pnd_read_file(const LSA_CHAR* path, EPS_APP_FILE_TAG_PTR_TYPE file_tag)
{
    FILE       * file;
    LSA_UINT32   fileLen;

    file = fopen(path, "rb");
    if (!file)
    {
        EPS_FATAL("EPS PnDevDrv eps_read_file: (Unable to open file)");
    }

    //Get file length
    fseek(file, 0, SEEK_END);
    fileLen = (LSA_UINT32)ftell(file);
    fseek(file, 0L, SEEK_SET);
    file_tag->uFileSize = fileLen;

    //Allocate memory
    file_tag->pFileData = (LSA_UINT8*)malloc(fileLen + 1);

    if (!file_tag->pFileData)
    {
        fclose(file);

        EPS_FATAL("EPS PnDevDrv eps_read_file: (Allocate memory error)");
    }

    //Read contents
    fread(file_tag->pFileData, 1, fileLen, file);
    fclose(file);
}

static LSA_BOOL pnd_file_exist(const LSA_CHAR * filename)
{
    FILE     * filep;
    LSA_BOOL   bExist = LSA_FALSE;

    filep = fopen(filename, "r");
    if (filep)
    {
        // file exists
        bExist = LSA_TRUE;
        fclose(filep);
    }
    else
    {
        // file not exists
        bExist = LSA_FALSE;
    }
    return bExist;
}

static LSA_VOID pnd_get_fw_filename(LSA_CHAR * path, const EPS_APP_FILE_TAG_PTR_TYPE file_tag) /*lint -save -e818 */
{
    LSA_CHAR file_name1[256] = {0};
    LSA_CHAR file_name2[256] = {0};
    LSA_CHAR file_name3[256] = {0};

	if (file_tag->FileType == EPS_APP_FILE_FW_LINUX)
	{
		switch (file_tag->pndevBoard)  // PNBoard Type
		{
			case EPS_PNDEV_BOARD_CP1625:
			{
				// linux cp1625 basic fw
				eps_strcpy(file_name1, "uboot_linux.bin");
				eps_strcpy(file_name2, "uboot+linux.bin");
				eps_strcpy(file_name3, "uboot-linux.bin");
				break;
			}
			default:
			{
				EPS_FATAL("Not supported PnBoard-Type for linux FW Loading");
			}
		}
	}

	else if (file_tag->FileType == EPS_APP_FILE_FW_ADONIS)
	{
		switch (file_tag->pndevBoard)  // PNBoard Type
		{
			case EPS_PNDEV_BOARD_CP1625:
			{
				if (file_tag->isHD)
				{
					eps_strcpy(file_name1, "..\\..\\..\\antgen\\antmake\\bin\\adonis_eps_hd_soc1\\hd_soc1.elf");
					eps_strcpy(file_name2, "..\\..\\antgen\\antmake\\bin\\adonis_eps_hd_soc1\\hd_soc1.elf");
					eps_strcpy(file_name3, "hd_soc1.elf");
				}
				else //LD
				{
					#if ( PSI_CFG_USE_TCIP_OBSD == 1 )
					//-> 6.0 OBSD LD
					#if ((EPS_PLF == EPS_PLF_LINUX_X86))
					eps_strcpy(file_name1, "..//..//..//source//antgen//antmake//bin//adonis_eps_openbsd_soc1//ld_openbsd_soc1.elf");
					eps_strcpy(file_name2, "..//..//antgen//antmake//bin//adonis_eps_openbsd_soc1//ld_openbsd_soc1.elf");
					#elif (EPS_PLF == EPS_PLF_WINDOWS_X86) 
					eps_strcpy(file_name1, "..\\..\\..\\source\\antgen\\antmake\\bin\\adonis_eps_openbsd_soc1\\ld_openbsd_soc1.elf");
					eps_strcpy(file_name2, "..\\..\\antgen\\antmake\\bin\\adonis_eps_openbsd_soc1\\ld_openbsd_soc1.elf");
					#endif
					eps_strcpy(file_name3, "ld_openbsd_soc1.elf");
					#else
					eps_strcpy(file_name1, "..\\..\\..\\antgen\\antmake\\bin\\adonis_eps_soc1\\ld_soc1.elf");
					eps_strcpy(file_name2, "..\\..\\antgen\\antmake\\bin\\adonis_eps_soc1\\ld_soc1.elf");
					eps_strcpy(file_name3, "ld_soc1.elf");
					#endif
				}
				break;
			}
			default:
			{
				EPS_FATAL("Not supported PnBoard-Type for adonis FW Loading");

			}
		}
	}

    if (pnd_file_exist(file_name1))
    {
        eps_strcpy(path, (const LSA_CHAR *)file_name1);
    }
    else if (pnd_file_exist(file_name2))
    {
        eps_strcpy(path, (const LSA_CHAR *)file_name2);
    }
    else if (pnd_file_exist(file_name3))
    {
        eps_strcpy(path, (const LSA_CHAR *)file_name3);
    }
    else
    {
        EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_APP_FW_NOT_AVAIL, EPS_EXIT_CODE_APP_FW_NOT_AVAIL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "pnd_get_fw_filename() - FW-File not existing");
    }
}

static LSA_VOID pnd_get_bootloader_filename(LSA_CHAR * path, const EPS_APP_FILE_TAG_PTR_TYPE file_tag)
{
    LSA_CHAR file_name1[256] = {0};
    LSA_CHAR file_name2[256] = {0};
    LSA_CHAR file_name3[256] = {0};

    switch (file_tag->pndevBoard)
    {
		case EPS_PNDEV_BOARD_CP1625:
		{
	#if ((EPS_PLF == EPS_PLF_LINUX_X86))
			eps_strcpy(file_name1, "..//..//..//source//pndevdrv//PnDev_Driver//bin//BootFw_Jmp_Soc1.img");
			eps_strcpy(file_name2, "..//..//pndevdrv//PnDev_Driver//bin//BootFw_Jmp_Soc1.img");
	#elif (EPS_PLF == EPS_PLF_WINDOWS_X86) 
			eps_strcpy(file_name1, "..\\..\\..\\source\\pndevdrv\\PnDev_Driver\\bin\\BootFw_Jmp_Soc1.img");
			eps_strcpy(file_name2, "..\\..\\pndevdrv\\PnDev_Driver\\bin\\BootFw_Jmp_Soc1.img");
	#endif
			eps_strcpy(file_name3, "BootFw_Jmp_Soc1.img");
			break;
		}
		default:
		{
			EPS_FATAL("Not supported PnBoard-Type for Boot-FW Loading");

		}
    }

    if (pnd_file_exist(file_name1))
    {
        eps_strcpy(path, (const LSA_CHAR *)file_name1);
    }
    else if (pnd_file_exist(file_name2))
    {
        eps_strcpy(path, (const LSA_CHAR *)file_name2);
    }
    else if (pnd_file_exist(file_name3))
    {
        eps_strcpy(path, (const LSA_CHAR *)file_name3);
    }
    else
    {
        EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_APP_FW_NOT_AVAIL, EPS_EXIT_CODE_APP_FW_NOT_AVAIL, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "pnd_get_bootloader_filename() - Bootloader-File not existing");
    }
}
#endif
LSA_VOID  EPS_APP_ALLOC_FILE(EPS_APP_FILE_TAG_PTR_TYPE file_tag) //0 = LD, 1 = HD
{
#if defined (EPS_CFG_USE_PNDEVDRV) && !defined (PSI_EDDS_CFG_HW_LPCAP)
    // Get FW path's
    eps_memset(file_tag->Path.sPath, 0, EPS_APP_PATH_MAX_LENGTH);

	// here the fw type can be set, switch between adonis/linux
	// EPS presets this value to EPS_APP_FILE_FW_ADONIS, so we simply override it with linux
	file_tag->FileType = EPS_APP_FILE_FW_LINUX;
	// --------------------------------------------------------

	switch (file_tag->FileType)
	{
		case EPS_APP_FILE_FW_ADONIS:
		case EPS_APP_FILE_FW_LINUX:
		{
			pnd_get_fw_filename(&file_tag->Path.sPath[0], file_tag);
			file_tag->Path.uPathLength = (unsigned long)eps_strlen(&file_tag->Path.sPath);
			break;
		}
		case EPS_APP_FILE_BOOTLOADER:
		{
			pnd_get_bootloader_filename(&file_tag->Path.sPath[0], file_tag);
			file_tag->Path.uPathLength = (unsigned long)eps_strlen(&file_tag->Path.sPath);
			break;
		}
		case EPS_APP_FILE_INVALID:
		default:
		{
			EPS_FATAL("Not expected File type received");
		}
	}

	if (file_tag->FileType != EPS_APP_FILE_FW_LINUX)
		// for linux fw we do not read the file
	{
		pnd_read_file(file_tag->Path.sPath, file_tag);
	}

#else
    LSA_UNUSED_ARG(file_tag);
#endif
}

/*----------------------------------------------------------------------------*/
LSA_VOID  EPS_APP_FREE_FILE(EPS_APP_FILE_TAG_PTR_TYPE file_tag)
{
#ifdef EPS_CFG_USE_PNDEVDRV

	if (file_tag->FileType != EPS_APP_FILE_FW_LINUX)
		// for linux fw we do not free the file
	{
		if (file_tag->pFileData != LSA_NULL)
		{
			free(file_tag->pFileData);
		}
	}

	file_tag->pFileData = LSA_NULL;
	file_tag->uFileSize = 0;

#else
	LSA_UNUSED_ARG(file_tag);
#endif
}

/*----------------------------------------------------------------------------*/
LSA_VOID EPS_APP_GET_DRIVER_PATH(EPS_APP_DRIVER_PATH_ARR_PTR_TYPE pPathArr, EPS_APP_DRIVER_TYPE_ENUM driverType)
{
	//LSA_CHAR sPath0RelativeToExeDir[256];
    
    LSA_CHAR* sPath0 = LSA_NULL;
    LSA_CHAR* sPath1 = LSA_NULL;
    LSA_CHAR* sPath2 = LSA_NULL;

    switch (driverType)
    {
		case EPS_APP_DRIVER_PNDEVDRV:
		{
			#if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000))
			sPath0 = "../../../pndevdrv/PnDev_Driver/bin/Linux/PnDev_DriverU32.so";
			sPath1 = "../../pndevdrv/PnDev_Driver/bin/Linux/PnDev_DriverU32.so";
			sPath2 = "./PnDev_DriverU32.so";
			#elif (EPS_PLF == EPS_PLF_WINDOWS_X86)      
			sPath0 = "..\\..\\..\\..\\source\\pndevdrv\\PnDev_Driver\\bin\\PnDev_DriverU32.dll";
			sPath1 = "..\\..\\pndevdrv\\PnDev_Driver\\bin\\PnDev_DriverU32.dll";
			sPath2 = "PnDev_DriverU32.dll";
			#else
			//No DLL for Adonis x86 
			sPath0 = "\0";
			sPath1 = "\0";
			sPath2 = "\0";
			#endif

			break;
		}

        case EPS_APP_DRIVER_INVALID:
        {
            EPS_FATAL("EPS_APP_GET_DRIVER_PATH called with invalid driver type");
            break;
        }

        default:
        {
            EPS_FATAL("EPS_APP_GET_DRIVER_PATH called with undefined driver type");
            break;
        }
    }
	/*lint --e{732} loss of sign */
	eps_strcpy(pPathArr->Path[0].sPath, sPath0);
    pPathArr->Path[0].uPathLength = eps_strlen(pPathArr->Path[0].sPath);

    eps_strcpy(pPathArr->Path[1].sPath, sPath1);
    pPathArr->Path[1].uPathLength = eps_strlen(pPathArr->Path[1].sPath);

    eps_strcpy(pPathArr->Path[2].sPath, sPath2);
    pPathArr->Path[2].uPathLength = eps_strlen(pPathArr->Path[2].sPath);
}

/*----------------------------------------------------------------------------*/
LSA_VOID EPS_APP_GET_MAC_ADDR(EPS_APP_MAC_ADDR_ARR_PTR_TYPE pMacArr, LSA_UINT16 hd_id, LSA_UINT16 countMacAddrRequested)
{
    LSA_UNUSED_ARG(hd_id);

// CP1625 Standalone
#if ((EPS_PLF == EPS_PLF_LINUX_SOC1) && (EPS_CFG_USE_HIF_LD == 0))

    PND_FLASH_DATA_CP1625 *pMem_Flash;              // Struct for flash memory (mtd6-file)
    FILE *pDirFlash                     = NULL;     // File /dev/mtd6 -- pointer to copied content of the flash
    LSA_UINT32 file_Size                = 0;        // Size of MTD6 file
    LSA_UINT32 length_Blockdata         = 0;        // Length for CRC32
    LSA_UINT32 calculated_CRC32         = 0;        // CRC32 calculated from copied data
    LSA_UINT32 stored_CRC32             = 0;        // CRC32 that is copied from mtd6-file


    FILE        *pCommand;
    LSA_UINT8   cnt;
    LSA_UINT8   dir_mac[20]             = {0};


    //-----------------------------------------------------
    // Get the address where the MAC-address is saved

    // Execute command and save output in stream of pCommand
    pCommand = popen("awk -F: '/spi-production-data/{print \"/dev/\"$1}' /proc/mtd", "r");

    if(pCommand != NULL)
    {
        // Get output of command
        if(fgets((char *)dir_mac, sizeof(dir_mac)-1, pCommand) == NULL)
        {
            pclose(pCommand);
            EPS_FATAL("EPS_APP_GET_MAC_ADDR - Can't read the output from awk...");
        }

        // Look where the end of the string is and replace '\n' with null-termination
        for(cnt = 0 ; cnt < 20 ; cnt++)
        {
            if(dir_mac[cnt] == '\n')
            {
                dir_mac[cnt] = '\0';
                break;
            }
        }

        pclose(pCommand);
    }
    else
    {
        EPS_FATAL("EPS_APP_GET_MAC_ADDR - Can't open command awk...");
    }

    //-----------------------------------------------------
    // Read from file MTD-X the flash memory


    pMem_Flash = (PND_FLASH_DATA_CP1625*) calloc(1, sizeof(PND_FLASH_DATA_CP1625)); // Allocate memory while runtime (too much for image)

    if(pMem_Flash == NULL)
    {
        EPS_FATAL("EPS_APP_GET_MAC_ADDR - There is not enough free memory for calloc()");
    }

    pDirFlash = fopen((char *)dir_mac, "r");

    if(pDirFlash != NULL)
    {
        fseek(pDirFlash, 0L, SEEK_END);     // Set file-cursor to end of the file
        file_Size = (LSA_UINT32)ftell(pDirFlash);
    	rewind(pDirFlash);                  // Set file-cursor to the start

        if(fread(pMem_Flash, file_Size, 1, pDirFlash) != 1) // Copy data from MTD6 to struct
        {
            free(pMem_Flash);
            EPS_FATAL("EPS_APP_GET_MAC_ADDR - Error during copying from mtd6 file");
        }

        fclose(pDirFlash);
    }
    else
    {
        free(pMem_Flash);
        EPS_FATAL("EPS_APP_GET_MAC_ADDR - Can not open file mtd6!");
    }

    //-----------------------------------------------------
    // Check version and CRC32

    // We only support Version 0x0001 for now
    if(!(pMem_Flash->uBlockDataVersion[1] == 0x00 && pMem_Flash->uBlockDataVersion[0] == 0x01))
    {
        free(pMem_Flash);
        EPS_FATAL("EPS_APP_GET_MAC_ADDR - Blockversion differs from 0x0001!");
    }

    // Map array to LSA_UINT32 for comparison
    length_Blockdata    |=  (LSA_UINT32)pMem_Flash->uLengthOfBlock[0];
    length_Blockdata    |= ((LSA_UINT32)pMem_Flash->uLengthOfBlock[1]) << 8;

    stored_CRC32        |=  (LSA_UINT32)pMem_Flash->uCrc32[0];
    stored_CRC32        |= ((LSA_UINT32)pMem_Flash->uCrc32[1]) << 8;
    stored_CRC32        |= ((LSA_UINT32)pMem_Flash->uCrc32[2]) << 16;
    stored_CRC32        |= ((LSA_UINT32)pMem_Flash->uCrc32[3]) << 24;

    calculated_CRC32 = pnd_eps_app_Calc_CRC32(pMem_Flash->uBlockDataVersion, length_Blockdata);

    // Compare new calculated CRC32 with the one copied
    if(calculated_CRC32 != stored_CRC32)
    {
        free(pMem_Flash);
        EPS_FATAL("EPS_APP_GET_MAC_ADDR - Stored and calculated CRC are different!");
    }

    //------------------------------------------------------
    // Copy MAC-Adresses

    switch(countMacAddrRequested)
    {
        case 4:
            memcpy(pMacArr->lArrayMacAdr[3], pMem_Flash->uMacAdr4, 6);
			/*lint -e{616, 825} control flows into case/default */
        case 3:
            memcpy(pMacArr->lArrayMacAdr[2], pMem_Flash->uMacAdr3, 6);
			/*lint -e{616, 825} control flows into case/default */
        case 2:
            memcpy(pMacArr->lArrayMacAdr[1], pMem_Flash->uMacAdr2, 6);
			/*lint -e{616, 825} control flows into case/default */
        case 1:
            memcpy(pMacArr->lArrayMacAdr[0], pMem_Flash->uMacAdr1, 6);
            break;
        default:
            free(pMem_Flash);
            EPS_FATAL("EPS_APP_GET_MAC_ADDR - Invalid countMacAddrRequested! Only 1...4 is allowed!");
    }

    free(pMem_Flash);
#elif ((EPS_PLF == EPS_PLF_LINUX_SOC1 ) && ( EPS_CFG_USE_HIF_LD == 1 ))
    // for linux soc1 basic, don't override mac adresses (@see eps_pn_soc1_open() and @see eps_pncore_get_mac_addr())    
#else
    LSA_UINT8 i = 0;

    for (i = 0; i < countMacAddrRequested; i++)
    {
        pMacArr->lArrayMacAdr[i][0] = 0x00;
        pMacArr->lArrayMacAdr[i][1] = 0xFE;
        pMacArr->lArrayMacAdr[i][2] = 0xBE;
        pMacArr->lArrayMacAdr[i][3] = 0xEF;
        pMacArr->lArrayMacAdr[i][4] = 0x00;
        pMacArr->lArrayMacAdr[i][5] = i;
    }

#endif

}

// CP1625 standalone
#if ((EPS_PLF == EPS_PLF_LINUX_SOC1) && (EPS_CFG_USE_HIF_LD == 0))
/*----------------------------------------------------------------------------*/
/* Function to calculate the CRC32 of pData                                   */
/*----------------------------------------------------------------------------*/
LSA_UINT32  pnd_eps_app_Calc_CRC32(const LSA_UINT8*  pData, LSA_UINT32  lLength)
{
    LSA_UINT32 lShiftReg        = 0xFFFFFFFF;
    LSA_UINT32 lPolynom         = 0xEDB88320;    // Generatorpolynom
    LSA_UINT32 i                = 0;
    LSA_UINT32 j                = 0;

    for (i = 0; i < lLength; i++)
    // Calculate for all data
    {
        LSA_UINT8 cByte = *(pData + i);
        for (j = 0; j < 8; ++j)
        // Calculate for each byte of data
        {
            if ((lShiftReg & 1) != (LSA_UINT32) (cByte & 1))
                lShiftReg = (lShiftReg >> 1) ^ lPolynom;
            else
                lShiftReg >>= 1;
            cByte >>= 1;
        }
    }
    return (lShiftReg ^ 0xffffffff);
}
#endif

LSA_VOID EPS_APP_INSTALL_DRV_OPEN(PSI_LD_RUNS_ON_TYPE ldRunsOnType)
{
    LSA_UNUSED_ARG(ldRunsOnType);
    // Embedded driver for SOC1 variant provided by EPS
    #if (EPS_PLF == EPS_PLF_SOC_MIPS || EPS_PLF == EPS_PLF_LINUX_SOC1)
    eps_pn_soc1_drv_install();                  /* Install EPS SOC1-Driver */
    #endif
    
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86 && !defined(EPS_CFG_USE_PNDEVDRV))
    //----- PORTING HINT -------------------------------------------------------------
    // When using the Intel NIC driver implementation you need to un-comment this line:
    // eps_stdmacdrv_install();           /* Install EPS StdMac-Driver  NOTE: Define EDDS_CFG_HW_INTEL in preprocessor settings    */
    // and comment this line:
    eps_wpcapdrv_install();              /* Install EPS WPCAP-Driver  NOTE: Define EDDS_CFG_HW_PACKET32 in preprocessor settings   */
    #endif

    #if defined (PSI_EDDS_CFG_HW_LPCAP)
    eps_lpcapdrv_install();
    #endif


}

LSA_VOID EPS_APP_INIT(LSA_VOID_PTR_TYPE hSys, EPS_HW_INFO_PTR_TYPE pEpsHw)
{
#if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    EPS_SHM_INSTALL_ARGS_TYPE NoShmDrvArgs;
    
    #if ((EPS_PLF == EPS_PLF_LINUX_SOC1) && (EPS_CFG_USE_HIF_LD == 1))
    EPS_SHM_INSTALL_ARGS_TYPE ShmArgs;
    #endif
    
    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    eps_plf_install_exc_handler(); // installs signal handler wich is needed for using signals in Linux!
    #endif
    #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    {
        //lock memory for future mmap operations @see EPS_PLF_MAP_MEMORY
        LSA_INT retVal = mlockall(MCL_CURRENT | MCL_FUTURE);
        EPS_ASSERT(retVal == 0);
    }
    #endif

    NoShmDrvArgs.pAppReadyCbf       = eps_plf_eps_appready;
    NoShmDrvArgs.pAppShutdownCbf    = eps_plf_eps_appshutdown;

#if ((EPS_PLF == EPS_PLF_LINUX_SOC1) && (EPS_CFG_USE_HIF_LD == 1))
	NoShmDrvArgs.sPntrcParam.bUseInitLogLevels = LSA_FALSE;
	NoShmDrvArgs.sPntrcParam.pInitLogLevels = LSA_NULL;

#else
	PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSetting = (PNIO_DEBUG_SETTINGS_PTR_TYPE)hSys;
	g_DebugSetting = *(PNIO_DEBUG_SETTINGS_PTR_TYPE)hSys;
	if(DebugSetting->CbfPntrcBufferFull != LSA_NULL)
	{
		pntrc_set_log_levels_from_components(InitLogLevels, DebugSetting->TraceLevels, PNIO_TRACE_COMP_NUM);

        NoShmDrvArgs.sPntrcParam.pInitLogLevels       = &InitLogLevels[0];
        NoShmDrvArgs.sPntrcParam.bUseInitLogLevels    = LSA_TRUE;
	}
	else
	{
		NoShmDrvArgs.sPntrcParam.bUseInitLogLevels    = LSA_FALSE;
		NoShmDrvArgs.sPntrcParam.pInitLogLevels       = LSA_NULL;
}
#endif

    NoShmDrvArgs.pTraceBufferFullCbf              = eps_plf_trace_buffer_full;
    NoShmDrvArgs.sPntrcParam.bIsTopMostCPU        = LSA_TRUE;

    /* Install drivers */
    eps_noshmdrv_install(&NoShmDrvArgs); /* Install EPS Shortcut driver DO NOT REMOVE */

    eps_hif_short_drv_install();         /* Install HIF Shortcut driver DO NOT REMOVE */

	#if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86)) && defined(PND_CFG_BASIC_VARIANT)
    eps_ipc_pndevdrv_drv_install();     /* Install PnDevDrv IPC driver */
    eps_hif_universal_drv_install();
    eps_hif_ipcsiedma_drv_install();    /* HIF IPC Driver uses PnDevDrv IPC driver */
    #endif
    
    #if ((EPS_PLF == EPS_PLF_LINUX_SOC1) && (EPS_CFG_USE_HIF_LD == 1))
    ShmArgs.sPntrcParam.bIsTopMostCPU        = LSA_FALSE;
    ShmArgs.sPntrcParam.bUseInitLogLevels    = LSA_TRUE; // the LogLevels are in eps_LogLevel-array
    ShmArgs.sPntrcParam.pInitLogLevels       = InitLogLevels;

    eps_ipc_linux_soc_drv_install();    /* Install Linux Soc IPC driver */
    
    eps_hif_universal_drv_install();    /* Universal Hif Driver */
    eps_hif_ipcsiedma_drv_install();    /* HIF IPC Driver uses Linux Soc IPC driver  */

    eps_pncoredrv_install(&ShmArgs);    /* PnCore Driver for Upper Shared Mem Interface */
    eps_hif_pncorestd_drv_install();    /* PnCore HIF Driver */
    #endif

    EPS_ASSERT(pEpsHw != LSA_NULL);

    /* Init Hw structure*/
    eps_memset(pEpsHw, 0, sizeof(*pEpsHw));

    //----- PORTING HINT --------------------------------------------------------------
    #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    pEpsHw->LocalMemPool.lSize                          = 0x1C00000; //Size of local mem used by PN Driver
    #else
    pEpsHw->LocalMemPool.lSize                          = 0x3400000; //Size of local mem used by PN Driver
    #endif
    pEpsHw->LocalMemPool.pBase                          = (LSA_UINT8*)malloc(pEpsHw->LocalMemPool.lSize);
    // When using eps_stdmacdrv_install() you need to provide the physical address of the local mem
    pEpsHw->LocalMemPool.uPhyAddr                       = (LSA_UINT32)pEpsHw->LocalMemPool.pBase;
    //---------------------------------------------------------------------------------

    /* Fast Memory */
    pEpsHw->FastMemPool.uPhyAddr                        = 0x00000000; /* No Specialized Fast Mem Used */
    pEpsHw->FastMemPool.pBase                           = 0x00000000; /* Take Fast Mem From Local Mem */
    pEpsHw->FastMemPool.lSize                           = 0x10000;
    
    /* Local Memory */
#else
    
	EPS_FATAL("Not supported PLF type.");

#endif

#if defined (EPS_CFG_USE_PNDEVDRV) && !defined(PSI_EDDS_CFG_HW_LPCAP)
eps_pndevdrv_install();
#endif
}
/*lint -e{818} symbol pEpsHw Pointer parameter 'Symbol' (Location) could be declared ptr to const*/
LSA_VOID EPS_APP_UNDO_INIT(LSA_VOID_PTR_TYPE hSys, EPS_HW_INFO_PTR_TYPE pEpsHw)
{
    LSA_UNUSED_ARG(hSys);
    // local memory is only allocated in Linux and Windows variant
    free(pEpsHw->LocalMemPool.pBase);

	#if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    eps_pndev_if_uninstall_from_app();
	#endif

    #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    {
        //unlock memory after last unmap is done @see EPS_PLF_UNMAP_MEMORY
        LSA_INT retVal = munlockall();
        EPS_ASSERT(retVal == 0);
    }
    #endif
}

LSA_VOID EPS_APP_KILL_EPS(EPS_TYPE_EXIT_CODE eps_exit_code)
{
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86 || EPS_PLF == EPS_PLF_LINUX_X86 || EPS_PLF == EPS_PLF_LINUX_IOT2000 || EPS_PLF == EPS_PLF_LINUX_SOC1)
    // duplicate: EPS_APP_FATAL_HOOK(eps_exit_code);

    if(EPS_APP_KILL_EPS != 0) // This code is here to prevent warnings
    {
        exit((int)eps_exit_code); /* c-lib exit! */
    }   
    #endif
}

LSA_VOID EPS_APP_FATAL_HOOK(EPS_TYPE_EXIT_CODE eps_exit_code)
{
    LSA_UNUSED_ARG(eps_exit_code);
    PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSetting = &g_DebugSetting;
	
    // HINT for developer: You can set your breakpoint in this function. If PND_DEBUG is set and an internal error occurs, this function is called. 
	// You can get get the callstack to see where the FATAL error occured.
	// If a debugger is attached, you can set a breakpoint using this implementation (that means the breakpoint is set every time).

	//__asm{int 3}; /* debugger "user-breakpoint" */

	// Note: You should only use an implementation like this if a debugger is attached. We strongly recommend not to call __asm{int 3}; 
	// in a software used for productive use cases.
    
    if (LSA_NULL != DebugSetting->CbfPntrcWriteBuffer) //do nothing if cbf is not set
    {
        // write trace buffers to file system
        DebugSetting->CbfPntrcWriteBuffer(LSA_TRUE);
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID EPS_APP_SAVE_DUMP_FILE(EPS_FATAL_STORE_DUMP_PTR_TYPE pDumpStoreInfo)
{
	LSA_UNUSED_ARG(pDumpStoreInfo);
}

LSA_VOID EPS_APP_HALT(LSA_VOID)
{
}

/*----------------------------------------------------------------------------*/
// z003nw8d: added with PNRunV0700
LSA_VOID EPS_APP_UNINSTALL_DRV_CLOSE(LSA_VOID_PTR_TYPE hSys)
{
    LSA_UNUSED_ARG(hSys);
}


// added for linux application
/*------------------------------------------------------------------------------
//	Source
//----------------------------------------------------------------------------*/
/**
 * eps main function
 *
 * @param args
 */
void *eps_main( const void * args )
{
    EPS_OPEN_TYPE uOpen;

    LSA_UNUSED_ARG(args);

	eps_memset(&uOpen, 0, sizeof(EPS_OPEN_TYPE));

    uOpen.hd_count      = 1;
    uOpen.pUsrHandle    = LSA_NULL;
    uOpen.ldRunsOnType  = PSI_LD_RUNS_ON_UNKNOWN;

    /* EPS entry point */
    eps_init(LSA_NULL); /* hSys and ldRunsOnType are not used in Adonis Targets */

    eps_open(&uOpen, LSA_NULL, PSI_EVENT_NO_TIMEOUT); /* pCbf are not used in Adonis Targets */

    return 0;
}
  




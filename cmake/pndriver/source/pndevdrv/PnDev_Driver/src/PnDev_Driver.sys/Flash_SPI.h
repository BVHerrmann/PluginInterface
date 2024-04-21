/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License version 2     */
/* as published by the Free Software Foundation; or, when distributed        */
/* separately from the Linux kernel or incorporated into other               */
/* software packages, subject to the following license:                      */
/*                                                                           */
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: Flash_SPI.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for SPI Flashes
//
//****************************************************************************/

#ifndef __Flash_SPI_h__
#define __Flash_SPI_h__

    //##################################################################################################################################
	//  General SPI Structures
	//##################################################################################################################################

    /* Serial stream structure */
    typedef struct _SSP_CHARSTREAM
    {
        UINT32 lLength;
        UINT8* pBuffer;

    }   SSP_CHARSTREAM;

    /* Acceptable values for SPI master side configuration */
    typedef enum _SSP_CONFIG_OPTION
    {
        SSP_OptionNull,  			    // do nothing
        SSP_OptionEnableChipSelect,	    // enable transmit
        SSP_OptionKeepUpChipSelect,     // another transmit is following (e.g. send command -> send data)
        SSP_OptionDisableChipSelect,    // no transmit is following

    }   SSP_CONFIG_OPTION;

	//##################################################################################################################################
	//  Defines
	//##################################################################################################################################
    
    //*****************************************************************************************************
	//  General defines used by all
    //*****************************************************************************************************

    #define SPI_FLASH_ALL__TIMEOUT_MAX_COUNT            50          // wait max 50 milliseconds

    //*****************************************************************************************************
    //*****************************************************************************************************
	//  N25Q128A (Micron)
	//*****************************************************************************************************
    //*****************************************************************************************************

    /* manufacturer id + mem type + mem capacity  */
    #define SPI_FLASH_N25Q128A__MEM_TYPE        	    0x20BB18    // values from flash spec
    #define SPI_FLASH_N25Q128A__MEM_TYPE_MANUFAC_ID	    0x20
    #define SPI_FLASH_N25Q128A__MEM_TYPE_MEMTYPE_ID	    0xBB
    #define SPI_FLASH_N25Q128A__MEM_TYPE_MEMSIZE_ID	    0x18

    /* Flash shape */
    #define SPI_FLASH_N25Q128A__SIZE           	        0x1000000   // 128 MB
    #define SPI_FLASH_N25Q128A__SECTOR_COUNT           	0x100       // 256 Sectors
    #define SPI_FLASH_N25Q128A__SECTOR_SIZE           	0x10000     // 64 kB
    #define SPI_FLASH_N25Q128A__SUBSECTOR_COUNT         0x1000      // 4096 SubSectors
    #define SPI_FLASH_N25Q128A__SUBSECTOR_SIZE          0x1000      // 4 kB
    #define SPI_FLASH_N25Q128A__PAGE_COUNT              0x10000     // 65536 Pages
    #define SPI_FLASH_N25Q128A__PAGE_SIZE               0x100       // 256 Bytes
    #define SPI_FLASH_N25Q128A__OTP_SIZE                0x40        // 64 Bytes
    #define SPI_FLASH_N25Q128A__ADDRESS_MASK            0xFF

    /***************************************************************************************************/
    /* Command definitions (please see datasheet for more details) */	
    /* These are not all possible defines because a full implementation of the flash was not necessary */

	/* RESET Operations */
    #define SPI_FLASH_N25Q128A__INS_REN		            0x66	// reset enable
	#define SPI_FLASH_N25Q128A__INS_RMEM                0x99	// reset memory

	/* IDENTIFICATION Operations */
	#define SPI_FLASH_N25Q128A__INS_RDID                0x9F	// read Identification
	#define SPI_FLASH_N25Q128A__INS_RDID_ALT            0x9E	// read Identification
	#define SPI_FLASH_N25Q128A__INS_MULT_IO_RDID        0xAF    // read multiple i/o read id
	#define SPI_FLASH_N25Q128A__INS_DISCOVER_PARAMETER  0x5A    // read serial flash discovery parameter

	/* READ operations */
	#define SPI_FLASH_N25Q128A__INS_READ                0x03	// read data bytes
	#define SPI_FLASH_N25Q128A__INS_FAST_READ           0x0B	// read data bytes at higher speed

	/* WRITE operations */
	#define SPI_FLASH_N25Q128A__INS_WREN                0x06    // write enable
	#define SPI_FLASH_N25Q128A__INS_WRDI                0x04    // write disable

	/* REGISTER operations */
	#define SPI_FLASH_N25Q128A__INS_RDSR                0x05    // read status register
	#define SPI_FLASH_N25Q128A__INS_WRSR                0x01    // write status register
	#define SPI_FLASH_N25Q128A__INS_RDLR                0xE8    // read lock register
	#define SPI_FLASH_N25Q128A__INS_CMD_WRLR            0xE5    // write lock register
	#define SPI_FLASH_N25Q128A__INS_RFSR                0x70    // read flag status register
	#define SPI_FLASH_N25Q128A__INS_CLFSR               0x50    // clear flag status register
	#define SPI_FLASH_N25Q128A__INS_RDNVCR              0xB5    // read non volatile configuration register
	#define SPI_FLASH_N25Q128A__INS_WRNVCR              0xB1    // write non volatile configuration register
	#define SPI_FLASH_N25Q128A__INS_RDVCR               0x85    // read volatile configuration register
	#define SPI_FLASH_N25Q128A__INS_WRVCR               0x81    // write volatile configuration register
	#define SPI_FLASH_N25Q128A__INS_RDVECR              0x65    // read volatile enhanced configuration register
	#define SPI_FLASH_N25Q128A__INS_WRVECR              0x61    // write volatile enhanced configuration register

	/* PROGRAM operations */
	#define SPI_FLASH_N25Q128A__INS_PP                  0x02    // PAGE PROGRAM

	/* ERASE operations */
	#define SPI_FLASH_N25Q128A__INS_SSE                 0x20    // sub sector erase
	#define SPI_FLASH_N25Q128A__INS_SE                  0xD8    // sector erase
	#define SPI_FLASH_N25Q128A__INS_DE                  0xC4    // die erase
    #define SPI_FLASH_N25Q128A__INS_BE                  0xC7    // bulk erase
	#define SPI_FLASH_N25Q128A__INS_PER                 0x7A    // program Erase Resume
	#define SPI_FLASH_N25Q128A__INS_PES                 0x75    // program Erase Suspend

	/* OTP operations */
	#define SPI_FLASH_N25Q128A__INS_RDOTP               0x4B    // read OTP array
	#define SPI_FLASH_N25Q128A__INS_PROTP               0x42    // program OTP array

	/* DEEP POWER-DOWN operation */
	#define SPI_FLASH_N25Q128A__INS_ENTERDPD            0xB9    // enter deep power-down
	#define SPI_FLASH_N25Q128A__INS_RELEASEDPD          0xA8    // release deep power-down
	
    /***************************************************************************************************/
    /* Register Definitions */
    
    /* Status Register */
    #define SPI_FLASH_N25Q128A__SR_SRWD                 0x80    // status Register Write Protect
	#define SPI_FLASH_N25Q128A__SR_BP3                  0x40    // block Protect Bit3
	#define SPI_FLASH_N25Q128A__SR_TB                   0x20    // top/Bottom bit
	#define SPI_FLASH_N25Q128A__SR_BP2                  0x10    // block Protect Bit2
	#define SPI_FLASH_N25Q128A__SR_BP1                  0x08    // block Protect Bit1
	#define SPI_FLASH_N25Q128A__SR_BP0                  0x04    // block Protect Bit0
	#define SPI_FLASH_N25Q128A__SR_WEL                  0x02    // write Enable Latch
	#define SPI_FLASH_N25Q128A__SR_WIP                  0x01    // write/Program/Erase in progress bit
    
    /* Flag Status Register */
	#define SPI_FLASH_N25Q128A__FSR_PROG_ERASE_CTL      0x80    // StatusBit - 0=Busy,1=Ready - Indicates whether a PROGRAM, ERASE, WRITE STATUS REGISTER, or WRITE NONVOLATILE CONFIGURATION command cycle is in progress
	#define SPI_FLASH_N25Q128A__FSR_ERASE_SUSP          0x40    // StatusBit - 0=NotInEffect,1=InEffect - Indicates whether an ERASE operation has been or is going to be suspended
	#define SPI_FLASH_N25Q128A__FSR_ERASE               0x20    // ErrorBit -  0=Clear,1=FailureOrProtectionError - Indicates whether an ERASE operation has succeeded or failed
	#define SPI_FLASH_N25Q128A__FSR_PROGRAM             0x10    // ErrorBit -  0=Clear,1=FailureOrProtectionError - Indicates whether a PROGRAM operation has succeeded or failed
	#define SPI_FLASH_N25Q128A__FSR_VPP                 0x08    // ErrorBit -  0=Enabled,1=Disabled(Default) - Indicates an invalid voltage on VPP during a PROGRAM or ERASE operation
	#define SPI_FLASH_N25Q128A__FSR_PROG_SUSP           0x04    // StatusBit - 0=NotInEffect,1=InEffect - Indicates whether a PROGRAM operation has been or is going to be suspended
	#define SPI_FLASH_N25Q128A__FSR_PROT                0x02    // ErrorBit -  0=Clear,1=FailureOrProtectionError - Indicates whether an ERASE or a PROGRAM operation has attempted to modify the protected array sector, or whether a PROGRAM operation has attempted to access the locked OTP space
	#define SPI_FLASH_N25Q128A__FSR_ADDR_MODE           0x01    // Reserved


    //*****************************************************************************************************
    //*****************************************************************************************************
	//  M25P10A (Micron)
	//*****************************************************************************************************
    //*****************************************************************************************************

    /* manufacturer id + mem type + mem capacity  */
    #define SPI_FLASH_M25P10A__MEM_TYPE        	    0x202011    // values from flash spec
    #define SPI_FLASH_M25P10A__MEM_TYPE_MANUFAC_ID	    0x20
    #define SPI_FLASH_M25P10A__MEM_TYPE_MEMTYPE_ID	    0x20
    #define SPI_FLASH_M25P10A__MEM_TYPE_MEMSIZE_ID	    0x11

    /* Flash shape */
    #define SPI_FLASH_M25P10A__SIZE           	        0x20000		// 1 MB
    #define SPI_FLASH_M25P10A__SECTOR_COUNT           	0x4         // 4 Sectors
    #define SPI_FLASH_M25P10A__SECTOR_SIZE				0x8000      // 256 kB    
    #define SPI_FLASH_M25P10A__PAGE_COUNT				0x200       // 512 Pages
    #define SPI_FLASH_M25P10A__PAGE_SIZE				0x100       // 256 Bytes    
    #define SPI_FLASH_M25P10A__ADDRESS_MASK				0xFF

    /***************************************************************************************************/
    /* Command definitions (please see datasheet for more details) */	
    /* These are not all possible defines because a full implementation of the flash was not necessary */

	/* WRITE operations */
	#define SPI_FLASH_M25P10A__INS_WREN                0x06		// write enable
	#define SPI_FLASH_M25P10A__INS_WRDI                0x04		// write disable

	/* IDENTIFICATION Operations */
	#define SPI_FLASH_M25P10A__INS_RDID                0x9F	    // read Identification
	#define SPI_FLASH_M25P10A__INS_RDID_ALT            0x9E		// read Identification

	/* READ operations */
	#define SPI_FLASH_M25P10A__INS_READ                0x03		// read data bytes
	#define SPI_FLASH_M25P10A__INS_FAST_READ           0x0B		// read data bytes at higher speed

	/* REGISTER operations */
	#define SPI_FLASH_M25P10A__INS_RDSR                0x05    // read status register
	#define SPI_FLASH_M25P10A__INS_WRSR                0x01    // write status register

	/* PROGRAM operations */
	#define SPI_FLASH_M25P10A__INS_PP                  0x02    // PAGE PROGRAM

	/* ERASE operations */
	#define SPI_FLASH_M25P10A__INS_SE                  0xD8    // sector erase
	#define SPI_FLASH_M25P10A__INS_BE                  0xC7    // bulk erase

	/* DEEP POWER-DOWN operation */
	#define SPI_FLASH_M25P10A__INS_ENTERDPD            0xB9    // enter deep power-down
	#define SPI_FLASH_M25P10A__INS_RELEASEDPD          0xAB    // release deep power-down
	
    /***************************************************************************************************/
    /* Register Definitions */
    
    /* Status Register */
    #define SPI_FLASH_M25P10A__SR_SRWD                 0x80    // status Register Write Protect
	#define SPI_FLASH_M25P10A__SR_RESERVED2            0x40    // 0
	#define SPI_FLASH_M25P10A__SR_RESERVED1            0x20    // 0
	#define SPI_FLASH_M25P10A__SR_RESERVED0            0x10    // 0
	#define SPI_FLASH_M25P10A__SR_BP1                  0x08    // block Protect Bit1
	#define SPI_FLASH_M25P10A__SR_BP0                  0x04    // block Protect Bit0
	#define SPI_FLASH_M25P10A__SR_WEL                  0x02    // write Enable Latch
	#define SPI_FLASH_M25P10A__SR_WIP                  0x01    // write/Program/Erase in progress bit




    //*****************************************************************************************************
    //*****************************************************************************************************
	//  A25LQ032 (AMIC)
	//*****************************************************************************************************
    //*****************************************************************************************************

    /* manufacturer id + mem type + mem capacity  */
    #define SPI_FLASH_A25LQ032__MEM_TYPE        	    0x374016    // values from flash spec
    #define SPI_FLASH_A25LQ032__MEM_TYPE_MANUFAC_ID	    0x37
    #define SPI_FLASH_A25LQ032__MEM_TYPE_MEMTYPE_ID	    0x40
    #define SPI_FLASH_A25LQ032__MEM_TYPE_MEMSIZE_ID	    0x16

    /* Flash shape */
    #define SPI_FLASH_A25LQ032__SIZE           	        0x400000    // 4 MB
    #define SPI_FLASH_A25LQ032__SECTOR_COUNT           	0x100       // 1024 Sectors
    #define SPI_FLASH_A25LQ032__SECTOR_SIZE           	0x8000      // 4 kB
    #define SPI_FLASH_A25LQ032__PAGE_COUNT              0x4000      // 16384 Pages
    #define SPI_FLASH_A25LQ032__PAGE_SIZE               0x100       // 256 Bytes
    #define SPI_FLASH_A25LQ032__OTP_SIZE                0x40        // 64 Bytes
    #define SPI_FLASH_A25LQ032__ADDRESS_MASK            0xFF

    /***************************************************************************************************/
    /* Command definitions (please see datasheet for more details) */	
    /* These are not all possible defines because a full implementation of the flash was not necessary */

	/* RESET Operations */
    /* no support ---------------------------- */

	/* IDENTIFICATION Operations */
	#define SPI_FLASH_A25LQ032__INS_RDID                0x9F	// Read Device Identification
	#define SPI_FLASH_A25LQ032__INS_REMS                0x90	// Read Electronic Manufacturer & Device Identification

	/* READ operations */
	#define SPI_FLASH_A25LQ032__INS_READ                0x03	// read data bytes
	#define SPI_FLASH_A25LQ032__INS_FAST_READ           0x0B	// read data bytes at higher speed

	/* WRITE operations */
	#define SPI_FLASH_A25LQ032__INS_WREN                0x06    // write enable
	#define SPI_FLASH_A25LQ032__INS_WRDI                0x04    // write disable

	/* REGISTER operations */
	#define SPI_FLASH_A25LQ032__INS_RDSR1               0x05    // read status register 1
    #define SPI_FLASH_A25LQ032__INS_RDSR2               0x35    // read status register 2
	#define SPI_FLASH_A25LQ032__INS_WRSR                0x01    // write status register
	
	/* PROGRAM operations */
	#define SPI_FLASH_A25LQ032__INS_PP                  0x02    // PAGE PROGRAM

	/* ERASE operations */
	#define SPI_FLASH_A25LQ032__INS_SE                  0x20    // sector erase
	#define SPI_FLASH_A25LQ032__INS_BE                  0xD8    // block erase
    #define SPI_FLASH_A25LQ032__INS_CE                  0xC7    // chip erase
	
	/* OTP operations */
	#define SPI_FLASH_A25LQ032__INS_RDOTP               0x4B    // read OTP array
	#define SPI_FLASH_A25LQ032__INS_PROTP               0x42    // program OTP array

	/* DEEP POWER-DOWN operation */
	#define SPI_FLASH_A25LQ032__INS_ENTERDPD            0xB9    // enter deep power-down
	#define SPI_FLASH_A25LQ032__INS_RELEASEDPD          0xAB    // release deep power-down
	
    /***************************************************************************************************/
    /* Register Definitions */
    
    /* Status Register 1 */
    #define SPI_FLASH_A25LQ032__SR_SRP0                 0x80    // status Register Write Protect 0
	#define SPI_FLASH_A25LQ032__SR_SEC                  0x40    // sector protect bit
	#define SPI_FLASH_A25LQ032__SR_TB                   0x20    // top/Bottom bit
	#define SPI_FLASH_A25LQ032__SR_BP2                  0x10    // block Protect Bit2
	#define SPI_FLASH_A25LQ032__SR_BP1                  0x08    // block Protect Bit1
	#define SPI_FLASH_A25LQ032__SR_BP0                  0x04    // block Protect Bit0
	#define SPI_FLASH_A25LQ032__SR_WEL                  0x02    // write Enable Latch
	#define SPI_FLASH_A25LQ032__SR_WIP                  0x01    // write/Program/Erase in progress bit

    /* Status Register 2 */
    #define SPI_FLASH_A25LQ032__SR_RESERVED4            0x80    // reserved
	#define SPI_FLASH_A25LQ032__SR_CMP                  0x40    // complement protect
	#define SPI_FLASH_A25LQ032__SR_RESERVED3            0x20    // reserved
	#define SPI_FLASH_A25LQ032__SR_RESERVED2            0x10    // reserved
	#define SPI_FLASH_A25LQ032__SR_RESERVED1            0x08    // reserved
	#define SPI_FLASH_A25LQ032__SR_APT                  0x04    // all protect
	#define SPI_FLASH_A25LQ032__SR_QE                   0x02    // quad enable
	#define SPI_FLASH_A25LQ032__SR_SRP1                 0x01    // status Register Write Protect 1

#endif

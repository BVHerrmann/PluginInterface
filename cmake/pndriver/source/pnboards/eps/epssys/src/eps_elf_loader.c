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
/*  F i l e               &F: eps_elf_loader.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS ELF File Loader                                                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20094
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <eps_sys.h>        /* Types / Prototypes         */
#include <eps_trc.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define EI_NIDENT 16

// field indices in e_ident array
#define EI_MAG0         0       ///< File identification
#define EI_MAG1         1       ///< File identification
#define EI_MAG2         2       ///< File identification
#define EI_MAG3         3       ///< File identification
#define EI_CLASS        4       ///< File class
#define EI_DATA         5       ///< Data encoding
#define EI_VERSION      6       ///< File version
#define EI_PAD          7       ///< Start of padding bytes
#define EI_NIDENT       16      ///< Size of e_ident[]

// values of EI_DATA field of elf ident
#define ELFDATANONE     0       ///< Invalid data encoding
#define ELFDATA2LSB     1       ///< See below
#define ELFDATA2MSB     2       ///< See below

// values of the fields in e_ident array
#define ELFMAG0         0x7f    ///< e_ident[EI_MAG0]
#define ELFMAG1         'E'     ///< e_ident[EI_MAG1]
#define ELFMAG2         'L'     ///< e_ident[EI_MAG2]
#define ELFMAG3         'F'     ///< e_ident[EI_MAG3]

// value of EI_CLASS field in e_ident array
#define ELFCLASSNONE    0       ///< Invalid class
#define ELFCLASS32      1       ///< 32-bit objects
#define ELFCLASS64      2       ///< 64-bit objects

// values of e_type
#define ET_REL          1       ///< Relocatable file
#define ET_EXEC         2       ///< Executable file
#define ET_DYN          3       ///< Shared object file
#define ET_CORE         4       ///< Core file
#define ET_LOPROC       0xff00  ///< Processor-specific
#define ET_HIPROC       0xffff  ///< Processor-specific
 
// values of program headers member p_type
#define PT_NULL         0
#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4
#define PT_SHLIB        5
#define PT_PHDR         6
#define PT_LOPROC       0x70000000
#define PT_HIPROC       0x7fffffff

//------------------------------------------------------------------------------
// Local types
//------------------------------------------------------------------------------

/**
 * ELF file header
 */
typedef struct {
    LSA_UINT8       e_ident[EI_NIDENT]; /**< ELF identification header                                                      */
    LSA_UINT16      e_type;             /**< object file type, relocatable, executable, shared, or core                     */
    LSA_UINT16      e_machine;          /**< Specifies target instruction set architecture                                  */
    LSA_UINT32      e_version;          /**< 1 for the original version of ELF.                                             */
    LSA_UINT32      e_entry;            /**< memory address of the entry point from where the process starts executing      */
    LSA_UINT32      e_phoff;            /**< Points to the start of the program header table                                */
    LSA_UINT32      e_shoff;            /**< Points to the start of the section header table.                               */
    LSA_UINT32      e_flags;            /**< Interpretation of this field depends on the target architecture.               */
    LSA_UINT16      e_ehsize;           /**< size of this header, normally 64 Bytes for 64-bit and 52 Bytes for 32-bit format.  */
    LSA_UINT16      e_phentsize;        /**< size of a program header table entry.                                          */
    LSA_UINT16      e_phnum;            /**< number of entries in the program header table                                  */
    LSA_UINT16      e_shentsize;        /**< size of a section header table entry                                           */
    LSA_UINT16      e_shnum;            /**< number of entries in the section header table                                  */
    LSA_UINT16      e_shstrndx;         /**< index of the section header table entry that contains the section names        */
} EPS_ELF_FILE_LOADER_E_HEADER_TYPE, *EPS_ELF_FILE_LOADER_E_HEADER_PTR_TYPE;

/**
 * ELF file program header
 */ 
typedef struct {
    LSA_UINT32      p_type;             /**< type of the segment                                                            */
    LSA_UINT32      p_offset;           /**< Offset of the segment in the file image                                        */
    LSA_UINT32      p_vaddr;            /**< Virtual address of the segment in memory                                       */
    LSA_UINT32      p_paddr;            /**< On systems where physical address is relevant, reserved for segment's physical address  */
    LSA_UINT32      p_filesz;           /**< Size in bytes of the segment in the file image                                 */
    LSA_UINT32      p_memsz;            /**< Size in bytes of the segment in memory                                         */
    LSA_UINT32      p_flags;            /**< Segment-dependent flags.                                                       */
    LSA_UINT32      p_align;            /**< Alignment                                                                      */
} EPS_ELF_FILE_LOADER_P_HEADER_TYPE, *EPS_ELF_FILE_LOADER_P_HEADER_PTR_TYPE;

LSA_RESULT eps_elf_loader_verify_header( EPS_ELF_FILE_LOADER_E_HEADER_PTR_TYPE pElfFileHEader);
LSA_RESULT eps_elf_loader_load_program_headers( LSA_UINT8* pFile, EPS_ELF_FILE_LOADER_E_HEADER_PTR_TYPE pElfFileHEader );
LSA_RESULT eps_elf_loader_load_segment( LSA_UINT8* pFile, EPS_ELF_FILE_LOADER_P_HEADER_PTR_TYPE pProgramHeader );

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

/**
 * 
 * @param [in] pFile Elf file base address
 * @param [in] length Length of elf file in byte
 * @param [out] entryAddress Program entry address
 * @return LSA_RET_OK on success, error code otherwise
 */
LSA_RESULT eps_elf_loader_load_firmware( LSA_UINT8* pFile, LSA_UINT32 length, LSA_UINT32* entryAddress )
{
    LSA_RESULT result;
    EPS_ELF_FILE_LOADER_E_HEADER_PTR_TYPE pElfFileHEader;
    
    pElfFileHEader = (EPS_ELF_FILE_LOADER_E_HEADER_PTR_TYPE)pFile;
    
    result = eps_elf_loader_verify_header(pElfFileHEader);
    if(LSA_RET_OK != result)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_elf_loader_load_firmware() - Elf file header verification failed.");
        return result;
    }
    
    result = eps_elf_loader_load_program_headers(pFile, pElfFileHEader);
    if(LSA_RET_OK != result)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_elf_loader_load_firmware() - Loading elf file program headers failed");
        return result;
    }
    
    *entryAddress = pElfFileHEader->e_entry;
    
    return LSA_RET_OK;
}

/**
 * Verify that the elf file is in the correct format and can be loaded
 * 
 * @param [in] pElfFileHEader Elf file header
 * @return LSA_RET_OK on success, error code otherwise
 */
LSA_RESULT eps_elf_loader_verify_header( EPS_ELF_FILE_LOADER_E_HEADER_PTR_TYPE pElfFileHEader)
{
    // test identifications
    if ((pElfFileHEader->e_ident[EI_MAG0] != ELFMAG0)
     || (pElfFileHEader->e_ident[EI_MAG1] != ELFMAG1)
     || (pElfFileHEader->e_ident[EI_MAG2] != ELFMAG2)
     || (pElfFileHEader->e_ident[EI_MAG3] != ELFMAG3))
    {
        // no elf-file
        return LSA_RET_ERR_PARAM;
    }

    // we can only load 32-bit elf files up to now
    if (pElfFileHEader->e_ident[EI_CLASS] != ELFCLASS32)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_elf_loader_verify_header() - Elf file not in 32-bit format");
        return LSA_RET_ERR_PARAM;
    }

    // test endianess
#if !defined LSA_HOST_ENDIANESS_BIG && !defined LSA_HOST_ENDIANESS_LITTLE
#error "LSA_HOST_ENDIANESS_BIG or LSA_HOST_ENDIANESS_LITTLE is missing in file lsa_cfg.h!"
#endif
#ifdef LSA_HOST_ENDIANESS_LITTLE  
    if (pElfFileHEader->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_elf_loader_verify_header() - Elf file endianess not correct");
        // wrong endianess
        return LSA_RET_ERR_PARAM;
    }
#endif
#ifdef LSA_HOST_ENDIANESS_BIG  
    if (pElfFileHEader->e_ident[EI_DATA] != ELFDATA2MSB)
    {
        // wrong endianess
        return LSA_RET_ERR_PARAM;
    }
#endif
    
    // is elf file an executable?
    if (pElfFileHEader->e_type != ET_EXEC)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_elf_loader_verify_header() - Elf file is not an executable");
        return LSA_RET_ERR_PARAM;
    }
    
    // Greenhills traget is not supported
    if(pElfFileHEader->e_flags == 0x170800)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_elf_loader_verify_header() - Greenhills traget is not supported");
        return LSA_RET_ERR_PARAM;  
    }
    
    return LSA_RET_OK;
}

/**
 * Load the ELF program headers. The program header table is an array
 * of structures, each describing a segment or other information the systems needs
 * to prepare the program for execution. An object file segment contains one or more sections.
 * 
 * @param [in] pFile  Elf file base address
 * @param [in] pElfFileHEader Elf file header
 * @return LSA_RET_OK on success, error code otherwise
 */
LSA_RESULT eps_elf_loader_load_program_headers( LSA_UINT8* pFile, EPS_ELF_FILE_LOADER_E_HEADER_PTR_TYPE pElfFileHEader )
{
    EPS_ELF_FILE_LOADER_P_HEADER_PTR_TYPE pProgramHeader;
    LSA_UINT32 offset;
    LSA_UINT32 i;
    LSA_RESULT ret;

    // iterate over all program headers
    for (i = 0; i < pElfFileHEader->e_phnum; i++)
    {
        offset = pElfFileHEader->e_phoff + (i * pElfFileHEader->e_phentsize);
        pProgramHeader = (EPS_ELF_FILE_LOADER_P_HEADER_PTR_TYPE)(pFile + offset);

        // must the segment be loaded?
        if (pProgramHeader->p_type == PT_LOAD)
        {
            ret = eps_elf_loader_load_segment(pFile, pProgramHeader);
            if(ret != LSA_RET_OK)
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_elf_loader_load_program_headers() - Loading segment failed");
                return ret;
            }
        }
    }
    
    return LSA_RET_OK;
}

/**
 * Load a program header segment.
 * The bytes from the file are copied to the specified memory segment. 
 * If the segment's memory size (p_memsz) is larger than the file size
 * (p_filesz) the "extra" bytes are filled with 0
 * 
 * @param [in] pFile  Elf file base address
 * @param [in] pProgramHeader  The program header that describes the segment
 * @return LSA_RET_OK on success, error code otherwise
 */
LSA_RESULT eps_elf_loader_load_segment( LSA_UINT8* pFile, EPS_ELF_FILE_LOADER_P_HEADER_PTR_TYPE pProgramHeader )
{
    // copy program section
    eps_memcpy((LSA_VOID*)pProgramHeader->p_paddr, (LSA_VOID*)(pFile + pProgramHeader->p_offset), pProgramHeader->p_filesz);
    // fill remaining bytes with zero
    eps_memset((LSA_VOID*)(pProgramHeader->p_paddr + pProgramHeader->p_filesz), 0, pProgramHeader->p_memsz - pProgramHeader->p_filesz);
    
    return LSA_RET_OK;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

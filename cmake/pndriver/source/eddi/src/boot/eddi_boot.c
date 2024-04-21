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
/*  F i l e               &F: eddi_boot.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_boot.h"
#include "eddi_ertec_image.h"

#define EDDI_MODULE_ID     M_ID_EDDI_BOOT
#define LTRC_ACT_MODUL_ID  29

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

typedef struct EDDI_IMAGE_HEADER_400
{
    LSA_UINT32    magic;    /* ImageIdent         */
    LSA_UINT32    version;  /* Software Version   */
    LSA_UINT32    ro_base;  /* Image$$RO$$Base    */
    LSA_UINT32    ro_limit; /* Image$$RO$$Limit   */
    LSA_UINT32    rw_base;  /* Image$$RW$$Base    */
    LSA_UINT32    rw_limit; /* Image$$RW$$Limit   */
    LSA_UINT32    zi_base;  /* Image$$ZI$$Base    */
    LSA_UINT32    zi_limit; /* Image$$ZI$$Limit   */
    LSA_UINT32    emif_bus; /* emif 0-3 bus size  */
    LSA_UINT32    chksum;   /* Checksum           */

} EDDI_IMAGE_HEADER_400;

#define   EDDI_KRAM_BOOT_DATA   (0x1020 - 40)

typedef struct _EDDI_KRAM_BOOT
{
    EDDI_IMAGE_HEADER_400     header;
    LSA_UINT8                data[EDDI_KRAM_BOOT_DATA];
    LSA_UINT32               sema_0;
    LSA_UINT32               sema_1;
    LSA_UINT32               sema_2;
    LSA_UINT32               sema_3;

} EDDI_KRAM_BOOT;

static LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_BootSemaEventWait(const volatile LSA_UINT32  * const pSema);

static LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_BootWriteImage( LSA_UINT8                       * const  pBaseAdr,
                                                          LSA_UINT8                       * const  pCopySrc,
                                                          const volatile EDDI_KRAM_BOOT   * const  pKramBoot);


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DisableBootLoader(       EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                       const EDDI_BOOTLOADER_TYPE     *  const  pBootLoader,
                                                             LSA_UINT32                  const  BOARD_SDRAM_BaseAddr )
{
    volatile  EDDI_KRAM_BOOT  *  pKramBoot;
    LSA_UINT32                  sema;
    LSA_UINT8                *  pCopySrc;

    if (pBootLoader->Activate_for_use_as_pci_slave == EDD_FEATURE_ENABLE)
    {
        return;
    }

    switch (pDDB->ERTEC_Version.MacroRevision)
    {
        case 5:
        {
            break;
        }

        case 6:
        case 7:
        default:
        {
            EDDI_Excp("EDDI_DisableBootLoader-> pDDB->ERTEC_Version.MacroRevision", EDDI_FATAL_ERR_EXCP, pDDB->ERTEC_Version.MacroRevision, 0);
            return;
        }
    }

    pKramBoot = (EDDI_KRAM_BOOT *)(void *)pDDB->pKRam;

    // Wait for ERTEC ARM CPU signal ready for HOST
    EDDI_BootSemaEventWait(&pKramBoot->sema_0);

    // Default Initialisierung
    EDDI_MemSet((void *)&pKramBoot->header,  (LSA_UINT8)0x11, (LSA_UINT32)sizeof(EDDI_IMAGE_HEADER_400));
    EDDI_MemSet((void *)&pKramBoot->data[0], (LSA_UINT8)0x22, (LSA_UINT32)EDDI_KRAM_BOOT_DATA);

    /* ImageHeader ins KRAM kopieren ab Offst 0 */
    EDDI_MemCopy((void *)&pKramBoot->header, &files[0].address[0], (LSA_UINT32)sizeof(EDDI_IMAGE_HEADER_400));

    pCopySrc = &files[0].address[sizeof(EDDI_IMAGE_HEADER_400)];
    /* Rest ins SDRAM kopieren */
    EDDI_BootWriteImage((LSA_UINT8 *)BOARD_SDRAM_BaseAddr, pCopySrc, pKramBoot);

    //do not LOCK I-Cache , Init DTCM
    //Sema setzen
    pKramBoot->sema_1 = 0;

    //Host has done Image Initialisation
    sema = pKramBoot->sema_0;

    sema &= 0xFFFFFFFEUL;

    pKramBoot->sema_0 = sema;

    //Image loaded

    EDDI_Excp("EDDI_DisableBootLoader-> STOP !!", EDDI_FATAL_ERR_EXCP, pDDB->ERTEC_Version.MacroRevision, 0);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_BootSemaEventWait( const volatile  LSA_UINT32  *  const  pSema )        //UL!!! guckst Du
{
    LSA_UINT32   reg = 0;
    LSA_UINT32   i;

    for (i = 0; i < 1000; i++)
    {
        reg = *pSema & 0x00000001;
        if (reg)
        {
            return;
        }
    }

    EDDI_Excp("EDDI_BootSemaEventWait - Timeout", EDDI_FATAL_ERR_EXCP, i, 0);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_BootWriteImage()                            */
/*                                                                         */
/* D e s c r i p t i o n: write Image from source to destination that is   */
/*                        specified in the image header                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_BootWriteImage(                 LSA_UINT8       *  const  pBaseAdr,
                                                                            LSA_UINT8       *  const  pCopySrc,
                                                            const volatile  EDDI_KRAM_BOOT  *  const  pKramBoot )
{
    LSA_UINT8   *  pSrc = pCopySrc;
    LSA_UINT32     len;
    LSA_UINT8   *  pDest;

    /* ro-data */
    pDest     = pBaseAdr + (pKramBoot->header.ro_base & 0x0FFFFFFF);
    len       = pKramBoot->header.ro_limit - pKramBoot->header.ro_base;
    EDDI_MemCopy(pDest, pSrc, len);

    /* rw-data */
    pSrc += len;
    pDest = pBaseAdr + (pKramBoot->header.rw_base & 0x0FFFFFFF);
    len   = pKramBoot->header.rw_limit - pKramBoot->header.rw_base;
    EDDI_MemCopy(pDest, pSrc, len);

    /* zi-data */
    pSrc += len;
    pDest = pBaseAdr + (pKramBoot->header.zi_base & 0x0FFFFFFF);
    len   = pKramBoot->header.zi_limit - pKramBoot->header.zi_base;
    EDDI_MemSet(pDest, (LSA_UINT8)0, len);
}
/*---------------------- end [subroutine] ---------------------------------*/


/****************************************************************************/
/*  end of file eddi_boot.c                                                 */
/****************************************************************************/


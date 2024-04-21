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
/*  F i l e               &F: eddi_swi_misc.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDI.                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_swi_ext.h"

//#include "eddi_time.h"
#include "eddi_ser_cmd.h"

#define EDDI_MODULE_ID     M_ID_SWI_MISC
#define LTRC_ACT_MODUL_ID  311

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_SWI_MISC) //satisfy lint!
#endif


/*===========================================================================*/
/*                                local variables                            */
/*===========================================================================*/
#define SWI_PORTINDEX_DUMMY 0xFFFFFFFFUL /* zum Fuellen des Arrays */

//see eddi_swi_int.h
SWI_PORTINDEX_REG_TYPE  const  ERTEC_RegAdr[] =
{
    #if defined (EDDI_CFG_ERTEC_400) /* **************************************************/
    /*  0UL */
    {
        {
            P0_NRT_CTRL,                 P1_NRT_CTRL,                P2_NRT_CTRL,                P3_NRT_CTRL
        }
    },
    /*  1UL */  { {P0_LINE_DELAY,               P1_LINE_DELAY,              P2_LINE_DELAY,              P3_LINE_DELAY              } },
    /*  2UL */  { {P0_MAC_CTRL,                 P1_MAC_CTRL,                P2_MAC_CTRL,                P3_MAC_CTRL                } },
    /*  3UL */  { {P0_PHY_CMD,                  P1_PHY_CMD,                 P2_PHY_CMD,                 P3_PHY_CMD                 } },
    /*  4UL */  { {P0_PHY_STAT,                 P1_PHY_STAT,                P2_PHY_STAT,                P3_PHY_STAT                } },
    /*  5UL */  { {P0_DEFAULT_VLAN_TAG,         P1_DEFAULT_VLAN_TAG,        P2_DEFAULT_VLAN_TAG,        P3_DEFAULT_VLAN_TAG        } },
    /*  6UL */  { {P0_MIN_PREAMBLE,             P1_MIN_PREAMBLE,            P2_MIN_PREAMBLE,            P3_MIN_PREAMBLE            } },
    /*  7UL */  { {P0_ARC_CTRL,                 P1_ARC_CTRL,                P2_ARC_CTRL,                P3_ARC_CTRL                } },
    /*  8UL */  { {P0_TRANS_CTRL,               P1_TRANS_CTRL,              P2_TRANS_CTRL,              P3_TRANS_CTRL              } },
    /*  9UL */  { {P0_RCV_CTRL,                 P1_RCV_CTRL,                P2_RCV_CTRL,                P3_RCV_CTRL                } },
    /* 10UL */  { {P0_IRT_DIAG,                 P1_IRT_DIAG,                P2_IRT_DIAG,                P3_IRT_DIAG                } },
    /* 11UL */  { {STAT_CTRL_PORT0_GBRX,        STAT_CTRL_PORT1_GBRX,       STAT_CTRL_PORT2_GBRX,       STAT_CTRL_PORT3_GBRX       } },
    /* 12UL */  { {STAT_CTRL_PORT0_TBRX,        STAT_CTRL_PORT1_TBRX,       STAT_CTRL_PORT2_TBRX,       STAT_CTRL_PORT3_TBRX       } },
    /* 13UL */  { {STAT_CTRL_PORT0_BTX,         STAT_CTRL_PORT1_BTX,        STAT_CTRL_PORT2_BTX,        STAT_CTRL_PORT3_BTX        } },
    /* 14UL */  { {STAT_CTRL_PORT0_FTX,         STAT_CTRL_PORT1_FTX,        STAT_CTRL_PORT2_FTX,        STAT_CTRL_PORT3_FTX        } },
    /* 15UL */  { {STAT_CTRL_PORT0_UFRX,        STAT_CTRL_PORT1_UFRX,       STAT_CTRL_PORT2_UFRX,       STAT_CTRL_PORT3_UFRX       } },
    /* 16UL */  { {STAT_CTRL_PORT0_UFRXU,       STAT_CTRL_PORT1_UFRXU,      STAT_CTRL_PORT2_UFRXU,      STAT_CTRL_PORT3_UFRXU      } },
    /* 17UL */  { {STAT_CTRL_PORT0_MFRX,        STAT_CTRL_PORT1_MFRX,       STAT_CTRL_PORT2_MFRX,       STAT_CTRL_PORT3_MFRX       } },
    /* 18UL */  { {STAT_CTRL_PORT0_MFRXU,       STAT_CTRL_PORT1_MFRXU,      STAT_CTRL_PORT2_MFRXU,      STAT_CTRL_PORT3_MFRXU      } },
    /* 19UL */  { {STAT_CTRL_PORT0_BFRX,        STAT_CTRL_PORT1_BFRX,       STAT_CTRL_PORT2_BFRX,       STAT_CTRL_PORT3_BFRX       } },
    /* 20UL */  { {STAT_CTRL_PORT0_UFTX,        STAT_CTRL_PORT1_UFTX,       STAT_CTRL_PORT2_UFTX,       STAT_CTRL_PORT3_UFTX       } },
    /* 21UL */  { {STAT_CTRL_PORT0_MFTX,        STAT_CTRL_PORT1_MFTX,       STAT_CTRL_PORT2_MFTX,       STAT_CTRL_PORT3_MFTX       } },
    /* 22UL */  { {STAT_CTRL_PORT0_BFTX,        STAT_CTRL_PORT1_BFTX,       STAT_CTRL_PORT2_BFTX,       STAT_CTRL_PORT3_BFTX       } },
    /* 23UL */  { {STAT_CTRL_PORT0_F0_64,       STAT_CTRL_PORT1_F0_64,      STAT_CTRL_PORT2_F0_64,      STAT_CTRL_PORT3_F0_64      } },
    /* 24UL */  { {STAT_CTRL_PORT0_F64,         STAT_CTRL_PORT1_F64,        STAT_CTRL_PORT2_F64,        STAT_CTRL_PORT3_F64        } },
    /* 25UL */  { {STAT_CTRL_PORT0_F65_127,     STAT_CTRL_PORT1_F65_127,    STAT_CTRL_PORT2_F65_127,    STAT_CTRL_PORT3_F65_127    } },
    /* 26UL */  { {STAT_CTRL_PORT0_F128_255,    STAT_CTRL_PORT1_F128_255,   STAT_CTRL_PORT2_F128_255,   STAT_CTRL_PORT3_F128_255   } },
    /* 27UL */  { {STAT_CTRL_PORT0_F256_511,    STAT_CTRL_PORT1_F256_511,   STAT_CTRL_PORT2_F256_511,   STAT_CTRL_PORT3_F256_511   } },
    /* 28UL */  { {STAT_CTRL_PORT0_F512_1023,   STAT_CTRL_PORT1_F512_1023,  STAT_CTRL_PORT2_F512_1023,  STAT_CTRL_PORT3_F512_1023  } },
    /* 29UL */  { {STAT_CTRL_PORT0_F1024_1536,  STAT_CTRL_PORT1_F1024_1536, STAT_CTRL_PORT2_F1024_1536, STAT_CTRL_PORT3_F1024_1536 } },
    /* 30UL */  { {STAT_CTRL_PORT0_GFRX,        STAT_CTRL_PORT1_GFRX,       STAT_CTRL_PORT2_GFRX,       STAT_CTRL_PORT3_GFRX       } },
    /* 31UL */  { {STAT_CTRL_PORT0_TFRX,        STAT_CTRL_PORT1_TFRX,       STAT_CTRL_PORT2_TFRX,       STAT_CTRL_PORT3_TFRX       } },
    /* 32UL */  { {STAT_CTRL_PORT0_FRX0_64_CRC, STAT_CTRL_PORT1_FRX0_64_CRC,STAT_CTRL_PORT2_FRX0_64_CRC,STAT_CTRL_PORT3_FRX0_64_CRC} },
    /* 33UL */  { {STAT_CTRL_PORT0_RXOF,        STAT_CTRL_PORT1_RXOF,       STAT_CTRL_PORT2_RXOF,       STAT_CTRL_PORT3_RXOF       } },
    /* 34UL */  { {STAT_CTRL_PORT0_RXOSF,       STAT_CTRL_PORT1_RXOSF,      STAT_CTRL_PORT2_RXOSF,      STAT_CTRL_PORT3_RXOSF      } },
    /* 35UL */  { {STAT_CTRL_PORT0_CRCER,       STAT_CTRL_PORT1_CRCER,      STAT_CTRL_PORT2_CRCER,      STAT_CTRL_PORT3_CRCER      } },
    /* 36UL */  { {STAT_CTRL_PORT0_FDROP,       STAT_CTRL_PORT1_FDROP,      STAT_CTRL_PORT2_FDROP,      STAT_CTRL_PORT3_FDROP      } },
    /* 37UL */  { {STAT_CTRL_PORT0_TXCOL,       STAT_CTRL_PORT1_TXCOL,      STAT_CTRL_PORT2_TXCOL,      STAT_CTRL_PORT3_TXCOL      } },
    /* 38UL */  { {STAT_CTRL_PORT0_HOLBC,       STAT_CTRL_PORT1_HOLBC,      STAT_CTRL_PORT2_HOLBC,      STAT_CTRL_PORT3_HOLBC      } },
    /* 39UL */  { {HOL_MASK_P0,                 HOL_MASK_P1,                HOL_MASK_P2,                HOL_MASK_P3                } },
    /* 40UL */  { {LIST_STATUS_P0,              LIST_STATUS_P1,             LIST_STATUS_P2,             LIST_STATUS_P3             } }
    #elif defined (EDDI_CFG_ERTEC_200) /* ********************************/
    /*  0UL */
    {
        {
            P0_NRT_CTRL,                 P1_NRT_CTRL,                SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY
        }
    },
    /*  1UL */  { {P0_LINE_DELAY,               P1_LINE_DELAY,              SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /*  2UL */  { {P0_MAC_CTRL,                 P1_MAC_CTRL,                SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /*  3UL */  { {P0_PHY_CMD,                  P1_PHY_CMD,                 SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /*  4UL */  { {P0_PHY_STAT,                 P1_PHY_STAT,                SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /*  5UL */  { {P0_DEFAULT_VLAN_TAG,         P1_DEFAULT_VLAN_TAG,        SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /*  6UL */  { {P0_MIN_PREAMBLE,             P1_MIN_PREAMBLE,            SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /*  7UL */  { {P0_ARC_CTRL,                 P1_ARC_CTRL,                SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /*  8UL */  { {P0_TRANS_CTRL,               P1_TRANS_CTRL,              SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /*  9UL */  { {P0_RCV_CTRL,                 P1_RCV_CTRL,                SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 10UL */  { {P0_IRT_DIAG,                 P1_IRT_DIAG,                SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 11UL */  { {STAT_CTRL_PORT0_GBRX,        STAT_CTRL_PORT1_GBRX,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 12UL */  { {STAT_CTRL_PORT0_TBRX,        STAT_CTRL_PORT1_TBRX,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 13UL */  { {STAT_CTRL_PORT0_BTX,         STAT_CTRL_PORT1_BTX,        SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 14UL */  { {STAT_CTRL_PORT0_FTX,         STAT_CTRL_PORT1_FTX,        SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 15UL */  { {STAT_CTRL_PORT0_UFRX,        STAT_CTRL_PORT1_UFRX,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 16UL */  { {STAT_CTRL_PORT0_UFRXU,       STAT_CTRL_PORT1_UFRXU,      SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 17UL */  { {STAT_CTRL_PORT0_MFRX,        STAT_CTRL_PORT1_MFRX,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 18UL */  { {STAT_CTRL_PORT0_MFRXU,       STAT_CTRL_PORT1_MFRXU,      SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 19UL */  { {STAT_CTRL_PORT0_BFRX,        STAT_CTRL_PORT1_BFRX,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 20UL */  { {STAT_CTRL_PORT0_UFTX,        STAT_CTRL_PORT1_UFTX,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 21UL */  { {STAT_CTRL_PORT0_MFTX,        STAT_CTRL_PORT1_MFTX,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 22UL */  { {STAT_CTRL_PORT0_BFTX,        STAT_CTRL_PORT1_BFTX,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 23UL */  { {STAT_CTRL_PORT0_F0_64,       STAT_CTRL_PORT1_F0_64,      SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 24UL */  { {STAT_CTRL_PORT0_F64,         STAT_CTRL_PORT1_F64,        SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 25UL */  { {STAT_CTRL_PORT0_F65_127,     STAT_CTRL_PORT1_F65_127,    SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 26UL */  { {STAT_CTRL_PORT0_F128_255,    STAT_CTRL_PORT1_F128_255,   SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 27UL */  { {STAT_CTRL_PORT0_F256_511,    STAT_CTRL_PORT1_F256_511,   SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 28UL */  { {STAT_CTRL_PORT0_F512_1023,   STAT_CTRL_PORT1_F512_1023,  SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 29UL */  { {STAT_CTRL_PORT0_F1024_1536,  STAT_CTRL_PORT1_F1024_1536, SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 30UL */  { {STAT_CTRL_PORT0_GFRX,        STAT_CTRL_PORT1_GFRX,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 31UL */  { {STAT_CTRL_PORT0_TFRX,        STAT_CTRL_PORT1_TFRX,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 32UL */  { {STAT_CTRL_PORT0_FRX0_64_CRC, STAT_CTRL_PORT1_FRX0_64_CRC,SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 33UL */  { {STAT_CTRL_PORT0_RXOF,        STAT_CTRL_PORT1_RXOF,       SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 34UL */  { {STAT_CTRL_PORT0_RXOSF,       STAT_CTRL_PORT1_RXOSF,      SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 35UL */  { {STAT_CTRL_PORT0_CRCER,       STAT_CTRL_PORT1_CRCER,      SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 36UL */  { {STAT_CTRL_PORT0_FDROP,       STAT_CTRL_PORT1_FDROP,      SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 37UL */  { {STAT_CTRL_PORT0_TXCOL,       STAT_CTRL_PORT1_TXCOL,      SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 38UL */  { {STAT_CTRL_PORT0_HOLBC,       STAT_CTRL_PORT1_HOLBC,      SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 39UL */  { {HOL_MASK_P0,                 HOL_MASK_P1,                SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 40UL */  { {STAT_CTRL_PORT0_CC_DIFF,     STAT_CTRL_PORT1_CC_DIFF,    SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 41UL */  { {STAT_CTRL_PORT0_CC_DELAY,    STAT_CTRL_PORT1_CC_DELAY,   SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 42UL */  { {STAT_CTRL_PORT0_CC_COUNT,    STAT_CTRL_PORT1_CC_COUNT,   SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } },
    /* 43UL */  { {LIST_STATUS_P0,              LIST_STATUS_P1,             SWI_PORTINDEX_DUMMY,        SWI_PORTINDEX_DUMMY        } }
    #elif defined (EDDI_CFG_SOC) /* ********************************/
    /*  0UL */
    {
        {
            P0_NRT_CTRL,                 P1_NRT_CTRL,                P2_NRT_CTRL,                SWI_PORTINDEX_DUMMY
        }
    },
    /*  1UL */  { {P0_LINE_DELAY,               P1_LINE_DELAY,              P2_LINE_DELAY,              SWI_PORTINDEX_DUMMY        } },
    /*  2UL */  { {P0_MAC_CTRL,                 P1_MAC_CTRL,                P2_MAC_CTRL,                SWI_PORTINDEX_DUMMY        } },
    /*  3UL */  { {P0_PHY_CMD,                  P1_PHY_CMD,                 P2_PHY_CMD,                 SWI_PORTINDEX_DUMMY        } },
    /*  4UL */  { {P0_PHY_STAT,                 P1_PHY_STAT,                P2_PHY_STAT,                SWI_PORTINDEX_DUMMY        } },
    /*  5UL */  { {P0_DEFAULT_VLAN_TAG,         P1_DEFAULT_VLAN_TAG,        P2_DEFAULT_VLAN_TAG,        SWI_PORTINDEX_DUMMY        } },
    /*  6UL */  { {P0_MIN_PREAMBLE,             P1_MIN_PREAMBLE,            P2_MIN_PREAMBLE,            SWI_PORTINDEX_DUMMY        } },
    /*  7UL */  { {P0_ARC_CTRL,                 P1_ARC_CTRL,                P2_ARC_CTRL,                SWI_PORTINDEX_DUMMY        } },
    /*  8UL */  { {P0_TRANS_CTRL,               P1_TRANS_CTRL,              P2_TRANS_CTRL,              SWI_PORTINDEX_DUMMY        } },
    /*  9UL */  { {P0_RCV_CTRL,                 P1_RCV_CTRL,                P2_RCV_CTRL,                SWI_PORTINDEX_DUMMY        } },
    /* 10UL */  { {P0_IRT_DIAG,                 P1_IRT_DIAG,                P2_IRT_DIAG,                SWI_PORTINDEX_DUMMY        } },
    /* 11UL */  { {STAT_CTRL_PORT0_GBRX,        STAT_CTRL_PORT1_GBRX,       STAT_CTRL_PORT2_GBRX,       SWI_PORTINDEX_DUMMY        } },
    /* 12UL */  { {STAT_CTRL_PORT0_TBRX,        STAT_CTRL_PORT1_TBRX,       STAT_CTRL_PORT2_TBRX,       SWI_PORTINDEX_DUMMY        } },
    /* 13UL */  { {STAT_CTRL_PORT0_BTX,         STAT_CTRL_PORT1_BTX,        STAT_CTRL_PORT2_BTX,        SWI_PORTINDEX_DUMMY        } },
    /* 14UL */  { {STAT_CTRL_PORT0_FTX,         STAT_CTRL_PORT1_FTX,        STAT_CTRL_PORT2_FTX,        SWI_PORTINDEX_DUMMY        } },
    /* 15UL */  { {STAT_CTRL_PORT0_UFRX,        STAT_CTRL_PORT1_UFRX,       STAT_CTRL_PORT2_UFRX,       SWI_PORTINDEX_DUMMY        } },
    /* 16UL */  { {STAT_CTRL_PORT0_UFRXU,       STAT_CTRL_PORT1_UFRXU,      STAT_CTRL_PORT2_UFRXU,      SWI_PORTINDEX_DUMMY        } },
    /* 17UL */  { {STAT_CTRL_PORT0_MFRX,        STAT_CTRL_PORT1_MFRX,       STAT_CTRL_PORT2_MFRX,       SWI_PORTINDEX_DUMMY        } },
    /* 18UL */  { {STAT_CTRL_PORT0_MFRXU,       STAT_CTRL_PORT1_MFRXU,      STAT_CTRL_PORT2_MFRXU,      SWI_PORTINDEX_DUMMY        } },
    /* 19UL */  { {STAT_CTRL_PORT0_BFRX,        STAT_CTRL_PORT1_BFRX,       STAT_CTRL_PORT2_BFRX,       SWI_PORTINDEX_DUMMY        } },
    /* 20UL */  { {STAT_CTRL_PORT0_UFTX,        STAT_CTRL_PORT1_UFTX,       STAT_CTRL_PORT2_UFTX,       SWI_PORTINDEX_DUMMY        } },
    /* 21UL */  { {STAT_CTRL_PORT0_MFTX,        STAT_CTRL_PORT1_MFTX,       STAT_CTRL_PORT2_MFTX,       SWI_PORTINDEX_DUMMY        } },
    /* 22UL */  { {STAT_CTRL_PORT0_BFTX,        STAT_CTRL_PORT1_BFTX,       STAT_CTRL_PORT2_BFTX,       SWI_PORTINDEX_DUMMY        } },
    /* 23UL */  { {STAT_CTRL_PORT0_F0_64,       STAT_CTRL_PORT1_F0_64,      STAT_CTRL_PORT2_F0_64,      SWI_PORTINDEX_DUMMY        } },
    /* 24UL */  { {STAT_CTRL_PORT0_F64,         STAT_CTRL_PORT1_F64,        STAT_CTRL_PORT2_F64,        SWI_PORTINDEX_DUMMY        } },
    /* 25UL */  { {STAT_CTRL_PORT0_F65_127,     STAT_CTRL_PORT1_F65_127,    STAT_CTRL_PORT2_F65_127,    SWI_PORTINDEX_DUMMY        } },
    /* 26UL */  { {STAT_CTRL_PORT0_F128_255,    STAT_CTRL_PORT1_F128_255,   STAT_CTRL_PORT2_F128_255,   SWI_PORTINDEX_DUMMY        } },
    /* 27UL */  { {STAT_CTRL_PORT0_F256_511,    STAT_CTRL_PORT1_F256_511,   STAT_CTRL_PORT2_F256_511,   SWI_PORTINDEX_DUMMY        } },
    /* 28UL */  { {STAT_CTRL_PORT0_F512_1023,   STAT_CTRL_PORT1_F512_1023,  STAT_CTRL_PORT2_F512_1023,  SWI_PORTINDEX_DUMMY        } },
    /* 29UL */  { {STAT_CTRL_PORT0_F1024_1536,  STAT_CTRL_PORT1_F1024_1536, STAT_CTRL_PORT2_F1024_1536, SWI_PORTINDEX_DUMMY        } },
    /* 30UL */  { {STAT_CTRL_PORT0_GFRX,        STAT_CTRL_PORT1_GFRX,       STAT_CTRL_PORT2_GFRX,       SWI_PORTINDEX_DUMMY        } },
    /* 31UL */  { {STAT_CTRL_PORT0_TFRX,        STAT_CTRL_PORT1_TFRX,       STAT_CTRL_PORT2_TFRX,       SWI_PORTINDEX_DUMMY        } },
    /* 32UL */  { {STAT_CTRL_PORT0_FRX0_64_CRC, STAT_CTRL_PORT1_FRX0_64_CRC,STAT_CTRL_PORT2_FRX0_64_CRC,SWI_PORTINDEX_DUMMY        } },
    /* 33UL */  { {STAT_CTRL_PORT0_RXOF,        STAT_CTRL_PORT1_RXOF,       STAT_CTRL_PORT2_RXOF,       SWI_PORTINDEX_DUMMY        } },
    /* 34UL */  { {STAT_CTRL_PORT0_RXOSF,       STAT_CTRL_PORT1_RXOSF,      STAT_CTRL_PORT2_RXOSF,      SWI_PORTINDEX_DUMMY        } },
    /* 35UL */  { {STAT_CTRL_PORT0_CRCER,       STAT_CTRL_PORT1_CRCER,      STAT_CTRL_PORT2_CRCER,      SWI_PORTINDEX_DUMMY        } },
    /* 36UL */  { {STAT_CTRL_PORT0_FDROP,       STAT_CTRL_PORT1_FDROP,      STAT_CTRL_PORT2_FDROP,      SWI_PORTINDEX_DUMMY        } },
    /* 37UL */  { {STAT_CTRL_PORT0_TXCOL,       STAT_CTRL_PORT1_TXCOL,      STAT_CTRL_PORT2_TXCOL,      SWI_PORTINDEX_DUMMY        } },
    /* 38UL */  { {STAT_CTRL_PORT0_HOLBC,       STAT_CTRL_PORT1_HOLBC,      STAT_CTRL_PORT2_HOLBC,      SWI_PORTINDEX_DUMMY        } },
    /* 39UL */  { {HOL_MASK_P0,                 HOL_MASK_P1,                HOL_MASK_P2,                SWI_PORTINDEX_DUMMY        } },
    /* 40UL */  { {STAT_CTRL_PORT0_CC_DIFF,     STAT_CTRL_PORT1_CC_DIFF,    STAT_CTRL_PORT2_CC_DIFF,    SWI_PORTINDEX_DUMMY        } },
    /* 41UL */  { {STAT_CTRL_PORT0_CC_DELAY,    STAT_CTRL_PORT1_CC_DELAY,   STAT_CTRL_PORT2_CC_DELAY,   SWI_PORTINDEX_DUMMY        } },
    /* 42UL */  { {STAT_CTRL_PORT0_CC_COUNT,    STAT_CTRL_PORT1_CC_COUNT,   STAT_CTRL_PORT2_CC_COUNT,   SWI_PORTINDEX_DUMMY        } },
    /* 43UL */  { {LIST_STATUS_P0,              LIST_STATUS_P1,             LIST_STATUS_P2,             SWI_PORTINDEX_DUMMY        } }
    #else
    #error EDDI_CFG_ERROR: Hardware not defined !
    #endif
};


/*=============================================================================
* function name: EDDI_SwiMiscGetRegisterNoSwap()
*
* function:
*
* parameters:
*
* return value:
*
* comment:
*==========================================================================*/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SwiMiscGetRegisterNoSwap( LSA_UINT32              const  HwPortIndex,
                                                                LSA_UINT32              const  RegType,
                                                                EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  RegisterVal;

    if (HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt)
    {
        EDDI_Excp("EDDI_SwiMiscGetRegisterNoSwap, HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt", EDDI_FATAL_ERR_EXCP, HwPortIndex, pDDB->PM.HwTypeMaxPortCnt);
        return 0;
    }

    if (RegType >= (sizeof(ERTEC_RegAdr) / sizeof(SWI_PORTINDEX_REG_TYPE)) )
    {
        EDDI_Excp("EDDI_SwiMiscGetRegisterNoSwap, RegType >= REG_MAX", EDDI_FATAL_ERR_EXCP, HwPortIndex, RegType);
        return 0;
    }

    if (SWI_PORTINDEX_DUMMY == ERTEC_RegAdr[RegType].HWPort[HwPortIndex])
    {
        EDDI_Excp("EDDI_SwiMiscGetRegisterNoSwap, SWI_PORTINDEX_DUMMY", EDDI_FATAL_ERR_EXCP, HwPortIndex, SWI_PORTINDEX_DUMMY);
        return 0;
    }

    RegisterVal = IO_x32(ERTEC_RegAdr[RegType].HWPort[HwPortIndex]);

    EDDI_SWI_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiMiscGetRegisterNoSwap->HwPortIndex:0x%X RegType:0x%X, RegVal:0x%X", 
                      HwPortIndex, RegType, RegisterVal);

    return RegisterVal;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiMiscGetRegister()
*
* function:
*
* parameters:
*
* return value:
*
* comment:
*==========================================================================*/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SwiMiscGetRegister( LSA_UINT32              const  HwPortIndex,
                                                          LSA_UINT32              const  RegType,
                                                          EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  RegisterVal;

    if (HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt)
    {
        EDDI_Excp("EDDI_SwiMiscGetRegister, HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt", EDDI_FATAL_ERR_EXCP, HwPortIndex, pDDB->PM.HwTypeMaxPortCnt);
        return 0;
    }

    if (RegType >= (sizeof(ERTEC_RegAdr) / sizeof(SWI_PORTINDEX_REG_TYPE)))
    {
        EDDI_Excp("EDDI_SwiMiscGetRegister, RegType >= REG_MAX", EDDI_FATAL_ERR_EXCP, HwPortIndex, RegType);
        return 0;
    }

    if (SWI_PORTINDEX_DUMMY == ERTEC_RegAdr[RegType].HWPort[HwPortIndex])
    {
        EDDI_Excp("EDDI_SwiMiscGetRegister, SWI_PORTINDEX_DUMMY", EDDI_FATAL_ERR_EXCP, HwPortIndex, SWI_PORTINDEX_DUMMY);
        return 0;
    }

    RegisterVal = IO_R32(ERTEC_RegAdr[RegType].HWPort[HwPortIndex]);

    EDDI_SWI_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiMiscGetRegister->HwPortIndex:0x%X RegType:0x%X, RegVal:0x%X", 
                      HwPortIndex, RegType, RegisterVal);
    return RegisterVal;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiMiscSetRegisterNoSwap()
*
* function:
*
* parameters:
*
* return value:
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiMiscSetRegisterNoSwap( LSA_UINT32              const  HwPortIndex,
                                                              LSA_UINT32              const  RegType,
                                                              LSA_UINT32              const  RegisterVal,
                                                              EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiMiscSetRegisterNoSwap->RegType:0x%X RegisterVal:0x%X", 
                      RegType, RegisterVal);

    if (HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt)
    {
        EDDI_Excp("EDDI_SwiMiscSetRegisterNoSwap, HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt", EDDI_FATAL_ERR_EXCP, HwPortIndex, pDDB->PM.HwTypeMaxPortCnt);
        return;
    }

    if (RegType >= (sizeof(ERTEC_RegAdr) / sizeof(SWI_PORTINDEX_REG_TYPE)))
    {
        EDDI_Excp("EDDI_SwiMiscSetRegisterNoSwap, RegType >= REG_MAX", EDDI_FATAL_ERR_EXCP, HwPortIndex, RegType);
        return;
    }

    if (SWI_PORTINDEX_DUMMY == ERTEC_RegAdr[RegType].HWPort[HwPortIndex])
    {
        EDDI_Excp("EDDI_SwiMiscSetRegisterNoSwap, SWI_PORTINDEX_DUMMY", EDDI_FATAL_ERR_EXCP, HwPortIndex, SWI_PORTINDEX_DUMMY);
        return;
    }

    IO_x32(ERTEC_RegAdr[RegType].HWPort[HwPortIndex]) = RegisterVal;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiMiscSetRegister()
*
* function:
*
* parameters:
*
* return value:
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiMiscSetRegister( LSA_UINT32              const  HwPortIndex,
                                                        LSA_UINT32              const  RegType,
                                                        LSA_UINT32              const  RegisterVal,
                                                        EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiMiscSetRegister->RegType:0x%X RegisterVal:0x%X", 
                      RegType, RegisterVal);

    if (HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt)
    {
        EDDI_Excp("EDDI_SwiMiscSetRegister, HwPortIndex >= pDDB->PM.HwTypeMaxPortCnt", EDDI_FATAL_ERR_EXCP, HwPortIndex, pDDB->PM.HwTypeMaxPortCnt);
        return;
    }

    if (RegType >= (sizeof(ERTEC_RegAdr) / sizeof(SWI_PORTINDEX_REG_TYPE)))
    {
        EDDI_Excp("EDDI_SwiMiscSetRegister, RegType >= REG_MAX", EDDI_FATAL_ERR_EXCP, HwPortIndex, RegType);
        return;
    }

    if (SWI_PORTINDEX_DUMMY == ERTEC_RegAdr[RegType].HWPort[HwPortIndex])
    {
        EDDI_Excp("EDDI_SwiMiscSetRegister, SWI_PORTINDEX_DUMMY", EDDI_FATAL_ERR_EXCP, HwPortIndex, SWI_PORTINDEX_DUMMY);
        return;
    }

    IO_W32(ERTEC_RegAdr[RegType].HWPort[HwPortIndex], RegisterVal);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiMiscSetClosedDisabled()
*
* function:
*
* parameters:
*
* return value:
*
* comment:
*==========================================================================*/
LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SwiMiscSetClosedDisabled( LSA_UINT32 const HwPortIndex,
                                                              LSA_BOOL   const Closed,
                                                              LSA_BOOL   const Disabled,
                                                              EDDI_LOCAL_DDB_PTR_TYPE const pDDB)
{
    LSA_UINT32  PortCtrl;
    LSA_UINT32  HWDropCtr;
    LSA_BOOL    bPortIsClosedOrDisabled;

    PortCtrl = IO_x32(PORT_CTRL);

    EDDI_SWI_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiMiscSetClosedDisabled->HwPortIndex:0x%X Disabled:0x%X Closed:0x%X", 
                      HwPortIndex, Disabled, Closed);

    switch (HwPortIndex)
    {
        case SWI_PORTINDEX_0:
        {
            bPortIsClosedOrDisabled = (EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableClosedP0)) ?LSA_TRUE:LSA_FALSE;
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__ClosedP0,  (Closed   != LSA_FALSE) ? 1 : 0);
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP0, (Disabled != LSA_FALSE) ? 1 : 0);
        }
        break;

        case SWI_PORTINDEX_1:
        {
            bPortIsClosedOrDisabled = (EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableClosedP1)) ?LSA_TRUE:LSA_FALSE;
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__ClosedP1  , (Closed   != LSA_FALSE) ? 1 : 0);
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP1 , (Disabled != LSA_FALSE) ? 1 : 0);
        }
        break;

        case SWI_PORTINDEX_2:
        {
            bPortIsClosedOrDisabled = (EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableClosedP2)) ?LSA_TRUE:LSA_FALSE;
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__ClosedP2  , (Closed   != LSA_FALSE) ? 1 : 0);
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP2 , (Disabled != LSA_FALSE) ? 1 : 0);
        }
        break;

        case SWI_PORTINDEX_3:
        {
            bPortIsClosedOrDisabled = (EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableClosedP3)) ?LSA_TRUE:LSA_FALSE;
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__ClosedP3  , (Closed   != LSA_FALSE) ? 1 : 0);
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP3 , (Disabled != LSA_FALSE) ? 1 : 0);
        }
        break;

        default:
        {
            bPortIsClosedOrDisabled = LSA_FALSE;
            EDDI_Excp("EDDI_SwiMiscSetClosedDisabled HwPortIndex > pDDB->PM.HwTypeMaxPortCnt", EDDI_FATAL_ERR_EXCP,
                      HwPortIndex, pDDB->PM.HwTypeMaxPortCnt);
            break;
        }
    }

    //Workaround for a bug in Rev6/Rev7: 
    //The DropCtr starts counting when receiving frames in DISABLED or CLOSED state
    if (   !bPortIsClosedOrDisabled 
        && (Closed || Disabled) )
    {
        //transition from OPEN to CLOSED ==> Get current HWDropCtr and store current substituted drop ctr:
        //1. get last dropctr in OPEN state ...
        HWDropCtr = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_FDROP, pDDB);
        //CurrClosedDropCtr = HWDropCtr
        //SubstClosedDropCtr = HWDropCtr - AccCorrDropCtr
        pDDB->LocalIFStats.CurrClosedDropCtr[HwPortIndex]  = HWDropCtr;
        //if the sequence OPEN->CLOSE->OPEN had been triggered by LinkDown/LinkUp, more illegal drops could have been calculated than really happened.
        //in this case, we can do nothing than limit the difference to 0
        if (pDDB->LocalIFStats.AccCorrDropCtr[HwPortIndex] > HWDropCtr)
        {
            pDDB->LocalIFStats.AccCorrDropCtr[HwPortIndex] = HWDropCtr;
        }
        pDDB->LocalIFStats.SubstClosedDropCtr[HwPortIndex] = HWDropCtr - pDDB->LocalIFStats.AccCorrDropCtr[HwPortIndex];
        pDDB->LocalIFStats.bOpen[HwPortIndex]              = LSA_FALSE;

        //2. ... then close port ...
    }

    IO_x32(PORT_CTRL) = PortCtrl;

    if (   bPortIsClosedOrDisabled
             && (!Closed && !Disabled) )
    {
        //transition from CLOSED to OPEN ==> Calculate accumulated drop ctr: 
        //1. First open port ...
        //2. ... then get dropctr in OPEN state.
        HWDropCtr = EDDI_SwiMiscGetRegister(HwPortIndex, REG_STAT_CTRL_FDROP, pDDB);
        //AccCorrDropCtr = AccCorrDropCtr + (HWDropCtr - CurrClosedDropCtr)
        pDDB->LocalIFStats.AccCorrDropCtr[HwPortIndex] += (HWDropCtr - pDDB->LocalIFStats.CurrClosedDropCtr[HwPortIndex]);
        //if the sequence OPEN->CLOSE->OPEN had been triggered by LinkDown/LinkUp, more illegal drops could have been calculated than really happened.
        //in this case, we can do nothing than limit the difference to 0
        if (pDDB->LocalIFStats.AccCorrDropCtr[HwPortIndex] > HWDropCtr)
        {
            pDDB->LocalIFStats.AccCorrDropCtr[HwPortIndex] = HWDropCtr;
        }
        pDDB->LocalIFStats.bOpen[HwPortIndex]           = LSA_TRUE;
    }

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SwiMiscSetClosedDisabled<-Disabled:0x%X Closed:0x%X", Disabled, Closed);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiMiscGetClosedDisabled()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiMiscGetClosedDisabled( LSA_UINT32               const  HwPortIndex,
                                                              LSA_BOOL                     *  pClosed,
                                                              LSA_BOOL                     *  pDisabled,
                                                              EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  PortCtrl;

    PortCtrl = IO_x32(PORT_CTRL);

    switch (HwPortIndex)
    {
        case SWI_PORTINDEX_0:
        {
            *pClosed    = (LSA_BOOL)((EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__ClosedP0)  == 1) ? LSA_TRUE : LSA_FALSE);
            *pDisabled  = (LSA_BOOL)((EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP0) == 1) ? LSA_TRUE : LSA_FALSE);
        }
        break;

        case SWI_PORTINDEX_1:
        {
            *pClosed    = (LSA_BOOL)((EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__ClosedP1)  == 1) ? LSA_TRUE : LSA_FALSE);
            *pDisabled  = (LSA_BOOL)((EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP1) == 1) ? LSA_TRUE : LSA_FALSE);
        }
        break;

        case SWI_PORTINDEX_2:
        {
            *pClosed    = (LSA_BOOL)((EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__ClosedP2)  == 1) ? LSA_TRUE : LSA_FALSE);
            *pDisabled  = (LSA_BOOL)((EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP2) == 1) ? LSA_TRUE : LSA_FALSE);
        }
        break;

        case SWI_PORTINDEX_3:
        {
            *pClosed    = (LSA_BOOL)((EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__ClosedP3)  == 1) ? LSA_TRUE : LSA_FALSE);
            *pDisabled  = (LSA_BOOL)((EDDI_GetBitField32(PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP3) == 1) ? LSA_TRUE : LSA_FALSE);
        }
        break;

        default:
        {
            EDDI_Excp("EDDI_SwiMiscGetClosedDisabled, invalid HwPortIndex", EDDI_FATAL_ERR_EXCP, HwPortIndex, 0);
            break;
        }
    }

    EDDI_SWI_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiMiscGetClosedDisabled<-HwPortIndex:0x%X Disabled:0x%X Closed:0x%X", HwPortIndex, *pDisabled, *pClosed);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiMiscResetPortSendQueue()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiMiscResetPortSendQueue( LSA_UINT32               const  HwPortIndex,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  SerQueue;
    LSA_UINT32  Qu_No = 1;

    SerQueue = 0;

    //Port-Queue
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Qu_Type, 0);

    Qu_No <<= HwPortIndex; // 0001: Port 0
                           // 0010: Port 1
                           // 0100: Port 2
                           // 1000: Port 3

    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Qu_No, Qu_No);

    //delete all Send Queues
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_0 , 1);
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_1 , 1);
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_2 , 1);
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_3 , 1);
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_4 , 1);
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_5 , 1);
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_6 , 1);
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_7 , 1);
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_8 , 1);
    EDDI_SetBitField32(&SerQueue, EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_9 , 1);

    //EDDI_SetBitField32 already swaps SerQueue.
    //Here SerQueue must be swapped again, because EDDI_SERSingleDirectCmd is swapping too!
    EDDI_SERSingleDirectCmd(FCODE_RESET_TX_QUEUE, EDDI_IRTE2HOST32(SerQueue), 0, pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: SERSetPortDisable()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiMiscSetDisable( LSA_UINT32               const  HwPortIndex,
                                                       LSA_BOOL                 const  bDisabled,
                                                       EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  PortCtrl;

    PortCtrl = IO_x32(PORT_CTRL);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiMiscSetDisable->HwPortIndex:0x%X bDisabled:0x%X",
                      HwPortIndex, bDisabled);

    switch (HwPortIndex)
    {
        case SWI_PORTINDEX_0:
        {
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP0 , (bDisabled) ? 1 : 0);
        }
        break;

        case SWI_PORTINDEX_1:
        {
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP1 , (bDisabled) ? 1 : 0);
        }
        break;

        case SWI_PORTINDEX_2:
        {
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP2 , (bDisabled) ? 1 : 0);
        }
        break;

        case SWI_PORTINDEX_3:
        {
            EDDI_SetBitField32(&PortCtrl, EDDI_SER_PORT_CTRL_BIT__DisableP3 , (bDisabled) ? 1 : 0);
        }
        break;

        default:
        {
            EDDI_Excp("EDDI_SwiMiscSetDisable, invalid HwPortIndex", EDDI_FATAL_ERR_EXCP, HwPortIndex, 0);
            return;
        }
    }

    IO_x32(PORT_CTRL) = PortCtrl;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiMiscSetDisable<-HwPortIndex:0x%X PortCtrl:0x%X", HwPortIndex, PortCtrl);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiMiscChangeSpMoConfigToSpMoPhy()
*
* function:
*
* parameters:
*
* return value:
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiMiscChangeSpMoConfigToSpMoPhy( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, 
                                                                      LSA_UINT16               const  LinkSpeedModeConfig,
                                                                      LSA_BOOL                     *  pbAutoNeg,
                                                                      LSA_UINT8                    *  pLinkSpeed,
                                                                      LSA_UINT8                    *  pLinkMode )
{
    switch (LinkSpeedModeConfig)
    {
        case EDD_LINK_AUTONEG:
        {
            *pbAutoNeg  = LSA_TRUE;
            *pLinkSpeed = EDD_LINK_UNKNOWN;
            *pLinkMode  = EDD_LINK_UNKNOWN;
        }
        break;

        case EDD_LINK_100MBIT_HALF:
        {
            *pbAutoNeg  = LSA_FALSE;
            *pLinkSpeed = EDD_LINK_SPEED_100;
            *pLinkMode  = EDD_LINK_MODE_HALF;
        }
        break;

        case EDD_LINK_100MBIT_FULL:
        {
            *pbAutoNeg  = LSA_FALSE;
            *pLinkSpeed = EDD_LINK_SPEED_100;
            *pLinkMode  = EDD_LINK_MODE_FULL;
        }
        break;

        case EDD_LINK_10MBIT_HALF:
        {
            *pbAutoNeg  = LSA_FALSE;
            *pLinkSpeed = EDD_LINK_SPEED_10;
            *pLinkMode  = EDD_LINK_MODE_HALF;
        }
        break;

        case EDD_LINK_10MBIT_FULL:
        {
            *pbAutoNeg  = LSA_FALSE;
            *pLinkSpeed = EDD_LINK_SPEED_10;
            *pLinkMode  = EDD_LINK_MODE_FULL;
        }
        break;

        default:
        {
            EDDI_Excp("EDDI_SwiMiscChangeSpMoConfigToSpMoPhy, LinkSpeedModeConfig", EDDI_FATAL_ERR_EXCP, LinkSpeedModeConfig, 0);
            return;
        }
    }

    EDDI_SWI_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiMiscChangeSpMoConfigToSpMoPhy<-LinkSpeedModeConfig:0x%X AutoNeg:0x%X LinkSpeed:0x%X LinkMode:0x%X", 
                      LinkSpeedModeConfig, *pbAutoNeg, *pLinkSpeed, *pLinkMode);
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_InsertBottomToDynList()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_InsertBottomToDynList( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                             EDDI_DYN_LIST_HEAD_PTR_TYPE  const  pListHeader,
                                                             LSA_VOID_PTR_TYPE            const  pData )
{
    EDDI_DYN_LIST_ENTRY_PTR_TYPE  pListDataEntry;

    if (pData == EDDI_NULL_PTR)
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_InsertBottomToDynList, pData == EDDI_NULL_PTR");
        return LSA_RET_ERR_PARAM;
    }

    EDDI_AllocLocalMem((void **)&pListDataEntry, (LSA_UINT32)sizeof(EDDI_DYN_LIST_ENTRY_TYPE));

    if (pListDataEntry == EDDI_NULL_PTR)
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_InsertBottomToDynList, pListDataEntry == EDDI_NULL_PTR");
        return LSA_RET_ERR_RESOURCE;
    }
    EDDI_MemSet(pListDataEntry, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DYN_LIST_ENTRY_TYPE));

    pListDataEntry->pData = pData;

    EDDI_PUT_BLOCK_BOTTOM(pListHeader->pBottom, pListHeader->pTop, pListDataEntry);

    pListHeader->Cnt++;

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       GetBottomFromDynList()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GetTopFromDynList( EDDI_LOCAL_DDB_PTR_TYPE                              const  pDDB,
                                                         EDDI_DYN_LIST_ENTRY_PTR_TYPE EDDI_LOCAL_MEM_ATTR  *  const  pTop,
                                                         LSA_VOID_PTR_TYPE            EDDI_LOCAL_MEM_ATTR  *  const  ppData )
{
    EDDI_DYN_LIST_ENTRY_PTR_TYPE  pListDataEntry;

    *ppData = EDDI_NULL_PTR;

    EDDI_GET_NEXT_TOP_NO_CAST(*pTop, pListDataEntry);

    if (pListDataEntry == EDDI_NULL_PTR)
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_GetTopFromDynList, pListDataEntry == EDDI_NULL_PTR");
        return EDD_STS_OK;
    }

    if (pListDataEntry->pData == EDDI_NULL_PTR)
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_GetTopFromDynList, pListDataEntry->pData == EDDI_NULL_PTR");
        return LSA_RET_ERR_PARAM;
    }

    *ppData = pListDataEntry->pData;

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RemoveBottomFromDynList()                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RemoveBottomFromDynList( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB, 
                                                               EDDI_DYN_LIST_HEAD_PTR_TYPE                   pListHeader,
                                                               LSA_VOID_PTR_TYPE  EDDI_LOCAL_MEM_ATTR     *  ppData )
{
    LSA_RESULT                   RetVal;
    EDDI_DYN_LIST_ENTRY_PTR_TYPE  pListDataEntry;

    *ppData = EDDI_NULL_PTR;

    EDDI_REM_BLOCK_TOP_NO_CAST(pListHeader->pBottom, pListHeader->pTop, pListDataEntry);

    if (pListDataEntry == EDDI_NULL_PTR)
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_RemoveBottomFromDynList, pListDataEntry == EDDI_NULL_PTR");
        return EDD_STS_OK;
    }

    if (pListDataEntry->pData == EDDI_NULL_PTR)
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RemoveBottomFromDynList, pListDataEntry->pData == EDDI_NULL_PTR");
        return EDD_STS_ERR_PARAM;
    }

    *ppData = pListDataEntry->pData;

    pListHeader->Cnt--;

    EDDI_FREE_LOCAL_MEM(&RetVal, pListDataEntry);
    if (EDD_STS_OK != RetVal)
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RemoveBottomFromDynList, EDDI_FREE_LOCAL_MEM, RetVal:0x%X", RetVal);
        return RetVal;
    }

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RemoveFromDynList()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RemoveFromDynList( EDDI_LOCAL_DDB_PTR_TYPE                           const  pDDB, 
                                                         EDDI_DYN_LIST_HEAD_PTR_TYPE                       const  pListHeader,
                                                         EDDI_DYN_LIST_ENTRY_PTR_TYPE EDDI_LOCAL_MEM_ATTR  const  pListDataEntry )
{
    LSA_RESULT  RetVal;

    if (pListDataEntry == EDDI_NULL_PTR)
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RemoveFromDynList, pListDataEntry == EDDI_NULL_PTR");

        return EDD_STS_ERR_PARAM;
    }

    EDDI_REM_BLOCK_NO_CAST(pListHeader->pBottom, pListHeader->pTop, pListDataEntry);

    pListHeader->Cnt--;

    EDDI_FREE_LOCAL_MEM(&RetVal, pListDataEntry);
    if (EDD_STS_OK != RetVal)
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_RemoveFromDynList, EDDI_FREE_LOCAL_MEM, RetVal:0x%X", RetVal);
        return RetVal;
    }

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_AutoNegCapToLinkSpeedMode()                 */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_AutoNegCapToLinkSpeedMode( LSA_UINT32                         const  HwPortIndex,
                                                               LSA_UINT8  EDDI_LOCAL_MEM_ATTR  *  const  pLinkSpeedMode,
                                                               EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB )
{
    LSA_UINT32  const  AutoNegCap = pDDB->Glob.PortParams[HwPortIndex].AutonegMappingCapability;

    //only one cap allowed
    switch (AutoNegCap)
    {
        case EDD_LINK_AUTONEG:
        case EDD_LINK_100MBIT_HALF:
        case EDD_LINK_100MBIT_FULL:
        case EDD_LINK_10MBIT_HALF:
        case EDD_LINK_10MBIT_FULL:
        case EDD_LINK_1GBIT_HALF:
        case EDD_LINK_1GBIT_FULL:
        case EDD_LINK_10GBIT_HALF:
        case EDD_LINK_10GBIT_FULL:
        {
            *pLinkSpeedMode = (LSA_UINT8)(AutoNegCap & 0xFFUL);
        }
        break;
        default:
        {
            EDDI_Excp("EDDI_AutoNegCapToLinkSpeedMode, AutoNegCap:", EDDI_FATAL_ERR_EXCP, AutoNegCap, 0);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_IfLinkSpeedModeInCapability()               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_IfLinkSpeedModeInCapability( LSA_UINT32               const  HwPortIndex,
                                                                 LSA_UINT32               const  LinkSpeedMode,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  const  Capability = pDDB->Glob.PortParams[HwPortIndex].AutonegCapAdvertised;
    LSA_UINT8   const  MediaType  = pDDB->Glob.PortParams[HwPortIndex].MediaType;
    //only one Link Speed Mode allowed

    if(    (EDD_AUTONEG_CAP_NONE == Capability)
        && (EDD_MEDIATYPE_FIBER_OPTIC_CABLE == MediaType ) )
    {
        return LSA_TRUE;
    }
    switch (LinkSpeedMode)
    {
        case EDD_LINK_AUTONEG:
        {
            //if any AUTONEG_CAP is set, then AUTONEG is supported
            if (0 == Capability)
            {
                return LSA_FALSE;
            }
        }
        break;
        case EDD_LINK_10GBIT_FULL:
        case EDD_LINK_10GBIT_HALF:
        case EDD_LINK_1GBIT_FULL:
        case EDD_LINK_1GBIT_HALF:
        {
            if (0 == (Capability & EDD_AUTONEG_CAP_OTHER))
            {
                return LSA_FALSE;
            }
        }
        break;
        case EDD_LINK_100MBIT_FULL:
        {
            if (0 == (Capability & EDDI_AUTONEG_CAP_100MBIT_FULL))
            {
                return LSA_FALSE;
            }
        }
        break;
        case EDD_LINK_100MBIT_HALF:
        {
            if (0 == (Capability & EDDI_AUTONEG_CAP_100MBIT_HALF))
            {
                return LSA_FALSE;
            }
        }
        break;
        case EDD_LINK_10MBIT_FULL:
        {
            if (0 == (Capability & EDDI_AUTONEG_CAP_10MBIT_FULL))
            {
                return LSA_FALSE;
            }
        }
        break;
        case EDD_LINK_10MBIT_HALF:
        {
            if (0 == (Capability & EDDI_AUTONEG_CAP_10MBIT_HALF))
            {
                return LSA_FALSE;
            }
        }
        break;
        default:
        {
            EDDI_Excp("EDDI_IfLinkSpeedModeInCapability, LinkSpeedMode, Capability", EDDI_FATAL_ERR_EXCP, LinkSpeedMode, Capability);
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_IfLinkSpeedModeInCapability, LinkSpeedMode:0x%X Capability:0x%X", LinkSpeedMode, Capability);
            return LSA_FALSE;
        }
    } //switch

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/****************************************************************************/
/*  end of file eddi_swi_misc.c                                             */
/****************************************************************************/

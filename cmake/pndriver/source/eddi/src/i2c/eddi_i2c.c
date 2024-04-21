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
/*  F i l e               &F: eddi_i2c.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI I2C functions                               */
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

//#include "eddi_ext.h"
//#include "eddi_swi_ext.h"
//#include "eddi_crt_ext.h"

//#include "eddi_sync_usr.h"

//#include "eddi_prm_req.h"
//#include "eddi_crt_brq.h"

//#include "eddi_req.h"
//#include "eddi_lock.h"

//#include "eddi_nrt_usr.h"
//#include "eddi_nrt_arp.h"
//#include "eddi_nrt_oc.h"

//#include "eddi_ev.h"

#define EDDI_MODULE_ID     M_ID_EDDI_I2C
#define LTRC_ACT_MODUL_ID  501

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/
#define EDDI_I2C_SDA_LEVEL_HIGH     1
#define EDDI_I2C_SDA_LEVEL_LOW      0

#undef  EDDI_I2C_TRACE
#undef  EDDI_I2C_TEST
#define EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse)\
        if(EDD_STS_OK!=EDDResponse)\
        {\
            eddi_I2C_Stop(pDDB);\
            return EDDResponse;\
        }

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                            local function declaration                     */
/*===========================================================================*/
static  LSA_BOOL   EDDI_LOCAL_FCT_ATTR  eddi_I2C_Select( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         LSA_UINT16               const  PortId,
	                                                     LSA_UINT16          	  const  I2CMuxSelect );

static  LSA_VOID   EDD_UPPER_IN_FCT_ATTR  eddi_I2C_Init_GPIO( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                     	                                      LSA_UINT32	           const  PortId );

static  EDD_RSP    EDD_UPPER_IN_FCT_ATTR  eddi_I2C_Write_Offset_GPIO( EDDI_LOCAL_DDB_PTR_TYPE        const    pDDB,
	                                                                  LSA_UINT32	                 const    PortId,
	                                                                  LSA_UINT8			             const    I2CDevAddr,
	                                                                  LSA_UINT8			             const    I2COffsetCnt,
	                                                                  LSA_UINT8			             const    I2COffset1,
	                                                                  LSA_UINT8			             const    I2COffset2,
	                                                                  LSA_UINT32                     const    Size,
	                                                                  LSA_UINT8  EDD_UPPER_MEM_ATTR  const *  pBuf );

static  EDD_RSP    EDD_UPPER_IN_FCT_ATTR  eddi_I2C_Read_Offset_GPIO( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB,
	                                                                 LSA_UINT32	                    const  PortId,
	                                                                 LSA_UINT8			            const  I2CDevAddr,
	                                                                 LSA_UINT8			            const  I2COffsetCnt,
	                                                                 LSA_UINT8			            const  I2COffset1,
	                                                                 LSA_UINT8			            const  I2COffset2,
	                                                                 LSA_UINT32                     const  Size,
	                                                                 LSA_UINT8  EDD_UPPER_MEM_ATTR      *  pBuf );

static  EDD_RSP   EDDI_LOCAL_FCT_ATTR  eddi_I2C_GetACK( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static  LSA_VOID   EDDI_LOCAL_FCT_ATTR  eddi_I2C_Start( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static  LSA_VOID   EDDI_LOCAL_FCT_ATTR  eddi_I2C_Stop( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static  LSA_VOID   EDDI_LOCAL_FCT_ATTR  eddi_I2C_SetACK( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static  LSA_UINT8  EDDI_LOCAL_FCT_ATTR  eddi_I2C_ReadByte( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

static  LSA_VOID   EDDI_LOCAL_FCT_ATTR  eddi_I2C_WriteByte( LSA_UINT8                const  Byte,
                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*=============================================================================
 * function name: eddi_I2C_Write_Offset()
 *
 * function:      API-function: writes n bytes to the I2C device of the selected port
 *
 * parameters:    ...
 *
 * return value:  EDD_RSP
 *
 *===========================================================================*/
EDD_RSP  EDD_UPPER_IN_FCT_ATTR  eddi_I2C_Write_Offset( EDDI_HANDLE  		          const  hDDB,	        //Device handle (refer to service EDDI_SRV_DEV_OPEN)                     	  
	                                                   LSA_UINT32	                  const  PortId,        //User-PortId 1...4
	                                                   LSA_UINT16		              const  I2CMuxSelect,  //can be used for HW-multiplexer-control
	                                                   LSA_UINT8			          const  I2CDevAddr,    //0...127
	                                                   LSA_UINT8			          const  I2COffsetCnt,  //0...2
	                                                   LSA_UINT8			          const  I2COffset1,    //optional: 0...255
	                                                   LSA_UINT8			          const  I2COffset2,    //optional: 0...255
                                                       LSA_UINT16                     const  RetryCnt,      //0: disabled,    1..65535
                                                       LSA_UINT16                     const  RetryTime_us,  //0: no waiting,  1..65535
	                                                   LSA_UINT32                     const  Size,      	//in bytes, 1...300
	                                                   LSA_UINT8  EDD_UPPER_MEM_ATTR      *  pBuf )    	    //pointer to buffer
{
    EDD_RSP                         EDDResponse;
    LSA_BOOL                        bRetVal;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB        = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    LSA_UINT16                      RetryCount  = RetryCnt;

    //plausible parameters
    if (   (PortId       > 4UL)
        || (I2COffsetCnt > (LSA_UINT8)2)
        || (Size         > EDD_I2C_MAX_DATA_SIZE))
    {
        return EDD_STS_ERR_PARAM;
    }

    switch (pDDB->I2C.I2C_Type)
    {
        case EDDI_I2C_TYPE_GPIO:
	    {
	        EDDI_ENTER_I2C_S();
            bRetVal = eddi_I2C_Select(pDDB, (LSA_UINT16)PortId, I2CMuxSelect);
            if (!bRetVal)
            {
			    EDDI_EXIT_I2C_S();
                EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "eddi_I2C_Write_Offset, unexpected bRetVal of eddi_I2C_Select(), PortId:0x%X I2CMuxSelect:0x%X bRetVal:0x%X", PortId, I2CMuxSelect, bRetVal);
                EDDI_Excp("eddi_I2C_Write_Offset, unexpected bRetVal of eddi_I2C_Select(), PortId: bRetVal:", EDDI_FATAL_ERR_EXCP, PortId, bRetVal);
                return EDD_STS_ERR_EXCP;
            }

            for (;;)
            {
			    EDDResponse = eddi_I2C_Write_Offset_GPIO(pDDB, PortId, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2, Size, pBuf); //internal EDDI function
                if (   (  (EDD_STS_ERR_PROTOCOL == EDDResponse) || (EDD_STS_ERR_HW == EDDResponse)  )
                    && (RetryCount > 0))
                {
                    RetryCount--;
                    if (RetryTime_us > 0)
                        EDDI_WAIT_10_NS(pDDB->hSysDev, RetryTime_us * 100UL);
                }
                else
                    break;
            }

			EDDI_EXIT_I2C_S();
	    } break;
        #if defined (EDDI_CFG_REV7)
		case EDDI_I2C_TYPE_SOC1_HW:
	    {
	        EDDI_ENTER_I2C_S();
            bRetVal = eddi_I2C_Select(pDDB, (LSA_UINT16)PortId, I2CMuxSelect);
            if (!bRetVal)
            {
			    EDDI_EXIT_I2C_S();
                EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "eddi_I2C_Write_Offset, unexpected bRetVal of eddi_I2C_Select(), PortId:0x%X I2CMuxSelect:0x%X bRetVal:0x%X", PortId, I2CMuxSelect, bRetVal);
                EDDI_Excp("eddi_I2C_Write_Offset, unexpected bRetVal of eddi_I2C_Select(), PortId: bRetVal:", EDDI_FATAL_ERR_EXCP, PortId, bRetVal);
                return EDD_STS_ERR_EXCP;
            }
            EDDResponse = EDD_STS_ERR_PARAM; //preset
            
            for (;;)
            {
    			EDDI_LL_I2C_WRITE_OFFSET_SOC(&EDDResponse, pDDB->hSysDev, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2, Size, pBuf); //EDDI output macro
                if (   (  (EDD_STS_ERR_PROTOCOL == EDDResponse) || (EDD_STS_ERR_HW == EDDResponse)  )
                    && (RetryCount > 0))
                {
                    RetryCount--;
                    if (RetryTime_us > 0)
                        EDDI_WAIT_10_NS(pDDB->hSysDev, RetryTime_us * 100UL);
                }
                else
                    break;
            }

			EDDI_EXIT_I2C_S();
	    } break;
        #endif
        default: return EDD_STS_ERR_PARAM;
    }

    return EDDResponse;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_I2C_Read_Offset()
 *
 * function:      API-function: reads n bytes from the I2C device of the selected port
 *
 * parameters:    ...
 *
 * return value:  EDD_RSP
 *
 *===========================================================================*/
EDD_RSP  EDD_UPPER_IN_FCT_ATTR  eddi_I2C_Read_Offset( EDDI_HANDLE  		             const  hDDB,	        //Device handle (refer to service EDDI_SRV_DEV_OPEN)                     	  
	                                                  LSA_UINT32	                 const  PortId,         //User-PortId 1...4
	                                                  LSA_UINT16		             const  I2CMuxSelect,   //can be used for HW-multiplexer-control
	                                                  LSA_UINT8			             const  I2CDevAddr,     //0...127
	                                                  LSA_UINT8			             const  I2COffsetCnt,   //0...2
	                                                  LSA_UINT8			             const  I2COffset1,     //optional: 0...255
	                                                  LSA_UINT8			             const  I2COffset2,     //optional: 0...255
                                                      LSA_UINT16                     const  RetryCnt,       //0: disabled,    1..65535
                                                      LSA_UINT16                     const  RetryTime_us,   //0: no waiting,  1..65535
	                                                  LSA_UINT32                     const  Size,      	    //in bytes, 1...300
	                                                  LSA_UINT8  EDD_UPPER_MEM_ATTR      *  pBuf )    	    //pointer to buffer
{
    EDD_RSP                         EDDResponse;
    LSA_BOOL                        bRetVal;
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB        = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    LSA_UINT16                      RetryCount  = RetryCnt;

    //plausible parameters
    if (   (PortId       > 4UL)
        || (I2COffsetCnt > (LSA_UINT8)2)
        || (Size         > EDD_I2C_MAX_DATA_SIZE))
    {
        return EDD_STS_ERR_PARAM;
    }

    switch (pDDB->I2C.I2C_Type)
    {
        case EDDI_I2C_TYPE_GPIO:
	    {
	        EDDI_ENTER_I2C_S();
            bRetVal = eddi_I2C_Select(pDDB, (LSA_UINT16)PortId, I2CMuxSelect);
            if (!bRetVal)
            {
			    EDDI_EXIT_I2C_S();
                EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "eddi_I2C_Write_Offset, unexpected bRetVal of eddi_I2C_Select(), PortId:0x%X I2CMuxSelect:0x%X bRetVal:0x%X", PortId, I2CMuxSelect, bRetVal);
                EDDI_Excp("eddi_I2C_Write_Offset, unexpected bRetVal of eddi_I2C_Select(), PortId: bRetVal:", EDDI_FATAL_ERR_EXCP, PortId, bRetVal);
                return EDD_STS_ERR_EXCP;
            }

            for (;;)
            {
    			EDDResponse = eddi_I2C_Read_Offset_GPIO(pDDB, PortId, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2, Size, pBuf); //internal EDDI function
                if (   (  (EDD_STS_ERR_PROTOCOL == EDDResponse) || (EDD_STS_ERR_HW == EDDResponse)  )
                    && (RetryCount > 0))
                {
                    RetryCount--;
                    if (RetryTime_us > 0)
                        EDDI_WAIT_10_NS(pDDB->hSysDev, RetryTime_us * 100UL);
                }
                else
                    break;
            }

			EDDI_EXIT_I2C_S();
	    } break;
        #if defined (EDDI_CFG_REV7)
		case EDDI_I2C_TYPE_SOC1_HW:
	    {
	        EDDI_ENTER_I2C_S();
            bRetVal = eddi_I2C_Select(pDDB, (LSA_UINT16)PortId, I2CMuxSelect);
            if (!bRetVal)
            {
			    EDDI_EXIT_I2C_S();
                EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "eddi_I2C_Write_Offset, unexpected bRetVal of eddi_I2C_Select(), PortId:0x%X I2CMuxSelect:0x%X bRetVal:0x%X", PortId, I2CMuxSelect, bRetVal);
                EDDI_Excp("eddi_I2C_Write_Offset, unexpected bRetVal of eddi_I2C_Select(), PortId: bRetVal:", EDDI_FATAL_ERR_EXCP, PortId, bRetVal);
                return EDD_STS_ERR_EXCP;
            }
    
            EDDResponse = EDD_STS_ERR_PARAM; //preset

            for (;;)
            {
    			EDDI_LL_I2C_READ_OFFSET_SOC(&EDDResponse, pDDB->hSysDev, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2, Size, pBuf); //EDDI output macro
                if (    (  (EDD_STS_ERR_PROTOCOL == EDDResponse) || (EDD_STS_ERR_HW == EDDResponse)  )
                    &&  (RetryCount > 0) )
                {
                    RetryCount--;
                    if (RetryTime_us > 0)
                        EDDI_WAIT_10_NS(pDDB->hSysDev, RetryTime_us * 100UL);
                }
                else
                    break;
            }

			EDDI_EXIT_I2C_S();
	    } break;
        #endif
        default: return EDD_STS_ERR_PARAM;
    }

    return EDDResponse;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_I2C_Select()
 *
 * function:      Selects an I2C port for the following I2C transfers
 *
 * parameters:    ...
 *
 * return value:  LSA_BOOL
 *
 *===========================================================================*/
static  LSA_BOOL  EDDI_LOCAL_FCT_ATTR  eddi_I2C_Select( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT16               const  PortId,
	                                                    LSA_UINT16          	 const  I2CMuxSelect )
{
    LSA_UINT16  const  I2CMuxSelectStored  = pDDB->I2C.I2CMuxSelectStored;
    LSA_BOOL    const  bI2CMuxSelectIgnore = pDDB->I2C.bI2CMuxSelectIgnore;

    #if defined (EDDI_I2C_TRACE)
    EDDI_FUNCTION_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_I2C_Select->pDDB:0x%X PortId:0x%X I2CMuxSelect:0x%X", pDDB, PortId, I2CMuxSelect);
    #endif

    if //I2CMuxSelect of I2C transfer activated and changed?
       (   (I2CMuxSelect)
        && ((bI2CMuxSelectIgnore) || (I2CMuxSelect != I2CMuxSelectStored)) )
    {
        LSA_UINT8  bRetVal; //preset

        //for test purposes, ignore I2CMuxSelectStored and call EDDP_I2C_SELECT() every time
        pDDB->I2C.I2CMuxSelectStored = (bI2CMuxSelectIgnore) ? 0 : I2CMuxSelect;

        EDDI_I2C_SELECT(&bRetVal, pDDB->hSysDev, PortId, I2CMuxSelect);

        return (EDD_I2C_DEVICE_ACTIVATED == bRetVal) ? LSA_TRUE : LSA_FALSE;
    }
    else
    {
        return LSA_TRUE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_I2C_Init_GPIO()
 *
 * function:      initializes I2C device of the selected port
 *
 * parameters:    ...
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_I2C_Init_GPIO( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,	    //pDDB
                     	                                     LSA_UINT32	              const  PortId )   //User-PortId 1...4
{
    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_I2C_Init_GPIO->pDDB:0x%X PortId:0x%X", pDDB, PortId);

    //phase 1
    EDDI_I2C_SDA_HIGHZ();
    EDDI_I2C_CLK_LOW();
    EDDI_I2C_WAIT_US(2UL);

    eddi_I2C_Stop(pDDB); //5 us
    eddi_I2C_Stop(pDDB); //5 us
    eddi_I2C_Stop(pDDB); //5 us

    #if defined (EDDI_I2C_TEST)
    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_I2C_Init_GPIO, Starting I2C-Test-Routine, pDDB:0x%X PortId:0x%X", pDDB, PortId);

    if (PortId > 1UL)
    {
        for (;;) //endless loop
        {
            EDDI_I2C_SDA_HIGHZ();
            EDDI_I2C_CLK_LOW();

            EDDI_I2C_WAIT_US(1UL);

            EDDI_I2C_SDA_LOW();
            EDDI_I2C_CLK_HIGHZ();

            EDDI_I2C_WAIT_US(1UL);
        }
    }
    #endif

    pDDB->I2C.bI2CAlreadyInitialized[PortId] = LSA_TRUE;

    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_I2C_Init_GPIO<-pDDB:0x%X PortId:0x%X", pDDB, PortId);
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_I2C_Write_Offset_GPIO()
 *
 * function:      internal function: writes n bytes to the I2C device of the selected port
 *
 * parameters:    ...
 *
 * return value:  EDD_RSP
 *
 *===========================================================================*/
static  EDD_RSP  EDD_UPPER_IN_FCT_ATTR  eddi_I2C_Write_Offset_GPIO( EDDI_LOCAL_DDB_PTR_TYPE         const    pDDB,	       //pDDB
	                                                                LSA_UINT32	                    const    PortId,       //User-PortId 1...4
	                                                                LSA_UINT8			            const    I2CDevAddr,   //0...127
	                                                                LSA_UINT8			            const    I2COffsetCnt, //0...2
	                                                                LSA_UINT8			            const    I2COffset1,   //optional: 0...255
	                                                                LSA_UINT8			            const    I2COffset2,   //optional: 0...255
	                                                                LSA_UINT32                      const    Size,         //in bytes, 1...300
                                                                    LSA_UINT8  EDD_UPPER_MEM_ATTR   const *  pBuf )    	   //pointer to buffer
{
    LSA_UINT32  Ctr;
    EDD_RSP EDDResponse=EDD_STS_OK;

    if //I2C device not yet initialized?
       (!pDDB->I2C.bI2CAlreadyInitialized[PortId])
    {
        eddi_I2C_Init_GPIO(pDDB, PortId);
    }

    #if defined (EDDI_I2C_TRACE)
    EDDI_FUNCTION_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_I2C_Write_Offset_GPIO->pDDB:0x%X PortId:0x%X I2CDevAddr:0x%X I2COffsetCnt:0x%X I2COffset1:0x%X I2COffset2:0x%X", pDDB, PortId, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2);
    #endif

    //random write

    eddi_I2C_Start(pDDB); //4us

    eddi_I2C_WriteByte((LSA_UINT8)(I2CDevAddr & (LSA_UINT8)0xFE), pDDB); //40us //clear read flag //I2CDevAddr A0, A2, ...

    EDDResponse=eddi_I2C_GetACK(pDDB); //6us
    EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse);

    switch //by I2COffsetCnt (already checked by value)
           (I2COffsetCnt)
    {
        case 1:
        {
            eddi_I2C_WriteByte(I2COffset1, pDDB); //40us

            EDDResponse=eddi_I2C_GetACK(pDDB); //6us
            EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse);

            break;
        }
        case 2:
        {
            eddi_I2C_WriteByte(I2COffset1, pDDB); //40us

            EDDResponse=eddi_I2C_GetACK(pDDB); //6us
            EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse);

            eddi_I2C_WriteByte(I2COffset2, pDDB); //40us

            EDDResponse=eddi_I2C_GetACK(pDDB); //6us
            EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse);

            break;
        }
        default: break;
    }

    for (Ctr = Size; Ctr; Ctr--)
    {
        eddi_I2C_WriteByte(*pBuf, pDDB); //40us
        pBuf++;

        EDDResponse=eddi_I2C_GetACK(pDDB); //6us
        EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse);
    }

    eddi_I2C_Stop(pDDB); //5us

    //time = 101us + n * 40us + (n - 1) * 6us
    //n = 1 -> 141us
    //n = 2 -> 187us
    //n = 3 -> 233us
    //n = 4 -> 279us
    //Diff  ->  46us

    #if defined (EDDI_I2C_TRACE)
    EDDI_FUNCTION_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_I2C_Write_Offset_GPIO<-pDDB:0x%X PortId:0x%X I2CDevAddr:0x%X I2COffsetCnt:0x%X I2COffset1:0x%X I2COffset2:0x%X", pDDB, PortId, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2);
    #endif

    return EDDResponse;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_I2C_Read_Offset_GPIO()
 *
 * function:      internal function: reads n bytes from the I2C device of the selected port
 *
 * parameters:    ...
 *
 * return value:  EDD_RSP
 *
 *===========================================================================*/
static  EDD_RSP  EDD_UPPER_IN_FCT_ATTR  eddi_I2C_Read_Offset_GPIO( EDDI_LOCAL_DDB_PTR_TYPE        const  pDDB,	       //pDDB
	                                                               LSA_UINT32	                  const  PortId,       //User-PortId 1...4
	                                                               LSA_UINT8			          const  I2CDevAddr,   //0...127
	                                                               LSA_UINT8			          const  I2COffsetCnt, //0...2
	                                                               LSA_UINT8			          const  I2COffset1,   //optional: 0...255
	                                                               LSA_UINT8			          const  I2COffset2,   //optional: 0...255
	                                                               LSA_UINT32                     const  Size,         //in bytes, 1...300
 	                                                               LSA_UINT8  EDD_UPPER_MEM_ATTR      *  pBuf )    	   //pointer to buffer
{
    LSA_UINT32  Ctr;
    EDD_RSP     EDDResponse = EDD_STS_OK;

    if //I2C device not yet initialized?
       (!pDDB->I2C.bI2CAlreadyInitialized[PortId])
    {
        eddi_I2C_Init_GPIO(pDDB, PortId);
    }

    #if defined (EDDI_I2C_TRACE)
    EDDI_FUNCTION_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_I2C_Read_Offset_GPIO->pDDB:0x%X PortId:0x%X I2CDevAddr:0x%X I2COffsetCnt:0x%X I2COffset1:0x%X I2COffset2:0x%X", pDDB, PortId, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2);
    #endif

    //random read

    eddi_I2C_Start(pDDB); //4us

    eddi_I2C_WriteByte((LSA_UINT8)(I2CDevAddr & (LSA_UINT8)0xFE), pDDB); //40us //clear read flag //I2CDevAddr A0, A2, ...

    EDDResponse=eddi_I2C_GetACK(pDDB); //6us
    EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse);

    switch //by I2COffsetCnt (already checked by value)
           (I2COffsetCnt)
    {
        case 1:
        {
            eddi_I2C_WriteByte(I2COffset1, pDDB); //40us

            EDDResponse=eddi_I2C_GetACK(pDDB); //6us
            EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse);

            break;
        }
        case 2:
        {
            eddi_I2C_WriteByte(I2COffset1, pDDB); //40us

            EDDResponse=eddi_I2C_GetACK(pDDB); //6us
            EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse);

            eddi_I2C_WriteByte(I2COffset2, pDDB); //40us

            EDDResponse=eddi_I2C_GetACK(pDDB); //6us
            EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse);

            break;
        }
        default: break;
    }

    eddi_I2C_Start(pDDB); //4us //repeated start condition

    eddi_I2C_WriteByte((LSA_UINT8)(I2CDevAddr | (LSA_UINT8)0x01), pDDB); //40us //set read flag //I2CDevAddr A0, A2, ...

    EDDResponse=eddi_I2C_GetACK(pDDB); //6us
    EDDI_I2C_ACK_EDD_RSP_HANDLER(EDDResponse);

    for (Ctr = 0; Ctr < Size; Ctr++)
    {
        *pBuf = eddi_I2C_ReadByte(pDDB); //48 us

        pBuf++;

        if (Ctr < (Size - 1))
        {
            eddi_I2C_SetACK(pDDB); //6 us
        }
    }

    eddi_I2C_Stop(pDDB); //5 us

    //time = 151us + n * 48us + (n - 1) * 6us
    //n = 1 -> 199us
    //n = 2 -> 253us
    //n = 3 -> 307us
    //n = 4 -> 361us
    //Diff  ->  54us

    #if defined (EDDI_I2C_TRACE)
    EDDI_FUNCTION_TRACE_06(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "eddi_I2C_Read_Offset_GPIO<-pDDB:0x%X PortId:0x%X I2CDevAddr:0x%X I2COffsetCnt:0x%X I2COffset1:0x%X I2COffset2:0x%X", pDDB, PortId, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2);
    #endif

    return EDDResponse;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*===========================================================================*/
/*                            local function definition                      */
/*===========================================================================*/

/*=============================================================================
 * function name: eddi_I2C_Start()
 *
 * function:      start I2C command
 *
 * parameters:    ...
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
static  LSA_VOID  eddi_I2C_Start( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    /* phase 1 */
    EDDI_I2C_CLK_HIGHZ();
    EDDI_I2C_SDA_HIGHZ();
    /* waiting: */
    EDDI_I2C_WAIT_US(1UL);

    /* phase 2 */
    EDDI_I2C_SDA_LOW();
    /* waiting: */
    /* t HD.STA, start hold time, min 0.6 us */
    /* t HIGH, high period of SCL, min. 0.6 us */
    /* both must be fulfilled */
    EDDI_I2C_WAIT_US(1UL);

    /* phase 3 */
    EDDI_I2C_CLK_LOW();
    /* waiting: */
    /* t LOW, low period of SCL, min. 1.3 us */
    EDDI_I2C_WAIT_US(2UL);

    /* phase 4 */
    EDDI_I2C_SDA_HIGHZ();

    /* Wait-Time -> 4us */
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_I2C_Stop()
 *
 * function:      stop I2C command
 *
 * parameters:    ...
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_I2C_Stop( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    /* phase 1 */
    EDDI_I2C_CLK_LOW();
    EDDI_I2C_SDA_LOW();
    /* waiting: */
    /* t LOW, low period of SCL, min. 1.3 us */
    EDDI_I2C_WAIT_US(2UL);

    /* phase 2 */
    EDDI_I2C_CLK_HIGHZ();
    /* waiting: */
    /* t SU.STO, stop setup time, min. 0.6 us */
    EDDI_I2C_WAIT_US(1UL);

    /* phase 3 */
    EDDI_I2C_SDA_HIGHZ();
    /* waiting: */
    /* t BUF, bus free time between STOP and START, min. 1.3 us */
    EDDI_I2C_WAIT_US(2UL);

    /* 5 us */
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_I2C_SetACK()
 *
 * function:      set I2C ACK command
 *
 * parameters:    ...
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_I2C_SetACK( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    /* phase 1 */
    EDDI_I2C_CLK_LOW();
    EDDI_I2C_SDA_LOW();
    /* waiting: */
    /* t LOW, low period of SCL, min. 1.3 us */
    EDDI_I2C_WAIT_US(2UL);

    /* phase 2 */
    EDDI_I2C_CLK_HIGHZ();
    /* waiting: */
    /* t HIGH, high period of SCL, min. 0.6 us */
    EDDI_I2C_WAIT_US(1UL);

    /* phase 3 */
    EDDI_I2C_CLK_LOW();
    /* t LOW, low period of SCL, min. 1.3 us */
    EDDI_I2C_WAIT_US(2UL);

    /* phase 4 */
    EDDI_I2C_SDA_HIGHZ();
    /* waiting: */
    EDDI_I2C_WAIT_US(1UL);

    /* 6 us */
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_I2C_GetACK()
 *
 * function:      get I2C ACK command
 *
 * parameters:    ...
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
static  EDD_RSP  EDDI_LOCAL_FCT_ATTR  eddi_I2C_GetACK( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  Counter;
    LSA_UINT8   ReadValue   = EDDI_I2C_SDA_LEVEL_HIGH;
    EDD_RSP     EDDResponse = EDD_STS_OK;

    /* phase 1 */
    EDDI_I2C_SDA_HIGHZ(); //SDA will set depend on slave signal, SDA-High from Master is ignored
    EDDI_I2C_CLK_LOW();
    /* waiting: */
    /* t LOW, low period of SCL, min. 1.3 us */
    EDDI_I2C_WAIT_US(2UL);

    /* phase 2: wait for ack */
    for (Counter = 0; Counter < 10000UL; Counter++) /* write cycle time max. 10ms */
    {
        EDDI_I2C_SDA_READ_BIT(&ReadValue); //0 or 1

        if (EDDI_I2C_SDA_LEVEL_LOW == ReadValue) //waiting for SDA == 0 (0 = ACK positive)
        {
            break;
        }
        EDDI_I2C_WAIT_US(1UL);
    }

    if (EDDI_I2C_SDA_LEVEL_HIGH == ReadValue)
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_UNEXP, "eddi_I2C_GetACK, timeout 10ms expired, the acknowledgement has not been received ");       
        EDDResponse = EDD_STS_ERR_PROTOCOL;
    }
   
    /* phase 3 */
    EDDI_I2C_CLK_HIGHZ();
    /* t HIGH, high period of SCL, min. 0.6 us */
    EDDI_I2C_WAIT_US(2UL);

    /* phase 4 */
    EDDI_I2C_CLK_LOW();
    /* t LOW, low period of SCL, min. 1.3 us */
    EDDI_I2C_WAIT_US(2UL);

    EDDI_I2C_SDA_HIGHZ();

    /* Wait-Time -> 6 us */
	return EDDResponse;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_I2C_WriteByte()
 *
 * function:      write I2C byte
 *
 * parameters:    ...
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_I2C_WriteByte( LSA_UINT8                const  byte,
                                                           EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT8  bit;

    for (bit = 0; bit < 8; bit++) /* 8-bit value */
    {
        /* Phase 1 */
        EDDI_I2C_CLK_LOW();
        if (((byte >> (7 - bit)) & 0x01)) /* MSB first of a 8-bit long data */
        {
            EDDI_I2C_SDA_HIGHZ();
        }
        else
        {
            EDDI_I2C_SDA_LOW();
        }
        /* waiting: */
        /* t SU.DAT, data setup time, min. 0.1 us */
        /* t LOW, low period of SCL, min. 1.3 us */
        EDDI_I2C_WAIT_US(2UL);

        /* Phase 2 */
        EDDI_I2C_CLK_HIGHZ();
        /* waiting: */
        /* t HIGH, high period of SCL, min. 0.6 us */
        EDDI_I2C_WAIT_US(1UL);

        /* Phase 3 */
        EDDI_I2C_CLK_LOW();
        /* waiting: */
        /* t LOW, low period of SCL, min. 1.3 us */
        EDDI_I2C_WAIT_US(2UL);
    }

    EDDI_I2C_SDA_HIGHZ();

    /* 40us */
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_I2C_ReadByte()
 *
 * function:      read I2C byte
 *
 * parameters:    ...
 *
 * return value:  LSA_UINT8
 *
 *===========================================================================*/
static  LSA_UINT8  EDDI_LOCAL_FCT_ATTR  eddi_I2C_ReadByte( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT8  ReadValue = 0;
    LSA_UINT8  bit;
    LSA_UINT8  byte = 0;

    EDDI_I2C_SDA_HIGHZ();

    for (bit = 0; bit < 8; bit++) //8-bit value
    {
        /* phase 1 */
        EDDI_I2C_CLK_LOW();
        EDDI_I2C_SDA_HIGHZ();
        /* waiting: */
        /* t LOW, low period of SCL, min. 1.3 us */
        EDDI_I2C_WAIT_US(2UL);

        /* phase 2 */
        EDDI_I2C_CLK_HIGHZ();
        /* waiting: */
        /* t HIGH, high period of SCL, min. 0.6 us */
        EDDI_I2C_WAIT_US(1UL);

        /* phase 3 */
        EDDI_I2C_SDA_READ_BIT(&ReadValue); //0 or 1
        byte |= (LSA_UINT8)((LSA_UINT32)ReadValue << (7 - bit)); /* MSB first of a 8-bit long data */
        /* waiting: */
        /* t HIGH, high period of SCL, min. 0.6 us */
        EDDI_I2C_WAIT_US(1UL);

        /* phase 4 */
        EDDI_I2C_CLK_LOW();
        /* waiting: */
        /* t LOW, low period of SCL, min. 1.3 us */
        EDDI_I2C_WAIT_US(2UL);
    }

    EDDI_I2C_SDA_HIGHZ();

    return byte; 

    /* 48 us */ 
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_i2c.c                                                   */
/*****************************************************************************/


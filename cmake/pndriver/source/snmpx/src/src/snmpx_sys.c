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
/*  C o m p o n e n t     &C: SNMPX (SNMP eXtensible agent)             :C&  */
/*                                                                           */
/*  F i l e               &F: snmpx_sys.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SNMPX-system functions                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  3
#define SNMPX_MODULE_ID      LTRC_ACT_MODUL_ID /* SNMPX_MODULE_ID_SNMPX_SYS */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "snmpx_inc.h"            /* SNMPX headerfiles */
#include "snmpx_int.h"            /* internal header   */

/* BTRACE-IF */
SNMPX_FILE_SYSTEM_EXTENSION(SNMPX_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

SNMPX_DATA_TYPE SNMPX_Data;

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_FreeMgm                               +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    SNMPX_OK                                    +*/
/*+                             SNMPX_ERR_SEQUENCE                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees SNMPX Managmenent if allocated.                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_UINT16 SNMPX_FreeMgm(LSA_VOID)
{
    LSA_UINT16 Response = SNMPX_OK;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "IN : SNMPX_FreeMgm()");
    {
        /* ----------------------------------------------------------------------*/
        /* free Timer if present                                                 */
        /* ----------------------------------------------------------------------*/
        if ( SNMPX_GET_GLOB_PTR()->CyclicTimerID != SNMPX_TIMER_ID_INVALID )
        {
            Response = SNMPX_TimerFree(SNMPX_GET_GLOB_PTR()->CyclicTimerID);

            /* Note: We dont expect the Timer RQB to be underway ! */
            if ( Response != SNMPX_OK )
            {
                SNMPX_PROGRAM_TRACE_01(LSA_TRACE_LEVEL_FATAL,"SNMPX_FreeMgm. Cyclic Timer-free (0x%X)",Response);
                SNMPX_FATAL(SNMPX_FATAL_ERR_INCONSISTENZ);
            }

            SNMPX_GET_GLOB_PTR()->CyclicTimerID = SNMPX_TIMER_ID_INVALID;
        }
    }
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_FreeMgm(), Response: 0x%X",
                            Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    snmpx_init                                  +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    SNMPX_RESP                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      SNMPX_OK                                                  +*/
/*+               SNMPX_ERR_RESOURCE                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the SNMPX.                                    +*/
/*+                                                                         +*/
/*+               Note: Must only be called if not already initialized, not +*/
/*+                     checked!                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 snmpx_init(LSA_VOID)
{
    LSA_UINT16             Status = SNMPX_OK;
    LSA_UINT16             i;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "IN : snmpx_init()");

    /* ----------------------------------------------------------------------*/
    /* If memory should be static we use global memory.                      */
    /* ----------------------------------------------------------------------*/
    SNMPX_MEMSET(&SNMPX_Data,0,sizeof(SNMPX_Data));

    /* --------------------------------------------------------------------------*/
    /* Setup management                                                          */
    /* --------------------------------------------------------------------------*/
    {
        SNMPX_Data.MMgm.LLError.lsa_component_id = LSA_COMP_ID_UNUSED;

        SNMPX_Data.MMgm.UsedUserCnt   = 0;        /* User-Channels in use */

        for ( i=0; i<SNMPX_CFG_MAX_CHANNELS; i++)
        {
            SNMPX_Data.HUser[i].UserState  = SNMPX_HANDLE_STATE_FREE;
            SNMPX_Data.HUser[i].ThisHandle = (LSA_HANDLE_TYPE) (i | SNMPX_HANDLE_TYPE_USER);
        }

        SNMPX_Data.MMgm.UsedLowerCnt   = 0;       /* Lower-Channels not in use */
		SNMPX_Data.MMgm.CyclicTimerID  = SNMPX_TIMER_ID_INVALID;

	    for ( i=0; i<SNMPX_MAX_TIMER; i++ )
		{
			SNMPX_Data.MMgm.Timer[i].InUse   = LSA_FALSE;
			SNMPX_Data.MMgm.Timer[i].TimerID = 0;
		}

        /* lower channel */
        SNMPX_Data.HSock.SockState  = SNMPX_HANDLE_STATE_FREE;
        SNMPX_Data.HSock.ThisHandle = (LSA_HANDLE_TYPE) (SNMPX_HANDLE_TYPE_SOCK);

#if SNMPX_CFG_SNMP_OPEN_BSD
		/* lower OHA channel */
		SNMPX_Data.HOha.State = SNMPX_HANDLE_STATE_FREE;
		SNMPX_Data.HOha.ThisHandle = (LSA_HANDLE_TYPE) (SNMPX_HANDLE_TYPE_OHA);
#endif
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: snmpx_init(), Status: 0x%X",
                            Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    snmpx_undo_init                             +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      SNMPX_OK                                                  +*/
/*+               SNMPX_ERR_SEQUENCE                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Uninizializes SNMPX.                                      +*/
/*+               This is only possible if no device is open                +*/
/*+               SNMPX_ERR_SEQUENCE if device is open.                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 snmpx_undo_init(LSA_VOID)
{
    LSA_UINT16  Status;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT, "IN : snmpx_undo_init()");

    if ( SNMPX_Data.MMgm.UsedUserCnt || SNMPX_Data.MMgm.UsedLowerCnt )
    {
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"snmpx_undo_init(). Not all channels closed yet.");
        Status = SNMPX_ERR_SEQUENCE;
    }
    else
    {
        Status = SNMPX_FreeMgm();
    }

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "OUT: snmpx_undo_init(), Status: 0x%X",
                            Status);

    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          : SNMPX_UserGetFreeHDB                           +*/
/*+  Input/Output          : SNMPX_HDB_USER_PTR_TYPE SNMPX_LOCAL..pHDBUser  +*/
/*+  Result                : LSA_UINT16                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : SNMPX user -channel handle to get pointer to HDB          +*/
/*+                                                                         +*/
/*+  Result     : SNMPX_OK                                                  +*/
/*+               SNMPX_ERR_RESOURCE                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free User-HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 SNMPX_UserGetFreeHDB(
    SNMPX_HDB_USER_PTR_TYPE *pHDBUser)
{
    LSA_UINT16                Status  = SNMPX_OK;
    LSA_BOOL                  Found   = LSA_FALSE;
    SNMPX_HDB_USER_PTR_TYPE   pHDB    = SNMPX_Data.HUser;
    LSA_UINT32                i       =   0;

    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_UserGetFreeHDB(pHDBUser: %Xh)",
                            pHDBUser);

    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/

    while ( (! Found)  &&  ( i < SNMPX_CFG_MAX_CHANNELS))
    {
        if ( pHDB->UserState == SNMPX_HANDLE_STATE_FREE )
        {
            Found = LSA_TRUE;
        }
        else
        {
            i++;
            pHDB++;
        }
    }

    /* --------------------------------------------------------------------------*/
    /* If a free handle management was found we initialize it..                  */
    /* --------------------------------------------------------------------------*/

    if ( Found )
    {
        /* HDB-entry of the UserCnt */
        SNMPX_Data.MMgm.UsedUserCnt++;
        pHDB->UserState = SNMPX_HANDLE_STATE_CLOSED;
        *pHDBUser = pHDB;
    }
    else
    {
        Status = SNMPX_ERR_RESOURCE;
        SNMPX_PROGRAM_TRACE_00(LSA_TRACE_LEVEL_NOTE_HIGH,"SNMPXGetFreeUserHDB(). No more free handles.");
    }

    SNMPX_FUNCTION_TRACE_02(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_UserGetFreeHDB(Status: 0x%X, pHDB: 0x%X )",
                            Status,*pHDBUser);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SNMPX_UserReleaseHDB                        +*/
/*+  Input/Output          :    SNMPX_HDB_USER_PTR_TYPE pHDBUser            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser   : Pointer to User Handle to release                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release User-HDB                                          +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to SNMPX_HANDLE_STATE_FREE              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID SNMPX_UserReleaseHDB(
    SNMPX_HDB_USER_PTR_TYPE pHDB)
{
    SNMPX_FUNCTION_TRACE_01(LSA_TRACE_LEVEL_CHAT,
                            "IN : SNMPX_UserReleaseHDB(pHDB: 0x%X)",
                            pHDB);

    SNMPX_ASSERT_NULL_PTR(pHDB);

    /* free MIB objects if no unregister agent is done */
    SNMPX_FreeAllRegisterEntries(pHDB);

    SNMPX_FreeAllSessionEntries(pHDB);

    pHDB->pOCReq = LSA_NULL; /* delete RQB */

    pHDB->UserState = SNMPX_HANDLE_STATE_FREE;

    /* HDB-entry of the UserCnt */
    SNMPX_Data.MMgm.UsedUserCnt--;

    SNMPX_FUNCTION_TRACE_00(LSA_TRACE_LEVEL_CHAT,
                            "OUT: SNMPX_UserReleaseHDB()");
}

/*------------------------------------------------------------------------------------*/
/* returns the string that contains the same characters but in the opposite order.    */
/*------------------------------------------------------------------------------------*/
static LSA_VOID snmpx_strreverse(SNMPX_COMMON_MEM_U8_PTR_TYPE begin, SNMPX_COMMON_MEM_U8_PTR_TYPE end)
{
	LSA_UINT8 aux;
	while(end>begin)
	{
		aux=*end;
		*end--=*begin;
		*begin++=aux;
	}
}

/*------------------------------------------------------------------------------------*/
/*  snmpx_ultoa converts an unsigned long value to a null-terminated string using     */
/*  the specified base and stores the result in the array given by str parameter.     */
/*  str should be an array long enough to contain any possible value:                 */
/*  (sizeof(long)*8+1) for radix=2, i.e. 33 bytes in 32-bits platforms.               */
/*                                                                                    */
/*  Parameters:                                                                       */
/*  value   Value (only positive) to be converted to a string.                        */
/*  str     Array in memory where to store the resulting null-terminated string.      */
/*  base    Numerical base used to represent the value as a string, between 2 and 16, */
/*          where 10 means decimal base, 16 hexadecimal, 8 octal, and 2 binary.       */
/*  numbers Fills up with leading zeros if numbers <> 0 is given.                     */
/*          If the buffer is too small, you risk a buffer overflow.                   */
/*                                                                                    */
/* returns the number of characters written, not including the terminating            */
/* null character or 0 if an output error occurs.                                     */
/*------------------------------------------------------------------------------------*/
LSA_INT snmpx_ultoa(LSA_UINT32 value, SNMPX_COMMON_MEM_U8_PTR_TYPE str, LSA_UINT base, LSA_INT numbers)
{
	LSA_INT count = 0;
	static const char num[] = "0123456789abcdef";
	SNMPX_COMMON_MEM_U8_PTR_TYPE wstr=str;

	/* Validate base */
	if (base<2 || base>16){ *wstr='\0'; return 0; }

	/* Conversion. Number is reversed. */
	do
	{
		*wstr++ = (LSA_UINT8)num[value%base];
		count++;
    } while((value/=base) != 0);

	/* Fill up with leading zeros */
    while(count < numbers)
    {
    	*wstr++ = '0';
		count++;
    }

	*wstr = '\0';

	/* Reverse string */
	snmpx_strreverse(str,wstr-1);

	return count;
}

/*----------------------------------------------------------------------------*/
/* OID to ascii, e.g. "1.0.62439.1.1.1.1.1.1"                                 */
/* Return Value:                                                              */
/* snmpx_oid2a returns the number of characters written,                      */
/* not including the terminating null character,                              */
/* or a negative value if an output error occurs.                             */
/* A negative value is also returned if oidlen or more                        */
/* wide characters are requested to be written.                               */
/*----------------------------------------------------------------------------*/
LSA_INT snmpx_oid2a (const SNMPX_UPPER_OID_PTR_TYPE oid, LSA_UINT16 oidlen,
                   SNMPX_COMMON_MEM_U8_PTR_TYPE s_oid, LSA_UINT16 s_oidlen)
{
    LSA_UINT16 i;
    LSA_INT c;  /* written characters */

	SNMPX_ASSERT_NULL_PTR(oid);
	SNMPX_ASSERT_NULL_PTR(s_oid);

	s_oid[0] = '\0';
    for (i = 0, c = 0; i < oidlen; i++)
    {
        if (i)
        {
			s_oid[c++] = '.';
        }

		c += snmpx_ultoa(oid[i], &s_oid[c], 10, 0);
		if (c == 0) return 0;

		if (c >= s_oidlen) { s_oid[0] = '\0'; return -1; }
    }

	s_oid[c+1] = '\0';

    return c;
}

/*****************************************************************************/
/*  end of file SNMPX_SYS.C                                                  */
/*****************************************************************************/


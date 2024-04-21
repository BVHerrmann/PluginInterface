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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: eps_msg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Msg Api                                                              */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20069
#define EPS_MODULE_ID    LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */
#include "eps_sys.h"
#include "eps_trc.h"
#include "eps_locks.h"
#include "eps_msg.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

/*-----------------------------------------------------------------*/
/* Additional includes                                             */
/*-----------------------------------------------------------------*/
//*****ADONIS***************************************************************
#ifdef EPS_USE_RTOS_ADONIS
    #include <rt/fmq.h>

//*****LINUX****************************************************************
#elif ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    #include <mqueue.h>
    #include <errno.h>
	#include <sys/types.h>
    #include <unistd.h> // for calling of "getpid()"

//*****WINDOWS**************************************************************
#elif (EPS_PLF == EPS_PLF_WINDOWS_X86)
//lint -save -e18 -e114 -e157
    #include <Windows.h>
//lint -restore
    #include <stdio.h>
#else
    #error "EPS_PLF adaption not implemented for eps_msg"
#endif

/*-----------------------------------------------------------------*/
/* Defines                                                         */
/*-----------------------------------------------------------------*/
#define EPS_MSG_MAX_MSGQ                10  // maximum count of available message queues

//*****ADONIS***************************************************************
#ifdef EPS_USE_RTOS_ADONIS
    #define EPS_MSG_MSGQ                    fmq_t    
    #define EPS_MSG_MAX_MSGQ_POOL_SIZE      20  // maximum count of available messages in message queue pool

//*****LINUX****************************************************************
#elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000)  || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    #define EPS_MSG_MSGQ                    mqd_t    
    #define EPS_MSG_MSGQ_NAME               LSA_UINT8
    #define EPS_MSG_MSGQ_NAME_MAX_SIZE      100
    #define EPS_MSG_MSGQ_PATH_NAME          "/msgq%d_%d"
    #define EPS_MSG_MSGQ_MAX_MSG			100

//*****WINDOWS**************************************************************
#elif(EPS_PLF == EPS_PLF_WINDOWS_X86)
    #define EPS_MSG_MSGQ                    HANDLE
    #define EPS_MSG_MSGQ_SLOT               HANDLE
    #define EPS_MSG_MSGQ_NAME               LSA_UINT8
    #define EPS_MSG_MSGQ_NAME_MAX_SIZE      100
    #define EPS_MSG_MSGQ_PATH_NAME          "\\\\.\\mailslot\\mailslot%d_%d"
#else
    #error "EPS_PLF adaption not implemented for eps_msg"
#endif
/*-----------------------------------------------------------------*/
/* Types                                                           */
/*-----------------------------------------------------------------*/
//*****ADONIS***************************************************************
#ifdef EPS_USE_RTOS_ADONIS
typedef struct _EPS_MSG_MSG_TYPE
{
    uintptr_t               msg_ptr;
    LSA_CHAR                msg_data[EPS_MSG_MAX_MSG_SIZE];
}EPS_MSG_MSG_TYPE, *EPS_MSG_MSG_PTR_TYPE;

typedef struct
{
    LSA_BOOL                is_used;
    LSA_UINT16              msgq_id;
    EPS_MSG_MSGQ            msgq;
    EPS_MSG_MSGQ            msgq_pool;
    EPS_MSG_MSG_TYPE        msg_pool[EPS_MSG_MAX_MSGQ_POOL_SIZE];
}EPS_MSG_MSGQ_TYPE; 

//*****LINUX****************************************************************
#elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
typedef struct _EPS_MSG_MSG_TYPE
{
    LSA_CHAR                msg_data[EPS_MSG_MAX_MSG_SIZE];
}EPS_MSG_MSG_TYPE, *EPS_MSG_MSG_PTR_TYPE;

typedef struct
{
    LSA_BOOL                is_used;
    LSA_UINT16              msgq_id;
    EPS_MSG_MSGQ            msgq;
    EPS_MSG_MSGQ_NAME       msgq_name[EPS_MSG_MSGQ_NAME_MAX_SIZE];
}EPS_MSG_MSGQ_TYPE;

//*****WINDOWS**************************************************************
#elif(EPS_PLF == EPS_PLF_WINDOWS_X86)
typedef struct _EPS_MSG_MSG_TYPE
{    
    LSA_CHAR                msg_data[EPS_MSG_MAX_MSG_SIZE];
}EPS_MSG_MSG_TYPE;

typedef struct
{
    LSA_BOOL                is_used;
    LSA_UINT16              msgq_id;
    EPS_MSG_MSGQ            msgq;
    EPS_MSG_MSGQ_SLOT       msgq_slot;
    EPS_MSG_MSGQ_NAME       msgq_name[EPS_MSG_MSGQ_NAME_MAX_SIZE];
}EPS_MSG_MSGQ_TYPE; 
#else
#error "EPS_PLF adaption not implemented for eps_msg"
#endif

typedef struct  
{ 
    LSA_BOOL               is_running;                           // Initialized yes/no
    LSA_UINT16             enter_exit;                           // Own reentrance lock
    EPS_MSG_MSGQ_TYPE      msgqs[EPS_MSG_MAX_MSGQ];              // MSGQ list
} EPS_MSG_INSTANCE_TYPE;
/*-----------------------------------------------------------------*/
/* Global data                                                     */
/*-----------------------------------------------------------------*/
static EPS_MSG_INSTANCE_TYPE g_Eps_msg = {0};

#if((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
static int err_desc = 0;    // to save the errno
#endif

/*-----------------------------------------------------------------*/
/* Implementation of eps msg                                       */
/*-----------------------------------------------------------------*/

//******************************************************************************
//  Function: eps_msg_init
//
//  Description:
//
//      Initialize the eps msg system
//
//  Parameters:
//
//      None
//
//  Return:
//
//      None
//******************************************************************************
LSA_VOID eps_msg_init ( LSA_VOID )
{
    LSA_UINT16        entry;
    LSA_UINT32        lProcessID = 0;
    LSA_RESPONSE_TYPE retVal;
    

    EPS_ASSERT( !g_Eps_msg.is_running );

    // alloc the critical section
    retVal = eps_alloc_critical_section(&g_Eps_msg.enter_exit, LSA_FALSE);
    EPS_ASSERT( retVal == LSA_RET_OK );

    for ( entry = 0; entry < EPS_MSG_MAX_MSGQ; entry++ )
    {
        // set default values
        g_Eps_msg.msgqs[entry].is_used = LSA_FALSE;
        g_Eps_msg.msgqs[entry].msgq_id = 0;

        // fill in the names for the queues
        #if(EPS_PLF == EPS_PLF_WINDOWS_X86)
        {
            int retValPrintf;

            lProcessID = (LSA_UINT32)GetCurrentProcessId();

            retValPrintf = eps_sprintf(g_Eps_msg.msgqs[entry].msgq_name, EPS_MSG_MSGQ_PATH_NAME, entry, lProcessID);
            EPS_ASSERT(retValPrintf != -1);
        }
        #elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
        {
            int retValPrintf;
            lProcessID = (LSA_UINT32)getpid();
    
            retValPrintf = eps_sprintf(g_Eps_msg.msgqs[entry].msgq_name, EPS_MSG_MSGQ_PATH_NAME, entry, lProcessID);
            EPS_ASSERT(retValPrintf != -1);
        }
        #else
        LSA_UNUSED_ARG(lProcessID);
        #endif   
    }

    g_Eps_msg.is_running = LSA_TRUE; // Now initialized
    
    EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_msg_init(): msgqs setup done, max queues(%u)", EPS_MSG_MAX_MSGQ );
}

//******************************************************************************
//  Function: eps_msg_undo_init
//
//  Description:
//
//      Deinitialize the eps msg system
//
//  Parameters:
//
//      None
//
//  Return:
//
//      None
//******************************************************************************
LSA_VOID eps_msg_undo_init ( LSA_VOID )
{
    LSA_UINT16 entry;
    LSA_RESPONSE_TYPE retVal;

    EPS_ASSERT( g_Eps_msg.is_running );

    for ( entry = 0; entry < EPS_MSG_MAX_MSGQ; entry++ )
    {
        // check all msgq are freed 
        EPS_ASSERT( !g_Eps_msg.msgqs[entry].is_used );
    }

    g_Eps_msg.is_running = LSA_FALSE;

    retVal = eps_free_critical_section(g_Eps_msg.enter_exit);
    EPS_ASSERT( LSA_RET_OK == retVal );

    EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_msg_undo_init(): msgqs cleanup done, max queues(%u)", EPS_MSG_MAX_MSGQ );
}

//******************************************************************************
//  Function: eps_msg_alloc_msgq
//
//  Description:
//
//      Allocates a message queue from the pool
//
//  Parameters:
//
//      msgq_id_ptr     in out: returns the id of the allocated message queue
//
//  Return:
//
//      LSA_RET_OK              Message queue allocated
//      LSA_RET_ERR_RESOURCE    NO message queue allocated
//******************************************************************************
LSA_RESPONSE_TYPE eps_msg_alloc_msgq ( LSA_UINT16 *msgq_id_ptr )
{    
    LSA_UINT16        entry;    
    LSA_RESPONSE_TYPE retVal = LSA_RET_ERR_RESOURCE;   

    #ifdef EPS_USE_RTOS_ADONIS
    LSA_UINT16        init_pool;
    #endif

    EPS_ASSERT( msgq_id_ptr != LSA_NULL );
    EPS_ASSERT( g_Eps_msg.is_running );

    eps_enter_critical_section(g_Eps_msg.enter_exit);

    // get the next available internal message queue structure
    for ( entry = 0; entry < EPS_MSG_MAX_MSGQ; entry++ )
    {    
        if ( !g_Eps_msg.msgqs[entry].is_used )
        {
            //*****ADONIS***************************************************************
            #ifdef EPS_USE_RTOS_ADONIS
            fmq_init(&g_Eps_msg.msgqs[entry].msgq);           // init the queue
            fmq_init(&g_Eps_msg.msgqs[entry].msgq_pool);      // init the pool queue
            
            // initialize the message pool containers
            for( init_pool = 0; init_pool < EPS_MSG_MAX_MSGQ_POOL_SIZE; init_pool ++ )
            {
                fmq_send(&g_Eps_msg.msgqs[entry].msgq_pool, &(g_Eps_msg.msgqs[entry].msg_pool[init_pool]), 0);
            }

            //*****LINUX****************************************************************
            #elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
            struct mq_attr msgq_attributes;

            eps_memset(&msgq_attributes, 0, sizeof(struct mq_attr));

            // set the queue attributes
            msgq_attributes.mq_msgsize 	= EPS_MSG_MAX_MSG_SIZE;
            msgq_attributes.mq_maxmsg	= EPS_MSG_MSGQ_MAX_MSG;

            g_Eps_msg.msgqs[entry].msgq	   = mq_open(	(char*) g_Eps_msg.msgqs[entry].msgq_name,
														O_CREAT | O_RDWR,		// create in read and write mode
														0777, 					// read,write,execute rights for all users
														&msgq_attributes);

            if(g_Eps_msg.msgqs[entry].msgq == (mqd_t)-1)
                // error
            {
                err_desc = errno;
                EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "eps_msg_alloc_msgq(): mq_open: errno(%d)", err_desc );
                EPS_FATAL("eps_msg_alloc_msgq(): mq_open");
            }
            
            //*****WINDOWS**************************************************************
            #elif(EPS_PLF == EPS_PLF_WINDOWS_X86)
            
            // open the mailslot
            g_Eps_msg.msgqs[entry].msgq_slot = CreateMailslot((LPCSTR)g_Eps_msg.msgqs[entry].msgq_name,
                                                            (DWORD) EPS_MSG_MAX_MSG_SIZE,
                                                            MAILSLOT_WAIT_FOREVER,
                                                            (LPSECURITY_ATTRIBUTES) LSA_NULL);

            EPS_ASSERT(g_Eps_msg.msgqs[entry].msgq_slot != INVALID_HANDLE_VALUE);

            // link a "file" to this mailslot, the "file" is used to write the messages to it
            g_Eps_msg.msgqs[entry].msgq = CreateFile( (LPCSTR)g_Eps_msg.msgqs[entry].msgq_name,
                                                    GENERIC_WRITE,
                                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                    (LPSECURITY_ATTRIBUTES) LSA_NULL,
                                                    OPEN_EXISTING,
                                                    FILE_ATTRIBUTE_NORMAL,
                                                    (HANDLE) LSA_NULL );
                                                    
            EPS_ASSERT(g_Eps_msg.msgqs[entry].msgq != INVALID_HANDLE_VALUE);

            #else
            #error "EPS_PLF adaption not implemented for eps_msg"
            #endif

            g_Eps_msg.msgqs[entry].msgq_id = entry;
      
            g_Eps_msg.msgqs[entry].is_used = LSA_TRUE;            
            
            *msgq_id_ptr = entry;
            
            EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_msg_alloc_msgq(): msgq allocated: msgq_id(%d)", entry );

            retVal = LSA_RET_OK;
            break;
        }
    }

    eps_exit_critical_section(g_Eps_msg.enter_exit);

    return ( retVal );
}

//******************************************************************************
//  Function: eps_msg_free_msgq
//
//  Description:
//
//      Frees a message queue from the pool
//
//  Parameters:
//
//      msgq_id     in : id of the message queue
//
//  Return:
//
//      LSA_RET_OK              Message queue free successfull
//      LSA_RET_ERR_PARAM       Given message queue not viable
//      LSA_RET_ERR_RESOURCE    Message queue free NOT successfull
//******************************************************************************
LSA_RESPONSE_TYPE eps_msg_free_msgq ( LSA_UINT16 msgq_id )
{    
    LSA_RESPONSE_TYPE   retVal = LSA_RET_OK;
    int                 retValMsgq;

    EPS_ASSERT( g_Eps_msg.is_running );

    eps_enter_critical_section(g_Eps_msg.enter_exit);

    if (   (msgq_id < EPS_MSG_MAX_MSGQ)                     /* in range ? */
        && (g_Eps_msg.msgqs[msgq_id].is_used == LSA_TRUE)   /* allocated ? */
       )
    {
        //*****ADONIS***************************************************************
        #ifdef EPS_USE_RTOS_ADONIS
        
        retValMsgq = fmq_destroy(&g_Eps_msg.msgqs[msgq_id].msgq);
        //EPS_ASSERT( retValMsgq == 0 );
        if (retValMsgq != 0)
        {
            EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "eps_msg_free_msgq(): retValMsgq(%d) != 0 for msgq_id(%d)", retValMsgq, msgq_id);
            EPS_FATAL(0);
        }

        //*****LINUX****************************************************************
        #elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
        
        // in linux you have to close and unlink the message queue so the system is "clean"
        
        retValMsgq = (int) mq_close(g_Eps_msg.msgqs[msgq_id].msgq);
        
        if (retValMsgq != 0)
        {
            err_desc = errno;
            EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "eps_msg_free_msgq(): mq_close: errno(%d) msgq_id(%d)", err_desc, msgq_id);
            EPS_FATAL("eps_msg_free_msgq(): mq_close");
        }

        retValMsgq = (int) mq_unlink((char*) g_Eps_msg.msgqs[msgq_id].msgq_name);

        if (retValMsgq != 0)
        {
            err_desc = errno;
            EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "eps_msg_free_msgq(): mq_unlink: errno(%d) msgq_id(%d)", err_desc, msgq_id);
            EPS_FATAL("eps_msg_free_msgq(): mq_unlink");
        }

        //*****WINDOWS**************************************************************
        #elif(EPS_PLF == EPS_PLF_WINDOWS_X86)
        
        // Close all handles of the message queue -> "file" and mailslot
        retValMsgq = (int) CloseHandle(g_Eps_msg.msgqs[msgq_id].msgq);
        EPS_ASSERT( retValMsgq != 0 );

        retValMsgq = (int) CloseHandle(g_Eps_msg.msgqs[msgq_id].msgq_slot);
        EPS_ASSERT( retValMsgq != 0 );

        #else
        #error "EPS_PLF adaption not implemented for eps_msg"
        #endif
        
        g_Eps_msg.msgqs[msgq_id].msgq_id = 0;

        g_Eps_msg.msgqs[msgq_id].is_used = LSA_FALSE;
    
        retVal = LSA_RET_OK;
        
        EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_msg_free_msgq(): msgq free: msgq_id(%d)", msgq_id );
    }
    else
    {
        retVal = LSA_RET_ERR_PARAM;

        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_msg_free_msgq(): msgq_id(%d) is ouf range or not allocated", msgq_id);
    }

    eps_exit_critical_section(g_Eps_msg.enter_exit);

    return ( retVal );
}

//******************************************************************************
//  Function: eps_msg_send
//
//  Description:
//
//      Sends a message to a message queue
//
//  Parameters:
//
//      msgq_id     in : id of the message queue you will send to
//      msg_ptr     in : pointer to the message you will send
//
//  Return:
//
//      None
//******************************************************************************
LSA_VOID eps_msg_send ( LSA_UINT16 msgq_id, const LSA_UINT8* msg_ptr )
{       
    #ifdef EPS_USE_RTOS_ADONIS   
    EPS_MSG_MSG_PTR_TYPE    send_msg_ptr;
    unsigned int            msg_prio;
	#elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    EPS_MSG_MSG_TYPE        send_msg;
    int                     retVal;
    #elif(EPS_PLF == EPS_PLF_WINDOWS_X86)
    EPS_MSG_MSG_TYPE        send_msg = {0};
    int                     retVal;
    LSA_UINT32              lWritten = 0;
    #endif
    
    EPS_ASSERT( g_Eps_msg.is_running );
    EPS_ASSERT( g_Eps_msg.msgqs[msgq_id].is_used );
    
    // in all cases the given pointer to the message is used to copy the message
    // in an internal variable which is used further

    //*****ADONIS***************************************************************
    #ifdef EPS_USE_RTOS_ADONIS
    // get a message container from message pool (thats the way it works, see adonis api)
    fmq_receive(&g_Eps_msg.msgqs[msgq_id].msgq_pool, (LSA_VOID*)&send_msg_ptr, &msg_prio);
    
    // fill in values
    eps_memset(&send_msg_ptr->msg_data, 0, sizeof(send_msg_ptr->msg_data));
    eps_strcpy(send_msg_ptr->msg_data, (const char*) msg_ptr);
    
    // send message to message queue
    fmq_send(&g_Eps_msg.msgqs[msgq_id].msgq, send_msg_ptr, 0);   

    //*****LINUX****************************************************************
    #elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))

    // fill in values
	eps_memset(send_msg.msg_data, 0, sizeof(send_msg.msg_data));
	eps_strcpy(send_msg.msg_data, msg_ptr);

	retVal = mq_send(	g_Eps_msg.msgqs[msgq_id].msgq,
						send_msg.msg_data,
						EPS_MSG_MAX_MSG_SIZE,
						0);

    if(retVal != 0)
        // error
    {
        err_desc = errno;
        EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "eps_msg_send(): mq_send: errno(%d)", err_desc );
        EPS_FATAL("eps_msg_send(): mq_send");
    }

    //*****WINDOWS**************************************************************
    #elif(EPS_PLF == EPS_PLF_WINDOWS_X86)
    
    // fill in values
    eps_memset(send_msg.msg_data, 0, sizeof(send_msg.msg_data));
    eps_strcpy(send_msg.msg_data, msg_ptr);
    
    // write the message to the "file" in windows
    retVal = (LSA_BOOL) WriteFile(  g_Eps_msg.msgqs[msgq_id].msgq,
                                    send_msg.msg_data,
                                    sizeof(send_msg.msg_data),
                                    (LPDWORD) &lWritten,
                                    (LPOVERLAPPED) NULL );
    
    EPS_ASSERT( retVal != 0 );

    #else
    #error "EPS_PLF adaption not implemented for eps_msg"
    #endif
}

//******************************************************************************
//  Function: eps_msg_receive
//
//  Description:
//
//      Receives a message from a message queue - with blocking wait for a msg
//
//  Parameters:
//
//      msgq_id     in : id of the message queue you will receive from
//      msg_ptr     in : pointer to the message storage to receive
//
//  Return:
//
//      None
//******************************************************************************
LSA_VOID eps_msg_receive ( LSA_UINT16 msgq_id, LSA_UINT8* msg_ptr )
{    
    #ifdef EPS_USE_RTOS_ADONIS   
    EPS_MSG_MSG_PTR_TYPE    receive_msg_ptr;
    unsigned int            msg_prio;
	#elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    EPS_MSG_MSG_TYPE        receive_msg;
	int                     retVal;
	unsigned int			msg_prio;
    #elif(EPS_PLF == EPS_PLF_WINDOWS_X86)
    EPS_MSG_MSG_TYPE        receive_msg = {0};
    LSA_UINT32              lRead = 0;
    #endif
    
    EPS_ASSERT( g_Eps_msg.is_running );
    EPS_ASSERT( g_Eps_msg.msgqs[msgq_id].is_used );
    
    // in all cases the receive message will be stored in local variable
    // then it will be copied to the given pointer

    //*****ADONIS***************************************************************
    #ifdef EPS_USE_RTOS_ADONIS
    // get a message from message queue
    fmq_receive(&g_Eps_msg.msgqs[msgq_id].msgq, (LSA_VOID*)&receive_msg_ptr, &msg_prio);
    
    // extract values
    eps_strcpy(msg_ptr, receive_msg_ptr->msg_data);
    
    // send message back to message pool, therefore it is available again
    fmq_send(&g_Eps_msg.msgqs[msgq_id].msgq_pool, receive_msg_ptr, 0);    

    //*****LINUX****************************************************************
    #elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))

    eps_memset(receive_msg.msg_data, 0, sizeof(receive_msg.msg_data));

    retVal = mq_receive(	g_Eps_msg.msgqs[msgq_id].msgq,
    						receive_msg.msg_data,
    						EPS_MSG_MAX_MSG_SIZE,
    						&msg_prio);

    if(retVal == -1)
        // error
    {
        err_desc = errno;
        EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "eps_msg_receive(): mq_receive: errno(%d)", err_desc );
        EPS_FATAL("eps_msg_receive(): mq_receive");
    }

    // extract values
	eps_strcpy(msg_ptr, receive_msg.msg_data);

    //*****WINDOWS**************************************************************
    #elif(EPS_PLF == EPS_PLF_WINDOWS_X86)

	// windows read file is nonblocking, therefore we have to do a while to simulate the blocking wait
	// it will loop until the number of read bytes is greater than 0
	
	while(lRead == 0)
    {
	    // read from the "file", discard return value
        (void)ReadFile(   g_Eps_msg.msgqs[msgq_id].msgq_slot,
                                        (LSA_VOID*)receive_msg.msg_data,
                                        EPS_MSG_MAX_MSG_SIZE,
                                        (LPDWORD) &lRead,
                                        (LPOVERLAPPED) NULL );   
    }

    // extract values
    eps_strcpy(msg_ptr, receive_msg.msg_data);    

    #else
    #error "EPS_PLF adaption not implemented for eps_msg"
    #endif
}

//******************************************************************************
//  Function: eps_msg_tryreceive
//
//  Description:
//
//      Receives a message from a message queue - no blocking wait
//
//  Parameters:
//
//      msgq_id     in : id of the message queue you will receive from
//      msg_ptr     in : pointer to the message storage to receive
//
//  Return:
//
//      0  - msg received
//  EAGAIN - no msg available
//******************************************************************************
LSA_UINT16 eps_msg_tryreceive ( LSA_UINT16 msgq_id, LSA_UINT8* msg_ptr )
{    
    #ifdef EPS_USE_RTOS_ADONIS   
    EPS_MSG_MSG_PTR_TYPE    receive_msg_ptr;
    LSA_UINT16              retVal;
    unsigned int            msg_prio;
    #elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    LSA_UNUSED_ARG(msg_ptr);
    #elif(EPS_PLF == EPS_PLF_WINDOWS_X86)
    LSA_UNUSED_ARG(msg_ptr);
    #endif
    
    EPS_ASSERT( g_Eps_msg.is_running );
    EPS_ASSERT( g_Eps_msg.msgqs[msgq_id].is_used );
    
    // in all cases the receive message will be stored in local variable
    // then it will be copied to the given pointer

    //*****ADONIS***************************************************************
    #ifdef EPS_USE_RTOS_ADONIS
    // get a message from message queue
    retVal = fmq_tryreceive(&g_Eps_msg.msgqs[msgq_id].msgq, (LSA_VOID*)&receive_msg_ptr, &msg_prio);
    
    if  (retVal == 0)
        // there was a msg
    {
        // extract values
        eps_strcpy(msg_ptr, receive_msg_ptr->msg_data);
        
        // send message back to message pool, therefore it is available again
        fmq_send(&g_Eps_msg.msgqs[msgq_id].msgq_pool, receive_msg_ptr, 0);    
    }
    
    return retVal;

    //*****LINUX****************************************************************
    #elif((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))

    EPS_FATAL("eps_msg_tryreceive - not implemented for LINUX!");
    return 0;

    //*****WINDOWS**************************************************************
    #elif(EPS_PLF == EPS_PLF_WINDOWS_X86)
    
    EPS_FATAL("eps_msg_tryreceive - not implemented for WINDOWS!");   
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    return 0;

    #else
    #error "EPS_PLF adaption not implemented for eps_msg"
    #endif
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

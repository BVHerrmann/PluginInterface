#ifndef EPS_MSG_H_
#define EPS_MSG_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: eps_msg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
    
/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
#define EPS_MSG_MAX_MSG_SIZE            256
    
/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
LSA_VOID eps_msg_init                   ( LSA_VOID );
LSA_VOID eps_msg_undo_init              ( LSA_VOID );

LSA_RESPONSE_TYPE eps_msg_alloc_msgq    ( LSA_UINT16 *msgq_id_ptr );
LSA_RESPONSE_TYPE eps_msg_free_msgq     ( LSA_UINT16 msgq_id );

LSA_VOID    eps_msg_send                   ( LSA_UINT16 msgq_id, const LSA_UINT8* msg_ptr );
LSA_VOID    eps_msg_receive                ( LSA_UINT16 msgq_id, LSA_UINT8* msg_ptr );
LSA_UINT16  eps_msg_tryreceive             ( LSA_UINT16 msgq_id, LSA_UINT8* msg_ptr );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_MSG_H_ */

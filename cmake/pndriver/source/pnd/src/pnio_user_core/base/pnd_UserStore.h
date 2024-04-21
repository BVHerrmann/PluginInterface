#ifndef PND_USER_STORE_H
#define PND_USER_STORE_H

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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_UserStore.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Interface for PNIO user storage class (used for one PNIO IF)             */
/*****************************************************************************/

#include "pnd_IUserStore.h"

/////////////////////////////////////////////////////////////////////////////
//  Types
/////////////////////////////////////////////////////////////////////////////

class CBgzdInfoStore;

/////////////////////////////////////////////////////////////////////////////
// CUserStore  PNIO User storage class for one PNIO IF
/////////////////////////////////////////////////////////////////////////////

class CUserStore: public IUserStore
{
	// Attributes
private:
	CUserNode       *m_pUser[PNIO_USER_MAX];    // PNIO Users for this PNIO IF
	UserNodeVector   m_requestList;             // List for startup/shutdown users in a sequence

	// Construction/ Destruction
public:
	CUserStore(); 
	CUserStore( PND_HANDLE_PTR_TYPE pHandle, PND_PNUC_LADDR_PTR_TYPE laddr_ptr, CUserNode *pParent );
	virtual ~CUserStore();

	// Methodes
	LSA_VOID StartUpdateBgzd();

	// Accessors 
	CUserNode* GetUser( PND_PNIO_USER_TYPE type );

	// overridable methodes methodes for CUserNode IF
	virtual string   GetKey();
	virtual LSA_UINT GetNumber();
	virtual LSA_VOID Startup( LSA_VOID* p_rb );
	virtual LSA_VOID Shutdown( LSA_VOID* p_rb );

protected:
	// Class methodes ( Used for callbacks )
	static LSA_VOID StartupDone( void* p_inst, void* p_rb );
	static LSA_VOID ShutdownDone( void* p_inst, void* p_rb );

private:
	CUserNode* getNextUser();
	CUserNode* createUser( PND_PNIO_USER_TYPE userType, PND_PNUC_LADDR_PTR_TYPE laddr_ptr );
};

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PCIOX_USER_STORE_H */

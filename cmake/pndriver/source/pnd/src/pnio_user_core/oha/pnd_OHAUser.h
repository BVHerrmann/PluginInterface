#ifndef PND_OHA_USER_H
#define PND_OHA_USER_H
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
/*  F i l e               &F: pnd_OHAUser.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Interface for OHA User                                                   */
/*****************************************************************************/

#include "pnd_IOHAUser.h"
#include "pnd_IPndAdapter.h"
#include "pnd_IOHAEventHandler.h"

class CUserNode;
class COHAEventHandler;

/////////////////////////////////////////////////////////////////////////////
// COHAUser Interface for OHA User
/////////////////////////////////////////////////////////////////////////////

class COHAUser : public IOHAUser
{
	// Attributes
private:
	LSA_HANDLE_TYPE           m_pnioHandle;      // OHA handle
	LSA_SYS_PATH_TYPE         m_sysPath;         // Sys path for Open channel
	PND_PARAM_OHA_HD_IF_TYPE  m_ohaParam;        // OHA setup params
	IOHAEventHandler         *m_pEventHandler;   // OHA event (indication) handling
	PND_RQB_PTR_TYPE          m_responseRQB;     // Stored Response RQB for startup/shutdown
	UserRqbVector             m_remaResources;   // Rema Resources
	LSA_BOOL				  m_isDcpRequestActive; // DCP request active yes/no

	// IF specific data  for BGZD LED check
	string  m_nameOfStation;   // NameOfStation
	string  m_ipAddr;          // IP address
	string  m_subnetMask;      // Subnet mask
	string  m_gatewayIP;       // Gateway

	// Construction/ Destruction
public:
	COHAUser( PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent,  IPndAdapter* _pPndAdapter);
	virtual ~COHAUser();
	LSA_VOID Create( LSA_VOID_PTR_TYPE pConfig, IOHAEventHandler* pOhaEventHandler );

	// Accessors Read
	LSA_VOID GetStationType(LSA_UINT8* pBuffer) const;
	LSA_VOID GetInterfaceData( LSA_UINT32* pIp, LSA_UINT32* pMask, LSA_UINT32* pGwIp, LSA_UINT8* pName ) const;
	LSA_BOOL GetIpAddrValidationState() const;
	LSA_BOOL GetNameOfStationValidationState() const;

	// Accessors Write
	LSA_VOID SetInterfaceData( const LSA_UINT8* pIp, const LSA_UINT8* pMask, const LSA_UINT8* pGwIp, LSA_BOOL ipDup, const LSA_UINT8* pName, LSA_UINT16 size, LSA_BOOL nosDup );

	// Methodes to implement UserIF interface
	virtual string   GetKey();
	virtual LSA_UINT GetNumber();
	virtual LSA_VOID Startup( LSA_VOID* p_rb );
	virtual LSA_VOID Shutdown( LSA_VOID* p_rb );
	virtual LSA_HANDLE_TYPE GetPnioHandle();

	// Requests
	LSA_VOID OpenChannel();
	LSA_VOID CloseChannel();
	LSA_VOID RemaRead( LSA_UINT32 remaIdx, LSA_UINT32 dataSize );
	LSA_VOID DcpAccessInit();
	LSA_VOID DcpRequest(LSA_UINT8 event, LSA_UINT8 reason);
	LSA_VOID SetDcpRole();
	LSA_VOID SetDcpStationType();
	LSA_VOID LldpAccessInit( LSA_UINT16 portID );
	LSA_VOID SNMPWriteSysName();
    LSA_VOID CreateSystemDescription(string& desc) const;
	LSA_VOID SNMPWriteSysDescr();
	LSA_VOID SNMPWriteIfDescr( LSA_UINT16 portID );
	LSA_VOID SetAddress( LSA_VOID* p_rb );
    LSA_VOID WriteParameter(LSA_VOID* p_rb);
    LSA_VOID RemaProvide( PND_RQB_PTR_TYPE p_rb );
    LSA_VOID RemaReProvide( PND_RQB_PTR_TYPE p_rb );
	LSA_VOID RemaReleaseResources();

protected:
	// Class methodes ( Used for callbacks from PNIO )
	static LSA_VOID OpenChannelDone( void* p_inst, void* p_rb);
	static LSA_VOID CloseChannelDone( void* p_inst, void* p_rb);
	static LSA_VOID RemaReadDone( void* p_inst, void* p_rb);
	static LSA_VOID DcpAccessInitDone( void* p_inst, void* p_rb);
	static LSA_VOID DcpRequestDone(void* p_inst, void* p_rb);
	static LSA_VOID SetDcpRoleDone( void* p_inst, void* p_rb);
	static LSA_VOID SetDcpStationTypeDone( void* p_inst, void* p_rb);
	static LSA_VOID LldpAccessInitDone( void* p_inst, void* p_rb);
	static LSA_VOID SNMPWriteSysNameDone( void* p_inst, void* p_rb);
	static LSA_VOID SNMPWriteSysDescrDone( void* p_inst, void* p_rb);
	static LSA_VOID SNMPWriteIfDescrDone( void* p_inst, void* p_rb);
	static LSA_VOID SetAddressDone( void* p_inst, void* p_rb);
    static LSA_VOID WriteParameterDone( void* p_inst, void* p_rb);

private:
	LSA_VOID startupDone( LSA_UINT32 pnioErr  );
	LSA_VOID shutdownDone( LSA_UINT32 pnioErr );
	LSA_VOID SetRemaResource(PND_RQB_PTR_TYPE pUser);
	PND_RQB_PTR_TYPE GetRemaResource();
    IPndAdapter* m_pPndAdapter;
};

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_OHA_USER_H */

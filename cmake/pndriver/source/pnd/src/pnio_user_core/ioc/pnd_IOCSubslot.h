#ifndef PND_IOC_SUBSLOT_H
#define PND_IOC_SUBSLOT_H
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
/*  F i l e               &F: pnd_IOCSubslot.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of controller subslot class (shadow real cfg on IOC)          */
/*                                                                           */
/*****************************************************************************/

class CIOCDevice;
class CIOCAr;

/////////////////////////////////////////////////////////////////////////////
// CIOCSubslot  storage class for an IOC subslot (shadow of device subslot)
/////////////////////////////////////////////////////////////////////////////

class CIOCSubslot : public CUserNode
{
    // Attributes
private:
    LSA_UINT32  m_api;                  // API number  (KEY value1 of node)
    LSA_UINT16  m_slotNr;               // Slot number  (KEY value2 of node)
    LSA_UINT16  m_subslotNr;            // Subslot number (KEY value3 of node)
    LSA_UINT16  m_slotLaddr;            // Logical address of slot
    LSA_UINT16  m_subslotLaddr;         // Logical address of subslot
    LSA_UINT32  m_slotIdent;            // Ident of slot
    LSA_UINT32  m_subslotIdent;         // Ident of subslot
	LSA_UINT32  m_reductionRatio;        
	LSA_UINT32  m_phase;                
	LSA_UINT32  m_cycleTime;

    // IO attributes
    LSA_UINT16        m_subslotProp;      // Subslot properties (with all reduced bits)
    LSA_BOOL          m_isDiscardIOXS;    // Discard IOXS in frame yes/no
    LSA_BOOL          m_isReducedInput;   // Input IO reduced to zero size in frame yes/no
    LSA_BOOL          m_isReducedOutput;  // Output IO reduced to zero size in frame yes/no
    PND_IO_ADDR_TYPE  m_input;            // IO properties input (dataless/input/inout)
    PND_IO_ADDR_TYPE  m_output;           // IO properties output (output/inout)

    // Construction/ Destruction
public:
    CIOCSubslot( LSA_UINT32 api, LSA_UINT16 slot, LSA_UINT16 subSlot, PND_HANDLE_PTR_TYPE pHandle, CUserNode* parent );
    virtual ~CIOCSubslot();
    LSA_VOID Create( LSA_VOID* pData );

    // Accessors Read
	static string GetKey(const LSA_UINT32 api, const LSA_UINT16 slot, const LSA_UINT16 subSlot);
    LSA_UINT32 GetApiNr() const;
    LSA_UINT16 GetSlotNr() const;
    LSA_UINT16 GetSubslotNr();
    LSA_UINT16 GetSlotLogAddr() const;
    LSA_UINT16 GetSubslotLogAddr() const;
    LSA_UINT32 GetSlotIdent() const;
    LSA_UINT32 GetSubslotIdent() const;
	LSA_UINT32 GetReductionRatio() const;
	LSA_UINT32 GetPhase() const;
	LSA_UINT32 GetCycleTime() const;
	LSA_UINT16 GetDeviceNr();
	CIOCDevice* getDevice();

    PND_IO_ADDR_TYPE GetAddressForInput() const;
    PND_IO_ADDR_TYPE GetAddressForOutput() const;
	LSA_BOOL isMixedSubmodule() const;
	
    // Accessors Write
    LSA_VOID SetState( LSA_UINT16 state );
    LSA_VOID SetInputOffset( LSA_UINT16 offset );
    LSA_VOID SetInputOffsetIOCS( LSA_UINT16 offset );
    LSA_VOID SetOutputOffset( LSA_UINT16 offset );
    LSA_VOID SetOutputOffsetIOCS( LSA_UINT16 offset );

    // Accessors to get/set BGZD state and IODATA info
    LSA_VOID UpdateFromAlarm( LSA_BOOL hasDiag, LSA_UINT32 maintenanceStatus );
    LSA_VOID ResetARP();

    // Overrides to implement UserNode methodes
    virtual string             GetKey();
    virtual LSA_UINT           GetNumber();
    virtual LSA_HANDLE_TYPE    GetPnioHandle();
	
    // Overrides to implement UserNode PND request methodes (CM-Requests)
	virtual LSA_VOID DeviceActivate( PND_RQB_PTR_TYPE p_rb );

    // Requests (to CM-CL)
    LSA_VOID RecordRead( PND_RQB_PTR_TYPE p_rb );
    LSA_VOID RecordWrite( PND_RQB_PTR_TYPE p_rb );
    LSA_VOID Activate( PND_RQB_PTR_TYPE p_rb );
    LSA_VOID Passivate( PND_RQB_PTR_TYPE p_rb );

protected:
    // Class methodes ( Used for callbacks from PNIO )
    static LSA_VOID RecordReadDone( LSA_VOID* p_inst, LSA_VOID* p_rb );
    static LSA_VOID RecordWriteDone( LSA_VOID* p_inst, LSA_VOID* p_rb );
    static LSA_VOID ActivateDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
    static LSA_VOID PassivateDone( LSA_VOID* p_inst, LSA_VOID* p_rb);

private:
    CIOCAr*     getAr();

    LSA_VOID setIOProperties();

};


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_IOC_SUBSLOT_H */

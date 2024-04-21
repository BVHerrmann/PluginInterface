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
/*  F i l e               &F: pnd_XmlParamStore.h                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition for PND XML param storage classes                             */
/*                                                                           */
/*****************************************************************************/

#ifndef PND_XML_PARAM_STORE_H
#define PND_XML_PARAM_STORE_H

extern "C" {
    #include "pnd_xml_std_interface.h"
    #include "pnd_xml_helper.h"
}

class CPnioXMLStore;
class CParamStoreImplIF;


/*----------------------------------------------------------------------------*/
/* Defines                                                                    */
/*----------------------------------------------------------------------------*/

#define XML_BLOCK_HEADER                  8       // length of block header in bytes

/*----------------------------------------------------------------------------*/
/* Types                                                                      */
/*----------------------------------------------------------------------------*/

typedef struct pnd_xml_record_tag // data records
{
    LSA_UINT32          record_index;                   // record index
    LSA_UINT32          record_data_length;             // size of record data  
    LSA_UINT8*          record_data;                    // record data buffer
} PND_XML_RECORD_TYPE, *PND_XML_RECORD_PTR_TYPE;

typedef struct pnd_xml_ioc_tag    // XML parameters - IOC
{
    PND_PARAM_IOC_HD_IF_TYPE  ioc_hd;                   // settigs used for IOC (CM-CL) User
    LSA_UINT16          cmi_timeout100ms;               // CMIActivityTimeout
    LSA_UINT16          appl_timeout1sec;               // RPC Remote Application Timeout
} PND_XML_IOC_TYPE;

typedef struct pnd_xml_oha_tag    // XML parameters - OHA
{
    PND_PARAM_OHA_HD_IF_TYPE  oha_hd;                   // settigs used for OHA
} PND_XML_OHA_TYPE;

typedef struct pnd_xml_pd_tag     // XML parameters - PD
{
    PND_PARAM_PD_HD_IF_TYPE pd_hd;                      // used to get the IF params for PD user
    PND_XML_RECORD_TYPE     ip_v4_suite;                // IPV4Suite
    LSA_UINT16              ip_address_validation_local;// IPAddressValidationLocal
    LSA_UINT16              name_of_station_validation; // NameOfStationValidation
    PND_XML_RECORD_TYPE     name_of_station;            // NameOfStation
    PND_XML_RECORD_TYPE     send_clock;                 // SendClock
    PrmRecordVector         prm_record_list;            // PRM Records for PDEV
} PND_XML_PD_TYPE;

typedef struct pnd_xml_iso_tag    // XML parameters - ISO
{
    PND_PARAM_ISO_HD_IF_TYPE  iso_hd;                   // settigs used for ISO
} PND_XML_ISO_TYPE;

typedef struct pnd_xml_rema_tag     // XML parameters - Rema
{
    PND_XML_RECORD_TYPE     ip_v4_suite;                // IPV4Suite
    LSA_UINT16              ip_address_validation_local;// IPAddressValidationLocal
    LSA_UINT16              name_of_station_validation; // NameOfStationValidation
    PND_XML_RECORD_TYPE     name_of_station;            // NameOfStation
} PND_REMA_XML_PD_TYPE;

typedef struct pnd_xml_pniod_properties_tag // XML parameters - Device Properties
{
    LSA_UINT16          vendor_id;                      // VendorID
    LSA_UINT16          device_id;                      // DeviceID
    LSA_UINT16          instance_id;                    // Instance
    LSA_UINT16          max_record_size;                // Max record data size
    LSA_BOOL            write_multiple_is_supported;    // MultipleWriteSupported
    LSA_UINT32          planned_mode;                   // DeviceMode
    LSA_UINT16          device_laddr;                   // Device LADDR
	LSA_UINT16          device_number;                  // Device Number (Station Number)
} PND_XML_PNIOD_PROPERTIES_TYPE, *PND_XML_PNIOD_PROPERTIES_PTR_TYPE;

typedef struct pnd_xml_alarm_cr_data_tag // XML parameters - AlarmCRData
{
    LSA_UINT32          alcr_properties;                // AlarmCRProperties
    LSA_UINT16          timeout100ms;                   // RTATimeoutFactor 
    LSA_UINT16          retries;                        // RTARetries
    LSA_UINT16          alcr_tag_header_high;           // VLAN
    LSA_UINT16          alcr_tag_header_low;            // VLAN
} PND_XML_ALARM_CR_DATA_TYPE, *PND_XML_ALARM_CR_DATA_PTR_TYPE;

struct pnd_xml_station_name_alias_tag // XML parameters - Station Name Alias
{
    LSA_UINT8           name_length;                    // AliasNameLength
    LSA_UINT8*          alias_name;                     // AliasName
};
typedef struct pnd_xml_station_name_alias_tag *PND_XML_STATION_NAME_ALIAS_PTR_TYPE;

typedef struct pnd_xml_ar_data_tag // XML parameters - AR Data
{
    LSA_UINT16          ar_nr;                          // local identification of this AR; 1..255
    LSA_UINT16          ar_type;                        // ARType
    CLRPC_UUID_TYPE     ar_uuid;                        // AR_UUID
    LSA_UINT32          ar_properties;                  // ARProperties
    LSA_UINT16          cmi_timeout100ms;               // CMIActivityTimeout
} PND_XML_AR_DATA_TYPE, *PND_XML_AR_DATA_PTR_TYPE;

typedef struct pnd_xml_iocr_io_list_tag // XML parameters - IOCS/IOData
{
    LSA_UINT16      slot_nr;                            // SlotNumber
    LSA_UINT16      subslot_nr;                         // Subslotnumber
    LSA_UINT16      frame_offset;                       // FrameOffset
} PND_XML_IOCR_IO_LIST_TYPE, *PND_XML_IOCR_IO_LIST_PTR_TYPE;

typedef vector<PND_XML_IOCR_IO_LIST_PTR_TYPE> IODataObjectVector;

struct pnd_xml_api_list_tag // XML parameters - API list
{
    LSA_UINT32          api_nr;                         // API Number
    IODataObjectVector  iodata_list;
    IODataObjectVector  iocs_list;
};
typedef struct pnd_xml_api_list_tag *PND_XML_API_LIST_PTR_TYPE;

typedef vector<PND_XML_API_LIST_PTR_TYPE> APIVector;

struct pnd_xml_iocr_data_tag // XML parameters - IOCR Data
{
    LSA_UINT16      iocr_type;                          // IOCRType
    LSA_UINT16      iocr_ref;                           // IOCRReference
    LSA_UINT32      iocr_properties;                    // IOCRProperties
    LSA_UINT16      data_length;                        // DataLength
    LSA_UINT16      frame_id;                           // FrameID
    LSA_UINT16      send_clock;                         // SendClockFactor
    LSA_UINT16      reduction_ratio;                    // ReductionRatio
    LSA_UINT16      phase;                              // Phase
    LSA_UINT16      sequence;                           // Sequence
    LSA_UINT32      frame_send_offset;                  // FrameSendOffset
    LSA_UINT16      watchdog_factor;                    // WatchdogFactor
    LSA_UINT16      data_hold_factor;                   // DataHoldFactor
    APIVector       api_list;                           // API entries
};
typedef struct pnd_xml_iocr_data_tag *PND_XML_IOCR_DATA_PTR_TYPE;

typedef vector<PND_XML_IOCR_DATA_PTR_TYPE> IOCRVector;
typedef vector<PND_XML_RECORD_PTR_TYPE> RecordVector;
typedef vector<PND_XML_STATION_NAME_ALIAS_PTR_TYPE> StationNameAliasVector;

struct pnd_xml_local_scf_adaption_tag // XML parameters - Local SCF Adaption
{
    LSA_UINT16      iocr_ref;                           // IOCRReference
    LSA_UINT16      send_clock_factor;                  // LocalSendClockFactor
    LSA_UINT16      reduction_ratio;                    // LocalReductionRatio
    LSA_UINT16      phase;                              // LocalPhase
    LSA_UINT32      frame_send_offset;                  // LocalFrameSendOffset
    LSA_UINT16      data_hold_factor;                   // LocalDataHoldFactor
};
typedef struct pnd_xml_local_scf_adaption_tag *PND_XML_LOCAL_SCF_ADAPTION_PTR_TYPE;

typedef vector<PND_XML_LOCAL_SCF_ADAPTION_PTR_TYPE> LocalSCFAdaptionVector;

typedef struct pnd_xml_dfp_iocr_tag // XML parameters - IR Info
{
    LSA_UINT16      iocr_ref;                           // IOCRReference
    LSA_UINT16      subframe_offset;                    // SubframeOffset
    LSA_UINT32      subframe_data;                      // SubframeData
} PND_XML_DFP_IOCR_TYPE, *PND_XML_DFP_IOCR_PTR_TYPE;

typedef vector<PND_XML_DFP_IOCR_PTR_TYPE> DFPIOCRVector;

typedef struct pnd_xml_ir_info_data_tag // XML parameters - IOC
{
    CLRPC_UUID_PTR_TYPE ir_data_uuid;                   // IRDataUUID                  
    DFPIOCRVector       dfp_iocr_list;                  // DFP IOCR Entries
} PND_XML_IRINFO_DATA_TYPE;

struct pnd_xml_arcb_record_tag // XML parameters - IOC
{
    PND_XML_RECORD_TYPE record;
    LSA_UINT16          transfer_sequence;              // Data Records Transfer Sequence
};
typedef struct pnd_xml_arcb_record_tag *PND_XML_ARCB_RECORD_PTR_TYPE;

typedef vector<PND_XML_ARCB_RECORD_PTR_TYPE> ArcbRecordVector;

struct pnd_xml_arcb_submodule_descr_tag {
    LSA_UINT16      data_description;                   // Submodule Data Description
    LSA_UINT16      data_length;                        // SubmoduleDataLength
    LSA_UINT16      iops_length;                        // Length IOPS
    LSA_UINT16      iocs_length;                        // Length IOCS
    LSA_UINT32      i_base_addr;                        // Ibase
    LSA_UINT32      q_base_addr;                        // Qbase
};
typedef struct pnd_xml_arcb_submodule_descr_tag *PND_XML_ARCB_SUBMODULE_DESCR_PTR_TYPE;

typedef vector<PND_XML_ARCB_SUBMODULE_DESCR_PTR_TYPE> ArcbSubmoduleDescrVector;

struct pnd_xml_arcb_submodule_tag {
    LSA_UINT16                  subslot_nr;                 // SubslotNr
    LSA_UINT16                  sub_properties;             // SubmoduleProperties
    LSA_UINT32                  sub_ident;                  // SubmoduleIdentNumber
    LSA_UINT16                  laddr;                      // Submodule LADDR
    LSA_BOOL                    isIsoSubmodule;             // Submodule Isochronous Info
    ArcbSubmoduleDescrVector    arcb_submodule_descr_list;  // ARCB submodule descriptions
    ArcbRecordVector            arcb_record_list;           // ARCB Records
};
typedef struct pnd_xml_arcb_submodule_tag *PND_XML_ARCB_SUBMODULE_PTR_TYPE;

typedef vector<PND_XML_ARCB_SUBMODULE_PTR_TYPE> ArcbSubmoduleVector;

struct pnd_xml_arcb_slot_tag {
    LSA_UINT16                  slot_nr;                    // SlotNumber
    LSA_UINT16                  module_properties;          // ModuleProperties
    LSA_UINT32                  module_ident;               // ModuleIdentNumber
    LSA_UINT16                  laddr;                      // Module LADDR
    ArcbSubmoduleVector         arcb_submodule_list;        // ARCB submodules
};
typedef struct pnd_xml_arcb_slot_tag *PND_XML_ARCB_SLOT_PTR_TYPE;

typedef vector<PND_XML_ARCB_SLOT_PTR_TYPE> ArcbSlotVector;

struct pnd_xml_arcb_api_tag {
    LSA_UINT32              api_nr;                         // API Number
    ArcbSlotVector          arcb_slot_list;                 // ARCB slots
};
typedef struct pnd_xml_arcb_api_tag *PND_XML_ARCB_API_PTR_TYPE;

typedef vector<PND_XML_ARCB_API_PTR_TYPE> ArcbAPIVector;

typedef struct pnd_xml_submodule_properties_tag // XML parameters - Device COC Properties
{
    LSA_UINT16          submodule_properties_data_len;                 // PropertiesLength
    LSA_UINT8*          submodule_properties_data;                     // Properties
} PND_XML_SUBMODULE_PROPERTIES_TYPE, *PND_XML_SUBMODULE_PROPERTIES_PTR_TYPE;

struct pnd_xml_iod_tag {
    LSA_UINT32                      ip_address;                     // IPAddress
    LSA_UINT32                      subnet_mask;                    // Subnet Mask
    LSA_UINT32                      gateway_ip;                     // IPAddress
    LSA_UINT8*                      name_of_station;                // NameOfStation
    LSA_UINT16                      name_of_station_length;         // NameOfStationLength
    LSA_UINT16                      ip_address_validation_remote;   // IPAddressValidationRemote
    LSA_UINT16                      ip_address_validation_prop;     // IPAddressValidationProperties
    LSA_UINT8                       is_active;                      // DeactivatedConfig
    PND_XML_IRINFO_DATA_TYPE        ir_info_data;                   // IRInfoData
    PND_XML_PNIOD_PROPERTIES_TYPE   pniod_properties;               // Device Properties
    PND_XML_ALARM_CR_DATA_TYPE      alarm_cr_data;                  // AlarmCRData
    RecordVector                    ar_record_list;                 // AR Record Data Entries
    StationNameAliasVector          station_name_alias_list;        // AliasName Entries
    PND_XML_AR_DATA_TYPE            ar_data;                        // AR Communication Data
    IOCRVector                      iocr_data;                      // IO CR Data
    LocalSCFAdaptionVector          local_scf_list;                 // Local SCF adaptation entries
    ArcbAPIVector                   exp_submod_data;                // Expected Submodule Data
    PND_XML_SUBMODULE_PROPERTIES_TYPE  submodule_properties;        // COC data
};
typedef struct pnd_xml_iod_tag *PND_XML_IOD_PTR_TYPE;

typedef vector<PND_XML_IOD_PTR_TYPE> IODVector;

typedef struct pnd_xml_interface_tag {
    PND_XML_PD_TYPE   pd;                                           // centralIOSystem
    PND_XML_IOC_TYPE  ioc;                                          // decentralIOSystem (controller properties)
    PND_XML_OHA_TYPE  oha;                                          //
    PND_XML_ISO_TYPE  iso;                                          //
    IODVector         iod_list;                                     // decentralIOSystem (IODevices)
} PND_XML_INTERFACE_TYPE, *PND_XML_INTERFACE_PTR_TYPE;

typedef struct pnd_rema_xml_interface_tag {
    PND_REMA_XML_PD_TYPE   base;                                    // basic rema
	PND_XML_RECORD_TYPE    mib2;
} PND_REMA_XML_INTERFACE_TYPE;
////////////////////////////////////////////////////////////////////////////////////
// CXmlParamStore  class to implement the param handling from XML-Files
////////////////////////////////////////////////////////////////////////////////////

class CXmlParamStore: public CParamStoreImplIF
{
    CPnioXMLStore *m_pPnioStore[PSI_CFG_MAX_HD_CNT+1];

    // Construction
public:
    CXmlParamStore();
    virtual ~CXmlParamStore();

    // IF methodes to implement delegation ParamStore to special class
  	LSA_UINT32 Create( LSA_UINT32 nrOfEntries, LSA_VOID* pParamsData, LSA_UINT32 length, LSA_VOID* pRemaData, LSA_UINT32 remaLength, PND_XML_REMA_DATA_TYPE* remaData );
	LSA_VOID InitializeRemaData(PND_XML_REMA_DATA_TYPE remaData);
	
	LSA_VOID UpdateRema(PND_XML_REMA_DATA_TYPE remaData, PNIO_UINT8** remaFileData, LSA_UINT32* dataLength);
    LSA_UINT32 UpdatePnioIF( LSA_UINT16 hdNr, LSA_UINT16 ifNr, PSI_HD_IF_OUTPUT_PTR_TYPE pOutput );

    LSA_UINT32 GetParamPnioIF( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_PNIO_IF_PTR_TYPE p_params );
    LSA_UINT32 GetParamOHA( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_OHA_HD_IF_PTR_TYPE p_params );
    LSA_UINT32 GetParamPD(  PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_PD_HD_IF_PTR_TYPE  p_params );
    LSA_UINT32 GetParamIOC( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_IOC_HD_IF_PTR_TYPE p_params );
    LSA_UINT32 GetParamISO( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_ISO_HD_IF_PTR_TYPE p_params );

    LSA_UINT32 CreatePrmRecordsPD( PND_HANDLE_PTR_TYPE cpHandle, PrmRecordVector* pRecList );
    LSA_UINT32 CreateDeviceParam( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, PND_PARAM_DEVICE_PTR_TYPE pDev, SubslotVector* pSubList );

    LSA_UINT32 CreateArcbAddCL( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, LSA_UINT32 hwType, CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pAdd );
};

/////////////////////////////////////////////////////////////////////////////
// CPnioIniStore: PnDriver PNIO XML storage class
/////////////////////////////////////////////////////////////////////////////

class CPnioXMLStore
{
private:
    LSA_UINT16  m_hdNr;             // HD Number
    LSA_UINT16  m_nrOfIF;           // Nr of IF for one HD
    xml_doc     m_XMLDoc;           // handle of the XML document
    xml_element m_RootXMLElement;   // root node of XML document

    // internal storage of XML parameters
    PND_XML_INTERFACE_TYPE m_InternalXMLData[PSI_CFG_MAX_HD_IF_CNT+1];
	PND_REMA_XML_INTERFACE_TYPE m_InternalRemaData;
	PND_REMA_XML_INTERFACE_TYPE m_InternalRemaDataArr[PSI_CFG_MAX_HD_IF_CNT+1];

// Construction
public:
    CPnioXMLStore();
    virtual     ~CPnioXMLStore();
 	LSA_UINT32  Create( LSA_UINT16 hdNr, LSA_VOID* pParamsData, LSA_VOID* pRemaData, PND_XML_REMA_DATA_TYPE* remaData  );
	LSA_VOID SetRemaData(PND_XML_REMA_DATA_TYPE remaData);
	LSA_VOID UpdateRema(PND_XML_REMA_DATA_TYPE remaData, PNIO_UINT8** remaFileData, PNIO_UINT32* dataLength);
    // Update PNIO IF params calculated by PSI
    LSA_VOID PrepareStore( LSA_UINT16 ifNr, PSI_HD_IF_OUTPUT_PTR_TYPE pOutput );

    // Accessors for PNIO IF and User setup
    LSA_BOOL IsPdevConfigCentral( LSA_UINT16 ifNr ) const;
    LSA_VOID GetPrmRecordsPD( LSA_UINT16 ifNr, PrmRecordVector* pRecList );

    PND_XML_INTERFACE_PTR_TYPE  GetXMLInput( LSA_UINT16 ifNr );
    PND_PARAM_PD_HD_IF_TYPE     GetXMLInputPD( LSA_UINT16 ifNr ) const;
    PND_PARAM_IOC_HD_IF_TYPE    GetXMLInputIOC( LSA_UINT16 ifNr ) const;
    PND_PARAM_OHA_HD_IF_TYPE    GetXMLInputOHA( LSA_UINT16 ifNr ) const;
    PND_PARAM_ISO_HD_IF_TYPE    GetXMLInputISO( LSA_UINT16 ifNr ) const;

    // Accessors for PNIO ARCB params
    LSA_VOID GetArcbAddCL( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, LSA_UINT32 hwType, CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pAdd );
    LSA_UINT32 GetDeviceParam( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, PND_PARAM_DEVICE_PTR_TYPE pDev, SubslotVector* pSubList  );

private:
    LSA_UINT16  getNrOfHDs();
    LSA_UINT16  getNrOfIF() const;
    xml_doc     createXMLDoc(LSA_CHAR* pXMLBuffer) const;
    LSA_VOID    freeXMLDoc(xml_doc XMLDoc) const;
    xml_element getRootXMLElement(xml_doc XMLDoc) const;
    LSA_VOID    initializeXMLParser() const;
	LSA_UINT16  readPNIOParametersFromXMLFile(LSA_UINT16 ifNr, LSA_CHAR* pXMLBuffer);
	LSA_UINT16  readRemaParametersFromXMLFile(LSA_CHAR* pRemaBuffer, PND_XML_REMA_DATA_TYPE* remaData);
	LSA_UINT16  readParametersFromXMLFiles(LSA_UINT16 ifNr, LSA_CHAR* pXMLBuffer, LSA_CHAR* pRemaBuffer, PND_XML_REMA_DATA_TYPE* remaData);

    LSA_UINT16  getPDNetworkParams(LSA_UINT16 ifNr);
	LSA_UINT16  getRemaNetworkParams(PND_XML_REMA_DATA_TYPE* remaData);
    LSA_UINT16  getPDParams(LSA_UINT16 ifNr);
    LSA_UINT16  getPDDataRecords(LSA_UINT16 ifNr);
    LSA_UINT16  getPDSubslotInfo(LSA_UINT16 ifNr);
    LSA_UINT16  getIOCParams(LSA_UINT16 ifNr);
    LSA_UINT16  getOHAParams(LSA_UINT16 ifNr);
    LSA_UINT16  getISOParams(LSA_UINT16 ifNr);
    	
    LSA_UINT16  getDeviceNetworkParams(LSA_UINT16 devNr, LSA_UINT32* ip_address, LSA_UINT32* subnet_mask, LSA_UINT32* gateway_ip, LSA_UINT8** name_of_station,
                                        LSA_UINT16* name_of_station_length, LSA_UINT16* ip_address_validation_remote, 
                                        LSA_UINT16* ip_address_validation_prop, StationNameAliasVector* station_name_alias_list);
    LSA_UINT16  getDeactivatedConfig(LSA_UINT16 devNr, LSA_UINT8* is_active);
    LSA_UINT16  getIODProperties(LSA_UINT16 devNr, PND_XML_PNIOD_PROPERTIES_PTR_TYPE pniod_properties);
    LSA_UINT16  getAlarmCRData(LSA_UINT16 devNr, PND_XML_ALARM_CR_DATA_PTR_TYPE alarm_cr_data);
    LSA_UINT16  getARCommunicationData(LSA_UINT16 devNr, PND_XML_AR_DATA_PTR_TYPE alarm_cr_data);
    LSA_UINT16  getIOCRData(LSA_UINT16 devNr, IOCRVector* iocr_data);
    LSA_UINT16  getExpectedSubmoduleData(LSA_UINT16 devNr, ArcbAPIVector* exp_submod_data);
    LSA_UINT16  getOtherSubmoduleData(LSA_UINT16 devNr, ArcbAPIVector* exp_submod_data);
    LSA_UINT16  getLocalSCFAdaption(LSA_UINT16 devNr, LocalSCFAdaptionVector* local_scf_adaption);
    LSA_UINT16  getARRecordData(LSA_UINT16 devNr, RecordVector* ar_record_list);
    LSA_UINT16  getIRInfoData(LSA_UINT16 devNr, PND_XML_IRINFO_DATA_TYPE* ir_info_data);
    LSA_UINT16  getSubmodulePropertiesData(LSA_UINT16 devNr, PND_XML_SUBMODULE_PROPERTIES_TYPE *submodule_properties);

    LSA_VOID    prepareFromOutputIOC( LSA_UINT16 ifNr, const PSI_HD_IF_OUTPUT_PTR_TYPE pOutput );
    LSA_VOID    prepareFromOutputOHA( LSA_UINT16 ifNr, const PSI_HD_IF_OUTPUT_PTR_TYPE pOutput );

    LSA_VOID*   createArcbExpectedConfig( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, LSA_VOID* psiSysPtr );
    LSA_UINT16  getDatalenIOCR( LSA_UINT16 iocrDatalen, LSA_UINT32 iocrProp ) const;

    // Helper Methods
    LSA_UINT16  getRecord(xml_element element, PND_XML_RECORD_PTR_TYPE record) const;
    LSA_UINT32  getValueFromRecord(const LSA_UINT8* record_data, LSA_UINT16 start, LSA_UINT16 length) const;
    LSA_UINT8   getUINT8FromRecord(LSA_UINT8* record_data, LSA_UINT16 start) const;
    LSA_UINT16  getUINT16FromRecord(LSA_UINT8* record_data, LSA_UINT16 start) const;
    LSA_UINT32  getUINT32FromRecord(LSA_UINT8* record_data, LSA_UINT16 start) const;
    xml_element getDecentralIOSystem();
    LSA_UINT16  getDeviceCount();
    LSA_UINT16  getPortCount();
    xml_element getDevice(LSA_UINT16 DevNr);
    LSA_VOID    getUUID(LSA_UINT8* record_data, LSA_UINT16 start_byte, CLRPC_UUID_PTR_TYPE uuid);
};


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_XML_PARAM_STORE_H */

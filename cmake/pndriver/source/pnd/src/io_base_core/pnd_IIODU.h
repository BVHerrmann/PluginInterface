#ifndef PND_IIODU_H
#define PND_IIODU_H

#include "pnd_int.h"

/*
* Access types
*/
enum PND_ACC_T
{
    PND_ACC_T_READ,
    PND_ACC_T_WRITE,
};

typedef struct
{
    PNIO_VOID_PTR_TYPE     eddi_buffer_handle;     //SOC:  virt. adr. of PAEARAM buffer (complete CR)
    PNIO_UINT16            edds_buffer_handle;
    PNIO_UINT8*            p_shadow_buffer;        //virt. adr. of shadow buffers for this CR
    PNIO_UINT32            shadow_buffer_size;     //size of CR
    PNIO_BOOL              is_provider;
} PND_IODU_BUFFER_TYPE;

typedef PND_IODU_BUFFER_TYPE * PND_IODU_BUFFER_PTR_TYPE;

typedef struct {
    PND_IODU_BUFFER_PTR_TYPE    provider_buffer_handle;
    PND_IODU_BUFFER_PTR_TYPE    consumer_buffer_handle;
    PNIO_UINT32                  IO_base_addr;        //don't care if length_IO_data 0
    PNIO_UINT32                  offset_IO_data;      //offset of IO data in frame buffer
    PNIO_UINT32                  length_IO_data;      //0 indicates a dataless submodule (was ist bei unterdrückten IO daten?)
    PNIO_UINT32                  offset_IOCS;         //offset of IOCS data in output frame buffer (controller view)
    PNIO_UINT32                  device_nr;           //device nr this IO address range belongs to
    PNIO_BOOL                    is_online;           //indicates if corresponding device is "INDATA"
    PNIO_BOOL                    isIsoIOData;         //indicates if corresponfing submodule is isochronous

} PND_IO_ADDR_MAPPING_TYPE;
typedef PND_IO_ADDR_MAPPING_TYPE * PND_IO_ADDR_MAPPING_PTR_TYPE;

class IIODU
{
public:
    virtual ~IIODU() {}

    virtual PNIO_VOID PI_Alloc(PND_RQB_PTR_TYPE  rqb_ptr) = 0;
    virtual PNIO_VOID PI_Free(PND_RQB_PTR_TYPE  rqb_ptr) = 0;
    virtual PNIO_UINT32 data_io_rw(const PNIO_ADDR * pAddr, PND_ACC_T accesst, const PNIO_IOXS * pLocStat, PNIO_IOXS * pRemStat, const PNIO_UINT32 * pDataLen, PNIO_UINT8 * pData, PNIO_BOOL updatePi) = 0;
    virtual PNIO_VOID update_IQ_table(PNIO_UINT32 device_nr, PNIO_BOOL is_online) = 0;
    virtual PNIO_BOOL isIsoData(const PNIO_ADDR * pAddr, PND_ACC_T accesst) = 0;
};


#endif
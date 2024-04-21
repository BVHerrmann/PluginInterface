#ifndef PND_IODU_H
#define PND_IODU_H

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
/*  F i l e               &F: pnd_iodu.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Interface for IODU (IO Data Update)                                      */
/*****************************************************************************/

#include "pnd_IPndAdapter.h"
#include "pnd_IIODU.h"

#include <vector>

using namespace std;

#define PND_IODU_IQ_MAX_ADDRESS                         0x8000
#define PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS            128
#define PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS            128

#define PND_IODU_IOXS_STATE_GOOD                        0x80
#define PND_IODU_IOXS_STATE_BAD                         0x00

class CIODU : public IIODU
{
    // Attributes
private:

protected:

    PND_HANDLE_TYPE pnd_handle;
    IPndAdapter* m_pPndAdapter;

	vector<PND_IO_ADDR_MAPPING_PTR_TYPE> I_addr_table;
	vector<PND_IO_ADDR_MAPPING_PTR_TYPE> Q_addr_table;

    PND_IODU_BUFFER_PTR_TYPE    consumer_pi_buffer_array[PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS];
    PND_IODU_BUFFER_PTR_TYPE    provider_pi_buffer_array[PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS];

protected:
    CIODU(const PND_HANDLE_TYPE &pndHandle, IPndAdapter *adapter);
    
    // virtual methodes
    virtual PNIO_UINT32 check_params_IO_addr(PND_RQB_PTR_TYPE rqb_ptr);
    virtual PNIO_UINT32 create_IQ_table(PND_RQB_PTR_TYPE  rqb_ptr);
    virtual PNIO_VOID delete_IQ_table_entries(PND_RQB_PTR_TYPE  rqb_ptr);

    virtual PNIO_VOID update_provider_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODU_buffer_handle) = 0;
    virtual PNIO_VOID update_consumer_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODU_buffer_handle) = 0;

public:
    virtual ~CIODU();

    // pure virtual methodes
    virtual PNIO_VOID PI_Alloc(PND_RQB_PTR_TYPE  rqb_ptr) = 0;
    virtual PNIO_VOID PI_Free(PND_RQB_PTR_TYPE  rqb_ptr) = 0;

    PNIO_UINT32 data_io_rw(const PNIO_ADDR * pAddr, PND_ACC_T accesst, const PNIO_IOXS * pLocStat, PNIO_IOXS * pRemStat, const PNIO_UINT32 * pDataLen, PNIO_UINT8 * pData, PNIO_BOOL updatePi);
    PNIO_VOID update_IQ_table(PNIO_UINT32 device_nr, PNIO_BOOL is_online);
    PNIO_BOOL isIsoData(const PNIO_ADDR * pAddr, PND_ACC_T accesst);
};

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_IODU_H */

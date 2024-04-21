#define LTRC_ACT_MODUL_ID  20090
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>   /* Types / Prototypes / Fucns    */
#include <eps_trc.h>                  /* Tracing                    */
#include <eps_tasks.h>

/* Defines */
#define EPS_GDMA_RET_OK  LSA_RET_OK
#define EPS_GDMA_RET_ERR LSA_RET_ERR_PARAM

#define EPS_GDMA_COUNT_JOBS              (32-1) /* 32 Jobs but last one is buggy */
#define EPS_GDMA_COUNT_TRANSFER_RECORDS  256

#define EPS_GDMA_MAX_INSTANCES 4

/* Helper Macros */
#define EPS_GDMA_WRITE_32(dest, val)              dest = ((LSA_UINT32)(val))
#define EPS_GDMA_READ_32(dest)                    ((LSA_UINT32)(dest))
#define EPS_GDMA_WRITE_MASKED_32(dest, mask, val) EPS_GDMA_WRITE_32(dest, (EPS_GDMA_READ_32(dest) & mask) | val);
#define EPS_GDMA_SET_BIT_32(dest, bit)            EPS_GDMA_WRITE_32(dest, (EPS_GDMA_READ_32(dest) | (((LSA_UINT32)1<<bit))))
#define EPS_GDMA_GET_BIT_32(dest, bit)            (EPS_GDMA_READ_32(dest) & (1<<bit))
#define EPS_GDMA_CLEAR_BIT_32(dest, bit)          EPS_GDMA_WRITE_32(dest, (EPS_GDMA_READ_32(dest) & (~(1<<bit))))   

typedef struct {
	struct
	{
		LSA_UINT32 REG_ADDR;
		LSA_UINT32 LIST_ADDR;
		LSA_UINT32 MAIN_CTRL;
		LSA_UINT32 JC_EN;
	} GDMAControlRegisters;
	struct
	{
		LSA_UINT32 JOB_CTRL[EPS_GDMA_COUNT_JOBS];
		LSA_UINT32 JOB_CTRL_BUGGY; /* Known Bug in GDMA with last Job use with caution */
	} JOBControlRegisters;
	struct
	{
		LSA_UINT32 JOB_STATUS;
		LSA_UINT32 FINISHED_JOBS;
		LSA_UINT32 ACTUAL_STATUS;
		LSA_UINT32 IRQ_STATUS;
		LSA_UINT32 ERR_IRQ_STATUS;
		LSA_UINT32 DMA_JOB_COUNT;
		LSA_UINT32 REVISION_CODE;
	} GDMAStatusRegisters;
} *EPS_GDMA_REG_PTR_TYPE;

typedef struct
{
	struct
	{
		struct
		{
			LSA_UINT32 SourceAddress;
			LSA_UINT32 DestinationAddress;
			LSA_UINT32 TransferControl;
			LSA_UINT32 TransferCount;
		} TransferList[EPS_GDMA_COUNT_TRANSFER_RECORDS];
	} LIST_RAM;
	struct
	{
		struct
		{
			LSA_UINT32 SourceAddressCounter;
			LSA_UINT32 DestinationAddressCounter;
			LSA_UINT32 TransferControlInformation;
			LSA_UINT32 TransferCountInformation;
		} JobStack[EPS_GDMA_COUNT_JOBS+1];
	} JOB_STACK_RAM;
} *EPS_GDMA_RAM_PTR_TYPE;

typedef struct {
	LSA_BOOL bUsed;
	LSA_UINT32 uFirstTransferRecord;
} EPS_GDMA_JOB_MGM_TYPE, *EPS_GDMA_JOB_MGM_PTR_TYPE;

typedef struct {
	LSA_BOOL bUsed;
	LSA_UINT32 uRamVirtBaseSrc;
	LSA_UINT32 uRamPhysBaseSrc;
	LSA_UINT32 uRamVirtBaseDst;
	LSA_UINT32 uRamPhysBaseDst;
	volatile EPS_GDMA_REG_PTR_TYPE pEpsGDMAReg;
	volatile EPS_GDMA_RAM_PTR_TYPE pEpsGDMARam;
	EPS_GDMA_JOB_MGM_TYPE Job[EPS_GDMA_COUNT_JOBS];
} EPS_GDMA_MGM_TYPE, *EPS_GDMA_MGM_PTR_TYPE;

typedef struct {
	EPS_GDMA_MGM_TYPE EpsGDMAMgm[EPS_GDMA_MAX_INSTANCES];
} EPS_GDMA_STORE_TYPE, *EPS_GDMA_STORE_PTR_TYPE;

EPS_GDMA_STORE_TYPE g_EpsGDMAMgmStore;

static LSA_VOID eps_gdma_hw_init(EPS_GDMA_MGM_PTR_TYPE pGdma, LSA_UINT8* pGdmaRegBasePhys, LSA_UINT8* pGdmaRamBasePhys)
{
	LSA_UINT32 uCurrREG_ADDR;
	LSA_UINT32 uListSize = 256;

	/* Only init GDMA HW if it isn't already initialized */	

	uCurrREG_ADDR = EPS_GDMA_READ_32( pGdma->pEpsGDMAReg->GDMAControlRegisters.REG_ADDR );
	if( uCurrREG_ADDR != (LSA_UINT32)pGdmaRegBasePhys )
	{

		EPS_GDMA_CLEAR_BIT_32(pGdma->pEpsGDMAReg->GDMAControlRegisters.MAIN_CTRL, 0); /* GDMA Disable */

		//EPS_GDMA_SET_BIT_32( pGdma->pEpsGDMAReg->GDMAControlRegisters.MAIN_CTRL, 1 );   /* reset On  */
		//eps_tasks_sleep(1);
		//EPS_GDMA_CLEAR_BIT_32( pGdma->pEpsGDMAReg->GDMAControlRegisters.MAIN_CTRL, 1 ); /* reset Off */

		EPS_GDMA_WRITE_32(pGdma->pEpsGDMAReg->GDMAControlRegisters.REG_ADDR,  (LSA_UINT32) pGdmaRegBasePhys); /* Register Base Addr = GDMA AHB Register Offset */
		EPS_GDMA_WRITE_32(pGdma->pEpsGDMAReg->GDMAControlRegisters.LIST_ADDR, (LSA_UINT32) pGdmaRamBasePhys); /* List Base Addr = GDMA AHB Register Offset */

		EPS_GDMA_WRITE_32(pGdma->pEpsGDMAReg->GDMAControlRegisters.MAIN_CTRL, (EPS_GDMA_READ_32(pGdma->pEpsGDMAReg->GDMAControlRegisters.MAIN_CTRL) & 0x00ffffff) | ((uListSize-1)%256)<<24); /* GDMA set list size */
	
		EPS_GDMA_SET_BIT_32(pGdma->pEpsGDMAReg->GDMAControlRegisters.MAIN_CTRL, 0); /* GDMA Enable */
	}
}

LSA_VOID* eps_gdma_init(LSA_UINT8* pGdmaRegBase, LSA_UINT8* pGdmaRegBasePhys, LSA_UINT8* pRamVirtBaseSrc, LSA_UINT8* pRamPhysBaseSrc, LSA_UINT8* pRamVirtBaseDst, LSA_UINT8* pRamPhysBaseDst)
{
	LSA_UINT32 i;
	LSA_UINT8* pGdmaRamBasePhys;
	LSA_UINT8* pGdmaRamBase;
	EPS_GDMA_MGM_PTR_TYPE pGdma = LSA_NULL;

	/* This function allocs an GDMA instance */

	for(i=0; i<EPS_GDMA_MAX_INSTANCES; i++)
	{
		if(g_EpsGDMAMgmStore.EpsGDMAMgm[i].bUsed == LSA_FALSE)
		{
			pGdma = &g_EpsGDMAMgmStore.EpsGDMAMgm[i];
			eps_memset(pGdma, 0, sizeof(*pGdma));
			pGdma->bUsed = LSA_TRUE;
			break;
		}
	}

	EPS_ASSERT(pGdma != LSA_NULL);
	EPS_ASSERT(pGdmaRegBase != LSA_NULL);

	pGdmaRamBase     = pGdmaRegBase     + 0xb0;
	pGdmaRamBasePhys = pGdmaRegBasePhys + 0xb0;
	
	pGdma->pEpsGDMAReg     = (EPS_GDMA_REG_PTR_TYPE)pGdmaRegBase;
	pGdma->pEpsGDMARam     = (EPS_GDMA_RAM_PTR_TYPE)pGdmaRamBase;
	pGdma->uRamVirtBaseSrc = (LSA_UINT32)pRamVirtBaseSrc;
	pGdma->uRamPhysBaseSrc = (LSA_UINT32)pRamPhysBaseSrc;
	pGdma->uRamVirtBaseDst = (LSA_UINT32)pRamVirtBaseDst;
	pGdma->uRamPhysBaseDst = (LSA_UINT32)pRamPhysBaseDst;

	eps_gdma_hw_init(pGdma, pGdmaRegBasePhys, pGdmaRamBasePhys);

	return (LSA_VOID*)pGdma;
}

LSA_VOID eps_gdma_undo_init(LSA_VOID* hGdma)
{
	EPS_GDMA_MGM_PTR_TYPE pGdma = (EPS_GDMA_MGM_PTR_TYPE) hGdma;

	EPS_ASSERT(pGdma != LSA_NULL);
	EPS_ASSERT(pGdma->bUsed == LSA_TRUE);

	pGdma->bUsed = LSA_FALSE;
}

LSA_UINT32 eps_gdma_single_hif_transfer_init(LSA_VOID* hGdma, LSA_UINT16 uHdNr, LSA_BOOL bUpper)
{
	EPS_GDMA_MGM_PTR_TYPE pGdma = (EPS_GDMA_MGM_PTR_TYPE) hGdma;
	LSA_UINT16 uTransferRecordIndex;
	LSA_UINT16 uJobIndex;

	EPS_ASSERT(pGdma != LSA_NULL);
	EPS_ASSERT(pGdma->bUsed == LSA_TRUE);

	/*
		We alloc 2 Transfer Records for each HIF instance => 1 for Quad Byte transfer and 1 for Byte transfer
		Further on GDMA can be used for Upper and Lower Hif instances which are located on different processors.
		This means we need to devide resources for upper / lower HIF.
	*/
	if(bUpper == LSA_TRUE)
	{
		uTransferRecordIndex = (uHdNr*2) * 2;
		uJobIndex = (uHdNr*2);
	}
	else
	{
		uTransferRecordIndex = ((uHdNr*2) * 2) + 2;
		uJobIndex = (uHdNr*2)+1;
	}

	EPS_GDMA_WRITE_32( pGdma->pEpsGDMARam->LIST_RAM.TransferList[uTransferRecordIndex+0].TransferControl, 0 );
	EPS_GDMA_WRITE_32( pGdma->pEpsGDMARam->LIST_RAM.TransferList[uTransferRecordIndex+1].TransferControl, 0 );

	pGdma->Job[uJobIndex].uFirstTransferRecord = uTransferRecordIndex;

	EPS_GDMA_WRITE_MASKED_32(pGdma->pEpsGDMAReg->JOBControlRegisters.JOB_CTRL[uJobIndex], 0x00ffffff, (pGdma->Job[uJobIndex].uFirstTransferRecord%256)<<24); /* Setup Transfer List Index in Job */

	EPS_GDMA_SET_BIT_32(pGdma->pEpsGDMAReg->JOBControlRegisters.JOB_CTRL[uJobIndex],1); /* Enable JOB_EN */	

	pGdma->Job[uJobIndex].bUsed = LSA_TRUE;

	return uJobIndex;
}

LSA_VOID eps_gdma_single_hif_transfer(LSA_VOID* hGdma, LSA_UINT32 hTransfer, LSA_UINT32 uSourceAddr, LSA_UINT32 uDestAddr, LSA_UINT32 uByteCount)
{
	LSA_UINT32 uTListIndex;
	LSA_UINT32 uPhysSourceAddr;
	LSA_UINT32 uPhysDestAddr;
	EPS_GDMA_MGM_PTR_TYPE pGdma = (EPS_GDMA_MGM_PTR_TYPE) hGdma;
	LSA_UINT32 uQuadByteCnt     = uByteCount / 4;
	LSA_UINT32 uQuadByteCntByte = uQuadByteCnt * 4;
	LSA_UINT32 uRestByteCnt     = uByteCount - uQuadByteCntByte;

	EPS_ASSERT(pGdma != LSA_NULL);
	EPS_ASSERT(pGdma->bUsed == LSA_TRUE);
	EPS_ASSERT( uByteCount < ( (0xffff*4) + (0xffff) ) ); /* Max Transfer Size this module can handle */
	
	uTListIndex = pGdma->Job[hTransfer].uFirstTransferRecord;
	uPhysSourceAddr = uSourceAddr - pGdma->uRamVirtBaseSrc + pGdma->uRamPhysBaseSrc;
	uPhysDestAddr = uDestAddr - pGdma->uRamVirtBaseDst + pGdma->uRamPhysBaseDst;

	if(uQuadByteCnt)
	{
		EPS_GDMA_WRITE_32( pGdma->pEpsGDMARam->LIST_RAM.TransferList[uTListIndex].TransferCount, (0x800000 /* Element Size 32Bit */ | ( (uQuadByteCnt&0xffff) - 1 ) /* Num Elements */ ) );	
		EPS_GDMA_WRITE_32( pGdma->pEpsGDMARam->LIST_RAM.TransferList[uTListIndex].SourceAddress, uPhysSourceAddr); 
		EPS_GDMA_WRITE_32( pGdma->pEpsGDMARam->LIST_RAM.TransferList[uTListIndex].DestinationAddress, uPhysDestAddr);
		uTListIndex++;	
	}
	
	if(uRestByteCnt)
	{
		EPS_GDMA_WRITE_32( pGdma->pEpsGDMARam->LIST_RAM.TransferList[uTListIndex].TransferCount, (0x000000 /* Element Size 8Bit */ | ( (uRestByteCnt&0xffff) - 1 ) /* Num Elements */  ) );
		EPS_GDMA_WRITE_32( pGdma->pEpsGDMARam->LIST_RAM.TransferList[uTListIndex].SourceAddress, (uPhysSourceAddr + uQuadByteCntByte) ); 
		EPS_GDMA_WRITE_32( pGdma->pEpsGDMARam->LIST_RAM.TransferList[uTListIndex].DestinationAddress, (uPhysDestAddr + uQuadByteCntByte) );	
		uTListIndex++;	
	}
	
	EPS_GDMA_SET_BIT_32( pGdma->pEpsGDMARam->LIST_RAM.TransferList[uTListIndex-1].TransferCount, 31 ); /* Last Transfer of the Job */		
	
	pGdma->pEpsGDMAReg->JOBControlRegisters.JOB_CTRL[hTransfer] |= 1; /* SW_JOB_START */

    EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT,"eps_gdma_single_hif_transfer wait for job to finish");
	
    /* wait until GDMA has finished */
    while(EPS_GDMA_GET_BIT_32(pGdma->pEpsGDMAReg->GDMAStatusRegisters.FINISHED_JOBS, hTransfer) != (1 << hTransfer))
	{
		/* GDMA running */
	}
    
    EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT,"eps_gdma_single_hif_transfer job finished");

    /* writing a 1 to a 1 resets it */
    EPS_GDMA_SET_BIT_32(pGdma->pEpsGDMAReg->GDMAStatusRegisters.FINISHED_JOBS, (hTransfer)); 

}

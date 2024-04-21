#ifndef EDDI_SER_TREE_H         //reinclude-protection
#define EDDI_SER_TREE_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_ser_tree.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What   Needed for Reduction-Trees                      */
/*                                                                           */
/*****************************************************************************/

#define EDDI_TREE_GET_LAST_OF_SEQ           ((LSA_UINT32)0) //SequenceNr needed to search for last Element in Sequence

#define EDDI_TREE_MAX_BINARY_REDUCTION      512UL           //Maximum of allowed Reductions
//#define EDDI_TREE_MAX_BINARY_REDUCTION    1UL             //Maximum of allowed Reductions

#define EDDI_FCW_TX_EOL_CNT                 4UL             //Maximum number of EOLs in IRT-TX-List per port: EDDI_FCW_TX_EOL_CNT > 1

typedef enum _EDDI_TREE_RT_CLASS_TYPE
{
    EDDI_TREE_RT_CLASS_FCW_TX,
    EDDI_TREE_RT_CLASS_FCW_RX,
    EDDI_TREE_RT_CLASS_ACW_TX

} EDDI_TREE_RT_CLASS_TYPE;

typedef enum _EDDI_TREE_REDUCTION_MODE_TYPE
{
    EDDI_TREE_REDUCTION_MODE_BINARY
    //EDDI_TREE_REDUCTION_MODE_NO_PROVIDER

} EDDI_TREE_REDUCTION_MODE_TYPE;

// Predefined Reduction Tree Element: Connects two neighbour-Tree-Elements and a lower CW
typedef struct _EDDI_TREE_ELEM_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_TREE_ELEM_PTR_TYPE;

//Reduction Tree Element: Connects two neighbour-Tree-Elements and a lower CW

#define EDDI_TREE_ELEM_OPC_FREE  0xFFFF
#define EDDI_TREE_ELEM_OPC_USED  0x7777
#define EDDI_TREE_ELEM_OPC_EMPTY 0x6666  //a used element, but pCW is NULL

typedef struct _EDDI_TREE_ELEM_TYPE
{
    EDDI_TREE_ELEM_PTR_TYPE       pPrevElem;
    EDDI_TREE_ELEM_PTR_TYPE       pNextElem;
    LSA_UINT32                    Opc;
    EDDI_SER_CCW_PTR_TYPE         pCW;
    LSA_UINT32                    hPoolCCW;  //Remembers Pooltype for free-function
    LSA_UINT32                    HwTxPortMask;
    #if defined (EDDI_CFG_REV5)
    LSA_BOOL                      bCwBranchLinking; //LSA_FALSE: Linking CW with next CW by CW_Link
    //LSA_TRUE:  Linking CW with next CW by CW_Branch
    #endif

    #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
    LSA_UINT32                    SequenceNr;  //SequenceNr in Provider-List! Only used for ACW-TX-Tree! Not used in Head-Elements!
    //0...FFFFFFFE = dedicated sequence-nr; EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED = insert at end of list (EDDI_TREE_GET_LAST_OF_SEQ) or Tail-Element!
    #endif

    union
    {
        //Does not need to be packed, as the union is for saving memspace only.
        struct //for Dynamic Entries
        {
            LSA_UINT32                Time;
            LSA_UINT16                DataLen;

        } TxRx;

        #if defined (EDDI_CFG_REV5)
        struct //for Single-Branch-Entries (BCW_MOD)
        {
            EDDI_TREE_ELEM_PTR_TYPE   pBranchedElem;

        } Branch;
        #endif

    } Sub;

} EDDI_TREE_ELEM_TYPE;

//Contains Infos about the load in a path of the tree.
typedef struct _EDDI_TREE_PATH_LOAD_TYPE
{
    LSA_UINT32  ByteCounter; //Count of Bytes sent in this path
    LSA_UINT32  ProvCounter; //Count of Providers  in this path

} EDDI_TREE_PATH_LOAD_TYPE;

typedef struct _EDDI_TREE_PATH_LOAD_TYPE EDDI_LOCAL_MEM_ATTR *  EDDI_TREE_PATH_LOAD_PTR_TYPE;

typedef struct _EDDI_TREE_TYPE
{
    EDDI_TREE_RT_CLASS_TYPE           RtClassType;
    EDDI_TREE_REDUCTION_MODE_TYPE     ReductionMode;
    LSA_UINT32                        MaxReduction;     //Maximum of allowed Reduction

    EDDI_TREE_ELEM_PTR_TYPE           pHeadElemArray;   //Pointer to the Array of TreeElements
    //marking the Begin of a Treebranch
    EDDI_TREE_ELEM_PTR_TYPE           pTailElemArray;   //Pointer to the Array of TreeElements
    //marking the End of a Treebranch
    EDDI_TREE_ELEM_PTR_TYPE           pTxRxElemArray;   //Pointer to the Array of TreeElements
    //marking Send Receive Entries Elements

    LSA_UINT32                        TxRxElemCountMax; //Maximum of Elements in pTxRxElemArray
    LSA_UINT32                        TxRxElemCountUsed;//Count of used Elements in pTxRxElemArray

    EDDI_TREE_ELEM_PTR_TYPE           pRootElem;        //Root Element
    EDDI_SER_CCW_PTR_TYPE             pRootCw;          //local Pointer to the RootCw

    EDDI_LOCAL_EOL_PTR_TYPE           pEOLn[EDDI_FCW_TX_EOL_CNT]; //reference the EOLn (not allocated by Tree).
    //more than 1 EOL are only used at EDDI_TREE_RT_CLASS_FCW_TX!

    EDDI_LOCAL_EOL_PTR_TYPE           pEOLMax;          //reference the EOL with the maximum StartTime.
    //this parameter is only used at EDDI_TREE_RT_CLASS_FCW_TX!

    EDDI_TREE_PATH_LOAD_PTR_TYPE      pPathLoadArray;   //Array containing info about sum of the load (in Bytes)
    //produced in each path.
    //Number of entries = MaxReduction.

    LSA_UINT32                        KramSize;

    //+++++++++++++++++++++++++++
    // BCW-MOD-Specific Part:
    //+++++++++++++++++++++++++++
    EDDI_TREE_BCW_MOD_SPECIFIC_TYPE   BcwMod;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    //+++++++++++++++++++++++++++
    // RCW-Specific Part:
    //+++++++++++++++++++++++++++
    EDDI_TREE_RCW_SPECIFIC_TYPE       Rcw;
    #endif

} EDDI_TREE_TYPE;

typedef             struct _EDDI_TREE_TYPE EDD_UPPER_MEM_ATTR *       EDDI_TREE_PTR_TYPE;
typedef     const   struct _EDDI_TREE_TYPE EDD_UPPER_MEM_ATTR * EDDI_CONST_TREE_PTR_TYPE;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Internal Functions
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

EDDI_TREE_ELEM_PTR_TYPE EDDI_LOCAL_FCT_ATTR EDDI_RedTreePopTxRxElem( EDDI_TREE_PTR_TYPE      const pTree );

LSA_VOID                EDDI_LOCAL_FCT_ATTR EDDI_RedTreePushTxRxElem( EDDI_TREE_PTR_TYPE      const pTree,
                                                                      EDDI_TREE_ELEM_PTR_TYPE const pCurrentElem );

LSA_VOID                EDDI_LOCAL_FCT_ATTR EDDI_RedTreeUnChainElem( EDDI_TREE_ELEM_PTR_TYPE const pActElem );

EDDI_TREE_ELEM_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeGetPrevElem( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB,
                                                                       EDDI_TREE_PTR_TYPE       const pTree,
                                                                       LSA_UINT16               const CycleReductionRatio,
                                                                       LSA_UINT16               const CyclePhase,
                                                                       LSA_UINT32               const CyclePhaseSequence );

LSA_VOID                EDDI_LOCAL_FCT_ATTR EDDI_RedTreeUpdatePathLoad( EDDI_TREE_PTR_TYPE      const pTree,
                                                                        LSA_UINT16              const CycleReductionRatio,
                                                                        LSA_UINT16              const CyclePhase,
                                                                        LSA_INT32               const DeltaLoad );

LSA_VOID                EDDI_LOCAL_FCT_ATTR EDDI_RedTreeChainElem( EDDI_TREE_ELEM_PTR_TYPE const pPrevElem,
                                                                   EDDI_TREE_ELEM_PTR_TYPE const pNewElem );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_TREE_H


/*****************************************************************************/
/*  end of file eddi_ser_tree.h                                              */
/*****************************************************************************/

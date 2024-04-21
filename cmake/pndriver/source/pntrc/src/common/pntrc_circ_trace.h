#ifndef PNTRC_CIRC_BUFF_H
#define PNTRC_CIRC_BUFF_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

    static const LSA_UINT32 RR_HEAD_REVISION = 0x1000;
    static const LSA_UINT32 TRACE_AREA_SIZE = 1024 * 1024;

    static const LSA_UINT8 RR_MAGIC_TAIL_CHAR = 0x5F; //  '_'
    static const LSA_UINT8 RR_MAGIC_BUMP_CHAR = 0x2B; //  '+'
    static const LSA_UINT8 RR_MAGIC_DATA_CHAR = 0x2E; //  '.' only for testing

    enum {
        RR_HEAD_MAGIC_NO = 0x55AA55AA, /// marker for rr_header start

        RR_DATA_MAGIC_TAIL = 0x5F5F5F5F, /// marker for empty data area before end of file         ==> rollover
        RR_DATA_MAGIC_BUMP = 0x2B2B2B2B  /// marker for empty data area between newest and oldest  ==> end of data
    };



    typedef enum {
        rrStateInitial = 1,            /// linear mode 
        rrStateRollOver = 2             /// rollover mode
    } RR_STATE;

    typedef struct
    {
        LSA_UINT32 magicNumber;            /// Header Areas magic number
        LSA_UINT32 headerSize;             /// size of this header 
        LSA_UINT32 headerRevision;         /// revision of this header
        LSA_UINT32 dataAreaStart;          /// Data Storage (static)
        LSA_UINT32 dataAreaEnd;            /// absolut end position for this file

        LSA_UINT32 newestDataPos;           /// Data area of newest record element
        LSA_UINT32 newestDataLen;           /// helper 
        LSA_UINT32 oldestDataPos;           /// Data Area of oldest record element

        LSA_UINT32 dataCounter;            /// records sets there are stored
        RR_STATE   state;                  /// indicates linear vs. rollover mode

    } rrbuffHeader_t;


    // The official entry point
    LSA_UINT32 rrWriteTrcBuf(const LSA_UINT8* pTrcBuf, LSA_UINT32 uLength, LSA_UINT8* pntrcFp);

    // for discussion
    // LSA_UINT32 rrWriteTrcBuf(const LSA_UINT8* pTrcBuf, LSA_UINT32 uLength, FILE * pntrcFp, LSA_UINT32 traceAreaSize);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PNTRC_CIRC_BUFF_H */
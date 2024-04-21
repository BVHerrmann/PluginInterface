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
/*  C o m p o n e n t     &C: PNTRC (PN Trace)                          :C&  */
/*                                                                           */
/*  F i l e               &F: pntrc_tbb.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*    Write tracebuffers to file using a cirular storage structure           */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 module-buffer_id                          */
/*===========================================================================*/

#define PNTRC_MODULE_ID		4
#define LTRC_ACT_MODUL_ID	4



#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

#include "pntrc_inc.h"
#include "pntrc_trc.h"
#include "pntrc_int.h"
#include "pntrc_circ_trace.h"

#undef  _CLEAN_DATA_FOR_TEST_

static rrbuffHeader_t rrBuffHeader;

/**
* @brief      Update rrBuffHeader from shadow memory to  file
*
* @param      pntrcFp  file pointer
* @param      rrHead   The rr head
*
* @return     true on success
* @return     false on write error
*/
static LSA_BOOL rrUpdateHeader(LSA_UINT8* pntrcFp, const rrbuffHeader_t *rrHead)
{
    //rewind(pntrcFp);
    //size_t bytesWritten = fwrite(rrHead, 1, sizeof(rrbuffHeader_t), pntrcFp);
    PNTRC_MEMCPY_LOCAL_MEM(pntrcFp, rrHead, sizeof(rrbuffHeader_t));
    //if (bytesWritten == 1 * sizeof(rrbuffHeader_t))
    {
        //fflush(pntrcFp);
        return LSA_TRUE;
    }
    //return LSA_FALSE;
}

/**
* @brief      Initialize the rrHeader in memory and write to file
*
* @param      pntrcFp          file pointer
* @param[in]  trcDataAreaSize  The trc data area size
*
* @return     false on error
* @return     true  on success
*/
static LSA_BOOL rrInitHeader(LSA_UINT8* pntrcFp, LSA_UINT32 trcDataAreaSize)
{
    // is this a valid file descriptor
    if (pntrcFp == LSA_NULL)
    {
        return LSA_FALSE;
    }
    /*
    // is this a new file (First Open?)
    fseek(pntrcFp, 0, SEEK_END);
    LSA_UINT32 fileSize = ftell(pntrcFp);
    if (fileSize != 0)
    {
        return LSA_FALSE;
    }
    */

    // Init shadow data struct an write to file
    rrBuffHeader.magicNumber = RR_HEAD_MAGIC_NO;
    rrBuffHeader.headerSize = sizeof(rrbuffHeader_t);
    rrBuffHeader.headerRevision = RR_HEAD_REVISION;
    rrBuffHeader.dataAreaStart = rrBuffHeader.headerSize;
    rrBuffHeader.dataAreaEnd = rrBuffHeader.headerSize + trcDataAreaSize;

    rrBuffHeader.newestDataPos = rrBuffHeader.dataAreaStart;
    rrBuffHeader.newestDataLen = 0;

    rrBuffHeader.oldestDataPos = rrBuffHeader.dataAreaStart;

    rrBuffHeader.dataCounter = 0;
    rrBuffHeader.state = rrStateInitial;

    return rrUpdateHeader(pntrcFp, &rrBuffHeader);
}

/**
* @brief      Fill block with a character used for marking a gap
*
* @param      pntrcFp  file pointer
* @param[in]  fpos     file positon
* @param[in]  uLength  length to fill
* @param[in]  ch       character to fill
*/
static LSA_VOID rrFillBlock(LSA_UINT8 *pntrcFp, LSA_UINT32 fpos, LSA_UINT32 uLength, LSA_CHAR ch)
{
    //fseek(pntrcFp, fpos, SEEK_SET);
    for (LSA_UINT32 i = 0; i < uLength; i++)
    {
        //fwrite(&ch, 1, 1, pntrcFp);
        PNTRC_MEMCPY_LOCAL_MEM((LSA_VOID*)((LSA_UINT32)pntrcFp + fpos + i), &ch, 1);
    }
}


/**
* @brief      Fill an area with a specific pattern for marking the tail
*
* @param      pntrcFp  file pointer
* @param[in]  fpos1    start position in file
* @param[in]  fpos2    end position in file
* @param[in]  ch       character to fill
*
* @return     tail size written
*/
static LSA_UINT32 rrFillTail(LSA_UINT8 *pntrcFp, LSA_UINT32 fpos1, LSA_UINT32 fpos2, LSA_CHAR ch)
{
    //fseek(pntrcFp, fpos1, SEEK_SET);
    for (LSA_UINT32 i = fpos1; i < fpos2; i++)
    {
        //fwrite(&ch, 1, 1, pntrcFp);
        PNTRC_MEMCPY_LOCAL_MEM((LSA_VOID*)((LSA_UINT32)pntrcFp + i), &ch, 1);
    }
    return (fpos2 - fpos1);
}

/**
* @brief      Return the lenght of a given trcBuffer check TRACE_BUF_MAGICNUMBER
*
* @param[in]  pTrcBufHeader  pointer to the given trace elements
*
* @return     calculated length of this data block.
* @return     0 if no TRACE_BUF_MAGICNUMBER was found
*/
static LSA_UINT32 getTrcBufLen(const PNTRC_BUFFER_HEADER_TYPE* pTrcBufHeader)
{
    if (pTrcBufHeader->MagicNumber == TRACE_BUF_MAGICNUMBER)
    {
        return pTrcBufHeader->FirstBlock + pTrcBufHeader->CurBlockCount * sizeof(PNTRC_BUFFER_ENTRY_TYPE);
    }

    return 0;
}

/**
* @brief      Read oldest record, clean gap, handle rollover, update header
*
* @param      pntrcFp  file pointer
* @param      rrHead   pointer to rrbuffHeader_t
*
* @return     length of  oldest record read
*/
static LSA_UINT32  rrReadOldest(LSA_UINT8 *pntrcFp, rrbuffHeader_t *rrHead)
{
    PNTRC_BUFFER_HEADER_TYPE trcHeader;
    LSA_UINT32  thisBlockLen;
    LSA_UINT32  nextBlockLen;
    //LSA_UINT32  bytesRead;

    // how big is this block?
    PNTRC_MEMSET_LOCAL_MEM(&trcHeader, 0, sizeof(PNTRC_BUFFER_HEADER_TYPE));
    //fseek(pntrcFp, rrHead->oldestDataPos, SEEK_SET);
    //bytesRead = fread(&trcHeader, 1, sizeof(PNTRC_BUFFER_HEADER_TYPE), pntrcFp);
    PNTRC_MEMCPY_LOCAL_MEM(&trcHeader, (LSA_VOID*)((LSA_UINT32)pntrcFp + rrHead->oldestDataPos), sizeof(PNTRC_BUFFER_HEADER_TYPE));
    thisBlockLen = getTrcBufLen(&trcHeader);

    // Are there any data in the next block?
    PNTRC_MEMSET_LOCAL_MEM(&trcHeader, 0, sizeof(PNTRC_BUFFER_HEADER_TYPE));
    LSA_UINT32 nxtRdPos = rrHead->oldestDataPos + thisBlockLen;
    //fseek(pntrcFp, nxtRdPos, SEEK_SET);
    //bytesRead = fread(&trcHeader, 1, sizeof(PNTRC_BUFFER_HEADER_TYPE), pntrcFp);
    PNTRC_MEMCPY_LOCAL_MEM(&trcHeader, (LSA_VOID*)((LSA_UINT32)pntrcFp + nxtRdPos), sizeof(PNTRC_BUFFER_HEADER_TYPE));
    nextBlockLen = getTrcBufLen(&trcHeader);


    if (thisBlockLen != 0)
    {
        // clean this block (only if exists (!= 0)), fill with bumper characters
        rrFillBlock(pntrcFp, rrHead->oldestDataPos, thisBlockLen, RR_MAGIC_BUMP_CHAR);
        //fflush(pntrcFp);

        // some housekeeping
        rrBuffHeader.dataCounter--;
    }

    if (nextBlockLen == 0)
    {
        if (thisBlockLen != 0)
        {
            thisBlockLen += rrHead->dataAreaEnd - rrHead->oldestDataPos; //do not forget the tailing bytes
        }
        else
        {
            thisBlockLen = rrHead->dataAreaEnd - rrHead->oldestDataPos; //do not forget the tailing bytes
        }
        rrHead->oldestDataPos = rrHead->dataAreaStart;
    }
    else
    {
        rrHead->oldestDataPos = rrHead->oldestDataPos + thisBlockLen;
    }

    rrUpdateHeader(pntrcFp, rrHead);

    return thisBlockLen;

}

/**
* @brief      Store incoming newest records,  prevent overrunning oldest record
*             and check border release space by calling rrReadOldes(nBytes)
*             Update header!
*
* @param      pntrcFp  file pointer
* @param      rrHead   The rr head
* @param[in]  pTrcBuf  trace record to write
* @param[in]  uLength  trace record length to write
*/
static LSA_VOID rrWriteNewest(LSA_UINT8* pntrcFp, rrbuffHeader_t *rrHead, const LSA_UINT8* pTrcBuf, LSA_UINT32 uLength)
{
    LSA_UINT32 newWrPos = rrHead->newestDataPos + rrHead->newestDataLen;   // the assumed new position for writing
    LSA_UINT32 newWrLen = uLength;

    if (newWrLen > (TRACE_AREA_SIZE))
    {
        //Trace buffer is to big for circle buffer. Fatal stops here.
        PNTRC_FATAL(0);
        
    }
    else if (newWrPos + newWrLen <= rrHead->dataAreaEnd)       // check end positon of new data should fit inside dataArea 
    {                                                     // Is there space between newest an EOF?
        if (rrHead->state == rrStateRollOver)
        {
            // Where is the oldest position in front or behind?
            // Berechne Abstand und Richtung Lesezeiger / Schreibzeiger 
            // Kann auch 0 sein.

            LSA_INT32  diff = (LSA_INT32)(rrHead->oldestDataPos - newWrPos); // signed! 
            if ((diff >= 0) && (newWrLen > (LSA_UINT32) diff))
            {
                // read oldest to give enough space
                LSA_UINT32 spaceToDelete = (LSA_UINT32)((LSA_INT32)newWrLen - diff); // assuming newWrLen > MAX_INT32 is not allowed
                for (LSA_UINT32 bytesRead = 0; bytesRead < spaceToDelete; /**/)
                {
                    LSA_UINT32 bytesFreed = rrReadOldest(pntrcFp, rrHead);
                    if (bytesFreed == 0) {
                        //there was nothing to free in the buffer --> Error
                        PNTRC_FATAL(0);
                    }
                    bytesRead += bytesFreed;
                }
            }
            else
            {
                // Block fits
            }
        }

        // write data to new position
        //fseek(pntrcFp, newWrPos, SEEK_SET);
        //fwrite(pTrcBuf, 1, newWrLen, pntrcFp);
        PNTRC_MEMCPY_LOCAL_MEM((LSA_VOID*)((LSA_UINT32)pntrcFp + newWrPos), pTrcBuf, newWrLen);
        //fflush(pntrcFp);

        // some housekeeping
        rrBuffHeader.dataCounter++;

#if  _CLEAN_DATA_FOR_TEST_
        // and fill data area for test (it's easier reading the file)
        rrFillBlock(pntrcFp, newWrPos + sizeof(PNTRC_BUFFER_HEADER_TYPE), newWrLen - sizeof(PNTRC_BUFFER_HEADER_TYPE), '.');
        fflush(pntrcFp);
#endif

        // store new position to header
        rrHead->newestDataPos = newWrPos;
        rrHead->newestDataLen = newWrLen;
        rrUpdateHeader(pntrcFp, rrHead);

    }
    else
    {
        // the newest record does not fit into data area
        // if newest record does not fit to end of data area 
        // (1) "remove"  oldest records
        // (2) clean tail over this area
        // (3) rewind to do a rollover 
        // (4) "remove" further oldest records til the newest record has enough space


        // (1) "remove" records
        if (rrHead->state == rrStateRollOver)
        {
            LSA_INT32  diff = (LSA_INT32)(rrHead->oldestDataPos - newWrPos);            // signed!
            if ((diff >= 0) && (newWrLen > (LSA_UINT32)diff))
            {
                LSA_UINT32 spaceToDelete = (LSA_UINT32)((LSA_INT32)newWrLen - diff);    // assuming newWrLen > MAX_INT32 is not allowed
                for (LSA_UINT32 bytesRead = 0; bytesRead < spaceToDelete; /**/)
                {
                    LSA_UINT32 bytesFreed = rrReadOldest(pntrcFp, rrHead);
                    if (bytesFreed == 0) {
                        //there was nothing to free in the buffer --> Error
                        PNTRC_FATAL(0);
                    }
                    bytesRead += bytesFreed;
                }
            }
        }


        // (2) Clean tail from potential newPos to EOF
        rrFillTail(pntrcFp, newWrPos, rrHead->dataAreaEnd, RR_MAGIC_TAIL_CHAR);
        //fflush(pntrcFp);

        // (3) Rewind to start of data area
        newWrPos = rrHead->dataAreaStart;
        newWrLen = newWrLen;


        // (4) calculate distance and directions between reader and writer position
        // zero is allowed
        LSA_INT32  diff = (LSA_INT32)(rrHead->oldestDataPos - newWrPos);            // signed!
        if ((diff >= 0) && (newWrLen > (LSA_UINT32) diff))
        {
            LSA_UINT32 spaceToDelete = (LSA_UINT32)((LSA_INT32)newWrLen - diff);    // assuming newWrLen > MAX_INT32 is not allowed
            for (LSA_UINT32 bytesRead = 0; bytesRead < spaceToDelete; /**/)
            {
                LSA_UINT32 bytesFreed = rrReadOldest(pntrcFp, rrHead);
                if (bytesFreed == 0) {
                    //there was nothing to free in the buffer --> Error
                    PNTRC_FATAL(0);
                }
                bytesRead += bytesFreed;
            }
        }


        // rewind to start position and write record
        //fseek(pntrcFp, newWrPos, SEEK_SET);
        //fwrite(pTrcBuf, 1, newWrLen, pntrcFp);
        PNTRC_MEMCPY_LOCAL_MEM((LSA_VOID*)((LSA_UINT32)pntrcFp + newWrPos), pTrcBuf, newWrLen);
        //fflush(pntrcFp);

        // some housekeeping
        rrBuffHeader.dataCounter++;

#if  _CLEAN_DATA_FOR_TEST_
        // and fill data area for test
        rrFillBlock(pntrcFp, newWrPos + sizeof(PNTRC_BUFFER_HEADER_TYPE), newWrLen - sizeof(PNTRC_BUFFER_HEADER_TYPE), '.');
        fflush(pntrcFp);
#endif

        // store new position to header
        rrHead->newestDataPos = rrHead->dataAreaStart;
        rrHead->newestDataLen = newWrLen;
        rrHead->state = rrStateRollOver;

        rrUpdateHeader(pntrcFp, rrHead);

    }

}

/**
* @brief      Write a new traceBuffer to file
*   - check file pointer
*   - init rrHeader in memory and file
*   - handle the  circular buffer strategy
*
* @param[in]  pTrcBuf  The trc buffer
* @param[in]  uLength  The u length
* @param      pntrcFp  The pntrc fp
*
* @return     { description_of_the_return_value }
*/
LSA_UINT32 rrWriteTrcBuf(const LSA_UINT8* pTrcBuf, LSA_UINT32 uLength, LSA_UINT8* pntrcFp)
{
    static LSA_BOOL firstUsage = LSA_TRUE;
    PNTRC_BUFFER_HEADER_TYPE *pTrcHeader;
    LSA_UINT32 length;

    // a onetime initialization
    if (firstUsage == LSA_TRUE)
    {
        if (rrInitHeader(pntrcFp, TRACE_AREA_SIZE) == LSA_FALSE)
        {
            return 0;
        }
        firstUsage = LSA_FALSE;
    }

    // Check assumption: 1:1   => uLenght == getTrcBufLen(pTrcBuf)  
    pTrcHeader = (PNTRC_BUFFER_HEADER_PTR_TYPE)pTrcBuf;
    length = getTrcBufLen(pTrcHeader);
    PNTRC_ASSERT(uLength == length);

    // write to circular buffered file
    rrWriteNewest(pntrcFp, &rrBuffHeader, pTrcBuf, uLength);


    return uLength;
}

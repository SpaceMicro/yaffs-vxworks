// Copyright (c) 2011 - 2018 Space Micro, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject
// to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "yportenv.h"
#include "yaffs_vxif.h"
#include "yaffs_packedtags2.h"
#include "nandflash_api.h"
#include "nand-ecc.h"


int ROWS_PER_CHIP = (FLASH_NUM_BLOCKS * FLASH_PAGES_PER_BLOCK);
#define CHIP(chunk)     (chunk/ROWS_PER_CHIP)
#define ROW(chunk)      (chunk%ROWS_PER_CHIP)
#define TAG_OFFSET 0

static __u8 page_data[FLASH_PAGE_SIZE + FLASH_PAGE_SPARE_SIZE];
#define COMBINE_DATA_AND_TAGS

// If you have hardware to do your error correction, you may not want this
#define NAND_ECC

#ifdef NAND_ECC
#define NAND_ECC_MARKER       0x0ECCC0DE
#define NAND_ECC_MARKER_SIZE  4
static unsigned long nand_marker;
#endif

int yaffs_ecc_corrections = 0;

#define NAND_WRITE(chip, col, row, data, size)   nand_page_program(chip, col, row, data, size)

#define NAND_READ(chip, col, row, data, size)    nand_read(chip, col, row, data, size)

void TickWatchDog( void )
{
    /*
     * TODO:  Kick watchdog here if watchdog is implemented
     */
}

#ifdef COMBINE_DATA_AND_TAGS
int ynand_WriteChunkWithTagsToNAND( yaffs_Device *dev,
                                    int chunkInNAND,
                                    const __u8 *data,
                                    const yaffs_ExtendedTags *tags )
{
    __u8 *page_data_ptr;
    int packet_size = 0;
    int i;
    char error=0;
    yaffs_PackedTags2 pt;
#ifdef NAND_ECC
    unsigned long marker = NAND_ECC_MARKER;
#endif

    T(YAFFS_TRACE_MTD,
      (TSTR("write chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));

    dev->nPageWrites++;

    /*
     * If there is data and tags, combine into one buffer and do one NAND write to help
     * speed up YAFFS writes
     */
    if( data && tags )
    {
        yaffs_PackTags2(&pt,tags);

        page_data_ptr = page_data;
        memcpy( page_data_ptr, data, dev->nDataBytesPerChunk );
        packet_size += dev->nDataBytesPerChunk;

        page_data_ptr += dev->nDataBytesPerChunk;
        memset( page_data_ptr, 0xFF, TAG_OFFSET );
        packet_size += TAG_OFFSET;

        page_data_ptr += TAG_OFFSET;
        memcpy( page_data_ptr, &pt, sizeof(pt) );
        packet_size += sizeof(pt);

#ifdef NAND_ECC
        page_data_ptr += sizeof(pt);
        memset( page_data_ptr, 0xFF, TAG_OFFSET );
        packet_size += TAG_OFFSET;

        page_data_ptr += TAG_OFFSET;
        memcpy( page_data_ptr, (char *)&marker, NAND_ECC_MARKER_SIZE );
        packet_size += NAND_ECC_MARKER_SIZE;

        page_data_ptr += sizeof(marker);
        nand_calculate_ecc( data,
                            page_data_ptr,
                            dev->nDataBytesPerChunk );
        packet_size += TOTAL_NAND_ECC_SIZE(FLASH_PAGE_SIZE);
#endif

        error |= NAND_WRITE( CHIP(chunkInNAND),
                             0,
                             ROW(chunkInNAND),
                             (__u8*)page_data,
                             packet_size );

    }
    else if( data || tags )
    {
        yaffs_PackTags2(&pt,tags);

        if( data)
        {
            page_data_ptr = page_data;
            memcpy( page_data_ptr, data, dev->nDataBytesPerChunk );
            packet_size += dev->nDataBytesPerChunk;

#ifdef NAND_ECC
            page_data_ptr += dev->nDataBytesPerChunk;
            memset( page_data_ptr, 0xFF, TAG_OFFSET + sizeof(pt) + TAG_OFFSET );
            packet_size += TAG_OFFSET + sizeof(pt) + TAG_OFFSET;

            page_data_ptr += TAG_OFFSET + sizeof(pt) + TAG_OFFSET;
            memcpy( page_data_ptr, (char *)&marker, NAND_ECC_MARKER_SIZE );
            packet_size += NAND_ECC_MARKER_SIZE;

            page_data_ptr += sizeof(marker);
            nand_calculate_ecc( data,
                                page_data_ptr,
                                dev->nDataBytesPerChunk );
            packet_size += TOTAL_NAND_ECC_SIZE(FLASH_PAGE_SIZE);
#endif

            error |= NAND_WRITE( CHIP(chunkInNAND),
                                 0,
                                 ROW(chunkInNAND),
                                 (__u8*)page_data,
                                 packet_size );
        }

        if( tags )
        {
            error |= NAND_WRITE( CHIP(chunkInNAND),
                                 dev->nDataBytesPerChunk + TAG_OFFSET,
                                 ROW(chunkInNAND),
                                 (__u8*)&pt,
                                 sizeof(pt));
        }

        if( error )
        {
            T(YAFFS_TRACE_ERROR,
              (TSTR("Write error: chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
            return YAFFS_FAIL;
        }
    }
    else
    {
        T(YAFFS_TRACE_ERROR,
          (TSTR("Invalid args: chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
        return YAFFS_FAIL;
    }

    return YAFFS_OK;
}
#else
int ynand_WriteChunkWithTagsToNAND(yaffs_Device * dev, int chunkInNAND,
                                   const __u8 * data,
                                   const yaffs_ExtendedTags * tags) {
    int i;
    char error=0;
    yaffs_PackedTags2 pt;

    T(YAFFS_TRACE_MTD,
      (TSTR("write chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));

    dev->nPageWrites++;

    if(data || tags){
        yaffs_PackTags2(&pt,tags);
        if(data){
            error |= NAND_WRITE(CHIP(chunkInNAND),0,ROW(chunkInNAND),
                                (__u8*)data,dev->nDataBytesPerChunk);
        }

        if(tags){
            error |= NAND_WRITE(CHIP(chunkInNAND),dev->nDataBytesPerChunk+TAG_OFFSET,ROW(chunkInNAND),
                                (__u8*)&pt,sizeof(pt));
        }

        if(error){
            T(YAFFS_TRACE_ERROR,
              (TSTR("Write error: chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
            return YAFFS_FAIL;
        }
    }else{
        T(YAFFS_TRACE_ERROR,
          (TSTR("Invalid args: chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
        return YAFFS_FAIL;
    }

    return YAFFS_OK;
}
#endif



#ifdef COMBINE_DATA_AND_TAGS
int ynand_ReadChunkWithTagsFromNAND(yaffs_Device * dev, int chunkInNAND,
                                    __u8 * data, yaffs_ExtendedTags * tags) {
    yaffs_PackedTags2 pt;
    char error=0;
    int read_size;

    dev->nPageReads++;

    T(YAFFS_TRACE_MTD,
      (TSTR("read chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));


    /*
     * If there is data and tags, combine into one buffer and do one NAND read to help
     * speed up YAFFS writes
     */
    if( data && tags )
    {

        read_size = dev->nDataBytesPerChunk + TAG_OFFSET + sizeof(pt);

#ifdef NAND_ECC
        read_size += TAG_OFFSET +
            NAND_ECC_MARKER_SIZE +
            TOTAL_NAND_ECC_SIZE(FLASH_PAGE_SIZE);
#endif

        error |= NAND_READ( CHIP(chunkInNAND),
                            0,
                            ROW(chunkInNAND),
                            (__u8*)page_data,
                            read_size );

        memcpy( data, page_data, dev->nDataBytesPerChunk );
        memcpy( &pt, &page_data[dev->nDataBytesPerChunk+TAG_OFFSET], sizeof(pt) );
        yaffs_UnpackTags2(tags, &pt);

#ifdef NAND_ECC
        if( !error )
        {
            unsigned char calc_ecc[TOTAL_NAND_ECC_SIZE(FLASH_PAGE_SIZE)];
            unsigned char *ecc;
            unsigned long marker = 0;
            int status;

            ecc = page_data +
                dev->nDataBytesPerChunk +
                TAG_OFFSET +
                sizeof(pt) +
                TAG_OFFSET;

            memcpy( (char *)&marker, ecc, NAND_ECC_MARKER_SIZE );

            if( marker == NAND_ECC_MARKER )
            {
                ecc += NAND_ECC_MARKER_SIZE;
                nand_calculate_ecc( data, calc_ecc, dev->nDataBytesPerChunk );
                status = nand_correct_data( data,
                                            ecc,
                                            calc_ecc,
                                            dev->nDataBytesPerChunk );
                if( status == -1 )
                {
                    T(YAFFS_TRACE_ERROR,
                      (TSTR("Read ECC error: chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
                    return YAFFS_FAIL;
                }

                if( status == 1 )
                {
                    yaffs_ecc_corrections++;
                }
            }
        }
#endif
    }
    else if( data || tags )
    {
        if( data )
        {
#ifdef NAND_ECC
            read_size = dev->nDataBytesPerChunk +
                TAG_OFFSET +
                sizeof(pt) +
                TAG_OFFSET +
                NAND_ECC_MARKER_SIZE +
                TOTAL_NAND_ECC_SIZE(FLASH_PAGE_SIZE);

            error |= NAND_READ( CHIP(chunkInNAND),
                                0,
                                ROW(chunkInNAND),
                                page_data,
                                read_size );

            memcpy( data, page_data, dev->nDataBytesPerChunk );

            if( !error )
            {
                unsigned char calc_ecc[TOTAL_NAND_ECC_SIZE(FLASH_PAGE_SIZE)];
                unsigned char *ecc;
                unsigned long marker = 0;
                int status;

                ecc = page_data +
                    dev->nDataBytesPerChunk +
                    TAG_OFFSET +
                    sizeof(pt) +
                    TAG_OFFSET;

                memcpy( (char *)&marker, ecc, NAND_ECC_MARKER_SIZE );

                if( marker == NAND_ECC_MARKER )
                {
                    ecc += NAND_ECC_MARKER_SIZE;
                    nand_calculate_ecc( data, calc_ecc, dev->nDataBytesPerChunk );

                    status = nand_correct_data( data,
                                                ecc,
                                                calc_ecc,
                                                dev->nDataBytesPerChunk );
                    if( status == -1 )
                    {
                        T(YAFFS_TRACE_ERROR,
                          (TSTR("Read ECC error: chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
                        return YAFFS_FAIL;
                    }

                    if( status == 1 )
                    {
                        yaffs_ecc_corrections++;
                    }
                }
            }
#else
            error |= NAND_READ( CHIP(chunkInNAND),
                                0,
                                ROW(chunkInNAND),
                                data,
                                dev->nDataBytesPerChunk );
#endif
        }

        if( tags )
        {
            error |= NAND_READ( CHIP(chunkInNAND),
                                dev->nDataBytesPerChunk + TAG_OFFSET,
                                ROW(chunkInNAND),
                                (__u8*)&pt,
                                sizeof(pt) );
            yaffs_UnpackTags2(tags, &pt);
        }

        if( error )
        {
            T(YAFFS_TRACE_ERROR,
              (TSTR("Read error: chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
            return YAFFS_FAIL;
        }
    }
    else
    {
        T(YAFFS_TRACE_ERROR,
          (TSTR("Invalid args: chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
        return YAFFS_FAIL;
    }

    return YAFFS_OK;
}
#else
int ynand_ReadChunkWithTagsFromNAND(yaffs_Device * dev, int chunkInNAND,
                                    __u8 * data, yaffs_ExtendedTags * tags) {
    yaffs_PackedTags2 pt;
    char error=0;

    dev->nPageReads++;

    T(YAFFS_TRACE_MTD,
      (TSTR("read chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
    if(data || tags){
        if(data){
            error |= NAND_READ(CHIP(chunkInNAND),0,ROW(chunkInNAND),
                               data, dev->nDataBytesPerChunk);
        }
        if(tags){
            error |= NAND_READ(CHIP(chunkInNAND),dev->nDataBytesPerChunk+TAG_OFFSET,ROW(chunkInNAND),
                               (__u8*)&pt, sizeof(pt));
            yaffs_UnpackTags2(tags, &pt);
        }

        if(error){
            T(YAFFS_TRACE_ERROR,
              (TSTR("Read error: chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
            return YAFFS_FAIL;
        }
    }else{
        T(YAFFS_TRACE_ERROR,
          (TSTR("Invalid args: chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
        return YAFFS_FAIL;
    }

    return YAFFS_OK;

}
#endif

int ynand_MarkNANDBlockBad(struct yaffs_DeviceStruct *dev, int blockNo) {
    char dummy=0;
    char error=0;

    T(YAFFS_TRACE_MTD,
      (TSTR("ynand_MarkNANDBlockBad %d" TENDSTR), blockNo));

    error |= nand_page_program(CHIP(blockNo*dev->nChunksPerBlock),
                               dev->nDataBytesPerChunk,
                               ROW(blockNo*dev->nChunksPerBlock), &dummy, 1);
    error |= nand_page_program(CHIP(blockNo*dev->nChunksPerBlock),
                               dev->nDataBytesPerChunk,
                               ROW(blockNo*dev->nChunksPerBlock)+1, &dummy, 1);

    if(error) return YAFFS_FAIL;

    return YAFFS_OK;
}

int ynand_QueryNANDBlock(struct yaffs_DeviceStruct *dev, int blockNo,
                         yaffs_BlockState * state, __u32 *sequenceNumber) {
    char bad;
    char dummy[2];
    char allff[]={0xff,0xff};

    T(YAFFS_TRACE_MTD,
      (TSTR("ynand_QueryNANDBlock %d" TENDSTR), blockNo));

    nand_read(CHIP(blockNo*dev->nChunksPerBlock),
              dev->nDataBytesPerChunk,
              ROW(blockNo*dev->nChunksPerBlock), &dummy[0], 1);

    nand_read(CHIP(blockNo*dev->nChunksPerBlock),
              dev->nDataBytesPerChunk,
              ROW(blockNo*dev->nChunksPerBlock)+1, &dummy[1], 1);

    bad=memcmp(dummy,allff,2);
    if(bad){
        T(YAFFS_TRACE_MTD, (TSTR("block is bad, bad flag = %x %x" TENDSTR),dummy[0],dummy[1]));

        *state = YAFFS_BLOCK_STATE_DEAD;
        *sequenceNumber = 0;
    } else {
        yaffs_ExtendedTags t;
        ynand_ReadChunkWithTagsFromNAND(dev,
                                        blockNo *
                                        dev->nChunksPerBlock, NULL,
                                        &t);

        if (t.chunkUsed) {
            *sequenceNumber = t.sequenceNumber;
            *state = YAFFS_BLOCK_STATE_NEEDS_SCANNING;
        } else {
            *sequenceNumber = 0;
            *state = YAFFS_BLOCK_STATE_EMPTY;
        }
    }

    T(YAFFS_TRACE_MTD,
      (TSTR("block query result seq %d state %d" TENDSTR), *sequenceNumber,
       *state));

    if (bad == 0)
        return YAFFS_OK;
    else
        return YAFFS_FAIL;
}

int ynand_EraseBlockInNAND(yaffs_Device * dev, int blockNumber) {
    int ret;
    int page=blockNumber*dev->nChunksPerBlock;
    T(YAFFS_TRACE_MTD,
      (TSTR("erase block %d\n" TENDSTR), blockNumber));

    ret = nand_block_erase(CHIP(page),page);
    return ret==0?YAFFS_OK:YAFFS_FAIL;
}

int ynand_InitialiseNAND(yaffs_Device * dev) {

    return YAFFS_OK;
}

/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * yaffscfg.c  The configuration for yaffs.
 *
 * This file is intended to be modified to your requirements.
 * There is no need to redistribute this file.
 */

#include "yaffscfg.h"
#include <errno.h>
#include "yaffs_guts.h"
#include "yaffs_vxif.h"
#include "yaffs_vxif64.h"
#include "semLib.h"


extern STATUS yaffsLibInit(int ignored);
extern STATUS yaffsDevCreate(
    char *pDevName,     /* device name */
    yaffs_Device *pDev,
    int maxFiles        /* max number of simultaneously open files */
    );
extern void yaffs_initialise(yaffsfs_DeviceConfiguration *cfgList);

unsigned yaffs_traceMask = 0 /*YAFFS_TRACE_OS | YAFFS_TRACE_TRACING*/;

SEM_ID yaffs_mutexSem;

static yaffs_Device nandDev[4];

yaffsfs_DeviceConfiguration yaffsfs_config[] = {
    { "/flash", &nandDev[0]},
    { "/test", &nandDev[1]},
    { "/boot", &nandDev[2] },
    {(void *)0,(void *)0}
};

void yaffsfs_SetError(int err)
{
    /*Do whatever to set error*/
    errno = err;
}

void yaffsfs_Lock(void)
{
    semTake(yaffs_mutexSem, WAIT_FOREVER);
}

void yaffsfs_Unlock(void)
{
    semGive(yaffs_mutexSem);
}

__u32 yaffsfs_CurrentTime(void)
{
    return time(NULL);
}

void yaffsfs_LocalInitialisation(void)
{
    /* Define locking semaphore.*/
    yaffs_mutexSem= semMCreate ( SEM_Q_PRIORITY | SEM_DELETE_SAFE );
}

void yaffsInitialise(yaffsfs_DeviceConfiguration *cfg){
    while(cfg && cfg->prefix && cfg->dev)
    {
        if(yaffsDevCreate((char *)cfg->prefix, cfg->dev, 20) ==ERROR){
            T(YAFFS_TRACE_ERROR, (TSTR("Create %s failed" TENDSTR), cfg->prefix));
        };
        cfg++;
    }
}

int yaffs_StartUp(void)
{
    static int initialized = 0;
    int start_block;
    int end_block;

    if( initialized )
    {
        return 0;
    }

    // Set up devices
    hw_init();    // Initialize your hardware as needed
    yaffsLibInit(0);
    // Stuff to configure YAFFS
    // Stuff to initialise anything special (eg lock semaphore).
    yaffsfs_LocalInitialisation();

    /* OS partition starts at 100 MB location */
    start_block = (100 * 1024 * 1024) /
        (FLASH_PAGE_SIZE * FLASH_PAGES_PER_BLOCK);
    end_block = (FLASH_NUM_BLOCKS - 128) - 1;

    nandDev[0].nDataBytesPerChunk = FLASH_PAGE_SIZE;
    nandDev[0].nChunksPerBlock = FLASH_PAGES_PER_BLOCK;
    nandDev[0].spareBytesPerChunk = FLASH_PAGE_SPARE_SIZE;
    nandDev[0].totalBytesPerChunk = FLASH_PAGE_SIZE;
    nandDev[0].startBlock = start_block;
    nandDev[0].endBlock = end_block;
    nandDev[0].nReservedBlocks = 10;
    nandDev[0].useNANDECC = 0;
    nandDev[0].nShortOpCaches = 0;
    nandDev[0].isYaffs2 = 1;
    nandDev[0].genericDevice = (void *) 0;
    nandDev[0].writeChunkWithTagsToNAND = ynand_WriteChunkWithTagsToNAND;
    nandDev[0].readChunkWithTagsFromNAND = ynand_ReadChunkWithTagsFromNAND;
    nandDev[0].eraseBlockInNAND = ynand_EraseBlockInNAND;
    nandDev[0].initialiseNAND = ynand_InitialiseNAND;
    nandDev[0].markNANDBlockBad = ynand_MarkNANDBlockBad;
    nandDev[0].queryNANDBlock = ynand_QueryNANDBlock;


    /* Test partition at the end of NAND */
    start_block = FLASH_NUM_BLOCKS - 128;
    end_block   = FLASH_NUM_BLOCKS - 1;

    nandDev[1].nDataBytesPerChunk = FLASH_PAGE_SIZE;
    nandDev[1].nChunksPerBlock = FLASH_PAGES_PER_BLOCK;
    nandDev[1].spareBytesPerChunk = FLASH_PAGE_SPARE_SIZE;
    nandDev[1].totalBytesPerChunk = FLASH_PAGE_SIZE;
    nandDev[1].startBlock = start_block;
    nandDev[1].endBlock = end_block;
    nandDev[1].nReservedBlocks = 10;
    nandDev[1].useNANDECC = 0;
    nandDev[1].nShortOpCaches = 0;
    nandDev[1].isYaffs2 = 1;
    nandDev[1].genericDevice = (void *) 0;
    nandDev[1].writeChunkWithTagsToNAND = ynand_WriteChunkWithTagsToNAND;
    nandDev[1].readChunkWithTagsFromNAND = ynand_ReadChunkWithTagsFromNAND;
    nandDev[1].eraseBlockInNAND = ynand_EraseBlockInNAND;
    nandDev[1].initialiseNAND = ynand_InitialiseNAND;
    nandDev[1].markNANDBlockBad = ynand_MarkNANDBlockBad;
    nandDev[1].queryNANDBlock = ynand_QueryNANDBlock;


    /* Boot Partition starts at 1 MB location with a size of 32 MB */
    start_block = (1 * 1024 * 1024) /
        (FLASH_PAGE_SIZE * FLASH_PAGES_PER_BLOCK);
    end_block   = start_block +
        ((32 * 1024 * 1024) /
         (FLASH_PAGE_SIZE * FLASH_PAGES_PER_BLOCK)) - 1;

    nandDev[2].nDataBytesPerChunk = FLASH_PAGE_SIZE;
    nandDev[2].nChunksPerBlock = FLASH_PAGES_PER_BLOCK;
    nandDev[2].spareBytesPerChunk = FLASH_PAGE_SPARE_SIZE;
    nandDev[2].totalBytesPerChunk = FLASH_PAGE_SIZE;
    nandDev[2].startBlock = start_block;
    nandDev[2].endBlock = end_block;
    nandDev[2].nReservedBlocks = 5;
    nandDev[2].useNANDECC = 0;
    nandDev[2].nShortOpCaches = 10;
    nandDev[2].isYaffs2 = 1;
    nandDev[2].genericDevice = (void *) 0;
    nandDev[2].writeChunkWithTagsToNAND = ynand_WriteChunkWithTagsToNAND;
    nandDev[2].readChunkWithTagsFromNAND = ynand_ReadChunkWithTagsFromNAND;
    nandDev[2].eraseBlockInNAND = ynand_EraseBlockInNAND;
    nandDev[2].initialiseNAND = ynand_InitialiseNAND;
    nandDev[2].markNANDBlockBad = ynand_MarkNANDBlockBad;
    nandDev[2].queryNANDBlock = ynand_QueryNANDBlock;

    /* direct interface */
    yaffs_initialise(yaffsfs_config);

    /* native interface */
    yaffsInitialise(yaffsfs_config);

    initialized = 1;

    return 0;
}

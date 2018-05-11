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

#ifndef _YAFFS2LIBP_H_
#define _YAFFS2LIBP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "yaffs2Lib.h"
#include "yaffs_guts.h"
#include "devextras.h"
#include "iosLib.h"
#include "semLib.h"
#include "dirent.h"

#define SLASH           '/'
#define BACK_SLASH      '\\'
#define DOT             '.'
#define SPACE           ' '

#define YAFFS_NFILES_DEFAULT	20
#define YAFFS_DFLT_DIR_MASK 0777
#define YAFFS_DFLT_FILE_MASK 0666

typedef u_long fsize_t;

typedef struct YAFFS_FILE_DESC * YAFFS_FILE_DESC_ID;

/* Volumn descriptor */

struct YAFFS_VOLUME_DESC{
	DEV_HDR	devHdr;	/* i/o system device header */

	/* TODO: does system needs these ? */
	u_int	magic;	/* control magic number */
	BOOL	mounted;	/* volumn mounted */

	SEM_ID devSem;
	SEM_ID shortSem;

	yaffs_Device *pYaffsDev;
	YAFFS_FILE_DESC_ID pFdList;	/* file descriptor lists */
	int maxFiles;
} ;

/* 
 * File descriptor 
 *  copy from yaffsfs_Handle
 *  yaffs_Object acts as DOS_FILE_HDL
 */

struct YAFFS_FILE_DESC
{
	__u8  inUse:1;		/* this file descriptor is in use */
	__u8  readOnly:1;	/* this file descriptor is readonly */
	__u8  append:1;		/* append only	*/
	__u8  exclusive:1;	/* exclusive	*/
	fsize_t position;		/* current position in file	*/
	YAFFS_VOLUME_DESC_ID pVolDesc;	/* yaffs volume pointer */
	yaffs_Object *obj;	/* the object */
};

#ifdef __cplusplus
}
#endif

#endif /* _YAFFS2LIB_H_ */

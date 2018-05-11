This is an interface between VxWorks v6.8 and the YAFFS flash file system.  The portions by Space Micro are released under the following license:

# Copyright (c) 2011 - 2018 Space Micro, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

The files yaffscfg.{ch} and yaffsfs.{ch} are Copyright (C) 2002-2007
Aleph One Ltd., and are released as stated therein.


This directory should be placed in target/thirdparty and incorporated
into your VxWorks build in the usual way.

You'll need to implement the functions in nandflash_api.h.  Unless
your hardware does error correction for you, you'll also need to
implement the functions in nand-ecc.h, being sure to use
sufficiently-powerful ECC for your NAND chips.  Beware, our hardware
does ECC, so this is the most likely thing to be broken.

yaffscfg.c
----------
This file contains the Yaffs configuration for each NAND device.  This
includes the definitions for the NAND devices (page size, oob size,
pages per block, etc) and also partition definitions (start block and
end block).


yaffs_vxif.c
------------
This file contains the functions required by the Yaffs device
configuration for each NAND device.  It defines the functions for
writeChunkWithTagsToNAND, readChunkWithTagsFromNAND, eraseBlockInNAND,
initialiseNAND, markNANDBlockBad and queryNANDBlock.


yaffsfs.c
---------
This file contains the Yaffs Direct Interface (YDI) functions.  It
provides a POSIX-like API but with Yaffs specific names
(e.g. yaffs_open(), yaffs_read(), yaffs_close(), etc.).


yaffs2Lib.c
-----------
This file contains functions similar to the YDI functions, but are
used to register a VxWorks I/O system driver.  This allows VxWorks to
use POSIX functions (e.g. open(), read(), close(), etc.) to access
Yaffs.

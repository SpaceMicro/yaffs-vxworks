# Makefile - makefile for yaffs2
#
# Copyright (c) 2011, 2018 Space Micro, Inc.
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

# This file contains the makefile macro values for the
# yaff2 driver
#
#

TGT_DIR=$(WIND_BASE)/target

DOC_FILES= 

LIB_BASE_NAME=spacemicro

EXTRA_INCLUDE  += -I$(TGT_DIR)/3rdparty/spacemicro/yaffs2Lib       \
                  -I$(TGT_DIR)/3rdparty/contrib/yaffs2

OBJS_COMMON     = yaffs_vxif.o        \
                  yaffs2Lib.o         \
                  yaffscfg.o          \
                  yaffsfs.o

OBJS = $(OBJS_COMMON) $(OBJS_$(CPU))

include $(TGT_DIR)/h/make/rules.library

CFLAGS += -g

CC_OPTIM=$(CC_OPTIM_NORMAL)
include  $(TGT_DIR)/h/make/defs.drivers
    

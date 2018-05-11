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

#ifndef __NAND_ECC_H__
#define __NAND_ECC_H__


#define NAND_ECC_BLOCK_SIZE    512
#define NAND_ECC_SIZE          3

#define TOTAL_NAND_ECC_SIZE(page_size)   ((page_size / NAND_ECC_BLOCK_SIZE) * NAND_ECC_SIZE)

/*
 * Calculate 3 byte ECC code for eccsize byte block
 */
void nand_calculate_ecc( const unsigned char *dat,
                         unsigned char *ecc_code,
                         int nand_page_size );

/*
 * Detect and correct a 1 bit error for eccsize byte block
 */
int nand_correct_data( unsigned char *dat,
                       unsigned char *read_ecc,
                       unsigned char *calc_ecc,
                       int nand_page_size );

#endif /* __NAND_ECC_H__ */

/*****************************************************************************
 * pixel_c64.c: h264 encoder library
 *****************************************************************************
 * Copyright (C) 2003-2008 x264 project
 *
 * Author: Yufei Yuan <yyuan@gmx.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *****************************************************************************/

#include "common.h"

/***********************************************************************
 * SAD routines
 **********************************************************************/
#define PIXEL_SAD_C64(name, lx, ly) \
int name(uint8_t *pix1, int i_stride_pix1,                                  \
         uint8_t *pix2, int i_stride_pix2)                                  \
{                                                                           \
    const uint32_t unit = 0x01010101U;                                      \
    uint32_t pack4;                                                         \
    int i_sum = 0;                                                          \
    int x, y;                                                               \
    for(y = 0; y < ly; y++)                                                 \
    {                                                                       \
        for(x = 0; x < lx; x += 4)                                          \
        {                                                                   \
            pack4 = _subabs4(_mem4_const(&pix1[x]), _mem4_const(&pix2[x])); \
            i_sum += _dotpu4(pack4, unit);                                  \
        }                                                                   \
        pix1 += i_stride_pix1;                                              \
        pix2 += i_stride_pix2;                                              \
    }                                                                       \
    return i_sum;                                                           \
}

PIXEL_SAD_C64(x264_pixel_sad_16x16_c64, 16, 16)
PIXEL_SAD_C64(x264_pixel_sad_8x16_c64,   8, 16)
PIXEL_SAD_C64(x264_pixel_sad_16x8_c64,  16,  8)
PIXEL_SAD_C64(x264_pixel_sad_8x8_c64,    8,  8)
PIXEL_SAD_C64(x264_pixel_sad_8x4_c64,    8,  4)
PIXEL_SAD_C64(x264_pixel_sad_4x8_c64,    4,  8)
PIXEL_SAD_C64(x264_pixel_sad_4x4_c64,    4,  4)


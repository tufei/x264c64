/*****************************************************************************
 * mc_c64.c: h264 encoder library (Motion Compensation)
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
#include "mc.h"

static inline void pixel_avg_wxh( uint8_t *dst, int i_dst, uint8_t *src1, int i_src1, uint8_t *src2, int i_src2, int width, int height )
{
    int x, y;

    if( width == 2 ) 
    {
        for( y = 0; y < height; y++ )
        {
            for( x = 0; x < width; x += 2 )
            {
                _mem2(&dst[x]) = _avgu4(_mem2_const(&src1[x]), _mem2_const(&src2[x]));
            }
            src1 += i_src1;
            src2 += i_src2;
            dst += i_dst;
        }
    }
    else 
    {
        for( y = 0; y < height; y++ )
        {
            for( x = 0; x < width; x += 4 )
            {
                _mem4(&dst[x]) = _avgu4(_mem4_const(&src1[x]), _mem4_const(&src2[x]));
            }
            src1 += i_src1;
            src2 += i_src2;
            dst += i_dst;
        }
    }
}

/* Implicit weighted bipred only:
 * assumes log2_denom = 5, offset = 0, weight1 + weight2 = 64
 * dst[x] = x264_clip_uint8( (src1[x]*i_weight1 + src2[x]*i_weight2 + (1<<5)) >> 6 )
 */
static inline void pixel_avg_weight_wxh( uint8_t *dst, int i_dst, uint8_t *src1, int i_src1, uint8_t *src2, int i_src2, int width, int height, int i_weight1 )
{
    const uint8_t i_weight2 = 64 - (uint8_t)i_weight1;
    int x, y;
    uint32_t a = 32 * (0x00010001U);
    uint32_t mask = 0x03FF03FFU;
    const uint32_t s1 = i_weight1 * (width == 2 ? 0x00000101U : 0x01010101U);
    const uint32_t s2 = i_weight2 * (width == 2 ? 0x00000101U : 0x01010101U);
    uint16_t temp1[4], temp2[4];

    if(width == 2) 
    {

        for(y = 0; y < height; y++, dst += i_dst, src1 += i_src1, src2 += i_src2) 
        {
            *(double *)temp1 = _mpyu4(_mem4_const(&src1[x]), s1);
            *(double *)temp2 = _mpyu4(_mem4_const(&src2[x]), s2);
            *(uint32_t *)temp1 = _add2(*(uint32_t *)temp1, *(uint32_t *)temp2);
            *(uint32_t *)temp1 = _add2(*(uint32_t *)temp1, a);
            *(uint32_t *)temp1 = (*(uint32_t *)temp1 >> 6) & mask;
            _mem2(&dst[x]) = _spacku4(0, *(int *)temp1);
        }
    }
    else
    {
        for(y = 0; y < height; y++, dst += i_dst, src1 += i_src1, src2 += i_src2) 
        {
            for(x = 0; x < width; x += 4) 
            {
                *(double *)temp1 = _mpyu4(_mem4_const(&src1[x]), s1);
                *(double *)temp2 = _mpyu4(_mem4_const(&src2[x]), s2);
                *(uint32_t *)temp1 = _add2(*(uint32_t *)temp1, *(uint32_t *)temp2);
                *(uint32_t *)temp1 = _add2(*(uint32_t *)temp1, a);
                *(uint32_t *)temp1 = (*(uint32_t *)temp1 >> 6) & mask;
                *(uint32_t *)&temp1[2] = _add2(*(uint32_t *)&temp1[2], *(uint32_t *)&temp2[2]);
                *(uint32_t *)&temp1[2] = _add2(*(uint32_t *)&temp1[2], a);
                *(uint32_t *)&temp1[2] = (*(uint32_t *)&temp1[2] >> 6) & mask;
                _mem4(&dst[x]) = _spacku4(*(int *)&temp1[2], *(int *)temp1);
            }
        }
    }
}

#define PIXEL_AVG_C64( name, width, height ) \
static void name( uint8_t *pix1, int i_stride_pix1, \
                  uint8_t *pix2, int i_stride_pix2, \
                  uint8_t *pix3, int i_stride_pix3, int weight ) \
{ \
    if( weight == 32 )\
        pixel_avg_wxh( pix1, i_stride_pix1, pix2, i_stride_pix2, pix3, i_stride_pix3, width, height ); \
    else\
        pixel_avg_weight_wxh( pix1, i_stride_pix1, pix2, i_stride_pix2, pix3, i_stride_pix3, width, height, weight ); \
}

PIXEL_AVG_C64( x264_pixel_avg_16x16_c64, 16, 16 )
PIXEL_AVG_C64( x264_pixel_avg_16x8_c64,  16,  8 )
PIXEL_AVG_C64( x264_pixel_avg_8x16_c64,   8, 16 )
PIXEL_AVG_C64( x264_pixel_avg_8x8_c64,    8,  8 )
PIXEL_AVG_C64( x264_pixel_avg_8x4_c64,    8,  4 )
PIXEL_AVG_C64( x264_pixel_avg_4x8_c64,    4,  8 )
PIXEL_AVG_C64( x264_pixel_avg_4x4_c64,    4,  4 )
PIXEL_AVG_C64( x264_pixel_avg_4x2_c64,    4,  2 )
PIXEL_AVG_C64( x264_pixel_avg_2x4_c64,    2,  4 )
PIXEL_AVG_C64( x264_pixel_avg_2x2_c64,    2,  2 )

void x264_mc_init_c64(x264_mc_functions_t *pf)
{
    pf->avg[PIXEL_16x16] = x264_pixel_avg_16x16_c64;
    pf->avg[PIXEL_16x8]  = x264_pixel_avg_16x8_c64;
    pf->avg[PIXEL_8x16]  = x264_pixel_avg_8x16_c64;
    pf->avg[PIXEL_8x8]   = x264_pixel_avg_8x8_c64;
    pf->avg[PIXEL_8x4]   = x264_pixel_avg_8x4_c64;
    pf->avg[PIXEL_4x8]   = x264_pixel_avg_4x8_c64;
    pf->avg[PIXEL_4x4]   = x264_pixel_avg_4x4_c64;
    pf->avg[PIXEL_4x2]   = x264_pixel_avg_4x2_c64;
    pf->avg[PIXEL_2x4]   = x264_pixel_avg_2x4_c64;
    pf->avg[PIXEL_2x2]   = x264_pixel_avg_2x2_c64;
}


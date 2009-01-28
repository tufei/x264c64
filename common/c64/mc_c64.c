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

static inline void pixel_avg( uint8_t *dst,  int i_dst_stride,
                              uint8_t *src1, int i_src1_stride,
                              uint8_t *src2, int i_src2_stride,
                              int i_width, int i_height )
{
    int x, y;

    if((i_width & 0x03) == 0)
    {
        for( y = 0; y < i_height; y++ )
        {
            for( x = 0; x < i_width; x += 4 )
            {
                _mem4(&dst[x]) = _avgu4(_mem4_const(&src1[x]), _mem4_const(&src2[x]));
            }
            dst  += i_dst_stride;
            src1 += i_src1_stride;
            src2 += i_src2_stride;
        }
    }
    else
    {
        for( y = 0; y < i_height; y++ )
        {
            for( x = 0; x < i_width; x++ )
            {
                dst[x] = ( src1[x] + src2[x] + 1 ) >> 1;
            }
            dst  += i_dst_stride;
            src1 += i_src1_stride;
            src2 += i_src2_stride;
        }
    }
}

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
    register const uint32_t a = 32 * (0x00010001U);
    register const uint32_t mask = 0x03FF03FFU;
    register const uint32_t s1 = i_weight1 * (width == 2 ? 0x00000101U : 0x01010101U);
    register const uint32_t s2 = i_weight2 * (width == 2 ? 0x00000101U : 0x01010101U);
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

static void mc_copy( uint8_t *src, int i_src_stride, uint8_t *dst, int i_dst_stride, int i_width, int i_height )
{
    int y;

    for( y = 0; y < i_height; y++ )
    {
        memcpy( dst, src, i_width );

        src += i_src_stride;
        dst += i_dst_stride;
    }
}

/*((pix)[-2*d] + (pix)[3*d] - 5*((pix)[-d] + (pix)[2*d]) + 20*((pix)[0] + (pix)[d]))*/
static inline uint64_t tap_filter_8(const uint8_t *pix, int stride) 
{
    uint32_t temp[2], data;
    uint64_t output;
    double input;
    register const uint32_t m5 = 0x05050505U;
    register const uint32_t m20 = 0x14141414U;

    input = _mpyu4(_mem4_const(pix), m20);
    output = *(uint64_t *)&input;
    input = _mpyu4(_mem4_const(&pix[stride]), m20);
    output += *(uint64_t *)&input;

    data = _mem4_const(&pix[-2 * stride]);
    temp[0] = _unpklu4(data); temp[1] = _unpkhu4(data);
    output += *(uint64_t *)temp;
    data = _mem4_const(&pix[3 * stride]);
    temp[0] = _unpklu4(data); temp[1] = _unpkhu4(data);
    output += *(uint64_t *)temp;

    input = _mpyu4(_mem4_const(&pix[2 * stride]), m5);
    output -= *(uint64_t *)&input;
    input = _mpyu4(_mem4_const(&pix[-1 * stride]), m5);
    output -= *(uint64_t *)&input;

    return output;
}

static inline uint64_t tap_filter_16(const uint16_t *pix, int stride) 
{
    uint32_t temp[2], data;
    uint64_t output;
    double input;
    register const uint32_t m5 = 0x00050005U;
    register const uint32_t m20 = 0x00140014U;

    input = _mpy2(_mem4_const(pix), m20);
    output = *(uint64_t *)&input;
    input = _mpy2(_mem4_const(&pix[stride]), m20);
    output += *(uint64_t *)&input;

    temp[0] = pix[-2 * stride];
    temp[1] = pix[-2 * stride + 1];
    output += *(uint64_t *)temp;
    temp[0] = pix[3 * stride];
    temp[1] = pix[3 * stride + 1];
    output += *(uint64_t *)temp;

    input = _mpy2(_mem4_const(&pix[2 * stride]), m5);
    output -= *(uint64_t *)&input;
    input = _mpy2(_mem4_const(&pix[-1 * stride]), m5);
    output -= *(uint64_t *)&input;

    return output;
}

static void hpel_filter_c64( uint8_t *dsth, uint8_t *dstv, uint8_t *dstc, uint8_t *src, int stride, int width, int height, int16_t *buf )
{
    int x, y;
    uint64_t v;
    register uint64_t round;
    register uint64_t mask;

    uint8_t *dsth_buf = dsth;
    uint8_t *dstv_buf = dstv;
    uint8_t *dstc_buf = dstc;
    uint8_t *src_buf = src;

    for(y = 0; y < height; y++) 
    {
        round = 0x0010001000100010ULL;
        mask = 0x07FF07FF07FF07FFULL;
        for(x = -2; x < (width + 3 - 4); x += 4)
        {
            v = tap_filter_8(&src[x], stride);
            _mem8(&buf[x + 2]) = v;
            v += round;
            v = (v >> 5) & mask;
            _mem4(&dstv[x]) = _spacku4(_hi(*(double *)&v), _lo(*(double *)&v));
        }
        v = tap_filter_8(&src[x], stride);
        buf[x + 2] = (int16_t)_lo(*(double *)&v);
        v += round;
        v = (v >> 5) & mask;
        dstv[x] = (uint8_t)_spacku4(0, _lo(*(double *)&v));

        for(x = 0; x < width; x += 4) 
        {
            v = tap_filter_8(&src[x], 1);
            v += round;
            v = (v >> 5) & mask;
            _mem4(&dsth[x]) = _spacku4(_hi(*(double *)&v), _lo(*(double *)&v));
        }

        round = 0x0000020000000200ULL;
        mask = 0x003FFFFF003FFFFFULL;
        for(x = 0; x < width; x += 4) 
        {
            uint64_t u = tap_filter_16((const uint16_t *)&buf[x + 2 + 2], 1);
            v = tap_filter_16((const uint16_t *)&buf[x + 2], 1);
            v += round;
            v = (v >> 10) & mask;
            u += round;
            u = (u >> 10) & mask;
            _mem4(&dstc[x]) = _spacku4(_spack2(_hi(*(double *)&u), _lo(*(double *)&u)), 
                _spack2(_hi(*(double *)&v), _lo(*(double *)&v)));
        }
        dsth += stride;
        dstv += stride;
        dstc += stride;
        src += stride;
    }
}

static const int hpel_ref0[16] = {0,1,1,1,0,1,1,1,2,3,3,3,0,1,1,1};
static const int hpel_ref1[16] = {0,0,0,0,2,2,3,2,2,2,3,2,2,2,3,2};

static void mc_luma_c64( uint8_t *dst, int i_dst_stride, uint8_t *src[4], int i_src_stride, int mvx, int mvy, int i_width, int i_height )
{
    int qpel_idx = ((mvy & 3) << 2) + (mvx & 3);
    int offset = (mvy >> 2) * i_src_stride + (mvx >> 2);
    uint8_t *src1 = src[hpel_ref0[qpel_idx]] + offset + ((mvy & 3) == 3) * i_src_stride;

    if( qpel_idx & 5 ) /* qpel interpolation needed */
    {
        uint8_t *src2 = src[hpel_ref1[qpel_idx]] + offset + ((mvx & 3) == 3);
        pixel_avg( dst, i_dst_stride, src1, i_src_stride, src2, i_src_stride, i_width, i_height );
    }
    else
    {
        mc_copy( src1, i_src_stride, dst, i_dst_stride, i_width, i_height );
    }
}

static uint8_t *get_ref_c64( uint8_t *dst, int *i_dst_stride, uint8_t *src[4], int i_src_stride, int mvx, int mvy, int i_width, int i_height )
{
    int qpel_idx = ((mvy & 3) << 2) + (mvx & 3);
    int offset = (mvy >> 2) * i_src_stride + (mvx >> 2);
    uint8_t *src1 = src[hpel_ref0[qpel_idx]] + offset + ((mvy & 3) == 3) * i_src_stride;

    if( qpel_idx & 5 ) /* qpel interpolation needed */
    {
        uint8_t *src2 = src[hpel_ref1[qpel_idx]] + offset + ((mvx & 3) == 3);
        pixel_avg( dst, *i_dst_stride, src1, i_src_stride, src2, i_src_stride, i_width, i_height );
        return dst;
    }
    else
    {
        *i_dst_stride = i_src_stride;
        return src1;
    }
}

/* full chroma mc (ie until 1/8 pixel)*/
static void mc_chroma_c64( uint8_t *dst, int i_dst_stride, uint8_t *src, int i_src_stride, int mvx, int mvy, int i_width, int i_height )
{
    uint8_t *srcp;
    int x, y;

    const int d8x = mvx & 0x07;
    const int d8y = mvy & 0x07;

    const int16_t cA = (8 - d8x) * (8 - d8y);
    const int16_t cB = d8x * (8 - d8y);
    const int16_t cC = (8 - d8x) * d8y;
    const int16_t cD = d8x * d8y;
    register const int32_t cBcA = _pack2(cB, cA);
    register const int32_t cDcC = _pack2(cD, cC);

    register uint32_t data_up, data_down;
    register int unpack_up, unpack_down;

    src += (mvy >> 3) * i_src_stride + (mvx >> 3);
    srcp = &src[i_src_stride];

    for( y = 0; y < i_height; y++ )
    {
        for( x = 0; x < i_width; x += 4 )
        {
            data_up = _mem4_const(&src[x]);
            data_down = _mem4_const(&srcp[x]);

            unpack_up = _unpklu4(data_up);
            unpack_up = _dotp2(cBcA, unpack_up);
            unpack_down = _unpklu4(data_down);
            unpack_down = _dotp2(cDcC, unpack_down);
            dst[x] = (unpack_up + unpack_down + 32) >> 6;

            unpack_up = _unpkhu4(data_up);
            unpack_up = _dotp2(cBcA, unpack_up);
            unpack_down = _unpkhu4(data_down);
            unpack_down = _dotp2(cDcC, unpack_down);
            dst[x + 2] = (unpack_up + unpack_down + 32) >> 6;
            
            data_up = _mem4_const(&src[x + 1]);
            data_down = _mem4_const(&srcp[x + 1]);

            unpack_up = _unpklu4(data_up);
            unpack_up = _dotp2(cBcA, unpack_up);
            unpack_down = _unpklu4(data_down);
            unpack_down = _dotp2(cDcC, unpack_down);
            dst[x + 1] = (unpack_up + unpack_down + 32) >> 6;

            unpack_up = _unpkhu4(data_up);
            unpack_up = _dotp2(cBcA, unpack_up);
            unpack_down = _unpkhu4(data_down);
            unpack_down = _dotp2(cDcC, unpack_down);
            dst[x + 3] = (unpack_up + unpack_down + 32) >> 6;
        }
        dst  += i_dst_stride;

        src   = srcp;
        srcp += i_src_stride;
    }
}

static void mc_copy_w4_c64( uint8_t *dst, int i_dst, uint8_t *src, int i_src, int i_height )
{
    int y;

    for( y = 0; y < i_height; y++ )
    {
        _mem4(dst) = _mem4_const(src);
        src += i_src;
        dst += i_dst;
    }
}

static void mc_copy_w8_c64( uint8_t *dst, int i_dst, uint8_t *src, int i_src, int i_height )
{
    int y;

    for( y = 0; y < i_height; y++ )
    {
        _mem8(dst) = _mem8_const(src);
        src += i_src;
        dst += i_dst;
    }
}

static void mc_copy_w16_c64( uint8_t *dst, int i_dst, uint8_t *src, int i_src, int i_height )
{
    int y;

    for( y = 0; y < i_height; y++ )
    {
        _mem8(dst) = _mem8_const(src);
        _mem8(&dst[8]) = _mem8_const(&src[8]);
        src += i_src;
        dst += i_dst;
    }
}

void x264_mc_init_c64(x264_mc_functions_t *pf)
{
    pf->mc_luma = mc_luma_c64;
    pf->get_ref = get_ref_c64;
    pf->mc_chroma = mc_chroma_c64;

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

    pf->copy_16x16_unaligned = mc_copy_w16_c64;
    pf->copy[PIXEL_16x16] = mc_copy_w16_c64;
    pf->copy[PIXEL_8x8]   = mc_copy_w8_c64;
    pf->copy[PIXEL_4x4]   = mc_copy_w4_c64;

    pf->hpel_filter = hpel_filter_c64;
}


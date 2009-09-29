/*****************************************************************************
 * pixel_c64.c: h264 encoder library
 *****************************************************************************
 * Copyright (C) 2003-2009 x264 project
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
#define PIXEL_SAD_C64_4(name, lx, ly) \
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

#define PIXEL_SAD_C64_8(name, lx, ly) \
int name(uint8_t *pix1, int i_stride_pix1,                                  \
         uint8_t *pix2, int i_stride_pix2)                                  \
{                                                                           \
    const uint32_t unit = 0x01010101U;                                      \
    uint64_t pack1, pack2;                                                  \
    uint32_t hi4, lo4;                                                      \
    int i_sum = 0;                                                          \
    int x, y;                                                               \
    for(y = 0; y < ly; y++)                                                 \
    {                                                                       \
        for(x = 0; x < lx; x += 8)                                          \
        {                                                                   \
            pack1 = _mem8_const(&pix1[x]);                                  \
            pack2 = _mem8_const(&pix2[x]);                                  \
            hi4 = _subabs4(_hill(pack1), _hill(pack2));                     \
            lo4 = _subabs4(_loll(pack1), _loll(pack2));                     \
            i_sum += _dotpu4(hi4, unit);                                    \
            i_sum += _dotpu4(lo4, unit);                                    \
        }                                                                   \
        pix1 += i_stride_pix1;                                              \
        pix2 += i_stride_pix2;                                              \
    }                                                                       \
    return i_sum;                                                           \
}

PIXEL_SAD_C64_8(x264_pixel_sad_16x16_c64, 16, 16)
PIXEL_SAD_C64_8(x264_pixel_sad_8x16_c64,   8, 16)
PIXEL_SAD_C64_8(x264_pixel_sad_16x8_c64,  16,  8)
PIXEL_SAD_C64_8(x264_pixel_sad_8x8_c64,    8,  8)
PIXEL_SAD_C64_8(x264_pixel_sad_8x4_c64,    8,  4)
PIXEL_SAD_C64_4(x264_pixel_sad_4x8_c64,    4,  8)
PIXEL_SAD_C64_4(x264_pixel_sad_4x4_c64,    4,  4)

/****************************************************************************
 * pixel_ssd_WxH
 ****************************************************************************/
#define PIXEL_SSD_C64_4( name, lx, ly ) \
int name( uint8_t *pix1, int i_stride_pix1,                                 \
          uint8_t *pix2, int i_stride_pix2 )                                \
{                                                                           \
    int i_sum = 0;                                                          \
    int x, y;                                                               \
    uint32_t pack4;                                                         \
    for( y = 0; y < ly; y++ )                                               \
    {                                                                       \
        for( x = 0; x < lx; x += 4 )                                        \
        {                                                                   \
            pack4 = _subabs4(_mem4_const(&pix1[x]), _mem4_const(&pix2[x])); \
            i_sum += _dotpu4(pack4, pack4);                                 \
        }                                                                   \
        pix1 += i_stride_pix1;                                              \
        pix2 += i_stride_pix2;                                              \
    }                                                                       \
    return i_sum;                                                           \
}

#define PIXEL_SSD_C64_8( name, lx, ly ) \
int name( uint8_t *pix1, int i_stride_pix1,                                 \
          uint8_t *pix2, int i_stride_pix2 )                                \
{                                                                           \
    int i_sum = 0;                                                          \
    int x, y;                                                               \
    uint64_t pack1, pack2;                                                  \
    uint32_t hi4, lo4;                                                      \
    for( y = 0; y < ly; y++ )                                               \
    {                                                                       \
        for( x = 0; x < lx; x += 8 )                                        \
        {                                                                   \
            pack1 = _mem8_const(&pix1[x]);                                  \
            pack2 = _mem8_const(&pix2[x]);                                  \
            hi4 = _subabs4(_hill(pack1), _hill(pack2));                     \
            lo4 = _subabs4(_loll(pack1), _loll(pack2));                     \
            i_sum += _dotpu4(hi4, hi4);                                     \
            i_sum += _dotpu4(lo4, lo4);                                     \
        }                                                                   \
        pix1 += i_stride_pix1;                                              \
        pix2 += i_stride_pix2;                                              \
    }                                                                       \
    return i_sum;                                                           \
}

PIXEL_SSD_C64_8( x264_pixel_ssd_16x16_c64, 16, 16 )
PIXEL_SSD_C64_8( x264_pixel_ssd_16x8_c64,  16,  8 )
PIXEL_SSD_C64_8( x264_pixel_ssd_8x16_c64,   8, 16 )
PIXEL_SSD_C64_8( x264_pixel_ssd_8x8_c64,    8,  8 )
PIXEL_SSD_C64_8( x264_pixel_ssd_8x4_c64,    8,  4 )
PIXEL_SSD_C64_4( x264_pixel_ssd_4x8_c64,    4,  8 )
PIXEL_SSD_C64_4( x264_pixel_ssd_4x4_c64,    4,  4 )

#define HADAMARD4(d0,d1,d2,d3,s0,s1,s2,s3) {\
    uint32_t t0 = _add2(s0, s1);\
    uint32_t t1 = _sub2(s0, s1);\
    uint32_t t2 = _add2(s2, s3);\
    uint32_t t3 = _sub2(s2, s3);\
    d0 = _add2(t0, t2);\
    d2 = _sub2(t0, t2);\
    d1 = _add2(t1, t3);\
    d3 = _sub2(t1, t3);\
}

int x264_pixel_satd_4x4_c64( uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 )
{
    uint32_t tmp[4][2];
    uint32_t a0,a1,a2,a3,b0,b1;
    uint32_t unit = 0x00010001;
    int sum=0, i;
    for( i=0; i<4; i++, pix1+=i_pix1, pix2+=i_pix2 )
    {
        b0 = _mem4_const(pix1);
        b1 = _mem4_const(pix2);
        a0 = _unpklu4(b0); a1 = _unpkhu4(b0);
        a2 = _unpklu4(b1); a3 = _unpkhu4(b1);
        b0 = _sub2(a0, a2); b1 = _sub2(a1, a3);
        a0 = _add2(b0, b1); a1 = _sub2(b0, b1);
        b0 = _packlh2(a0, a0); b1 = _packlh2(a1, a1);
        a0 = _dotp2(b0, unit); a1 = _dotpn2(b0, unit);
        a2 = _dotp2(b1, unit); a3 = _dotpn2(b1, unit);
        tmp[i][0] = _spack2(a1, a0);
        tmp[i][1] = _spack2(a3, a2);
    }
    for( i=0; i<2; i++ )
    {
        HADAMARD4(a0, a1, a2, a3, tmp[0][i], tmp[1][i], tmp[2][i], tmp[3][i]);
        a0 = _abs2(a0) + _abs2(a1) + _abs2(a2) + _abs2(a3);
        sum += ((uint16_t)a0) + (a0>>16);
    }
    return sum >> 1;
}

int x264_pixel_satd_8x4_c64( uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 )
{
    uint32_t tmp[4][4];
    uint32_t a0,a1,a2,a3;
    uint32_t b0,b1,b2,b3;
    uint64_t d0, d1;
    int sum=0, i;
    for( i=0; i<4; i++, pix1+=i_pix1, pix2+=i_pix2 )
    {
        d0 = _mem8_const(pix1); d1 = _mem8_const(pix2);
        b0 = _sub2(_unpklu4(_loll(d0)), _unpklu4(_loll(d1)));
        b1 = _sub2(_unpkhu4(_loll(d0)), _unpkhu4(_loll(d1)));
        b2 = _sub2(_unpklu4(_hill(d0)), _unpklu4(_hill(d1)));
        b3 = _sub2(_unpkhu4(_hill(d0)), _unpkhu4(_hill(d1)));
        a0 = _pack2(b2, b0); a1 = _packh2(b2, b0);
        a2 = _pack2(b3, b1); a3 = _packh2(b3, b1);
        HADAMARD4( tmp[i][0], tmp[i][1], tmp[i][2], tmp[i][3], a0,a1,a2,a3 );
    }
    for( i=0; i<4; i++ )
    {
        HADAMARD4( a0,a1,a2,a3, tmp[0][i], tmp[1][i], tmp[2][i], tmp[3][i] );
        sum += _abs2(a0) + _abs2(a1) + _abs2(a2) + _abs2(a3);
    }
    return (((uint16_t)sum) + ((uint32_t)sum>>16)) >> 1;
}

#define PIXEL_SATD_C64( w, h, sub )\
int x264_pixel_satd_##w##x##h##_c64( uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 )\
{\
    int sum = sub( pix1, i_pix1, pix2, i_pix2 )\
            + sub( pix1+4*i_pix1, i_pix1, pix2+4*i_pix2, i_pix2 );\
    if( w==16 )\
        sum+= sub( pix1+8, i_pix1, pix2+8, i_pix2 )\
            + sub( pix1+8+4*i_pix1, i_pix1, pix2+8+4*i_pix2, i_pix2 );\
    if( h==16 )\
        sum+= sub( pix1+8*i_pix1, i_pix1, pix2+8*i_pix2, i_pix2 )\
            + sub( pix1+12*i_pix1, i_pix1, pix2+12*i_pix2, i_pix2 );\
    if( w==16 && h==16 )\
        sum+= sub( pix1+8+8*i_pix1, i_pix1, pix2+8+8*i_pix2, i_pix2 )\
            + sub( pix1+8+12*i_pix1, i_pix1, pix2+8+12*i_pix2, i_pix2 );\
    return sum;\
}
PIXEL_SATD_C64( 16, 16, x264_pixel_satd_8x4_c64 )
PIXEL_SATD_C64( 16, 8,  x264_pixel_satd_8x4_c64 )
PIXEL_SATD_C64( 8,  16, x264_pixel_satd_8x4_c64 )
PIXEL_SATD_C64( 8,  8,  x264_pixel_satd_8x4_c64 )
PIXEL_SATD_C64( 4,  8,  x264_pixel_satd_4x4_c64 )

/****************************************************************************
 * pixel_var_wxh
 ****************************************************************************/
#define PIXEL_VAR_C64( name, w, shift ) \
int name( uint8_t *pix, int i_stride )                      \
{                                                           \
    uint32_t var = 0, sum = 0, sqr = 0;                     \
    const uint32_t unit = 0x01010101U;                      \
    int x, y;                                               \
    for( y = 0; y < w; y++ )                                \
    {                                                       \
        for( x = 0; x < w; x += 8 )                         \
        {                                                   \
            uint64_t data = _mem8_const(&pix[x]);           \
            sum += _dotpu4(_loll(data), unit);              \
            sum += _dotpu4(_hill(data), unit);              \
            sqr += _dotpu4(_loll(data), _loll(data));       \
            sqr += _dotpu4(_hill(data), _hill(data));       \
        }                                                   \
        pix += i_stride;                                    \
    }                                                       \
    var = sqr - (sum * sum >> shift);                       \
    return var;                                             \
}

PIXEL_VAR_C64( x264_pixel_var_16x16_c64, 16, 8 )
PIXEL_VAR_C64( x264_pixel_var_8x8_c64,    8, 6 )

static NOINLINE int sa8d_8x8( uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 )
{
    uint32_t tmp[8][4];
    uint32_t a0,a1,a2,a3,a4,a5,a6,a7,b0,b1,b2,b3;
    uint32_t unit = 0x00010001;
    int sum=0, i;
    uint64_t d0, d1;
    for( i=0; i<8; i++, pix1+=i_pix1, pix2+=i_pix2 )
    {
        d0 = _mem8_const(pix1);
        d1 = _mem8_const(pix2);
        b0 = _swap4(_loll(d0)); b1 = _swap4(_loll(d1));
        b2 = _swap4(_hill(d0)); b3 = _swap4(_hill(d1));
        a0 = _sub2(_unpklu4(b0), _unpklu4(b1));
        a1 = _sub2(_unpkhu4(b0), _unpkhu4(b1));
        a2 = _sub2(_unpklu4(b2), _unpklu4(b3));
        a3 = _sub2(_unpkhu4(b2), _unpkhu4(b3));
        b0 = _spack2(_dotpn2(a0, unit), _dotp2(a0, unit));
        b1 = _spack2(_dotpn2(a1, unit), _dotp2(a1, unit));
        b2 = _spack2(_dotpn2(a2, unit), _dotp2(a2, unit));
        b3 = _spack2(_dotpn2(a3, unit), _dotp2(a3, unit));
        HADAMARD4( tmp[i][0], tmp[i][1], tmp[i][2], tmp[i][3], b0,b1,b2,b3 );
    }
    for( i=0; i<4; i++ )
    {
        HADAMARD4( a0,a1,a2,a3, tmp[0][i], tmp[1][i], tmp[2][i], tmp[3][i] );
        HADAMARD4( a4,a5,a6,a7, tmp[4][i], tmp[5][i], tmp[6][i], tmp[7][i] );
        b0  = _abs2(_add2(a0, a4)) + _abs2(_sub2(a0, a4));
        b0 += _abs2(_add2(a1, a5)) + _abs2(_sub2(a1, a5));
        b0 += _abs2(_add2(a2, a6)) + _abs2(_sub2(a2, a6));
        b0 += _abs2(_add2(a3, a7)) + _abs2(_sub2(a3, a7));
        sum += (uint16_t)b0 + (b0>>16);
    }
    return sum;
}

int x264_pixel_sa8d_8x8_c64( uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 )
{
    int sum = sa8d_8x8( pix1, i_pix1, pix2, i_pix2 );
    return (sum+2)>>2;
}

int x264_pixel_sa8d_16x16_c64( uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 )
{
    int sum = sa8d_8x8( pix1, i_pix1, pix2, i_pix2 )
            + sa8d_8x8( pix1+8, i_pix1, pix2+8, i_pix2 )
            + sa8d_8x8( pix1+8*i_pix1, i_pix1, pix2+8*i_pix2, i_pix2 )
            + sa8d_8x8( pix1+8+8*i_pix1, i_pix1, pix2+8+8*i_pix2, i_pix2 );
    return (sum+2)>>2;
}


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

/* 0x01 = '+', 0xFF = '-' */
static const uint32_t sign_table[16] = 
{
    0x01010101U, 0x010101FFU, 0x0101FF01U, 0x0101FFFFU,
    0x01FF0101U, 0x01FF01FFU, 0x01FFFF01U, 0x01FFFFFFU,
    0xFF010101U, 0xFF0101FFU, 0xFF01FF01U, 0xFF01FFFFU,
    0xFFFF0101U, 0xFFFF01FFU, 0xFFFFFF01U, 0xFFFFFFFFU
};

/****************************************************************************
 * pixel_satd_WxH: sum of 4x4 Hadamard transformed differences
 ****************************************************************************/
static int pixel_satd_wxh( uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2, int i_width, int i_height )
{
    static const uint8_t hadamard_sign[4] = 
    {
        0, /* + + + + */
        5, /* + - + - */
        3, /* + + - - */
        6  /* + - - + */
    };
    int16_t tmp[4][4];
    int x, y;
    int i_satd = 0;

    for( y = 0; y < i_height; y += 4 )
    {
        for( x = 0; x < i_width; x += 4 )
        {
            uint8_t *p1 = pix1 + x, *p2 = pix2 + x;
            int i, s, sign;

            for( i = 0; i < 4; i++, p1 += i_pix1, p2 += i_pix2 )
            {
                sign = _cmpgtu4(_mem4_const(p2), _mem4_const(p1));
                s = _subabs4(_mem4_const(p1), _mem4_const(p2));
                _mem4(tmp[i]) = _pack2(_dotpsu4(sign_table[sign ^ hadamard_sign[1]], s), 
                    _dotpsu4(sign_table[sign ^ hadamard_sign[0]], s));
                _mem4(&tmp[i][2]) = _pack2(_dotpsu4(sign_table[sign ^ hadamard_sign[3]], s), 
                    _dotpsu4(sign_table[sign ^ hadamard_sign[2]], s));
            }
            for( i = 0; i < 4; i++ )
            {
                int16_t middle[4];
                int pack;

                middle[0] = tmp[0][i]; middle[1] = tmp[1][i];
                middle[2] = tmp[2][i]; middle[3] = tmp[3][i];
                pack = _add2(_mem4_const(middle), _mem4_const(&middle[2]));
                i_satd += _abs(_ext(pack, 16, 16) + _ext(pack, 0, 16));

                pack = _add2(_mem4_const(middle), _mem4_const(&middle[2]));
                i_satd += _abs(_ext(pack, 16, 16) - _ext(pack, 0, 16));

                pack = _sub2(_mem4_const(middle), _mem4_const(&middle[2]));
                i_satd += _abs(_ext(pack, 16, 16) + _ext(pack, 0, 16));

                pack = _sub2(_mem4_const(middle), _mem4_const(&middle[2]));
                i_satd += _abs(_ext(pack, 16, 16) - _ext(pack, 0, 16));
            }
        }
        pix1 += 4 * i_pix1;
        pix2 += 4 * i_pix2;
    }

    return i_satd >> 1;
}

#define PIXEL_SATD_C64( name, width, height ) \
int name( uint8_t *pix1, int i_stride_pix1, \
                 uint8_t *pix2, int i_stride_pix2 ) \
{ \
    return pixel_satd_wxh( pix1, i_stride_pix1, pix2, i_stride_pix2, width, height ); \
}
PIXEL_SATD_C64( x264_pixel_satd_16x16_c64, 16, 16 )
PIXEL_SATD_C64( x264_pixel_satd_16x8_c64,  16,  8 )
PIXEL_SATD_C64( x264_pixel_satd_8x16_c64,   8, 16 )
PIXEL_SATD_C64( x264_pixel_satd_8x8_c64,    8,  8 )
PIXEL_SATD_C64( x264_pixel_satd_8x4_c64,    8,  4 )
PIXEL_SATD_C64( x264_pixel_satd_4x8_c64,    4,  8 )
/*PIXEL_SATD_C64( x264_pixel_satd_4x4_c64,    4,  4 )*/

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


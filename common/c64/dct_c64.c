/*****************************************************************************
 * dct_c64.c: h264 encoder library
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

void x264_dct4x4dc_c64( int16_t d[16] )
{
    int16_t tmp[16];
    uint64_t a;
    int lo, hi;
    int i;
    const uint32_t unit = 0x00010001;

    for( i = 0; i < 4; i++ )
    {
        a = _mem8(&d[i * 4]);
        lo = _packlh2(_loll(a), _loll(a));
        hi = _packlh2(_hill(a), _hill(a));
        tmp[0 * 4 + i] = _dotp2(_add2(lo, hi), unit);
        tmp[1 * 4 + i] = _dotp2(_sub2(lo, hi), unit);
        tmp[2 * 4 + i] = _dotpn2(_sub2(lo, hi), unit);
        tmp[3 * 4 + i] = _dotpn2(_add2(lo, hi), unit);
    }

    for( i = 0; i < 4; i++ )
    {
        uint32_t dl, dh;

        a = _mem8(&tmp[i * 4]);
        lo = _packlh2(_loll(a), _loll(a));
        hi = _packlh2(_hill(a), _hill(a));
        dl = _spack2(_dotp2(_sub2(lo, hi), unit), _dotp2(_add2(lo, hi), unit));
        dh = _spack2(_dotpn2(_add2(lo, hi), unit), _dotpn2(_sub2(lo, hi), unit));
        dl = _shr2(_add2(dl, unit), 1);
        dh = _shr2(_add2(dh, unit), 1);
        _mem8(&d[i * 4]) = _itoll(dh, dl);
    }
}

void x264_idct4x4dc_c64( int16_t d[16] )
{
    int16_t tmp[16];
    uint64_t a;
    int lo, hi;
    int i;
    const uint32_t unit = 0x00010001;

    for( i = 0; i < 4; i++ )
    {
        a = _mem8(&d[i * 4]);
        lo = _packlh2(_loll(a), _loll(a));
        hi = _packlh2(_hill(a), _hill(a));
        tmp[0 * 4 + i] = _dotp2(_add2(lo, hi), unit);
        tmp[1 * 4 + i] = _dotp2(_sub2(lo, hi), unit);
        tmp[2 * 4 + i] = _dotpn2(_sub2(lo, hi), unit);
        tmp[3 * 4 + i] = _dotpn2(_add2(lo, hi), unit);
    }

    for( i = 0; i < 4; i++ )
    {
        uint32_t dl, dh;

        a = _mem8(&tmp[i * 4]);
        lo = _packlh2(_loll(a), _loll(a));
        hi = _packlh2(_hill(a), _hill(a));
        dl = _spack2(_dotp2(_sub2(lo, hi), unit), _dotp2(_add2(lo, hi), unit));
        dh = _spack2(_dotpn2(_add2(lo, hi), unit), _dotpn2(_sub2(lo, hi), unit));
        _mem8(&d[i * 4]) = _itoll(dh, dl);
    }
}

static inline void pixel_sub_wxh( int16_t *diff, int i_size,
                                  uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 )
{
    int y, x;
    uint32_t p1, p2;
    uint32_t lo, hi;
#pragma MUST_ITERATE(4, 8, 4);
    for( y = 0; y < i_size; y++ )
    {
#pragma MUST_ITERATE(1, 2, 1);
        for( x = 0; x < i_size; x += 4 )
        {
            p1 = _mem4_const(&pix1[x]); p2 = _mem4_const(&pix2[x]);
            lo = _sub2(_unpklu4(p1), _unpklu4(p2));
            hi = _sub2(_unpkhu4(p1), _unpkhu4(p2));
            _mem8(&diff[x + y * i_size]) = _itoll(hi, lo);
        }
        pix1 += i_pix1;
        pix2 += i_pix2;
    }
}

void x264_sub4x4_dct_c64( int16_t dct[16], uint8_t *pix1, uint8_t *pix2 )
{
    int16_t d[16];
    int16_t tmp[16];
    int i;
    uint32_t unit = 0x00010001;
    uint32_t lo, hi, lo2, hi2;
    uint64_t m;

    pixel_sub_wxh( d, 4, pix1, FENC_STRIDE, pix2, FDEC_STRIDE );

    for( i = 0; i < 4; i++ )
    {
        m = _mem8_const(&d[i * 4]);
        lo = _packlh2(_loll(m), _loll(m)); hi = _hill(m);
        lo2 = _clr(lo << 1, 16, 16); hi2 = _clr(hi << 1, 16, 16);
        tmp[0 * 4 + i] = _dotp2(_add2(lo, hi), unit);
        tmp[2 * 4 + i] = _dotpn2(_add2(lo, hi), unit);
        tmp[1 * 4 + i] = _dotp2(_packhl2(_sub2(lo2, hi2), _sub2(lo, hi)), unit);
        tmp[3 * 4 + i] = _dotpn2(_packhl2(_sub2(lo, hi), _sub2(lo2, hi2)), unit);
    }

    for( i = 0; i < 4; i++ )
    {
        uint32_t ll, hh;
        m = _mem8_const(&tmp[i * 4]);
        lo = _packlh2(_loll(m), _loll(m)); hi = _hill(m);
        lo2 = _clr(lo << 1, 16, 16); hi2 = _clr(hi << 1, 16, 16);
        ll = _spack2(_dotp2(_packhl2(_sub2(lo2, hi2), _sub2(lo, hi)), unit), _dotp2(_add2(lo, hi), unit));
        hh = _spack2(_dotpn2(_packhl2(_sub2(lo, hi), _sub2(lo2, hi2)), unit), _dotpn2(_add2(lo, hi), unit));
        _mem8(&dct[i * 4]) = _itoll(hh, ll);
    }
}

void x264_sub8x8_dct_c64( int16_t dct[4][16], uint8_t *pix1, uint8_t *pix2 )
{
    x264_sub4x4_dct_c64( dct[0], &pix1[0], &pix2[0] );
    x264_sub4x4_dct_c64( dct[1], &pix1[4], &pix2[4] );
    x264_sub4x4_dct_c64( dct[2], &pix1[4*FENC_STRIDE+0], &pix2[4*FDEC_STRIDE+0] );
    x264_sub4x4_dct_c64( dct[3], &pix1[4*FENC_STRIDE+4], &pix2[4*FDEC_STRIDE+4] );
}

void x264_sub16x16_dct_c64( int16_t dct[16][16], uint8_t *pix1, uint8_t *pix2 )
{
    x264_sub8x8_dct_c64( &dct[ 0], &pix1[0], &pix2[0] );
    x264_sub8x8_dct_c64( &dct[ 4], &pix1[8], &pix2[8] );
    x264_sub8x8_dct_c64( &dct[ 8], &pix1[8*FENC_STRIDE+0], &pix2[8*FDEC_STRIDE+0] );
    x264_sub8x8_dct_c64( &dct[12], &pix1[8*FENC_STRIDE+8], &pix2[8*FDEC_STRIDE+8] );
}

static int sub4x4_dct_dc( uint8_t *pix1, uint8_t *pix2 )
{
    int16_t d[16];
    int i, sum = 0;

    pixel_sub_wxh( d, 4, pix1, FENC_STRIDE, pix2, FDEC_STRIDE );

    for(i = 0; i < 4; i++)
    {
        int m;
        uint64_t d4;

        d4 = _mem8_const(&d[i * 4]);
        m = _add2(_loll(d4), _hill(d4));
        sum += (int16_t)m + (m >> 16);
    }

    return sum;
}

void x264_sub8x8_dct_dc_c64( int16_t dct[4], uint8_t *pix1, uint8_t *pix2 )
{
    dct[0] = sub4x4_dct_dc( &pix1[0], &pix2[0] );
    dct[1] = sub4x4_dct_dc( &pix1[4], &pix2[4] );
    dct[2] = sub4x4_dct_dc( &pix1[4*FENC_STRIDE+0], &pix2[4*FDEC_STRIDE+0] );
    dct[3] = sub4x4_dct_dc( &pix1[4*FENC_STRIDE+4], &pix2[4*FDEC_STRIDE+4] );
}

void x264_add4x4_idct_c64( uint8_t *p_dst, int16_t dct[16] )
{
    int16_t d[16];
    int16_t tmp[16];
    int x, y;
    int i;
    uint32_t r0, r1, r2, r3;

    for( i = 0; i < 4; i += 2 )
    {
        int s02, d02, s13, d13;

        r0 = _mem4_const(&dct[0 * 4 + i]); r1 = _mem4_const(&dct[1 * 4 + i]);
        r2 = _mem4_const(&dct[2 * 4 + i]); r3 = _mem4_const(&dct[3 * 4 + i]);

        s02 = _add2(r0, r2);
        d02 = _sub2(r0, r2);
        s13 = _add2(r1, _shr2(r3, 1));
        d13 = _sub2(_shr2(r1, 1), r3);

        r0 = _add2(s02, s13); r1 = _add2(d02, d13);
        r2 = _sub2(d02, d13); r3 = _sub2(s02, s13);
        _mem8(&tmp[i * 4]) = _itoll(_pack2(r3, r2), _pack2(r1, r0));
        _mem8(&tmp[(i + 1) * 4]) = _itoll(_packh2(r3, r2), _packh2(r1, r0));
    }

    for( i = 0; i < 4; i += 2 )
    {
        int s02, d02, s13, d13;
        uint32_t round = 0x00200020;

        r0 = _mem4_const(&tmp[0 * 4 + i]); r1 = _mem4_const(&tmp[1 * 4 + i]);
        r2 = _mem4_const(&tmp[2 * 4 + i]); r3 = _mem4_const(&tmp[3 * 4 + i]);

        s02 = _add2(r0, r2);
        d02 = _sub2(r0, r2);
        s13 = _add2(r1, _shr2(r3, 1));
        d13 = _sub2(_shr2(r1, 1), r3);

        r0 = _add2(s02, s13); r1 = _add2(d02, d13);
        r2 = _sub2(d02, d13); r3 = _sub2(s02, s13);
        _mem4(&d[0 * 4 + i]) = _shr2(_add2(r0, round), 6);
        _mem4(&d[1 * 4 + i]) = _shr2(_add2(r1, round), 6);
        _mem4(&d[2 * 4 + i]) = _shr2(_add2(r2, round), 6);
        _mem4(&d[3 * 4 + i]) = _shr2(_add2(r3, round), 6);
    }


    for( y = 0; y < 4; y++ )
    {
        int hi, lo;
        uint32_t dst4 = _mem4(p_dst);
        uint64_t d4 = _mem8_const(&d[y * 4]);

        lo = _saddus2(_unpklu4(dst4), _loll(d4));
        hi = _saddus2(_unpkhu4(dst4), _hill(d4));
        _mem4(p_dst) = _spacku4(hi, lo);
        p_dst += FDEC_STRIDE;
    }
}

void x264_add8x8_idct_c64( uint8_t *p_dst, int16_t dct[4][16] )
{
    x264_add4x4_idct_c64( &p_dst[0],               dct[0] );
    x264_add4x4_idct_c64( &p_dst[4],               dct[1] );
    x264_add4x4_idct_c64( &p_dst[4*FDEC_STRIDE+0], dct[2] );
    x264_add4x4_idct_c64( &p_dst[4*FDEC_STRIDE+4], dct[3] );
}

void x264_add16x16_idct_c64( uint8_t *p_dst, int16_t dct[16][16] )
{
    x264_add8x8_idct_c64( &p_dst[0],               &dct[0] );
    x264_add8x8_idct_c64( &p_dst[8],               &dct[4] );
    x264_add8x8_idct_c64( &p_dst[8*FDEC_STRIDE+0], &dct[8] );
    x264_add8x8_idct_c64( &p_dst[8*FDEC_STRIDE+8], &dct[12] );
}


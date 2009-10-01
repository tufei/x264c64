/*****************************************************************************
 * dct.c: h264 encoder library
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

void x264_dct4x4dc_c64( int16_t d[4][4] )
{
    int16_t tmp[4][4];
    uint64_t a;
    int lo, hi;
    int i;
    const uint32_t unit = 0x00010001;

    for( i = 0; i < 4; i++ )
    {
        a = _mem8(d[i]);
        lo = _packlh2(_loll(a), _loll(a));
        hi = _packlh2(_hill(a), _hill(a));
        tmp[0][i] = _dotp2(_add2(lo, hi), unit);
        tmp[1][i] = _dotp2(_sub2(lo, hi), unit);
        tmp[2][i] = _dotpn2(_sub2(lo, hi), unit);
        tmp[3][i] = _dotpn2(_add2(lo, hi), unit);
    }

    for( i = 0; i < 4; i++ )
    {
        uint32_t dl, dh;

        a = _mem8(tmp[i]);
        lo = _packlh2(_loll(a), _loll(a));
        hi = _packlh2(_hill(a), _hill(a));
        dl = _spack2(_dotp2(_sub2(lo, hi), unit), _dotp2(_add2(lo, hi), unit));
        dh = _spack2(_dotpn2(_add2(lo, hi), unit), _dotpn2(_sub2(lo, hi), unit));
        dl = _shr2(_add2(dl, unit), 1);
        dh = _shr2(_add2(dh, unit), 1);
        _mem8(d[i]) = _itoll(dh, dl);
    }
}

void x264_idct4x4dc_c64( int16_t d[4][4] )
{
    int16_t tmp[4][4];
    uint64_t a;
    int lo, hi;
    int i;
    const uint32_t unit = 0x00010001;

    for( i = 0; i < 4; i++ )
    {
        a = _mem8(d[i]);
        lo = _packlh2(_loll(a), _loll(a));
        hi = _packlh2(_hill(a), _hill(a));
        tmp[0][i] = _dotp2(_add2(lo, hi), unit);
        tmp[1][i] = _dotp2(_sub2(lo, hi), unit);
        tmp[2][i] = _dotpn2(_sub2(lo, hi), unit);
        tmp[3][i] = _dotpn2(_add2(lo, hi), unit);
    }

    for( i = 0; i < 4; i++ )
    {
        uint32_t dl, dh;

        a = _mem8(tmp[i]);
        lo = _packlh2(_loll(a), _loll(a));
        hi = _packlh2(_hill(a), _hill(a));
        dl = _spack2(_dotp2(_sub2(lo, hi), unit), _dotp2(_add2(lo, hi), unit));
        dh = _spack2(_dotpn2(_add2(lo, hi), unit), _dotpn2(_sub2(lo, hi), unit));
        _mem8(d[i]) = _itoll(dh, dl);
    }
}


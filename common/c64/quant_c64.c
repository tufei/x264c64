/*****************************************************************************
 * quant_c64.c: h264 encoder library
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

#if 1
#define QUANT_FOUR( coef, mf, f ) \
{                                                                   \
    uint32_t s0, s1, a0, a1;                                        \
    uint64_t bf, m, c;                                              \
    c = _mem8(&(coef));                                             \
    bf = _mem8_const(&(f));                                         \
    a0 = _abs2(_loll(c)); a1 = _abs2(_hill(c));                     \
    s0 = _cmpgt2(a0, _loll(c)); s1 = _cmpgt2(a1, _hill(c));         \
    m = _mem8_const(&(mf));                                         \
    a0 = _add2(a0, _loll(bf)); a1 = _add2(a1, _hill(bf));           \
    c = _mpy2ll(a0, _loll(m));                                      \
    a0 = _packh2(_hill(c), _loll(c));                               \
    c = _mpy2ll(a1, _hill(m));                                      \
    a1 = _packh2(_hill(c), _loll(c));                               \
    a0 ^= _xpnd2(s0); a1 ^= _xpnd2(s1);                             \
    a0 = _add2(a0, _deal(s0)); a1 = _add2(a1, _deal(s1));           \
    c = _itoll(a1, a0);                                             \
    _mem8(&(coef)) = c;                                             \
    nz |= !!c;                                                      \
}
#else
#define QUANT_FOUR( coef, mf, f ) \
{                                                                   \
    uint32_t b0, b1, b2, b3;                                        \
    uint32_t s0, s1, a0, a1;                                        \
    uint64_t bf, m, c;                                              \
    c = _mem8(&(coef));                                             \
    bf = _mem8_const(&(f));                                         \
    a0 = _abs2(_loll(c)); a1 = _abs2(_hill(c));                     \
    s0 = _cmpgt2(a0, _loll(c)); s1 = _cmpgt2(a1, _hill(c));         \
    m = _mem8_const(&(mf));                                         \
    a0 = _add2(a0, _loll(bf)); a1 = _add2(a1, _hill(bf));           \
    b0 = _mpyu(a0, _loll(m)); b1 = _mpyhu(a0, _loll(m));            \
    b2 = _mpyu(a1, _hill(m)); b3 = _mpyhu(a1, _hill(m));            \
    a0 = _packh2(b1, b0); a1 = _packh2(b3, b2);                     \
    a0 ^= _xpnd2(s0); a1 ^= _xpnd2(s1);                             \
    a0 = _add2(a0, _deal(s0)); a1 = _add2(a1, _deal(s1));           \
    c = _itoll(a1, a0);                                             \
    _mem8(&(coef)) = c;                                             \
    nz |= !!c;                                                      \
}
#endif

int x264_quant_8x8_c64( int16_t dct[8][8], uint16_t mf[64], uint16_t bias[64] )
{
    int i, nz = 0;
    for( i = 0; i < 64; i += 4 )
        QUANT_FOUR( dct[0][i], mf[i], bias[i] );
    return nz;
}

int x264_quant_4x4_c64( int16_t dct[4][4], uint16_t mf[16], uint16_t bias[16] )
{
    int i, nz = 0;
    for( i = 0; i < 16; i += 4 )
        QUANT_FOUR( dct[0][i], mf[i], bias[i] );
    return nz;
}


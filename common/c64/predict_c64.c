/*****************************************************************************
 * predict_c64.c: h264 encoder
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

/****************************************************************************
 * 16x16 prediction for intra luma block
 ****************************************************************************/

#define PREDICT_16x16_DC(v) \
    for( i = 0; i < 16; i++ )\
    {\
        uint64_t *p = (uint64_t*)src;\
        _mem8(p++) = v;\
        _mem8(p++) = v;\
        src += FDEC_STRIDE;\
    }

static void predict_16x16_dc_c64( uint8_t *src )
{
    uint32_t dc = 0;
    int i;

    for( i = 0; i < 16; i++ )
    {
        dc += src[-1 + i * FDEC_STRIDE];
        dc += src[i - FDEC_STRIDE];
    }
#if 0
    for( i = 0; i < 16; i += 8 )
    {
        uint64_t s8;
        uint32_t unit = 0x01010101UL;
        s8 = _mem8_const(&src[i - FDEC_STRIDE]);
        dc += _dotpu4(_loll(s8), unit);
        dc += _dotpu4(_hill(s8), unit);
        /*dc += src[i - FDEC_STRIDE];*/
    }
#endif
    dc = (( dc + 16 ) >> 5) * 0x01010101U;

    PREDICT_16x16_DC(_itoll(dc, dc));
}

static void predict_16x16_dc_left_c64( uint8_t *src )
{
    uint32_t dc = 0;
    int i;

    for( i = 0; i < 16; i++ )
    {
        dc += src[-1 + i * FDEC_STRIDE];
    }
    dc = (( dc + 8 ) >> 4) * 0x01010101U;

    PREDICT_16x16_DC(_itoll(dc, dc));
}

static void predict_16x16_dc_top_c64( uint8_t *src )
{
    uint32_t dc = 0;
    uint32_t unit = 0x01010101U;
    int i;

    for( i = 0; i < 16; i += 8 )
    {
        uint64_t s8;

        s8 = _mem8_const(&src[i - FDEC_STRIDE]);
        dc += _dotpu4(_loll(s8), unit);
        dc += _dotpu4(_hill(s8), unit);
    }
    dc = (( dc + 8 ) >> 4) * unit;

    PREDICT_16x16_DC(_itoll(dc, dc));
}

static void predict_16x16_dc_128_c64( uint8_t *src )
{
    int i;
    PREDICT_16x16_DC(_itoll(0x80808080U, 0x80808080U));
}

static void predict_16x16_h_c64( uint8_t *src )
{
    int i;

    for( i = 0; i < 16; i++ )
    {
        const uint32_t v = 0x01010101U * src[-1];
        const uint64_t v2 = _itoll(v, v);
        uint64_t *p = (uint64_t *)src;

        _mem8(p++) = v2;
        _mem8(p++) = v2;

        src += FDEC_STRIDE;
    }
}

static void predict_16x16_v_c64( uint8_t *src )
{
    uint64_t v0 = _mem8_const(&src[0 - FDEC_STRIDE]);
    uint64_t v1 = _mem8_const(&src[8 - FDEC_STRIDE]);
    int i;

    for( i = 0; i < 16; i++ )
    {
        uint64_t *p = (uint64_t*)src;
        _mem8(p++) = v0;
        _mem8(p++) = v1;
        src += FDEC_STRIDE;
    }
}

static void predict_16x16_p_c64( uint8_t *src )
{
    int x, y, i;
    int a, b, c;
    int H = 0;
    int V = 0;
    int i00;
    int VH;
    int m5 = 0x00050005;
    int m7 = 0x00070007;
    int a32 = 0x00200020;
    uint64_t mVH;

    /* calculate H and V */
    for( i = 0; i <= 7; i++ )
    {
        H += ( i + 1 ) * ( src[ 8 + i - FDEC_STRIDE ] - src[6 -i -FDEC_STRIDE] );
        V += ( i + 1 ) * ( src[-1 + (8+i)*FDEC_STRIDE] - src[-1 + (6-i)*FDEC_STRIDE] );
    }

    a = 16 * ( src[-1 + 15*FDEC_STRIDE] + src[15 - FDEC_STRIDE] );
    VH = _pack2(V, H);
    mVH = _mpy2ll(VH, m5);
    VH = _pack2(_hill(mVH), _loll(mVH));
    VH = _shr2(_add2(VH, a32), 6);
    b = (int16_t)VH;
    c = VH >> 16;

    i00 = a + 16 - _dotp2(VH, m7);

    for( y = 0; y < 16; y++ )
    {
        int pix = i00;
        for( x = 0; x < 16; x += 4 )
        {
            const int p10 = _pack2(pix + b, pix);
            const int p32 = _add2(p10, _pack2(b << 1, b << 1));
            _mem4(&src[x]) = _spacku4(_shr2(p32, 5), _shr2(p10, 5));
            pix += b << 2;
        }
        src += FDEC_STRIDE;
        i00 += c;
    }
}

/****************************************************************************
 * Exported functions:
 ****************************************************************************/
void x264_predict_16x16_init_c64( x264_predict_t pf[7] )
{
    pf[I_PRED_16x16_V ]      = predict_16x16_v_c64;
    pf[I_PRED_16x16_H ]      = predict_16x16_h_c64;
    pf[I_PRED_16x16_DC]      = predict_16x16_dc_c64;
    pf[I_PRED_16x16_P ]      = predict_16x16_p_c64;
    pf[I_PRED_16x16_DC_LEFT] = predict_16x16_dc_left_c64;
    pf[I_PRED_16x16_DC_TOP ] = predict_16x16_dc_top_c64;
    pf[I_PRED_16x16_DC_128 ] = predict_16x16_dc_128_c64;
}


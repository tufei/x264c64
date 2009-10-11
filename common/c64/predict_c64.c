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

/****************************************************************************
 * Exported functions:
 ****************************************************************************/
void x264_predict_16x16_init_c64( x264_predict_t pf[7] )
{
#if 0
    pf[I_PRED_16x16_V ]      = predict_16x16_v_altivec;
    pf[I_PRED_16x16_H ]      = predict_16x16_h_altivec;
#endif
    pf[I_PRED_16x16_DC]      = predict_16x16_dc_c64;
#if 0
    pf[I_PRED_16x16_P ]      = predict_16x16_p_altivec;
    pf[I_PRED_16x16_DC_LEFT] = predict_16x16_dc_left_altivec;
    pf[I_PRED_16x16_DC_TOP ] = predict_16x16_dc_top_altivec;
    pf[I_PRED_16x16_DC_128 ] = predict_16x16_dc_128_altivec;
#endif
}


/*****************************************************************************
 * align_check.c: h264 data alignment checking facility for TI C6x compiler
 *****************************************************************************
 * Copyright (C) 2003-2008 x264 project
 *
 * Authors: Laurent Aimar <fenrir@via.ecp.fr>
 *          Loren Merritt <lorenm@u.washington.edu>
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

#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include "align_check.h"
#include "x264.h"
#include "common.h"
#include "me.h"
#include "cabac.h"

/****************************************************************************
 * x264_init_align_check:
 ****************************************************************************/
void x264_init_align_check(void)
{
    /*
     * this macro only checks the alignment of member in a structure
     */
    assert(X264_ALIGN_CHECK(x264_me_t, mvp, 4));
    assert(X264_ALIGN_CHECK(x264_me_t, mv, 4));

    assert(X264_ALIGN_CHECK(x264_cabac_t, f8_bits_encoded, 16));
    assert(X264_ALIGN_CHECK(struct x264_t, nr_residual_sum, 16));
    assert(X264_ALIGN_CHECK(struct x264_t, nr_offset, 16));
    assert(X264_ALIGN_CHECK_UNNAMED(struct x264_t, dct, luma16x16_dc, 16));
    assert(X264_ALIGN_CHECK_UNNAMED(struct x264_t, dct, chroma_dc, 16));
    assert(X264_ALIGN_CHECK_UNNAMED(struct x264_t, dct, luma8x8, 16));
    assert(X264_ALIGN_CHECK_UNNAMED(struct x264_t, dct, luma4x4, 16));

    assert(X264_ALIGN_CHECK_UNNAMED(struct x264_t, mb, i_sub_partition, 4));

    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, pic, fenc_buf, 16));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, pic, fdec_buf, 16));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, pic, i4x4_fdec_buf, 16));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, pic, i8x8_fdec_buf, 16));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, pic, i8x8_dct_buf, 16));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, pic, i4x4_dct_buf, 16));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, pic, fenc_dct8, 16));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, pic, fenc_dct4, 16));

    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, cache, ref, 4));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, cache, mv, 16));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, cache, mvd, 8));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, cache, skip, 4));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, cache, direct_mv, 16));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, cache, direct_ref, 4));
    assert(X264_ALIGN_CHECK_UNNAMED1(struct x264_t, mb, cache, pskip_mv, 4));
}


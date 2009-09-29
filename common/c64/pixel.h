/*****************************************************************************
 * pixel.h: h264 encoder library
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

#ifndef X264_C64_PIXEL_H
#define X264_C64_PIXEL_H

#define DECL_PIXELS( ret, name, suffix, args ) \
    ret x264_pixel_##name##_16x16_##suffix args;\
    ret x264_pixel_##name##_16x8_##suffix args;\
    ret x264_pixel_##name##_8x16_##suffix args;\
    ret x264_pixel_##name##_8x8_##suffix args;\
    ret x264_pixel_##name##_8x4_##suffix args;\
    ret x264_pixel_##name##_4x8_##suffix args;\
    ret x264_pixel_##name##_4x4_##suffix args;\

#define DECL_X1(name, suffix) \
    DECL_PIXELS(int, name, suffix, (uint8_t *, int, uint8_t *, int))

DECL_X1(sad, c64)
DECL_X1(satd, c64)
DECL_X1(ssd, c64)

#undef DECL_PIXELS
#undef DECL_X1

int x264_pixel_var_16x16_c64( uint8_t *pix, int i_stride );
int x264_pixel_var_8x8_c64( uint8_t *pix, int i_stride );

int x264_pixel_sa8d_16x16_c64( uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 );
int x264_pixel_sa8d_8x8_c64( uint8_t *pix1, int i_pix1, uint8_t *pix2, int i_pix2 );
#endif /* X264_C64_PIXEL_H */


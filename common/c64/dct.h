/*****************************************************************************
 * dct.h: h264 encoder library
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

#ifndef X264_C64_DCT_H
#define X264_C64_DCT_H

void x264_sub4x4_dct_c64( int16_t dct[4][4], uint8_t *pix1, uint8_t *pix2 );
void x264_sub8x8_dct_c64( int16_t dct[4][4][4], uint8_t *pix1, uint8_t *pix2 );
void x264_sub16x16_dct_c64( int16_t dct[16][4][4], uint8_t *pix1, uint8_t *pix2 );
void x264_dct4x4dc_c64( int16_t d[4][4] );
void x264_idct4x4dc_c64( int16_t d[4][4] );

void x264_sub8x8_dct_dc_c64( int16_t dct[2][2], uint8_t *pix1, uint8_t *pix2 );
#endif /* X264_C64_DCT_H */


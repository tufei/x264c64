/*****************************************************************************
 * predict.h: h264 encoder library
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

#ifndef X264_C64_PREDICT_H
#define X264_C64_PREDICT_H

void x264_predict_16x16_init_c64( x264_predict_t pf[7] );
void x264_predict_8x8c_init_c64( x264_predict_t pf[7] );

#endif /* X264_C64_PREDICT_H */
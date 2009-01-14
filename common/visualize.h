/*****************************************************************************
 * x264: h264 encoder
 *****************************************************************************
 * Copyright (C) 2005 Tuukka Toivonen <tuukkat@ee.oulu.fi>
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

#ifndef X264_VISUALIZE_H
#define X264_VISUALIZE_H

#ifndef _TMS320C6400
#include "common/common.h"
#endif

#if VISUALIZE
void x264_visualize_init( x264_t *h );
void x264_visualize_mb( x264_t *h );
void x264_visualize_show( x264_t *h );
void x264_visualize_close( x264_t *h );
#endif /* VISUALIZE */

#endif

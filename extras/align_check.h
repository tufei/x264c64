/*****************************************************************************
 * align_check.h: h264 data alignment checking facility for TI C6x compiler
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

#ifndef X264_ALIGN_CHECK_H
#define X264_ALIGN_CHECK_H

/* n has to be a power of 2 */
#define offsetofof(_type, _ident1, _ident2) ((size_t)__intaddr__(&(((_type *)0)->_ident1._ident2)))
#define offsetofofof(_type, _ident1, _ident2, _ident3) ((size_t)__intaddr__(&(((_type *)0)->_ident1._ident2._ident3)))
#define X264_ALIGN_CHECK(s, m, n) ((offsetof(s, m) & (n - 1)) == 0)
#define X264_ALIGN_CHECK_UNNAMED(s, m, mm, n) ((offsetofof(s, m, mm) & (n - 1)) == 0)
#define X264_ALIGN_CHECK_UNNAMED1(s, m, mm, mmm, n) ((offsetofofof(s, m, mm, mmm) & (n - 1)) == 0)

void x264_init_align_check(void);

#endif /* X264_ALIGN_CHECK_H */


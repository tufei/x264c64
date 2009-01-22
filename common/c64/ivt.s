; *****************************************************************************
; * ivt.s: h264 encoder library
; *****************************************************************************
; * Copyright (C) 2003-2008 x264 project
; *
; * Author: Yufei Yuan <yyuan@gmx.com>
; *
; * This program is free software; you can redistribute it and/or modify
; * it under the terms of the GNU General Public License as published by
; * the Free Software Foundation; either version 2 of the License, or
; * (at your option) any later version.
; *
; * This program is distributed in the hope that it will be useful,
; * but WITHOUT ANY WARRANTY; without even the implied warranty of
; * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; * GNU General Public License for more details.
; *
; * You should have received a copy of the GNU General Public License
; * along with this program; if not, write to the Free Software
; * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
; *****************************************************************************

                .ref        _c_int00
                .ref        _c64_timer_isr

                .asg        a15, fp
                .asg        b14, dp
                .asg        b15, sp
          
set_vector
                .macro		addr
                stw			b0, *sp--
||              mvkl		addr, b0
                mvkh		addr, b0
                b			b0
                ldw			*++sp, b0
                nop			2
                nop
                nop
				.endm

_null_interrupt:
                b           irp
                nop         5

				.sect		".vector"
				.align		1024

                set_vector  _c_int00
                set_vector  _null_interrupt
                set_vector  _null_interrupt
                set_vector  _null_interrupt

                set_vector  _null_interrupt
                set_vector  _null_interrupt
                set_vector  _null_interrupt
                set_vector  _null_interrupt

                set_vector  _null_interrupt
                set_vector  _null_interrupt
                set_vector  _null_interrupt
                set_vector  _null_interrupt

                set_vector  _null_interrupt
                set_vector  _null_interrupt
                set_vector  _null_interrupt
                set_vector  _c64_timer_isr

;------------------------------------------------------------------------------


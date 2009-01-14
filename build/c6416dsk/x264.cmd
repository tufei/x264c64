/*****************************************************************************
 * x264.cmd: hardware configuration for h264 encoder testing program on C6x
 *****************************************************************************
 * Copyright (C) 2003-2008 x264 project
 *
 * Authors: Loren Merritt <lorenm@u.washington.edu>
 *          Laurent Aimar <fenrir@via.ecp.fr>
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

-a
--args 0x20 /* nominal size, will be replaced in the program */
-stack 0x10000
-heap 0xA00000

MEMORY
{
	IST:        origin = 0x00000000, len = 0x00000200 /* interrupt service table */
	ISRAM:      origin = 0x00000200, len = 0x000BFE00 /* C6416 RAM */
    CACHE_L2:   origin = 0x000C0000, len = 0x00040000
	FLASH:      origin = 0x64000000, len = 0x00080000
	SDRAM:      origin = 0x80000000, len = 0x01000000
}

SECTIONS
{
	.text:		> ISRAM
	.cinit:		> SDRAM
	.data:		> ISRAM
	.cio:		> SDRAM
    .args       > SDRAM
	.bss:		> ISRAM
	.far:		> SDRAM
	.stack:		> ISRAM
	.const:		> ISRAM
	.pinit:		> ISRAM
	.switch:	> ISRAM
	.sysmem:	> SDRAM

    .bsp_text:  { -lrts6400.lib(.text) -lcsl6416.lib(.text) } > SDRAM
    .bsp_bss:   { -lrts6400.lib(.bss) -lrts6400.lib(.far) -lcsl6416.lib(.bss) -lcsl6416.lib(.far) } > SDRAM
    .bsp_data:  { -lrts6400.lib(.const) -lcsl6416.lib(.const) } > SDRAM
}                        


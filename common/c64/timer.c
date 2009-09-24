/*****************************************************************************
 * timer.c: h264 encoder library
 *****************************************************************************
 * Copyright (C) 2003-2008 x264 project
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

#include <stdio.h>
#include <stdlib.h>
#include <csl.h>
#include <csl_irq.h>
#include <csl_timer.h>
#include "timer.h"

uint32_t profile_call_count = 0;
uint64_t profile_overhead = 0;
uint64_t profile_cycle_count = 0;

#pragma DATA_ALIGN(cycle_counter, 8);
static volatile near uint32_t cycle_counter[2];
static TIMER_Handle h;

uint64_t c64_timer_read(void) 
{
    uint64_t cycles;

    cycle_counter[0] = TIMER_getCount(h);
    cycles = *(uint64_t *)cycle_counter;

    /* sampling the timer twice to check timer wraparound */
    if(TIMER_getCount(h) > cycle_counter[0]) 
    {
        return cycles << 3;
    }
    else 
    {
        cycle_counter[0] = TIMER_getCount(h);
        return *(uint64_t *)cycle_counter << 3;   /* the timer clock is CPU_clock/8 */
    }
}

int c64_timer_init(void) 
{
    *(uint64_t *)cycle_counter = 0;

    IRQ_resetAll();

    /* note that interrupt vector 15 is set up in ivt.s */
    IRQ_map(IRQ_EVT_TINT2, 15);

    /* initialize timer */
    h = TIMER_open(TIMER_DEV2, TIMER_OPEN_RESET);
    if(h == NULL) 
    {
        fprintf(stderr, "x264 [error]: error open timer device\n");
        return -1;
    }
    TIMER_configArgs(h, 0x00000200U, 0xFFFFFFFFU, 0x00000000U);

    IRQ_globalEnable();
    IRQ_nmiEnable();
    IRQ_enable(IRQ_EVT_TINT2);
    return 0;
}

void c64_timer_reset(void) 
{
    *(uint64_t *)cycle_counter = 0;
    TIMER_setCount(h, 0);
}

void c64_timer_go(void) 
{
    if(*(uint64_t *)cycle_counter) 
    {
        TIMER_resume(h);
    }
    else 
    {
        TIMER_start(h);
    }
}

void c64_timer_hold(void) 
{
    TIMER_pause(h);
    cycle_counter[0] = TIMER_getCount(h);
}

void c64_timer_close(void) 
{
    TIMER_close(h);
}

interrupt void c64_timer_isr(void) 
{
    cycle_counter[1] += 1;
}


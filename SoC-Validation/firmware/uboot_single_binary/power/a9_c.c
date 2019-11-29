/*
 * Copyright (C) 2008-2010 ARM Limited                           
 *
 * This software is provided 'as-is', without any express or implied
 * warranties including the implied warranties of satisfactory quality, 
 * fitness for purpose or non infringement.  In no event will  ARM be 
 * liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use, copy and modify this software for 
 * any purpose, and to redistribute the software, subject to the following 
 * restrictions: 
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.                                       
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * ----------------------------------------------------------------
 * File:     a9_c.c
 * ----------------------------------------------------------------
 */

#include "appf_types.h"
#include "appf_internals.h"
#include "appf_helpers.h"

typedef struct
{
    /* 0x00 */ volatile unsigned timer_load;
    /* 0x04 */ volatile unsigned timer_counter;
    /* 0x08 */ volatile unsigned timer_control;
    /* 0x0c */ volatile unsigned timer_interrupt_status;
                char padding1[0x10];
    /* 0x20 */ volatile unsigned watchdog_load;
    /* 0x24 */ volatile unsigned watchdog_counter;
    /* 0x28 */ volatile unsigned watchdog_control;
    /* 0x2c */ volatile unsigned watchdog_interrupt_status;
    /* 0x30 */ volatile unsigned watchdog_reset_status;
    /* 0x34 */ volatile unsigned watchdog_disable;
} a9_timer_registers;

typedef struct
{
    unsigned timer_load;
    unsigned timer_counter;
    unsigned timer_control;
    unsigned timer_interrupt_status;
    unsigned watchdog_load;
    unsigned watchdog_counter;
    unsigned watchdog_control;
    unsigned watchdog_interrupt_status;
} a9_timer_context;

#define A9_TIMERS_OFFSET 0x600

void save_a9_timers(appf_u32 *pointer, unsigned scu_address)
{
    a9_timer_context *context = (a9_timer_context *)pointer;
    a9_timer_registers *timers = (a9_timer_registers *)(scu_address + A9_TIMERS_OFFSET);

    /* 
     * First, stop the timers
     */
    context->timer_control    = timers->timer_control;
    timers->timer_control     = 0;
    context->watchdog_control = timers->watchdog_control;
    timers->watchdog_control  = 0;

    context->timer_load                = timers->timer_load;
    context->timer_counter             = timers->timer_counter;
    context->timer_interrupt_status    = timers->timer_interrupt_status;
    context->watchdog_load             = timers->watchdog_load;
    context->watchdog_counter          = timers->watchdog_counter;
    context->watchdog_interrupt_status = timers->watchdog_interrupt_status;
    /* 
     * We ignore watchdog_reset_status, since it can only clear the status bit.
     * If the watchdog has reset the system, the OS will want to know about it.
     * Similarly, we have no use for watchdog_disable - this is only used for
     * returning to timer mode, which is the default mode after reset.
     */
}

void restore_a9_timers(appf_u32 *pointer, unsigned scu_address)
{
    a9_timer_context *context = (a9_timer_context *)pointer;
    a9_timer_registers *timers = (a9_timer_registers *)(scu_address + A9_TIMERS_OFFSET);

    timers->timer_control = 0;
    timers->watchdog_control = 0;

    /*
     * We restore the load register first, because it also sets the counter register.
     */
    timers->timer_load    = context->timer_load;
    timers->watchdog_load = context->watchdog_load;

    /*
     * If a timer has reached zero (presumably during the context save) and triggered
     * an interrupt, then we set it to the shortest possible expiry time, to make it
     * trigger again real soon.
     * We could fake this up properly, but we would have to wait around until the timer 
     * ticked, which could be some time if PERIPHCLK is slow. This approach should be 
     * good enough in most cases.
     */
    if (context->timer_interrupt_status)
    {
        timers->timer_counter = 1;
    }
    else
    {
        timers->timer_counter = context->timer_counter;
    }

    if (context->watchdog_interrupt_status)
    {
        timers->watchdog_counter = 1;
    }
    else
    {
        timers->watchdog_counter = context->watchdog_counter;
    }

    timers->timer_control = context->timer_control;
    timers->watchdog_control = context->watchdog_control;
}

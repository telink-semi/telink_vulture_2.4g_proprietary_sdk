/********************************************************************************************************
 * @file	timer_event.c
 *
 * @brief	This is the source file for 8355
 *
 * @author	2.4G Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#include "timer_event.h"
#include "common.h"

ev_time_event_t timer_list[10];

static int inline ev_is_timer_expired(ev_time_event_t *e, u32 now)
{
    assert(e);
    return ((u32)(now - e->t) < (TIMER_SAFE_MARGIN_US*sys_tick_per_us));
}


__attribute__((section(".ram_code")))void ev_start_timer(ev_time_event_t * e)
{  
    assert(e);

    u8 r = irq_disable();

    u32 now = ClockTime();
    u32 t = now + e->interval;    // becare of overflow

    e->t = t;
    e->valid = 1;
   
    irq_restore(r);
}

__attribute__((section(".ram_code")))void ev_cancel_timer(ev_time_event_t * e)
{
    assert(e);

    u8 r = irq_disable();
    e->valid = 0;
    e->busy = 0;
    irq_restore(r);
}

__attribute__((section(".ram_code")))ev_time_event_t *ev_on_timer(ev_timer_callback_t cb, void *data, u32 t_us)
{
    int i;
    ev_time_event_t *e = NULL;

    assert(cb);

    for (i = 0; i < LengthOfArray(timer_list); i++)
    {
        if (timer_list[i].busy == 0)
        {
            timer_list[i].busy = 1;
            e = timer_list + i;
            break;
        }
    }
    if (e == NULL) {
        while(1);
    }
    e->interval = t_us * sys_tick_per_us;
    e->cb = cb;
    e->data = data;
    ev_start_timer(e);
    return e;
}

__attribute__((section(".ram_code")))void ev_unon_timer(ev_time_event_t ** e)
{
    assert(e && (*e));
    
    ev_cancel_timer(*e);
    *e = NULL;
}


/* Process time events */
__attribute__((section(".ram_code")))void ev_process_timer()
{
    u32 now = ClockTime();
    ev_time_event_t *te;
    for (te = timer_list; te < timer_list + LengthOfArray(timer_list); te++) {
        if ((!is_timer_expired(te)) && ev_is_timer_expired(te, now))
        {
            int t;
            if ((unsigned int)(te->cb) < 0x100 || (unsigned int)(te->cb) > 0x20000) {
                while(1);
            }
            t = te->cb(te->data);
            if(t < 0) {
                ev_cancel_timer(te);        // delete timer
            }
            else if(0 == t) {
                te->t = now + te->interval;    // becare of overflow
            }
            else {
                te->interval = t * sys_tick_per_us;
                te->t = now + te->interval;    // becare of overflow
            }
        }
    }
}

__attribute__((section(".ram_code")))int is_timer_expired(ev_time_event_t *e)
{
    if ( e == NULL ) {
        return TRUE;
    }
    if (e->valid == 1) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

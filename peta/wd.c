/*
 * Copyright (C) 2015 Hauke Petersen <mail@haukepetersen.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @ingroup     peta
 * @{
 *
 * @file
 * @brief       High priority task that makes sure everything toes smoothly
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "thread.h"
#include "xtimer.h"
#include "periph/pwm.h"

#include "wd.h"
#include "brain.h"
#include "peta_config.h"


static char wd_stack[THREAD_STACKSIZE_DEFAULT];

static int active = 1;
static volatile int pkt_count = 0;


static void _shutdown(void)
{
    brain_set_speed(0);
    /* HACK */
    pwm_set(PWM_1, 1, 0);
}

static void _be_happy(void)
{
    /* HACK */
    pwm_set(PWM_1, 1, 10000);
}

static void *_thread(void *arg)
{
    (void)arg;

    while (1) {
        xtimer_usleep(CONF_WD_INTERVAL);
        if (active && pkt_count < CONF_WD_THRESSHOLD) {
            _shutdown();
        }
        else {
            _be_happy();
        }
        pkt_count = 0;
    }

    /* never reached */
    return NULL;
}

void wd_init(void)
{
    /* start the thread */
    thread_create(wd_stack, sizeof(wd_stack), CONF_WD_PRIO, CREATE_STACKTEST,
                  _thread, NULL, "wd");
}

void wd_report(void)
{
    ++pkt_count;
}

void wd_activate(int state)
{
    active = state;
}

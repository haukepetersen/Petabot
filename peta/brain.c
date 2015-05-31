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
 * @brief       The brain controls everything
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "kernel.h"
#include "thread.h"
#include "msg.h"
#include "periph/gpio.h"
#include "periph/pwm.h"
#include "servo.h"
#include "net/ng_netbase.h"

#include "brain.h"
#include "comm.h"
#include "wd.h"
#include "peta_config.h"

#define STACKSIZE           (THREAD_STACKSIZE_MAIN)
#define STACKPRIO           (THREAD_PRIORITY_MAIN - 2)

static char stack[STACKSIZE];

static servo_t steering;


static void _dispatch(uint8_t *data, size_t len)
{
    int16_t speed, dir;

    if (data[0] == COMM_MSG_CTRL) {
        for (int i = 0; i < 5; i++) {
            printf("0x%02x ", data[i]);
        }
        printf("\n");
        memcpy(&speed, &(data[1]), 2);
        memcpy(&dir, &(data[3]), 2);
        printf("got CTRL - speed: %i, dir: %i\n", speed, dir);
        brain_set_speed(speed);
        brain_steer(dir);
    } else {
        puts("unknown data");
    }
}

static void *_brain_thread(void *arg)
{
    ng_netreg_entry_t netreg;
    ng_pktsnip_t *snip;
    msg_t msg;

    netreg.pid = thread_getpid();
    netreg.demux_ctx = NG_NETREG_DEMUX_CTX_ALL;
    ng_netreg_register(NG_NETTYPE_UNDEF, &netreg);

    while (1) {
        msg_receive(&msg);

        if (msg.type == NG_NETAPI_MSG_TYPE_RCV) {
            snip = (ng_pktsnip_t *)msg.content.ptr;
            _dispatch(snip->data, snip->size);
            wd_report();
            ng_pktbuf_release(snip);
        }
    }

    /* never reached */
    return NULL;
}

void brain_init(void)
{
    /* initialize the steering */
    puts("init servo");
    if (servo_init(&steering, CONF_STEERING_PWM, CONF_STEERING_PWM_CHAN,
                   CONF_STEERING_MIN, CONF_STEERING_MAX) < 0) {
        puts("ERROR initializing the STEERING\n");
        return;
    }
    /* initialize motor control */
    puts("init motor");
    gpio_init_out(CONF_MOTOR_DIRA, GPIO_NOPULL);
    gpio_init_out(CONF_MOTOR_DIRB, GPIO_NOPULL);
    if (pwm_init(CONF_MOTOR_PWM, CONF_MOTOR_PWM_CHAN,
                 CONF_MOTOR_FREQ, CONF_MOTOR_RES) < 0) {
        puts("ERROR initializing the DRIVE PWM\n");
        return;
    }
    pwm_set(CONF_MOTOR_PWM, CONF_MOTOR_PWM_CHAN, 0);
    /* initialize the software watchdog */
    wd_init();
    /* initializing network support */
    puts("init comm");
    comm_init();
    /* run brain thread */
    puts("run the brain");
    thread_create(stack, STACKSIZE, STACKPRIO, CREATE_STACKTEST, _brain_thread,
                  NULL, "brain");
}

void brain_set_speed(int16_t speed)
{
    if (speed > 0) {
        gpio_set(CONF_MOTOR_DIRA);
        gpio_clear(CONF_MOTOR_DIRB);
    } else {
        gpio_clear(CONF_MOTOR_DIRA);
        gpio_set(CONF_MOTOR_DIRB);
        speed *= -1;
    }
    pwm_set(CONF_MOTOR_PWM, CONF_MOTOR_PWM_CHAN, speed);
}

void brain_steer(int16_t dir)
{
    printf("seteering to %i\n", dir);
    dir = (dir / 2) + CONF_STEERING_CENTER;
    if (dir < 0) {
        dir = CONF_STEERING_MIN;
    }
    servo_set(&steering, (unsigned int)dir);
    puts("done steering");
}

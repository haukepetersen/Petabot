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

#include "thread.h"
#include "msg.h"
#include "periph/gpio.h"
#include "periph/pwm.h"
#include "servo.h"
#include "net/gnrc.h"

#include "brain.h"
#include "comm.h"
#include "wd.h"
#include "peta_config.h"

#define STACKSIZE           (THREAD_STACKSIZE_MAIN)
#define STACKPRIO           (THREAD_PRIORITY_MAIN - 2)

static char stack[STACKSIZE];

static servo_t steering;

#define BRAIN_QUEUE_SIZE     (8)
static msg_t brain_msg_queue[BRAIN_QUEUE_SIZE];

static void dispatch(uint8_t *data, size_t len)
{
    int16_t speed, dir;

    printf("got data: ");

    if (data[0] == COMM_MSG_CTRL && len == COMM_MSG_LEN) {
        memcpy(&speed, &(data[1]), 2);
        memcpy(&dir, &(data[3]), 2);
        brain_set_speed(speed);
        brain_steer(dir);
        brain_switches(data[5]);
        wd_report();
        printf("speed: %i, steer: %i", (int)speed, (int)dir);
    } else {
        // puts("unknown data");
    }
    puts("");
}

static void *brain_thread(void *arg)
{
    gnrc_netreg_entry_t netreg;
    gnrc_pktsnip_t *snip;
    msg_t msg;

    /* A message que is mandatory for a thread that registers at netreg */
    msg_init_queue(brain_msg_queue, BRAIN_QUEUE_SIZE);

    netreg.target.pid = thread_getpid();
    netreg.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &netreg);

    while (1) {
        msg_receive(&msg);

        if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
            snip = (gnrc_pktsnip_t *)msg.content.ptr;
            dispatch(snip->data, snip->size);
            gnrc_pktbuf_release(snip);
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
    servo_set(&steering, CONF_STEERING_CENTER);
    /* initialize motor control */
    puts("init motor");
    gpio_init(CONF_MOTOR_DIRA, GPIO_OUT);
    gpio_init(CONF_MOTOR_DIRB, GPIO_OUT);
    if (pwm_init(CONF_MOTOR_PWM, CONF_MOTOR_PWM_CHAN,
                 CONF_MOTOR_FREQ, CONF_MOTOR_RES) < 0) {
        puts("ERROR initializing the DRIVE PWM\n");
        return;
    }
    pwm_set(CONF_MOTOR_PWM, CONF_MOTOR_PWM_CHAN, 0);
    /* initialize switches */
    gpio_init(CONF_DISCO_PIN, GPIO_OUT);
    /* initialize the software watchdog */
    wd_init();
    /* initializing network support */
    puts("init comm");
    comm_init();
    /* run brain thread */
    puts("run the brain");
    thread_create(stack, STACKSIZE, STACKPRIO, 0, brain_thread,
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
    dir = (dir / 2) + CONF_STEERING_CENTER;
    if (dir < 0) {
        dir = CONF_STEERING_MIN;
    }
    servo_set(&steering, (unsigned int)dir);
}

void brain_switches(uint8_t state)
{
    if (state & CONF_DISCO_SWITCH) {
        gpio_set(CONF_DISCO_PIN);
    }
    else {
        gpio_clear(CONF_DISCO_PIN);
    }
}

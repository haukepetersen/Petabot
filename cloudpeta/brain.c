/*
 * Copyright (C) 2015 Hauke Petersen <devel@haukepetersen.de>
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
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/pwm.h"
#include "servo.h"
#include "srf02.h"
#include "xtimer.h"
#include "net/gnrc.h"

#include "brain.h"
#include "comm.h"
#include "motor.h"
#include "peta_config.h"

#define FILTER_SIZE         (3)

static char stack[CONF_STACKSIZE];

static const motor_params_t mot_params = {
    .pwm   = CONF_MOTOR_PWM_DEV,
    .chan  = CONF_MOTOR_PWM_CHAN,
    .pin_a = CONF_MOTOR_DIRA,
    .pin_b = CONF_MOTOR_DIRB
};

/**
 * @brief   Allocation of device descriptors
 * @{
 */
static servo_t steering;
static motor_t mot;
static srf02_t dist_front;
static srf02_t dist_back;
/** @} */

/**
 * @brief   Distance filter state
 * @{
 */
static uint16_t front_filter[FILTER_SIZE];
static uint16_t back_filter[FILTER_SIZE];
static int filter_pos = 0;
/** @} */

/**
 * @brief   Movement state
 * @{
 */
int behavior = 0;
int front_blocked = 1;
int back_blocked = 1;
int16_t speed = 0;
int16_t dir = 0;
/** @} */

/**
 * @brief   State:
 *
 * 0 := going forward straight until obstacle in front
 * 1 := going backwards, turning hard until obstacle in back
 */
static int state = 0;

static void event(int evt)
{
    switch (evt) {
        case EVT_FRONT_BLOCKED:
            puts("front blocked");
            break;
        case EVT_BACK_BLOCKED:
            puts("back blocked");
            break;
        case EVT_FRONT_FREE:
            puts("front free");
            break;
        case EVT_BACK_FREE:
            puts("back free");
            break;
        default:
            puts("unkown");
    }

    /* start condition */
    if ((state == 0) && (evt == EVT_FRONT_FREE)) {
        puts("evt: start");
        speed = CONF_BRAIN_SPEED;
        motor_set(&mot, speed);
    }

    /* going forward, hitting obstacle -> steer full and go backwards */
    else if ((state == 0) && (evt == EVT_FRONT_BLOCKED)) {
        motor_stop(&mot);

        puts("evt: hit forward obstacle");

        dir = (behavior == 0) ? -1000 : 1000;
        speed = -CONF_BRAIN_SPEED;
        brain_steer(dir);
        motor_set(&mot, speed);
        state = 1;
    }

    /* going backwards, hitting obstacle */
    else if ((state == 1) && (evt == EVT_BACK_BLOCKED)) {
        motor_stop(&mot);

        puts("evt: hit backwards obstacle");

        dir = 0;
        speed = CONF_BRAIN_SPEED;
        brain_steer(dir);
        motor_set(&mot, speed);
        state = 0;
    }
}

static void *colllision_detection(void *arg)
{
    uint32_t lw = xtimer_now();

    while (1) {
        /* trigger sensor reading */
        srf02_trigger(&dist_front, SRF02_MODE_REAL_CM);
        srf02_trigger(&dist_back, SRF02_MODE_REAL_CM);
        /* wait for results */
        xtimer_usleep(SRF02_RANGE_DELAY);
        /* read distance data */
        front_filter[filter_pos] = srf02_read(&dist_front);
        xtimer_usleep(1);   /* hack, otherwise the 2nd srf02_read fucks up */
        back_filter[filter_pos] = srf02_read(&dist_back);
        // printf(" f: %3i,  b: %3i  %i\n", (int)front_filter[filter_pos], (int)back_filter[filter_pos], filter_pos);
        filter_pos = (++filter_pos >= FILTER_SIZE) ? 0 : filter_pos;

        /* analyze data and trigger events base on it */
        uint16_t fd = 0;
        uint16_t bd = 0;
        for (int i = 0; i < FILTER_SIZE; i++) {
            fd += front_filter[i];
            bd += back_filter[i];
        }
        if ((fd < (FILTER_SIZE * CONF_DIST_THOLD)) && (front_blocked == 0)) {
            front_blocked = 1;
            event(EVT_FRONT_BLOCKED);
        }
        else if ((fd >= (FILTER_SIZE * CONF_DIST_THOLD)) && (front_blocked == 1)) {
            front_blocked = 0;
            event(EVT_FRONT_FREE);
        }
        if ((bd < (FILTER_SIZE * CONF_DIST_THOLD)) && (back_blocked == 0)) {
            back_blocked = 1;
            event(EVT_BACK_BLOCKED);
        }
        else if ((bd >= (FILTER_SIZE * CONF_DIST_THOLD)) && (back_blocked == 1)) {
            back_blocked = 0;
            event(EVT_BACK_FREE);
        }

        xtimer_usleep_until(&lw, CONF_DIST_SENSE_DELAY);
    }

    return NULL;
}

void brain_run(void)
{
    /* initialize components */
    puts("[brain] initializing front distance sensor...");
    if (srf02_init(&dist_front, CONF_DIST_FRONT_I2C, CONF_DIST_FRONT_ADDR) < 0) {
        puts("[failed]");
        return;
    }

    puts("[brain] initializing back distance sensor...");
    if (srf02_init(&dist_back, CONF_DIST_BACK_I2C, CONF_DIST_BACK_ADDR) < 0) {
        puts("[failed]");
        return;
    }

    puts("[brain] initializing steering servo...");
    if (servo_init(&steering, CONF_STEERING_PWM, CONF_STEERING_PWM_CHAN,
                   CONF_STEERING_MIN, CONF_STEERING_MAX) < 0) {
        puts("[failed]");
        return;
    }
    servo_set(&steering, CONF_STEERING_CENTER);

    puts("[brain] initializing motor driver...");
    if (motor_init(&mot, &mot_params)) {
        puts("[failed]");
        return;
    }

    /* go and have fun */
    puts("[brain] components ready, all up an running!");
    thread_create(stack, sizeof(stack),
                  CONF_PRIO_COL_DETECT, THREAD_CREATE_STACKTEST,
                  colllision_detection, NULL, "col detect");
}


void brain_set_speed(int16_t val)
{
    motor_set(&mot, val);
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

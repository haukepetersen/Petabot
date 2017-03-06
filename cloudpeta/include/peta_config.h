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
 * @brief       Global configuration options
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 */

#ifndef PETA_CONFIG_H
#define PETA_CONFIG_H

#include "periph/gpio.h"
#include "periph/pwm.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Configure the steering servo motor
 * @{
 */
#define CONF_STEERING_PWM       (PWM_1)
#define CONF_STEERING_PWM_CHAN  (0U)
#define CONF_STEERING_RES       (0x03ff)    /* 10-bit resolution */
#define CONF_STEERING_MIN       (1000)
#define CONF_STEERING_CENTER    (1500)
#define CONF_STEERING_MAX       (2000)
/** @} */

/**
 * @brief   Configure the main motor
 * @{
 */
#define CONF_MOTOR_PWM_DEV      (PWM_0)
#define CONF_MOTOR_PWM_CHAN     (0U)
#define CONF_MOTOR_DIRA         GPIO_PIN(PA,13)
#define CONF_MOTOR_DIRB         GPIO_PIN(PA,28)
/** @} */

/**
 * @brief   Distance sensor configuration
 * @{
 */
#define CONF_DIST_FRONT_I2C     (I2C_DEV(0))
#define CONF_DIST_FRONT_ADDR    (224U)
#define CONF_DIST_BACK_I2C      (I2C_DEV(0))
#define CONF_DIST_BACK_ADDR     (226U)
/** @} */

/**
 * @brief   Switch configurations
 * @{
 */
#define CONF_DISCO_SWITCH       (0x01)
#define CONF_DISCO_PIN          GPIO_PIN(PB,03)
#define CONF_DISCO_PARTYTIME    (3000000)
/** @} */

/**
 * @brief    Local control loop configuration
 * @{
 */
#define CONF_DIST_SENSE_DELAY   (100000U)       /* sense every 100ms */
#define CONF_DIST_THOLD         (50)            /* stop on dist this value */
/** @} */

/**
 * @brief   Brain parameters
 * @{
 */
#define CONF_BRAIN_SPEED        (200)
/** @} */

/**
 * @brief   Configure communication
 * @{
 */
#define CONF_COMM_CHAN          (26U)
#define CONF_COMM_UPDATE_DELAY  (250000)
#define CONF_COMM_NODEID        "RIOT-Car"
#define CONF_COMM_HORST_ADDR    "2001:affe:0815::1"
#define CONF_COMM_COAP_PORT     (5683U)
/** @} */

/**
 * @brief   Thread priority configuration
 * @{
 */
#define CONF_STACKSIZE          (THREAD_STACKSIZE_MAIN)
#define CONF_MSGQUEUE_SIZE      (8)
#define CONF_PRIO_COL_DETECT    (1)
#define CONF_PRIO_COAP_SERVER   (3)
#define CONF_PRIO_PUPDATE       (4)
/** @} */

enum
{
    MSG_EVT = 0x1234
};

enum {
    EVT_FRONT_BLOCKED,
    EVT_FRONT_FREE,
    EVT_BACK_BLOCKED,
    EVT_BACK_FREE
};

#ifdef __cplusplus
}
#endif

#endif /* PETA_CONFIG_H*/
/** @} */

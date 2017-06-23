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
#define CONF_STEERING_PWM       (PWM_DEV(1))
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
#define CONF_MOTOR_PWM          (PWM_DEV(0))
#define CONF_MOTOR_PWM_CHAN     (0U)
#define CONF_MOTOR_FREQ         (1000U)
#define CONF_MOTOR_RES          (0x03ff)    /* results in ~10MHz timer freq */
#define CONF_MOTOR_DIRA         GPIO_PIN(PA,13)
#define CONF_MOTOR_DIRB         GPIO_PIN(PA,28)
/** @} */

/**
 * @brief   Switch configurations
 * @{
 */
#define CONF_DISCO_SWITCH       (0x01)
#define CONF_DISCO_PIN          GPIO_PIN(PB,03)
/** @} */

/**
 * @brief   Configure communication
 * @{
 */
#define CONF_COMM_PAN           (0xf0ba)
#define CONF_COMM_ADDR          {0x23, 0x01}
#define CONF_COMM_CHAN          (11U)
/** @} */

/**
 * @brief   Watchdog configuration
 * @{
 */
#define CONF_WD_PRIO            (THREAD_PRIORITY_MAIN - 4)
#define CONF_WD_INTERVAL        (200 * 1000)        /* 200ms */
#define CONF_WD_THRESSHOLD      (3U)                /* at least 3 packets before
                                                     * shutdown */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PETA_CONFIG_H*/
/** @} */

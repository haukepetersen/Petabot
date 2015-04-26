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
#define CONF_MOTOR_PWM          (PWM_0)
#define CONF_MOTOR_PWM_CHAN     (0U)
#define CONF_MOTOR_FREQ         (10000U)
#define CONF_MOTOR_RES          (0x03ff)    /* results in ~10MHz timer freq */
#define CONF_MOTOR_DIRA         (GPIO_3)
#define CONF_MOTOR_DIRB         (GPIO_4)
/** @} */

/**
 * @brief   Configure communication
 * @{
 */
#define CONF_COMM_XBEE_UART     (UART_1)
#define CONF_COMM_XBEE_BR       (9600U)

#define CONF_COMM_PAN           (0x2300)
#define CONF_COMM_ADDR          (0x2301)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PETA_CONFIG_H*/
/** @} */

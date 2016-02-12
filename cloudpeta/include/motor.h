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
 * @brief       Motor control
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#ifndef PETA_MOTOR_H
#define PETA_MOTOR_H

#include "peta_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MOTOR_PWM_FREQ
#define MOTOR_PWM_FREQ          (10000U)
#endif

#ifndef MOTOR_PWM_RES
#define MOTOR_PWM_RES           (1024)
#endif



typedef struct {
    pwm_t pwm;
    int chan;
    gpio_t pin_a;
    gpio_t pin_b;
} motor_t;

typedef motor_t motor_params_t;

int motor_init(motor_t *mot, const motor_params_t *params);

void motor_set(motor_t *mot, int16_t speed);

void motor_stop(motor_t *mot);

#ifdef __cplusplus
}
#endif

#endif /* PETA_MOTOR_H*/
/** @} */

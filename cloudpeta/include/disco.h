/*
 * Copyright (C) 2016 Hauke Petersen <devel@haukepetersen.de>
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
 * @brief       Disco light driver
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 */

#ifndef DISCO_H
#define DISCO_H

#include <stdint.h>

#include "xtimer.h"
#include "periph/gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct {
    gpio_t pin;
    xtimer_t timer;
} disco_t;


typedef struct {
    gpio_t pin;
} disco_params_t;

int disco_init(disco_t *dev, const disco_params_t *params);

void disco_fun(disco_t *dev);

void disco_lame(disco_t *dev);

void disco_party(disco_t *dev, uint32_t dusk_till_dawn);

#ifdef __cplusplus
}
#endif

#endif /* DISCO_H*/
/** @} */

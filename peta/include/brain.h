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
 * @brief       Interfaces of Peta's brain
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 */

#ifndef BRAIN_H_
#define BRAIN_H_

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Initialize Peta and start the brain's thread
 */
void brain_init(void);

void brain_set_speed(int16_t speed);

void brain_steer(int16_t dir);

void brain_switches(uint8_t state);

#ifdef __cplusplus
}
#endif

#endif /* BRAIN_H_*/
/** @} */

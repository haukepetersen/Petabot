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
 * @brief       Interfaces of the communication module
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 */

#ifndef COMM_H_
#define COMM_H_

#ifdef __cplusplus
 extern "C" {
#endif


#define COMM_MSG_CTRL           (0xee)
#define COMM_MSG_LEN            (6U)

void comm_init(void);


#ifdef __cplusplus
}
#endif

#endif /* COMM_H_*/
/** @} */

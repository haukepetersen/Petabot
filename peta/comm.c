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
 * @brief       Communication module
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include <stdio.h>

#include "comm.h"
#include "peta_config.h"

#include "kernel.h"
#include "thread.h"
#include "periph/gpio.h"
#include "xbee.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"


#define IF_STACKSIZE            (KERNEL_CONF_STACKSIZE_DEFAULT)
#define IF_STACKPRIO            (PRIORITY_MAIN - 4)

static char if_stack[IF_STACKSIZE];

static xbee_t xbee;

static kernel_pid_t if_pid;



void comm_init(void)
{
    uint16_t addr = CONF_COMM_ADDR;
    uint16_t pan = CONF_COMM_PAN;

    /* initialize xbee driver */
    puts("comm: init xbee");
    xbee_init(&xbee, CONF_COMM_XBEE_UART, CONF_COMM_XBEE_BR,
              GPIO_NUMOF, GPIO_NUMOF);
    puts("comm: starting nomac");
    if_pid = ng_nomac_init(if_stack, IF_STACKSIZE, IF_STACKPRIO, "xbee",
                           (ng_netdev_t *)(&xbee));
    puts("comm: setting address and PAN");
    ng_netapi_set(if_pid, NETCONF_OPT_ADDRESS, 0, &addr, 2);
    ng_netapi_set(if_pid, NETCONF_OPT_NID, 0, &pan, 2);
}

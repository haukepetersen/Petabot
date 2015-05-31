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

#include "kernel.h"
#include "net/ng_netif.h"
#include "net/ng_netapi.h"

#include "comm.h"
#include "peta_config.h"


void comm_init(void)
{
    kernel_pid_t ifs[NG_NETIF_NUMOF];
    uint8_t addr[2] = CONF_COMM_ADDR;
    uint16_t pan = CONF_COMM_PAN;
    uint16_t chan = CONF_COMM_CHAN;

    /* get the PID of the first radio */
    if (ng_netif_get(ifs) <= 0) {
        puts("comm: ERROR during init, not radio found\n");
        return;
    }

    /* initialize the radio */
    puts("comm: setting address and PAN");
    ng_netapi_set(ifs[0], NETCONF_OPT_ADDRESS, 0, &addr, 2);
    ng_netapi_set(ifs[0], NETCONF_OPT_NID, 0, &pan, 2);
    ng_netapi_set(ifs[0], NETCONF_OPT_CHANNEL, 0, &chan, 2);
}

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
 * @defgroup    remote Remote Control Node
 * @brief       Firmware the the remote controller node
 * @{
 *
 * @file
 * @brief       Bootstrap the communication module and the shell
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "shell.h"
#include "net/gnrc.h"

#include "comm.h"
#include "remote_config.h"

#define SHELL_BUFSIZE           (64U)

static kernel_pid_t if_pid;

static uint8_t peta_addr[2];

static uint8_t ctrl[6];


static int _ctrl(int argc, char **argv)
{
    int16_t speed, dir;
    gnrc_pktsnip_t *pkt;
    gnrc_netif_hdr_t *nethdr;

    if (argc < 4) {
        printf("usage: %s <speed> <dir> <switches>\n", argv[0]);
        puts("       - speed [-1023,1023]\n"
             "       - dir   [[-1023,1023]\n"
             "       - switches: 8-bit bitfield");
        return 1;
    }

    speed = (uint16_t)atoi(argv[1]);
    memcpy(&(ctrl[1]), &speed, 2);
    dir = (uint16_t)atoi(argv[2]);
    memcpy(&(ctrl[3]), &dir, 2);
    ctrl[5] = (uint8_t)atoi(argv[3]);

    printf("CTRL - speed: %i, dir: %i, switches: 0x%02x\n", speed, dir, ctrl[5]);
    for (int i = 0; i < 6; i++) {
        printf("0x%02x ", ctrl[i]);
    }
    printf("\n");

    /* allocate and send packet */
    pkt = gnrc_pktbuf_add(NULL, ctrl, sizeof(ctrl), GNRC_NETTYPE_UNDEF);
    pkt = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_netif_hdr_t) + 2, GNRC_NETTYPE_NETIF);
    nethdr = (gnrc_netif_hdr_t *)pkt->data;
    gnrc_netif_hdr_init(nethdr, 0, 2);
    gnrc_netif_hdr_set_dst_addr(nethdr, peta_addr, 2);
    gnrc_netapi_send(if_pid, pkt);
    return 0;
}

/**
 * @brief   Define some shell commands for testing the brain
 */
static const shell_command_t _commands[] = {
    { "ctrl", "transmit a control message", _ctrl },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    uint8_t addr[2] = CONF_COMM_ADDR;
    uint8_t peta[2] = CONF_COMM_PETA_ADDR;
    uint16_t pan = CONF_COMM_PAN;
    uint16_t chan = CONF_COMM_CHAN;

    ctrl[0] = COMM_MSG_CTRL;
    memcpy(peta_addr, peta, 2);

    /* get network interface PID */
    if (gnrc_netif_get(ifs) <= 0) {
        puts("ERROR: no network interface found");
        return 1;
    }
    if_pid = ifs[0];

    /* bootstrap networking */
    puts("setting address and PAN");
    gnrc_netapi_set(if_pid, NETOPT_ADDRESS, 0, &addr, 2);
    gnrc_netapi_set(if_pid, NETOPT_NID, 0, &pan, 2);
    gnrc_netapi_set(if_pid, NETOPT_CHANNEL, 0, &chan, 2);

    /* run the shell */
    shell_run(_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}

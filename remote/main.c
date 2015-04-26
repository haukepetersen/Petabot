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

#include "comm.h"
#include "remote_config.h"

#include "board_uart0.h"
#include "posix_io.h"
#include "shell.h"
#include "ng_at86rf2xx.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"

#define SHELL_BUFSIZE           (64U)

#define IF_STACKSIZE            (KERNEL_CONF_STACKSIZE_DEFAULT)
#define IF_STACKPRIO            (PRIORITY_MAIN - 4)

static char if_stack[IF_STACKSIZE];

static kernel_pid_t if_pid;

static ng_at86rf2xx_t radio;

static uint8_t peta_addr[2];

static uint8_t ctrl[5];


static int _ctrl(int argc, char **argv)
{
    int16_t speed, dir;
    ng_pktsnip_t *pkt;
    ng_netif_hdr_t *nethdr;

    if (argc < 3) {
        printf("usage: %s SPEED DIR [both: -1023 to 1023]\n", argv[0]);
        return 1;
    }

    speed = (uint16_t)atoi(argv[1]);
    dir = (uint16_t)atoi(argv[2]);
    memcpy(&(ctrl[1]), &speed, 2);
    memcpy(&(ctrl[3]), &dir, 2);
    printf("CTRL - speed: %i, dir: %i\n", speed, dir);
    for (int i = 0; i < 5; i++) {
        printf("0x%02x ", ctrl[i]);
    }
    printf("\n");

    /* allocate and send packet */
    pkt = ng_pktbuf_add(NULL, ctrl, 5, NG_NETTYPE_UNDEF);
    pkt = ng_pktbuf_add(pkt, NULL, sizeof(ng_netif_hdr_t) + 2, NG_NETTYPE_NETIF);
    nethdr = (ng_netif_hdr_t *)pkt->data;
    ng_netif_hdr_init(nethdr, 0, 2);
    ng_netif_hdr_set_dst_addr(nethdr, peta_addr, 2);
    ng_netapi_send(if_pid, pkt);
    return 0;
}

static int _readc(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void _putc(int c)
{
    putchar((char)c);
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
    shell_t shell;
    uint16_t addr = CONF_COMM_ADDR;
    uint16_t pan = CONF_COMM_PAN;
    uint16_t peta = CONF_COMM_PETA_ADDR;
    ctrl[0] = COMM_MSG_CTRL;
    peta_addr[0] = peta >> 8;
    peta_addr[1] = peta & 0xff;

    /* bootstrap networking */
    puts("init radio");
    ng_at86rf2xx_init(&radio, CONF_IOTLAB_RADIO_SPI, CONF_IOTLAB_RADIO_SPI_SPEED,
                      CONF_IOTLAB_RADIO_CS, CONF_IOTLAB_RADIO_INT,
                      CONF_IOTLAB_RADIO_SLEEP, CONF_IOTLAB_RADIO_RESET);
    puts("init MAC");
    if_pid = ng_nomac_init(if_stack, IF_STACKSIZE, IF_STACKPRIO, "radio",
                          (ng_netdev_t *)(&radio));
    puts("setting address and PAN");
    ng_netapi_set(if_pid, NETCONF_OPT_ADDRESS, 0, &addr, 2);
    ng_netapi_set(if_pid, NETCONF_OPT_NID, 0, &pan, 2);

    /* run the shell */
    (void) posix_open(uart0_handler_pid, 0);
    shell_init(&shell, _commands, SHELL_BUFSIZE, _readc, _putc);
    shell_run(&shell);

    /* never reached */
    return 0;
}

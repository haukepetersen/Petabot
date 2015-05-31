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
 * @defgroup    peta Peta Firmware
 * @brief       Firmware for Peta, the RIOT based remote controlled RC car
 * @{
 *
 * @file
 * @brief       Bootstraps the Peta firmware and run the shell if DEVELHELP
 *              is activated
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "shell.h"
#include "board.h"

#include "brain.h"
#include "wd.h"
#include "peta_config.h"

#define SHELL_BUFSIZE       (64U)


static int _speed(int argc, char **argv)
{
    int16_t speed;

    if (argc < 2) {
        printf("usage: %s SPEED [-1024 to 1023]\n", argv[0]);
        return 1;
    }

    speed = (int16_t)atoi(argv[1]);
    if (speed < -0x03ff || speed > 0x03ff) {
        puts("speed value out of range\n");
        return 1;
    }
    brain_set_speed(speed);
    return 0;
}

static int _steer(int argc, char **argv)
{
    int16_t dir;

    if (argc < 2) {
        printf("usage: %s DIR [-1024 to 1023]\n", argv[0]);
        return 1;
    }

    dir = (int16_t)atoi(argv[1]);
    printf("read steering value: %i\n", dir);
    if (dir < -0x03ff || dir > 0x03ff) {
        puts("dir value out of range\n");
        return 1;
    }
    brain_steer(dir);
    return 0;
}

static int _debug(int argc, char **argv)
{
    int state;

    if (argc < 2) {
        printf("usage: %s <1|0>\n", argv[0]);
        return 1;
    }
    state = atoi(argv[1]);
    if (state) {
        wd_activate(0);
    }
    else {
        wd_activate(1);
    }
    return 0;
}

static int _readc(void)
{
    return (int)getchar();
}

static void _putc(int c)
{
    putchar((char)c);
}

/**
 * @brief   Define some shell commands for testing the brain
 */
static const shell_command_t _commands[] = {
    { "speed", "set Peta's speed", _speed },
    { "steer", "set direction", _steer },
    { "debug", "enable debug mode", _debug },
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_t shell;
    LED_RED_OFF;

    /* initialize (and run) the brain */
    puts("initializing the brain");
    brain_init();

    /* run the shell for debugging purposes */
    puts("running the shell");
    shell_init(&shell, _commands, SHELL_BUFSIZE, _readc, _putc);
    shell_run(&shell);

    /* should never be reached */
    return 0;
}

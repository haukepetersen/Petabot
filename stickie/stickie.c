
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define TYPE_BUTTON         1
#define TYPE_AXIS           2

void main(int argc, char **argv)
{
    int js;
    int remote;

    ssize_t n;
    uint8_t buf[8];

    uint8_t newdat = 0;

    int buttons = 0;
    int axes = 0;

    int axis_x;
    int axis_y;

    int16_t x;
    int16_t y;

    if (argc < 4) {
        printf("usage: %s <device> <x-axis> <y-axis>\n", argv[0]);
    }
    axis_x = atoi(argv[2]);
    axis_y = atoi(argv[3]);

    js = open(argv[1], O_RDONLY);
    if (js < 0) {
        puts("error: unable to open joystick");
        return;
    }

    do {
        n = read(js, buf, 8);
        if (buf[6] == 0x81) {
            ++buttons;
        }
        if (buf[6] == 0x82) {
            ++axes;
        }
    } while (buf[6] & 0x80);

    printf("Found Joystick: %i buttons, %i axes\n", buttons, axes);

    while (1) {
        n = read(js, buf, 8);
        if (n == 8) {

            if (buf[6] & 0x80) {
                printf("config ");
                for (int i = 0; i < 8; i++) {
                    printf("0x%02x ", buf[i]);
                }
                printf("\n");
                continue;
            }

            if (buf[6] == TYPE_BUTTON) {
                if (buf[4]) {
                    printf("button %u pressed\n", buf[7]);
                } else {
                    printf("button %u released\n", buf[7]);
                }
            } else if (buf[6] == TYPE_AXIS) {
                if (buf[7] == axis_x) {
                    x = ((int16_t)buf[5] << 8) | buf[4];
                    newdat = 1;
                } else if (buf[7] == axis_y) {
                    y = ((int16_t)buf[5] << 8) | buf[4];
                    newdat = 1;
                }
            }

            if (newdat) {
                newdat = 0;
                printf("ctrl %i %i\n", x, y);
            }


        }
        else {
            puts("input not length 8");
        }
    }


    close(js);
}

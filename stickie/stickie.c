
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <termio.h>
#include <fcntl.h>
#include <err.h>
#include <linux/serial.h>

#define TYPE_BUTTON         1
#define TYPE_AXIS           2

#define BAUDRATE            (B500000)

static int buttons = 0;
static int axes = 0;

static int open_remote(const char *dev)
{
    int fd;
    struct termios options;

    fd = open(dev, (O_RDWR | O_NOCTTY));
    if (fd < 0) {
        return fd;
    }

    fcntl(fd, F_SETFL, 0);
    tcgetattr(fd, &options);
    cfsetispeed(&options, BAUDRATE);
    cfsetospeed(&options, BAUDRATE);
    cfmakeraw(&options);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CRTSCTS;
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        return -1;
    }

    return fd;
}

static int open_stick(const char *dev)
{
    int js;
    uint8_t buf[8];

    js = open(dev, O_RDONLY);
    if (js < 0) {
        return js;
    }

    read(js, buf, 8);
    while (buf[6] & 0x80) {
        if (buf[6] == 0x81) {
            ++buttons;
        }
        if (buf[6] == 0x82) {
            ++axes;
        }
        read(js, buf, 8);
    };

    printf("Found Joystick: %i buttons, %i axes\n", buttons, axes);
    return js;
}

int main(int argc, char **argv)
{
    int js;
    int remote;

    ssize_t n;
    uint8_t buf[8];

    char line[128];
    int line_len;

    uint8_t newdat = 0;

    int axis_x;
    int axis_y;

    int16_t x;
    int16_t y;

    if (argc < 5) {
        printf("usage: %s <device> <x-axis> <y-axis> <antenna dev>\n", argv[0]);
        return 1;
    }
    axis_x = atoi(argv[2]);
    axis_y = atoi(argv[3]);

    js = open_stick(argv[1]);
    if (js < 0) {
        puts("error: unable to open joystick");
        return 1;
    }

    /* set baudrate */
    remote = open_remote(argv[4]);
    if (remote < 0) {
        puts("error: unable to open output port");
        return 1;
    }

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

            // if (buf[6] == TYPE_BUTTON) {
            //     if (buf[4]) {
            //         printf("button %u pressed\n", buf[7]);
            //     } else {
            //         printf("button %u released\n", buf[7]);
            //     }
            // }
            else if (buf[6] == TYPE_AXIS) {
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
                printf("raw: %7i %7i\n", x, y);
                x = (-1) * x / 32;
                y = (-1) * y / 32;
                if (x > -200 && x < 200) {
                    x = 0;
                }
                // if (y > -200 && y < 200) {
                //     y = 0;
                // }
                printf("ctrl %7i %7i\n", x, y);

                printf("\033[F\033[F");


                line_len = sprintf(line, "ctrl %i %i\n", x, y);
                write(remote, line, line_len);
                // line_len = read(remote, line, sizeof(line));
                // line[line_len] = '\0';
                // printf("res: %s\n", line);
            }
        }
        else {
            puts("input not length 8");
        }
    }


    close(js);
    return 0;
}

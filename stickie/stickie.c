
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
#include <pthread.h>

#define TYPE_BUTTON         (1U)
#define TYPE_AXIS           (2U)

#define BAUDRATE            (B115200)       /* iotlab: (B500000) */

#define UPDATE_INTERVAL     (20 * 1000)     /* 20ms */

#define AXIS_STEERING       (0)
#define AXIS_FW             (2)
#define AXIS_BW             (5)
#define BUTTON_DISCO        (7)

static int remote;
static int js;

static volatile int dir = 0;
static volatile int speed = 0;
static volatile uint8_t switches = 0;

static int buttons = 0;
static int axes = 0;

static int limiter = 1;

static int open_remote(const char *dev, int baudrate)
{
    int fd;
    struct termios options;

    fd = open(dev, (O_RDWR | O_NOCTTY));
    if (fd < 0) {
        return fd;
    }

    fcntl(fd, F_SETFL, 0);
    tcgetattr(fd, &options);
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    cfmakeraw(&options);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CRTSCTS;
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        return -1;
    }

    printf("Found Remote: %s @ %ibps\n", dev, 115200);
    return fd;
}

static int open_stick(const char *dev)
{
    int fd;
    uint8_t buf[8];

    fd = open(dev, O_RDONLY);
    if (fd < 0) {
        return fd;
    }

    read(fd, buf, 8);
    while (buf[6] & 0x80) {
        if (buf[6] == 0x81) {
            ++buttons;
        }
        if (buf[6] == 0x82) {
            ++axes;
        }
        read(fd, buf, 8);
    };

    printf("Found Joystick: %i buttons, %i axes\n", buttons, axes);
    return fd;
}

static void *stick_reader(void *arg)
{
    ssize_t n;
    uint8_t buf[8];

    int newdata = 0;

    int16_t steering = 0;
    int16_t fw = -32767;
    int16_t bw = -32767;

    printf("\n     steering  speed_fw  speed_bw\n");

    while (1) {
        n = read(js, buf, 8);
        if (n == 8) {

            if (buf[6] == TYPE_BUTTON) {
                if (buf[4]) {
                    /* button pressed actions here, buf[7] holds button number */
                    if (buf[7] == BUTTON_DISCO) {
                        switches ^= (1 << 0);
                        newdata = 1;
                    }
                } else {
                    /* put button release action here if needed */
                }
            }

            if (buf[6] == TYPE_AXIS) {
                if (buf[7] == AXIS_STEERING) {
                    steering = ((int16_t)buf[5] << 8) | buf[4];
                    newdata = 1;
                }
                else if (buf[7] == AXIS_FW) {
                    fw = ((int16_t)buf[5] << 8) | buf[4];
                    newdata = 1;
                }
                else if (buf[7] == AXIS_BW) {
                    bw = ((int16_t)buf[5] << 8) | buf[4];
                    newdata = 1;
                }
            }

            if (newdata) {
                newdata = 0;
                int a = fw + 32767;
                int b = bw + 32767;

                /* scale values to actual control values */
                if (a == 0) {
                    speed = -(b / 64);
                }
                else {
                    speed = a / 64;
                }
                /* apply limiter */
                speed /= limiter;
                /* calculate steering value */
                dir = (steering / 32);

                printf("raw:  %7i   %7u   %7u\n", steering, a, b);
                puts("");
                printf("        speed  steering  switches\n");
                printf("ctrl  %7i   %7i      0x%02x\n", speed, dir, switches);
                printf("\033[F\033[F\033[F\033[F");
            }
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t reader_thread;
    int res;

    char line[128];
    int line_len;

    if (argc < 3) {
        printf("usage: %s <device> <antenna dev> [limiter]\n", argv[0]);
        return 1;
    }

    if (argc >= 4) {
        limiter = atoi(argv[3]);
    }

    js = open_stick(argv[1]);
    if (js < 0) {
        puts("error: unable to open joystick");
        return 1;
    }

    remote = open_remote(argv[2], BAUDRATE);
    if (remote < 0) {
        puts("error: unable to open output port");
        return 1;
    }

    /* start the reader thread */
    res = pthread_create(&reader_thread, NULL, stick_reader, NULL);
    if (res < 0) {
        puts("error: unable to start reader thread");
        return 1;
    }

    while (1) {
        usleep(UPDATE_INTERVAL);
        line_len = sprintf(line, "ctrl %i %i %i\n", speed, dir,
                           (unsigned int)switches);
        write(remote, line, line_len);
    }

    pthread_join(reader_thread, NULL);
    close(js);
    return 0;
}

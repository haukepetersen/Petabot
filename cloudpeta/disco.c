

#include <string.h>

#include "disco.h"


void clear(void *d)
{
    disco_lame((disco_t *)d);
}

int disco_init(disco_t *dev, const disco_params_t *params)
{
    memcpy(dev, params, sizeof(disco_params_t));

    gpio_init(dev->pin, GPIO_DIR_OUT, GPIO_NOPULL);

    return 0;
}

void disco_fun(disco_t *dev)
{
    gpio_set(dev->pin);
}

void disco_lame(disco_t *dev)
{
    gpio_clear(dev->pin);
}

void disco_party(disco_t *dev, uint32_t dusk_till_dawn)
{
    dev->timer.callback = clear;
    dev->timer.arg = (void *)dev;
    disco_fun(dev);
    xtimer_set(&dev->timer, dusk_till_dawn);
}

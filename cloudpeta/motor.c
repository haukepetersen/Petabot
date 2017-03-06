
#include <string.h>

#include "motor.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"


int motor_init(motor_t *mot, const motor_params_t *params)
{
    memcpy(mot, params, sizeof(motor_params_t));

    int f_real = pwm_init(mot->pwm, PWM_LEFT, MOTOR_PWM_FREQ, MOTOR_PWM_RES);

    DEBUG("[motor] PWM initialized with %i Hz and resolution of %i\n",
           f_real, (int)MOTOR_PWM_RES);

    if (f_real < 0) {
        return -1;
    }

    gpio_init(mot->pin_a, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init(mot->pin_b, GPIO_DIR_OUT, GPIO_NOPULL);

    return 0;
}

void motor_set(motor_t *mot, int16_t speed)
{
    if (speed >= 0) {
        gpio_clear(mot->pin_a);
        gpio_set(mot->pin_b);
    }
    else {
        gpio_clear(mot->pin_b);
        gpio_set(mot->pin_a);
        speed *= -1;
    }

    pwm_set(mot->pwm, mot->chan, (unsigned)speed);
}

void motor_stop(motor_t *mot)
{
    pwm_set(mot->pwm, mot->chan, 0);
    gpio_clear(mot->pin_b);
    gpio_clear(mot->pin_b);
}

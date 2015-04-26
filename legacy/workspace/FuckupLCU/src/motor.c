/*
 * motor.c
 *
 *  Created on: Mar 20, 2013
 *      Author: PETERSEN
 */

#include "motor.h"


#define MOTOR_REG_PRESCALER		((72*MOTOR_PWM_RES)-1)
#define MOTOR_REG_PERIOD		((MOTOR_PWM_PERIOD/MOTOR_PWM_RES)-1)


void motor_init(void) {

	// initialization data structures
	GPIO_InitTypeDef gpio_init;
	TIM_TimeBaseInitTypeDef tim_init_timebase;
	TIM_OCInitTypeDef tim_init_oc;

	// enable MOTOR_TIMER and MOTOR_PORT
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	// setup GPIO port used for direction
	gpio_init.GPIO_Pin = MOTOR_DIR_PINS;
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_DOWN;							// enable pull-down for direction indication
	GPIO_Init(MOTOR_DIR_PORT, &gpio_init);

	// setup GPIO port used by PWM
	gpio_init.GPIO_Pin = MOTOR_PWM_PINS;
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(MOTOR_PWM_PORT, &gpio_init);
	// set the correct alternate function for MOTOR_PINS
	GPIO_PinAFConfig(MOTOR_PWM_PORT, GPIO_PinSource6, GPIO_AF_2);
	GPIO_PinAFConfig(MOTOR_PWM_PORT, GPIO_PinSource7, GPIO_AF_2);
	GPIO_PinAFConfig(MOTOR_PWM_PORT, GPIO_PinSource8, GPIO_AF_2);
	GPIO_PinAFConfig(MOTOR_PWM_PORT, GPIO_PinSource9, GPIO_AF_2);

	// setup the timebase for MOTOR_TIMER
	tim_init_timebase.TIM_ClockDivision = TIM_CKD_DIV1;				// use full 72MHz for timer
	tim_init_timebase.TIM_Prescaler = MOTOR_REG_PRESCALER;
	tim_init_timebase.TIM_Period = MOTOR_REG_PERIOD;
	tim_init_timebase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(MOTOR_TIMER, &tim_init_timebase);

	// setup the output compare units for MOTOR_TIMER
	tim_init_oc.TIM_OCMode = TIM_OCMode_PWM1;
	tim_init_oc.TIM_OutputState = TIM_OutputState_Enable;
	tim_init_oc.TIM_OutputNState = TIM_OutputNState_Disable;
	tim_init_oc.TIM_OCIdleState = TIM_OCIdleState_Reset;
	tim_init_oc.TIM_OCPolarity = TIM_OCPolarity_High;
	tim_init_oc.TIM_Pulse = 0;										// make sure duty cycle is 0% at startup
	// write setup for all 4 OC units
	TIM_OC1Init(MOTOR_TIMER, &tim_init_oc);
	TIM_OC2Init(MOTOR_TIMER, &tim_init_oc);
	TIM_OC3Init(MOTOR_TIMER, &tim_init_oc);
	TIM_OC4Init(MOTOR_TIMER, &tim_init_oc);
}

void motor_enable(void)
{
	motor_stop_all();
	TIM_Cmd(MOTOR_TIMER, ENABLE);									// enable timer
	TIM_CtrlPWMOutputs(MOTOR_TIMER, ENABLE);						// enable PWM outputs
}

void motor_disable()
{
	TIM_CtrlPWMOutputs(MOTOR_TIMER, DISABLE);						// disable PWM outputs
	TIM_Cmd(MOTOR_TIMER, DISABLE);									// disable timer
}

uint16_t motor_get_speed(motor_chan_t motor) {
	switch (motor) {
	case MOTOR_1:
		return TIM_GetCapture1(MOTOR_TIMER);
	case MOTOR_2:
		return TIM_GetCapture2(MOTOR_TIMER);
	case MOTOR_3:
		return TIM_GetCapture3(MOTOR_TIMER);
	case MOTOR_4:
		return TIM_GetCapture4(MOTOR_TIMER);
	}
	return 0;
}

void motor_set(motor_chan_t motor, int16_t speed)
{
	if (speed < 0) {
		speed = speed * -1;
		motor_set_dir(motor, MOTOR_FW);
	} else {
		motor_set_dir(motor, MOTOR_BW);
	}
	speed = (speed > MOTOR_SPEED_MAX) ? MOTOR_SPEED_MAX : speed;
	motor_set_speed(motor, speed);
}

void motor_set_all(int16_t speed)
{
	motor_set(MOTOR_1, speed);
	motor_set(MOTOR_2, speed);
	motor_set(MOTOR_3, speed);
	motor_set(MOTOR_4, speed);
}


void motor_set_speed(motor_chan_t motor, uint16_t speed)
{
	speed = (speed > MOTOR_SPEED_MAX) ? MOTOR_SPEED_MAX : speed;	// make a range check on speed value
	switch (motor) {
	case MOTOR_1:
		TIM_SetCompare1(MOTOR_TIMER, speed);
		break;
	case MOTOR_2:
		TIM_SetCompare2(MOTOR_TIMER, speed);
		break;
	case MOTOR_3:
		TIM_SetCompare3(MOTOR_TIMER, speed);
		break;
	case MOTOR_4:
		TIM_SetCompare4(MOTOR_TIMER, speed);
		break;
	}
}

void motor_set_speed_all(uint16_t speed)
{
	motor_set_speed(MOTOR_1, speed);
	motor_set_speed(MOTOR_2, speed);
	motor_set_speed(MOTOR_3, speed);
	motor_set_speed(MOTOR_4, speed);
}

void motor_set_dir(uint8_t motor, motor_dir_t dir)
{
	switch (dir) {
	case MOTOR_BRAKE:
		GPIO_ResetBits(MOTOR_DIR_PORT, MOTOR_1_DIR_A | MOTOR_1_DIR_B);
		break;
	case MOTOR_FW:
		GPIO_SetBits(MOTOR_DIR_PORT, MOTOR_1_DIR_A);
		GPIO_ResetBits(MOTOR_DIR_PORT, MOTOR_1_DIR_B);
		break;
	case MOTOR_BW:
		GPIO_ResetBits(MOTOR_DIR_PORT, MOTOR_1_DIR_A);
		GPIO_SetBits(MOTOR_DIR_PORT, MOTOR_1_DIR_B);
		break;
	}
}

void motor_set_dir_all(motor_dir_t dir)
{
	motor_set_dir(MOTOR_1, dir);
	motor_set_dir(MOTOR_2, dir);
	motor_set_dir(MOTOR_3, dir);
	motor_set_dir(MOTOR_4, dir);
}

void motor_stop(motor_chan_t motor)
{
	motor_set_speed(motor, 0);
}

void motor_stop_all()
{
	motor_stop(MOTOR_1);
	motor_stop(MOTOR_2);
	motor_stop(MOTOR_3);
	motor_stop(MOTOR_4);
}

void motor_full_fw(motor_chan_t motor)
{
	motor_set_dir(motor, MOTOR_FW);
	motor_set_speed(motor, MOTOR_SPEED_MAX);
}

void motor_full_fw_all(void)
{
	motor_full_fw(MOTOR_1);
	motor_full_fw(MOTOR_2);
	motor_full_fw(MOTOR_3);
	motor_full_fw(MOTOR_4);
}

void motor_full_bw(motor_chan_t motor)
{
	motor_set_dir(motor, MOTOR_BW);
	motor_set_speed(motor, MOTOR_SPEED_MAX);
}

void motor_full_bw_all(void)
{
	motor_full_bw(MOTOR_1);
	motor_full_bw(MOTOR_2);
	motor_full_bw(MOTOR_3);
	motor_full_bw(MOTOR_4);
}

void motor_set_cmd(uint8_t chan, uint8_t mode, uint8_t speed)
{
	uint16_t s = ((float)speed / 255) * 1000;
	if (mode == 2) { // fw
		motor_set_dir(chan, MOTOR_FW);
		motor_set_speed(MOTOR_1, s);
	} else if (mode == 1) { // bw
		motor_set_dir(chan, MOTOR_BW);
		motor_set_speed(MOTOR_1, s);
	} else if (mode == 3) { // brake
		motor_set_dir(chan, MOTOR_BRAKE);
		motor_stop(MOTOR_1);
	}
}

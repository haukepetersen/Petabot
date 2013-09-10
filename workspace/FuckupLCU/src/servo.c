/*
 * servo.c
 *
 *  Created on: Mar 20, 2013
 *      Author: PETERSEN
 */

#include "servo.h"


servo_cal_t servo_cal[4];


void servo_init(void)
{
	// initialize calibration values (TODO: get from EEPROM)
	for (uint8_t i = 0; i < SERVO_NUMOF_CHANNELS; i++) {
		servo_cal[i].left = 1000;
		servo_cal[i].center = 1500;
		servo_cal[i].right = 2000;
		servo_cal[i].neutral = 1500;
	}

	// initialization data structures
	GPIO_InitTypeDef gpio_init;
	TIM_TimeBaseInitTypeDef tim_init_timebase;
	TIM_OCInitTypeDef tim_init_oc;

	// enable SERVO_TIMER and SERVO_PORT modules
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

	// set GPIOD pins 12 to 15
	gpio_init.GPIO_Pin = SERVO_PINS;
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SERVO_PORT, &gpio_init);

	GPIO_PinAFConfig(SERVO_PORT, GPIO_PinSource12, SERVO_PIN_AF);
	GPIO_PinAFConfig(SERVO_PORT, GPIO_PinSource13, SERVO_PIN_AF);
	GPIO_PinAFConfig(SERVO_PORT, GPIO_PinSource14, SERVO_PIN_AF);
	GPIO_PinAFConfig(SERVO_PORT, GPIO_PinSource15, SERVO_PIN_AF);


	// configure the timer to run with a period of 20ms
	tim_init_timebase.TIM_ClockDivision = TIM_CKD_DIV1;			// the counter runs with the full 72MHz
	tim_init_timebase.TIM_Prescaler = 72 - 1;						// count up every 1us
	tim_init_timebase.TIM_Period = 20000 - 1;					// period = 20ms -> 50Hz
	tim_init_timebase.TIM_CounterMode = TIM_CounterMode_Up;		// let the counter count up
	TIM_TimeBaseInit(SERVO_TIMER, &tim_init_timebase);

	// configure the 4 OC channels
	tim_init_oc.TIM_OCMode = TIM_OCMode_PWM1;
	tim_init_oc.TIM_OutputState = TIM_OutputState_Enable;
	tim_init_oc.TIM_OutputNState = TIM_OutputNState_Disable;
	tim_init_oc.TIM_OCIdleState = TIM_OCIdleState_Reset;
	tim_init_oc.TIM_OCPolarity = TIM_OCPolarity_High;
	tim_init_oc.TIM_Pulse = 0;									// set initial pulse width to zero
	// setup all 4 servo channels
	TIM_OC1Init(SERVO_TIMER, &tim_init_oc);
	TIM_OC2Init(SERVO_TIMER, &tim_init_oc);
	TIM_OC3Init(SERVO_TIMER, &tim_init_oc);
	TIM_OC4Init(SERVO_TIMER, &tim_init_oc);
}


void servo_enable()
{
	servo_neutral_all();										// center all servos on start
	TIM_Cmd(SERVO_TIMER, ENABLE);								// enable timer
	TIM_CtrlPWMOutputs(SERVO_TIMER, ENABLE);					// enable PWM outputs
}

void servo_disable()
{
	TIM_CtrlPWMOutputs(SERVO_TIMER, DISABLE);					// disable PWM outputs
	TIM_Cmd(SERVO_TIMER, DISABLE);								// disable timer
}

uint16_t servo_get_pos(servo_chan_t channel)
{
	switch (channel) {
	case SERVO_1:
		return TIM_GetCapture1(SERVO_TIMER);
	case SERVO_2:
		return TIM_GetCapture2(SERVO_TIMER);
	case SERVO_3:
		return TIM_GetCapture3(SERVO_TIMER);
	case SERVO_4:
		return TIM_GetCapture4(SERVO_TIMER);
	}
	return 0;
}


void servo_center(servo_chan_t channel)
{
	servo_set(channel, servo_cal[channel].center);
}

void servo_center_all()
{
	servo_center(SERVO_1);
	servo_center(SERVO_2);
	servo_center(SERVO_3);
	servo_center(SERVO_4);
}

void servo_neutral(servo_chan_t channel)
{
	servo_set(channel, servo_cal[channel].neutral);
}

void servo_neutral_all()
{
	servo_neutral(SERVO_1);
	servo_neutral(SERVO_2);
	servo_neutral(SERVO_3);
	servo_neutral(SERVO_4);
}


void servo_set(servo_chan_t channel, int16_t value)
{
	value = (value < servo_cal[channel].left) ? servo_cal[channel].left : value;
	value = (value > servo_cal[channel].right) ? servo_cal[channel].right : value;
	switch (channel) {
	case SERVO_1:
		TIM_SetCompare1(SERVO_TIMER, value);
		break;
	case SERVO_2:
		TIM_SetCompare2(SERVO_TIMER, value);
		break;
	case SERVO_3:
		TIM_SetCompare3(SERVO_TIMER, value);
		break;
	case SERVO_4:
		TIM_SetCompare4(SERVO_TIMER, value);
		break;
	}
}

void servo_set_all(uint16_t value)
{
	servo_set(SERVO_1, value);
	servo_set(SERVO_2, value);
	servo_set(SERVO_3, value);
	servo_set(SERVO_4, value);
}

void servo_cal_get(servo_chan_t channel, servo_cal_t* cal)
{
	cal->left = servo_cal[channel].left;
	cal->center = servo_cal[channel].center;
	cal->right = servo_cal[channel].right;
}

void servo_cal_set(servo_chan_t channel, servo_cal_t* cal)
{
	servo_cal[channel].left = cal->left;
	servo_cal[channel].center = cal->center;
	servo_cal[channel].right = cal->right;
}

void servo_set_cmd(uint8_t channel, uint8_t mode, uint8_t value)
{
	if (mode == CENTER) {
		servo_center(SERVO_1);
	} else if (mode ==  LEFT) {
		uint16_t set = 1500 - ((float)value / 255) * 500;
		servo_set(SERVO_1, set);
	} else {
		uint16_t set = 1500 + ((float)value / 255) * 500;
		servo_set(SERVO_1, set);
	}

}

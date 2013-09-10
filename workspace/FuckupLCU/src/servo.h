/*
 * servo.h
 *
 *  Created on: Mar 20, 2013
 *      Author: PETERSEN
 */

#ifndef SERVO_H_
#define SERVO_H_

#include "stm32f30x.h"


#define SERVO_TIMER		TIM4
#define SERVO_PORT		GPIOD
#define SERVO_PINS		(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)
#define SERVO_PIN_AF	GPIO_AF_2
#define SERVO_NUMOF_CHANNELS 4


typedef enum {
	SERVO_1 = 0x00,
	SERVO_2,
	SERVO_3,
	SERVO_4
} servo_chan_t;

typedef enum {
	LEFT = 1,
	RIGHT = 2,
	CENTER = 3
} servo_mode_t;

typedef struct {
	uint16_t left;
	uint16_t center;
	uint16_t right;
	uint16_t neutral;
} servo_cal_t;


void servo_init();

void servo_enable();
void servo_disable();

uint16_t servo_get_pos(servo_chan_t channel);

void servo_center(servo_chan_t channel);
void servo_center_all();

void servo_neutral(servo_chan_t channel);
void servo_neutral_all();

void servo_set(servo_chan_t channel, int16_t value);
void servo_set_all(uint16_t value);


void servo_cal_get(servo_chan_t channel, servo_cal_t* cal);
void servo_cal_set(servo_chan_t channel, servo_cal_t* cal);

void servo_set_cmd(uint8_t channel, uint8_t mode, uint8_t value);



#endif /* SERVO_H_ */

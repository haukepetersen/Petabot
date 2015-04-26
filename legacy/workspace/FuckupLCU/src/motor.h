/*
 * motor.h
 *
 *  Created on: Mar 20, 2013
 *      Author: PETERSEN
 */

#ifndef MOTOR_H_
#define MOTOR_H_

/*** Includes **********************************************************************************************/
#include "stm32f30x.h"
#include "stdio.h"


/*** Defines ***********************************************************************************************/
#define MOTOR_PWM_PERIOD	1000		// PWM period in us
#define MOTOR_PWM_RES		1			// PWM resolution in us

#define MOTOR_TIMER			TIM3
#define MOTOR_PWM_PORT		GPIOC
#define MOTOR_PWM_PINS		(GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9)

#define MOTOR_DIR_PORT		GPIOC
#define MOTOR_DIR_PIN1		GPIO_Pin_2
#define MOTOR_DIR_PIN2		GPIO_Pin_3
#define MOTOR_DIR_PIN3		GPIO_Pin_4
#define MOTOR_DIR_PIN4		GPIO_Pin_5
#define MOTOR_DIR_PIN5		GPIO_Pin_10
#define MOTOR_DIR_PIN6		GPIO_Pin_11
#define MOTOR_DIR_PIN7		GPIO_Pin_12
#define MOTOR_DIR_PIN8		GPIO_Pin_13
#define MOTOR_DIR_PINS		(MOTOR_DIR_PIN1|MOTOR_DIR_PIN2|MOTOR_DIR_PIN3|MOTOR_DIR_PIN4|MOTOR_DIR_PIN5|MOTOR_DIR_PIN6|MOTOR_DIR_PIN7|MOTOR_DIR_PIN8)

#define MOTOR_1_DIR_A		MOTOR_DIR_PIN1
#define MOTOR_1_DIR_B		MOTOR_DIR_PIN2


#define MOTOR_SPEED_MAX		1000
#define MOTOR_SPEED_MIN		0

#define MOTOR_NUM_CHANNELS	4


/*** Typedefs **********************************************************************************************/
typedef enum {
	MOTOR_1 = MOTOR_DIR_PIN1,
	MOTOR_2 = MOTOR_DIR_PIN2,
	MOTOR_3 = MOTOR_DIR_PIN3,
	MOTOR_4 = MOTOR_DIR_PIN4
} motor_chan_t;

typedef enum {
	MOTOR_BRAKE	= 0,
	MOTOR_FW	= 1,
	MOTOR_BW	= 2
} motor_dir_t;


/*** Public Variables **************************************************************************************/





/*** Public Functions **************************************************************************************/
void motor_init();

void motor_enable();
void motor_disable();

uint16_t motor_get_speed(motor_chan_t motor);

void motor_set(motor_chan_t motor, int16_t speed);
void motor_set_all(int16_t speed);

void motor_set_speed(motor_chan_t motor, uint16_t speed);
void motor_set_speed_all(uint16_t speed);

void motor_set_dir(uint8_t motor, motor_dir_t dir);
void motor_set_dir_all(motor_dir_t dir);

void motor_stop(motor_chan_t motor);
void motor_stop_all();

void motor_full_fw(motor_chan_t motor);
void motor_full_fw_all();

void motor_full_bw(motor_chan_t motor);
void motor_full_bw_all();

void motor_set_cmd(uint8_t chan, uint8_t mode, uint8_t speed);



#endif /* MOTOR_H_ */

/**
 *****************************************************************************
 **
 **  File        : main.c
 **
 **  Abstract    : main function.
 **
 **  Functions   : main
 **
 **  Environment : Atollic TrueSTUDIO(R)
 **
 **  Distribution: The file is distributed “as is,” without any warranty
 **                of any kind.
 **
 **  (c)Copyright Atollic AB.
 **  You may use this file as-is or modify it according to the needs of your
 **  project. Distribution of this file (unmodified or modified) is not
 **  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
 **  rights to distribute the assembled, compiled & linked contents of this
 **  file as part of an application binary file, provided that it is built
 **  using the Atollic TrueSTUDIO(R) toolchain.
 **
 *****************************************************************************
 */

/* Includes */
#include "stm32f30x.h"
#include "stm32f3_discovery.h"

#include "uart.h"
#include "servo.h"
#include "motor.h"
#include "support.h"

/* Private typedef */

/* Private define  */

#define B_PERIOD	500

/* Private macro */

/* Private variables */

/* Private function prototypes */

/* Private functions */

/* Global variables */
volatile uint32_t sys_tick = 0;

/**
 **===========================================================================
 **
 **  Abstract: SysTick interrupt handler
 **
 **===========================================================================
 */
void main_print(uint8_t *buf, uint8_t *pos, char *str, uint8_t length);
void main_send(uint8_t *buf, uint8_t length);
void main_bin2bcd(uint16_t val, uint8_t *buf, uint8_t bufsize);


void SysTick_Handler(void)
{
	++sys_tick;
	if (sys_tick == 500) {
		sys_tick = 0;
		STM_EVAL_LEDToggle(LED5);
	}
}

void sys_init()
{
	/* Example use SysTick timer and read System core clock */
	SysTick_Config(72000); /* 1 ms if clock frequency 72 MHz */
	SystemCoreClockUpdate();
}


/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 */
int main(void) {

	sys_init();



	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED6);
	STM_EVAL_LEDInit(LED8);
	STM_EVAL_LEDInit(LED10);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED9);

	uart_init();
	motor_init();
	servo_init();

	// test ringbuf
	sup_ringbuf_t ringbuf;
	sup_ringbuf_init(&ringbuf, 13);
	uint8_t testdata[5];
	sup_ringbuf_write(&ringbuf, testdata, 5);


	// test motor
	motor_enable();

	// test servo
	servo_enable();
	servo_center_all();


	uint8_t linebuf[120];
	uint8_t linepos;
	uint8_t numbuf[6];

	while (1) {
//		if (sys_tick % 100 == 0) {
//			linepos = 0;
//			for (uint8_t i = 0; i < 120; i++) {
//				linebuf[i] = ' ';
//			}
//
//			main_print(linebuf, &linepos, "\rFuckupCtrl - ", 14);
//			main_print(linebuf, &linepos, "Motor 1: ", 9);
//			main_bin2bcd(motor_get_speed(MOTOR_1), numbuf, 5);
//			main_print(linebuf, &linepos, (char*)numbuf, 5);
//
//			main_print(linebuf, &linepos, "    SERVO_1: ", 13);
//			main_bin2bcd(servo_get_pos(SERVO_1), numbuf, 5);
//			main_print(linebuf, &linepos, (char*)numbuf, 5);
//
//			main_send(linebuf, 120);
//		}
	}

	/* Program will never run to this line */
	return 0;
}


void main_print(uint8_t *buf, uint8_t *pos, char *str, uint8_t length)
{
	for (uint8_t i = 0; i < length; i++) {
		buf[(*pos)++] = str[i];
	}
}

void main_send(uint8_t *buf, uint8_t length)
{
	for (uint8_t i = 0; i < length; i++) {
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, buf[i]);
	}
}

void main_bin2bcd(uint16_t val, uint8_t *buf, uint8_t bufsize)
{
	for (uint8_t i = 0; i < bufsize; i++) {
		buf[i] = ' ';
	}
	do {
		--bufsize;
		uint8_t digit = val % 10;
		buf[bufsize] = '0' + digit;
		val = val / 10;
	} while (val > 0 && bufsize > 0);
}
/**
 **===========================================================================
 **
 **  Abstract: ISRs
 **
 **===========================================================================
 */


/*
 * comm.c
 *
 *  Created on: Mar 19, 2013
 *      Author: PETERSEN
 */

#include "uart.h"
#include "motor.h"
#include "servo.h"
#include "support.h"
#include "comm.h"

#include "stm32f3_discovery.h"


#define UART_NODE_ID	0xaa


/**** Local variables **************************************************************************************/
typedef struct {
	uint8_t type;
	uint8_t size;
	uint8_t rw;
	uint8_t count;
	uint8_t data[16];
} uart_mssg_t;

uart_mssg_t uart_incoming;

sup_ringbuf_t uart_sendqueue;


/**** Local function definitions ***************************************************************************/
void uart_receive(uint8_t data);
void uart_process(void);
void uart_tx_int(FunctionalState state);

void uart_cmd_motor(void);
void uart_cmd_servo(void);
void uart_cmd_switch(void);
void uart_cmd_stat(void);

/**** Public functions *************************************************************************************/
void uart_init(void)
{
	// initialize send queue
	sup_ringbuf_init(&uart_sendqueue, UART_SENDQUEUE_SIZE);

	// initialize receive struct
	uart_incoming.count = 0;

	// config structs
	USART_InitTypeDef usart_init_struct;
	GPIO_InitTypeDef gpio_init_struct;
	NVIC_InitTypeDef nvic_init_struct;

	// enable USART1 and GPIO_A
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	// connect pins to USART1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);

	// set GPIO config options
	gpio_init_struct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_AF;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_struct.GPIO_OType = GPIO_OType_PP;
	gpio_init_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	// write config to pins
	GPIO_Init(GPIOA, &gpio_init_struct);

	// set USART config options
	usart_init_struct.USART_BaudRate = UART_BAUDRATE;
	usart_init_struct.USART_WordLength = USART_WordLength_8b;
	usart_init_struct.USART_Parity = USART_Parity_No;
	usart_init_struct.USART_StopBits = USART_StopBits_1;
	usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// write USART1 options
	USART_Init(USART1, &usart_init_struct);

	// setup USART1 interrupt for Rx and Tx
	nvic_init_struct.NVIC_IRQChannel = USART1_IRQn;
	nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init_struct.NVIC_IRQChannelSubPriority = 0;
	nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
	// write NVIC config
	NVIC_Init(&nvic_init_struct);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	// enable USART1
	USART_Cmd(USART1, ENABLE);
}

void uart_enable(uart_module_t module)
{
	switch (module) {
	case UART_Tx:

		break;
	case UART_Rx:

		break;
	}
}

void uart_enable_all()
{
	uart_enable(UART_Tx);
	uart_enable(UART_Rx);
}

void uart_send(uint8_t head, uint8_t size, uint8_t *data)
{
	sup_ringbuf_write(&uart_sendqueue, &head, 1);
	sup_ringbuf_write(&uart_sendqueue, data, size);
	uart_tx_int(ENABLE);								// make sure tx interrupt is enabled
}


/**** Private functions ************************************************************************************/
void uart_receive(uint8_t data)
{
	if (uart_incoming.count == 0) {
		uart_incoming.type = ((data & COMM_CMD_MASK) >> COMM_CMD_SHIFT);
		uart_incoming.size = ((data & COMM_SIZE_MASK) >> COMM_SIZE_SHIFT);
		uart_incoming.rw = ((data & COMM_RW_MASK) >> COMM_RW_SHIFT);
		uart_incoming.count = 1;
	} else {
		uart_incoming.data[uart_incoming.count - 1] = data;
		++uart_incoming.count;
	}
	if (uart_incoming.count == uart_incoming.size + 1) {
		uart_process();
		uart_incoming.count = 0;
	}
}

void uart_process()
{
	STM_EVAL_LEDToggle(LED3);
	switch (uart_incoming.type) {
	case COMM_MOT_CMD:
		uart_cmd_motor();
		break;
	case COMM_SERV_CMD:
		uart_cmd_servo();
		break;
	case COMM_SWC_CMD:
		uart_cmd_switch();
		break;
	case COMM_STAT_CMD:
		uart_cmd_stat();
		break;
	}
}


void uart_cmd_motor(void)
{
	STM_EVAL_LEDToggle(LED4);
	if (uart_incoming.rw == COMM_RW_WRITE) {
		uint8_t chan = ((uart_incoming.data[0] & COMM_MOT_CHAN_MASK) >> COMM_MOT_CHAN_SHIFT);
		uint8_t mode = ((uart_incoming.data[0] & COMM_MOT_STAT_MASK) >> COMM_MOT_STAT_SHIFT);
		motor_set_cmd(chan, mode, uart_incoming.data[1]);
	}
}

void uart_cmd_servo(void)
{
	STM_EVAL_LEDToggle(LED6);
	if (uart_incoming.rw == COMM_RW_WRITE) {
		uint8_t chan = ((uart_incoming.data[0] & COMM_SERV_CHAN_MASK) >> COMM_SERV_CHAN_SHIFT);
		uint8_t mode = ((uart_incoming.data[0] & COMM_SERV_STAT_MASK) >> COMM_SERV_STAT_SHIFT);
		servo_set_cmd(chan, mode, uart_incoming.data[1]);
	}
}

void uart_cmd_switch(void)
{
	STM_EVAL_LEDToggle(LED8);
}

void uart_cmd_stat(void)
{
	STM_EVAL_LEDToggle(LED10);
	uint8_t c[2];
	c[0] = (COMM_STAT_CMD << COMM_CMD_SHIFT);
	c[0] |= ((COMM_RW_WRITE << COMM_RW_SHIFT) & COMM_RW_MASK);
	c[0] |= 1;
	c[1] = UART_NODE_ID;
	uart_send(c[0], 1, &c[2]);
}

void uart_tx_int(FunctionalState state)
{
	USART_ITConfig(USART1, USART_IT_TXE, state);
}

/**** Interrupt handlers ***********************************************************************************/
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {		// RXNE is cleared by reading the RDR register
		uint8_t tmp = (uint8_t)USART_ReceiveData(USART1);
		uart_receive(tmp);
	}
	else if (USART_GetITStatus(USART1, USART_IT_TXE) == SET) {	// TXE is cleared by writing the TDR register
		if (uart_sendqueue.count > 0) {
			USART_SendData(USART1, (uint16_t)sup_ringbuf_get(&uart_sendqueue));
		} else {
			uart_tx_int(DISABLE);
		}
	}
}

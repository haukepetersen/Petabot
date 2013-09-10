/*
 * support.h
 *
 *  Created on: Apr 4, 2013
 *      Author: PETERSEN
 */

#ifndef SUPPORT_H_
#define SUPPORT_H_

#include "stm32f30x.h"
#include "stdlib.h"


typedef struct {
	uint8_t size;
	uint8_t count;
	uint8_t head;
	uint8_t tail;
	uint8_t *data;
} sup_ringbuf_t;



void sup_ringbuf_init(sup_ringbuf_t *buf, uint8_t size);
uint8_t sup_ringbuf_write(sup_ringbuf_t *buf, uint8_t *data, uint8_t size);
uint8_t sup_ringbuf_read(sup_ringbuf_t *buf, uint8_t *data, uint8_t size);

void sup_ringbuf_put(sup_ringbuf_t *buf, uint8_t data);
uint8_t sup_ringbuf_get(sup_ringbuf_t *buf);





#endif /* SUPPORT_H_ */

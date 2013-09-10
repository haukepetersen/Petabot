/*
 * support.c
 *
 *  Created on: Apr 4, 2013
 *      Author: PETERSEN
 */

#include "support.h"


/**** local function definitions ***************************************************************************/



void sup_ringbuf_put(sup_ringbuf_t *buf, uint8_t data)
{
	buf->data[buf->head] = data;
	buf->head = (++buf->head == buf->size) ? 0 : buf->head;
	++buf->count;
}

uint8_t sup_ringbuf_get(sup_ringbuf_t *buf)
{
	uint8_t res = buf->data[buf->tail];
	buf->tail = (++buf->tail == buf->size) ? 0 : buf->tail;
	--buf->count;
	return res;
}




void sup_ringbuf_init(sup_ringbuf_t *buf, uint8_t size)
{
	buf->size = size;
	buf->count = 0;
	buf->head = 0;
	buf->tail = 0;
	buf->data = malloc(sizeof(uint8_t) * size);
}


uint8_t sup_ringbuf_write(sup_ringbuf_t *buf, uint8_t *data, uint8_t size)
{
	if (size > buf->size - buf->count) {
		size = buf->size - buf->count;
	}
	for (uint8_t i = 0; i < size; i++) {
		sup_ringbuf_put(buf, data[i]);
	}
	return size;
}

uint8_t sup_ringbuf_read(sup_ringbuf_t *buf, uint8_t *data, uint8_t size)
{
	if (size > buf->count) {
		size = buf->count;
	}
	for (uint8_t i = 0; i < size; i++) {
		data[i] = sup_ringbuf_get(buf);
	}
	return size;
}



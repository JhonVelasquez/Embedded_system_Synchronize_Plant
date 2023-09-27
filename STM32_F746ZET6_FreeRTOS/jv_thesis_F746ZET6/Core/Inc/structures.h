/*
 * structures.h
 *
 *  Created on: Sep 2, 2023
 *      Author: jvlsq
 */

#ifndef INC_STRUCTURES_H_
#define INC_STRUCTURES_H_

#include "define.h"

typedef struct {
	uint8_t msg[MAX_MESSSAGE_SIZE];
	int len;
} queue_commandRx;


typedef struct {
	uint8_t rxBuffer;
	uint8_t state_rx;
	queue_commandRx queueCommandRx;
	uint8_t cnt_rx;
} cli_char_buffer_handler;

#endif /* INC_STRUCTURES_H_ */

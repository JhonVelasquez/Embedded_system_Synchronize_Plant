/*
 * utils.h
 *
 *  Created on: Jun 27, 2023
 *      Author: jvlsq
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

float get_ticks_from_ms(float ms){
	return (float)ms/portTICK_PERIOD_MS;
}

int get_ms_from_ticks(float tick){
	return (int)tick*portTICK_PERIOD_MS;
}

/*
 * There are these definitions on other archives

#define portTICK_RATE_MS      portTICK_PERIOD_MS
#define portTICK_PERIOD_MS	  ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define configTICK_RATE_HZ    ((TickType_t)1000)


*/

#endif /* INC_UTILS_H_ */

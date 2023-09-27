/*
 * trigger.c
 *
 *  Created on: Jun 24, 2023
 *      Author: jvlsq
 */

#include "trigger.h"

//TRIGGER FUNCTIONS
void on_trigger_RPI(){
	HAL_GPIO_WritePin(TRIGGER_RPI_GPIO, TRIGGER_RPI_PIN, GPIO_PIN_SET);
}
void off_trigger_RPI(){
	HAL_GPIO_WritePin(TRIGGER_RPI_GPIO, TRIGGER_RPI_PIN, GPIO_PIN_RESET);
}
void trigger_RPI(){
	on_trigger_RPI();
	osDelay(get_ticks_from_ms(TIME_TRIGGERS_WIDTH));
	off_trigger_RPI();
}
bool get_trigger_RPI(){
	return (bool) HAL_GPIO_ReadPin(TRIGGER_RPI_GPIO, TRIGGER_RPI_PIN);
}


void on_trigger_KEYENCE(){
	HAL_GPIO_WritePin(TRIGGER_KEYENCE_GPIO, TRIGGER_KEYENCE_PIN, GPIO_PIN_SET);
}
void off_trigger_KEYENCE(){
	HAL_GPIO_WritePin(TRIGGER_KEYENCE_GPIO, TRIGGER_KEYENCE_PIN, GPIO_PIN_RESET);
}
void trigger_KEYENCE(){
	on_trigger_KEYENCE();
	osDelay(get_ticks_from_ms(TIME_TRIGGERS_WIDTH));
	off_trigger_KEYENCE();
}
bool get_trigger_KEYENCE(){
	return (bool) HAL_GPIO_ReadPin(TRIGGER_KEYENCE_GPIO, TRIGGER_KEYENCE_PIN);
}


void on_trigger_FG1(){
	HAL_GPIO_WritePin(TRIGGER_FG1_GPIO, TRIGGER_FG1_PIN, GPIO_PIN_SET);
}
void off_trigger_FG1(){
	HAL_GPIO_WritePin(TRIGGER_FG1_GPIO, TRIGGER_FG1_PIN, GPIO_PIN_RESET);
}
void trigger_FG1(){
	on_trigger_FG1();
	osDelay(get_ticks_from_ms(TIME_TRIGGERS_WIDTH));
	off_trigger_FG1();
}
bool get_trigger_FG1(){
	return (bool) HAL_GPIO_ReadPin(TRIGGER_FG1_GPIO, TRIGGER_FG1_PIN);
}


void on_trigger_FG2(){
	HAL_GPIO_WritePin(TRIGGER_FG2_GPIO, TRIGGER_FG2_PIN, GPIO_PIN_SET);
}
void off_trigger_FG2(){
	HAL_GPIO_WritePin(TRIGGER_FG2_GPIO, TRIGGER_FG2_PIN, GPIO_PIN_RESET);
}
void trigger_FG2(){
	on_trigger_FG2();
	osDelay(get_ticks_from_ms(TIME_TRIGGERS_WIDTH));
	off_trigger_FG2();
}
bool get_trigger_FG2(){
	return (bool) HAL_GPIO_ReadPin(TRIGGER_FG2_GPIO, TRIGGER_FG2_PIN);
}


void on_trigger_FG3(){
	HAL_GPIO_WritePin(TRIGGER_FG3_GPIO, TRIGGER_FG3_PIN, GPIO_PIN_SET);
}
void off_trigger_FG3(){
	HAL_GPIO_WritePin(TRIGGER_FG3_GPIO, TRIGGER_FG3_PIN, GPIO_PIN_RESET);
}
void trigger_FG3(){
	on_trigger_FG3();
	osDelay(get_ticks_from_ms(TIME_TRIGGERS_WIDTH));
	off_trigger_FG3();
}
bool get_trigger_FG3(){
	return (bool) HAL_GPIO_ReadPin(TRIGGER_FG3_GPIO, TRIGGER_FG3_PIN);
}


void on_trigger_FG4(){
	HAL_GPIO_WritePin(TRIGGER_FG4_GPIO, TRIGGER_FG4_PIN, GPIO_PIN_SET);
}
void off_trigger_FG4(){
	HAL_GPIO_WritePin(TRIGGER_FG4_GPIO, TRIGGER_FG4_PIN, GPIO_PIN_RESET);
}
void trigger_FG4(){
	on_trigger_FG4();
	osDelay(get_ticks_from_ms(TIME_TRIGGERS_WIDTH));
	off_trigger_FG4();
}
bool get_trigger_FG4(){
	return (bool) HAL_GPIO_ReadPin(TRIGGER_FG4_GPIO, TRIGGER_FG4_PIN);
}


void on_trigger_DISCARD(){
	HAL_GPIO_WritePin(TRIGGER_DISCARD_GPIO, TRIGGER_DISCARD_PIN, GPIO_PIN_SET);
}
void off_trigger_DISCARD(){
	HAL_GPIO_WritePin(TRIGGER_DISCARD_GPIO, TRIGGER_DISCARD_PIN, GPIO_PIN_RESET);
}
void trigger_DISCARD(){
	on_trigger_DISCARD();
	osDelay(get_ticks_from_ms(TIME_TRIGGERS_WIDTH));
	off_trigger_DISCARD();
}
bool get_trigger_DISCARD(){
	return (bool) HAL_GPIO_ReadPin(TRIGGER_DISCARD_GPIO, TRIGGER_DISCARD_PIN);
}
// Simulator OC Event
void trigger_simulator_oc_event(){
	HAL_GPIO_WritePin(TRIGGER_SIMULATOR_OC_GPIO, TRIGGER_SIMULATOR_OC_PIN, GPIO_PIN_SET);
	osDelay(get_ticks_from_ms(TIME_TRIGGERS_WIDTH));
	HAL_GPIO_WritePin(TRIGGER_SIMULATOR_OC_GPIO, TRIGGER_SIMULATOR_OC_PIN, GPIO_PIN_RESET);
}

//INDICATOR FUNCTIONS


void turn_on_Indicator_R(){
	HAL_GPIO_WritePin(INDICATOR_R_GPIO, INDICATOR_R_PIN, GPIO_PIN_SET);
}
void turn_off_Indicator_R(){
	HAL_GPIO_WritePin(INDICATOR_R_GPIO, INDICATOR_R_PIN, GPIO_PIN_RESET);
}

void turn_on_Indicator_G(){
	HAL_GPIO_WritePin(INDICATOR_G_GPIO, INDICATOR_G_PIN, GPIO_PIN_SET);
}
void turn_off_Indicator_G(){
	HAL_GPIO_WritePin(INDICATOR_G_GPIO, INDICATOR_G_PIN, GPIO_PIN_RESET);
}


void turn_on_Indicator_B(){
	HAL_GPIO_WritePin(INDICATOR_B_GPIO, INDICATOR_B_PIN, GPIO_PIN_SET);
}
void turn_off_Indicator_B(){
	HAL_GPIO_WritePin(INDICATOR_B_GPIO, INDICATOR_B_PIN, GPIO_PIN_RESET);
}




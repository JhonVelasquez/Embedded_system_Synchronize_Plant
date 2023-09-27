/*
 * trigger.h
 *
 *  Created on: Jun 24, 2023
 *      Author: jvlsq
 */

#ifndef INC_TRIGGER_H_
#define INC_TRIGGER_H_

#include "main.h"
#include "define.h"
#include <stdbool.h>
#include "cmsis_os.h"
//#include "utils.h"

//TRIGGER FUNCTIONS
void on_trigger_RPI();
void off_trigger_RPI();
void trigger_RPI();
bool get_trigger_RPI();

void on_trigger_KEYENCE();
void off_trigger_KEYENCE();
void trigger_KEYENCE();
bool get_trigger_KEYENCE();

void on_trigger_FG1();
void off_trigger_FG1();
void trigger_FG1();
bool get_trigger_FG1();

void on_trigger_FG2();
void off_trigger_FG2();
void trigger_FG2();
bool get_trigger_FG2();

void on_trigger_FG3();
void off_trigger_FG3();
void trigger_FG3();
bool get_trigger_FG3();

void on_trigger_FG4();
void off_trigger_FG4();
void trigger_FG4();
bool get_trigger_FG4();

void on_trigger_DISCARD();
void off_trigger_DISCARD();
void trigger_DISCARD();
bool get_trigger_DISCARD();
//Simulator OC Event
void trigger_simulator_oc_event();

//INDICATOR FUNCTIONS
void turn_on_Indicator_R();
void turn_off_Indicator_R();

void turn_on_Indicator_G();
void turn_off_Indicator_G();


void turn_on_Indicator_B();
void turn_off_Indicator_B();

#endif /* INC_TRIGGER_H_ */

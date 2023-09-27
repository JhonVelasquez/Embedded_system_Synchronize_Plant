/*
 * define.h
 *
 *  Created on: Jun 27, 2023
 *      Author: jvlsq
 */

#ifndef INC_DEFINE_H_
#define INC_DEFINE_H_

#include <stdbool.h>
///////////////////////////////////////
////////////NOTES//////////////////////
///////////////////////////////////////
//All distances are in millimeters.
//EDM(edit manually on code)


//////////////////////////////////////////////////////
////////////////////SIMULATOR ////////////////
//////////////////////////////////////////////////////
//CONVEYOR POSITION
#define USE_SIMULATOR_CONVEYOR false
// #define SIM_CONV_SPEED 300 // mm/s , not used in code
#define SIM_STEP_DISTANCE 1.5 // mm
#define TIM_SIMUL_STEP 5 // ms, only works if USE_SIMULATOR_CONVEYOR is true

//THE PERIODIC OPTICAL CURTAIN EVENT SIMULATION
// PF13
#define TRIGGER_SIMULATOR_OC_GPIO GPIOF
#define TRIGGER_SIMULATOR_OC_PIN GPIO_PIN_13
#define TIM_OC_EVENTS_SIM 255 // ms
#define USE_OC_EVENTS_SIM false

// THE ENCODER CHANNELS SIMULATION
// using TIMER 1 - PE13
// using TIMER 3 - PB4

//////////////////////////////////////////////////////
////////////////////TCP ADDRESSES/////////////////////
//////////////////////////////////////////////////////
#define ENABLE_TCP_CONNECTION true
#define ENABLE_TCP_CONNECTION_RPI true
#define ENABLE_TCP_CONNECTION_KEYENCE true
#define ENABLE_TCP_CONNECTION_FG1 true


# define RPI_IP4_1 192
# define RPI_IP4_2 168
# define RPI_IP4_3 0
# define RPI_IP4_4 103 //102
# define RPI_DEST_PORT 10
# define RPI_SOURC_PORT 102

# define KEYENCE_IP4_1 192
# define KEYENCE_IP4_2 168
# define KEYENCE_IP4_3 0 //1
# define KEYENCE_IP4_4 103 //21,103
# define KEYENCE_DEST_PORT 11//2000
# define KEYENCE_SOURC_PORT 103

# define FG1_IP4_1 192
# define FG1_IP4_2 168
# define FG1_IP4_3 0
# define FG1_IP4_4 103 //103
# define FG1_DEST_PORT 13
# define FG1_SOURC_PORT 100


///////////////////////////////////////////
//////////CONVEYOR DISTANCES////////////
///////////////////////////////////////////

#define TOLERANCE_TRG 0//mm

#define RPI_POS 550
#define KEYENCE_POS 1320
#define FG1_POS 1750
#define FG2_POS 1860
#define FG3_POS 1990
#define FG4_POS 2150
#define DISCARD_POS 2300

#define DEAD_DISTANCE 420 // Minimum distance between valid artifacts



//#define LENGTH_COVEYOR 2600 // missing
//#define MAX_NUM_OBJ_CONV 6 // missing
#define DISTANCE_SEND_RECIPE_EARLIER 100//mm
//#define DISTANCE_SEND_COMPUTER_EARLIER 100//mm
#define DIS_RELEASE_RPI 180 // half of DEAD_DISTANCE so the thread have time to prepare for next object
#define DIS_RELEASE_KEYENCE 180 // half of DEAD_DISTANCE

//////////////////////////////////////////
////////Timings, delays, in threads///////
//////////////////////////////////////////
#define TIM_PRINT_OPT_POS 100 // Time osDelay for  Task defaultTask, and printing conveyor position

# define TIM_SLEEP_COMMAND_H 100 // Time osDelay for Task t_command_handler
# define TIM_SLEEP_SEND_TCP 5 // Time osDelay for Task t_send_tcp
# define TIM_SLEEP_RECV_TCP 5 // Time osDelay for Task t_recv_tcp

							// Tasks: t_conn_tcp_RPI, t_conn_tcp_KEYENCE, and t_conn_tcp_FG1
# define TIM_TCP_SLEEP_ATTEMP 500  // Time osDelay for periodic initialization of TCP connection
# define TIM_TCP_SLEEP_CHECK 200  // Time osDelay for periodic check if there is connection
# define TIM_TCP_SLEEP_TIMEOUT 3000 // Time allowed a connection is lost, when it´s over. New Connection attemp happens.

# define TIM_SLEEP_DEBUG_MSSGS 20 // Time osDelay for Task t_debug_mssgs

# define TIM_SLEEP_CURTAIN 3  // Time osDelay for Task t_CURTAIN

//#define TIM_WATCH_QUEUES 3 //ms, time for timer_watch_queues
#define TIM_UPDT_CSCD_SOFT 3 // time for timer  timer_update_encoder, only works if USE_CASCADE_BY_SOFTWARE is false

# define TIM_SLEEP_QUEUES 3 //ms, time for timer_watch_queues

///////////////////////////////////////////////////
/////////////////////GPIO//////////////////////////
///////////////////////////////////////////////////
#define TIME_TRIGGERS_WIDTH 1 //ms


//INPUTS
//#define OPTICAL_CURTAIN_GPIO GPIOB
//#define OPTICAL_CURTAIN_PIN GPIO_PIN_14

//#define ENCODER_CH1_GPIO GPIOD
//#define ENCODER_CH1_PIN GPIO_PIN_12

//#define ENCODER_CH2_GPIO GPIOD
//#define ENCODER_CH2_PIN GPIO_PIN_13

// TRIGGERS
#define TRIGGER_RPI_GPIO GPIOE
#define TRIGGER_RPI_PIN GPIO_PIN_15

#define TRIGGER_KEYENCE_GPIO GPIOE
#define TRIGGER_KEYENCE_PIN GPIO_PIN_14

#define TRIGGER_FG1_GPIO GPIOE
#define TRIGGER_FG1_PIN GPIO_PIN_12

#define TRIGGER_FG2_GPIO GPIOE
#define TRIGGER_FG2_PIN GPIO_PIN_11

#define TRIGGER_FG3_GPIO GPIOE
#define TRIGGER_FG3_PIN GPIO_PIN_10

#define TRIGGER_FG4_GPIO GPIOE
#define TRIGGER_FG4_PIN GPIO_PIN_9

#define TRIGGER_DISCARD_GPIO GPIOE
#define TRIGGER_DISCARD_PIN GPIO_PIN_8

// INT EVENT
#define OC_EXTI_PIN GPIO_PIN_11 //check the datasheet or Configuration Interface

//INDICATORS
#define INDICATOR_R_GPIO GPIOB
#define INDICATOR_R_PIN GPIO_PIN_0

#define INDICATOR_G_GPIO GPIOB
#define INDICATOR_G_PIN GPIO_PIN_1

#define INDICATOR_B_GPIO GPIOB
#define INDICATOR_B_PIN GPIO_PIN_2

//////////////////////////////////////////////////////
////////////////////ENCODER///////////////////////////
//////////////////////////////////////////////////////

#define ENCODER_INTERFACE_RES_STM 65536
//#define ENCODER_RESOLUTION_QUADRATURE_TICKS 60000 // quadrature TICKS in one rotation
//#define ENCODER_MM_PER_ROTATION 39.7887 // mm in one rotation, DIAMETER OF ENCODER
#define ENCODER_TICK_PER_MM 480.00043155 // 60000/(39.7887*pi)
											// linea max speed 30cm/s == 7 times overloading per second (16 bits) quadrature
											// so it needs at least 21 times check overload per second
											// 50 times revision == 20ms

#define USE_CASCADE_BY_SOFTWARE true// Cascade counter by software counts overflows and underflows. If it is false
											// then cascade counter by TIMER is used, which only uses overflow counter.
											// This last one is likely to be more precise iving that it is done by hardware.

//////////////////////////////////////////////////////
////////////////////MESSAGES//////////////////////////
//////////////////////////////////////////////////////

#define MAX_MESSSAGE_SIZE 100 //linked to debugging messages, lower than limit of UART DEBUG
#define MAX_STATIONS_TCP 50 //classifier, recipe, orientation, computer

#define TX_SIZE_UART_DEBUG 110
#define RX_SIZE_UART_DEBUG 90

#define BUFFER_TCP_RX_SIZE 60
#define BUFFER_TCP_TX_SIZE 60
#define COMMAND_HANDLER_START '<'
#define COMMAND_HANDLER_DELIMITER '/'
#define COMMAND_HANDLER_DELIMITER_STR "/"

#define COMMAND_HANDLER_END '>'

#define USE_USB true
#define USE_UART_EXT true //UART_3
#define USE_UART_CM4_MAIN true //UART_5
#define USE_UART_CM4_SECN false //UART_6

////////////////////////////////////////////////////////
/////////////////////DEBUG//////////////////////////////
////////////////////////////////////////////////////////
# define USE_ACK_CONVEYOR_POS true; // Enable conveyor displacement printing
# define ENABLE_QUEUE_RESULT false
# define ENABLE_TCP_MSSGS false
//# define ENABLE_ECHO_MSSGS false
//# define ENABLE_ERROR_MSSGS false
//# define ENABLE_SUCCESS_MSSGS false
//# define ENABLE_ACK_MSSGS false
//# define ENABLE_TCP_MSSGS false
//# define ENABLE_PROCESS_MSSGS false


#endif /* INC_DEFINE_H_ */

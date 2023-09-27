/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "define.h"
#include "utils.h"
#include "stdio.h"
#include "tagging.h"
#include "trigger.h"

//tcp
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"
//#include "tcpclient.h"
#include "string.h"
#include "stdbool.h"
#include "tcpbase.h"
#include "tcp.h"
#include "usbd_cdc_if.h"

#include "structures.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

////////////////////////////
/*GENERAL VARIABLES*/
////////////////////////////



bool use_simulator = USE_SIMULATOR_CONVEYOR;
//Simulator conveyor position
float last_optical_curtain = 0; //last optical recorded position
bool en_ackn_pos = USE_ACK_CONVEYOR_POS; //enable optical curtain debug
float sim_opt_curt_mm = 0; //simulated value

//Simulator OC events
bool en_period_oc_event_sim = USE_OC_EVENTS_SIM; //enable optical curtain debug
bool en_period_oc_print = false;
bool en_period_oc_event_sim_last = USE_OC_EVENTS_SIM;

//Task t_update_encoder
int16_t tick_encoder_var = 0;
int16_t last_tick_encoder = 0;

int32_t counter_cascade_by_softw = 0; // implemented by software counts overflow and underflow
int32_t counter_cascade_by_timer = 0; // implemented by TIM slave, it counts only passing by overflow


/*GENERAL*/
float get_current_conveyor_pos();

//////////////
/*DEAULT TASK*/
//////////////


//////////////
/*STATIONS*/
//////////////

//general
typedef struct {
	uint8_t classifier[MAX_STATIONS_TCP];
	uint8_t orientation[MAX_STATIONS_TCP];
	float optical_curtain;
} queue_inter_stations;

typedef struct {
	char msg[MAX_STATIONS_TCP];
} queue_stations_tcp;

float count_discard = 0;
float count_done = 0;
float count_total_OC_events = 0;
float count_total_OC_trigger_sim = 0;

float sim_per_oc_start_pos =0;
float sim_per_oc_end_pos =0;
/*RPI*/
void send_to_CLASSIFIER_queue(char * value);
/*KEYENCE*/
void send_to_ORIENTATION_queue(char * value);

//////////////
/*TCP*/
//////////////

typedef struct {
	uint8_t msg[BUFFER_TCP_TX_SIZE];
	struct netconn *conn;
	char * point_conn_name;
} queue_tx_rx_tcp;

static struct netconn *conn_RPI;
static struct netconn *conn_KEYENCE;
static struct netconn *conn_FG1;

char smsgc_rx_tcp_buff_debug[MAX_MESSSAGE_SIZE];
char smsgc_tx_tcp_buff_debug[MAX_MESSSAGE_SIZE];
extern ip4_addr_t ipaddr;


const char str_RPI[] = "RPI";
const char str_KEYENCE[] = "KEYENCE";
const char str_FG1[] = "FG1";

/*SEND_TCP*/
osStatus_t send_tcp_to_station(char* value, char * str_station_name, struct netconn *conn_var);
void send_tcp_to_RPI(char* value);
void send_tcp_to_KEYENCE(char* value);
void send_tcp_to_FG1(char* value);

/*RECV_TCP*/
void recv_tcp(char * str_conn_name, char * msgc, struct netbuf *buf, struct netconn *conn);

/*CONN_MONITOR_TCP*/
void conn_init_monitor_tcp(char * str_conn_name, struct netconn **pointer_conn, ip4_addr_t* sourc_addr, unsigned short sourc_port, ip4_addr_t dest_addr, unsigned short dest_port);


////////////////////////////
/* CONVEYOR POSITION */
////////////////////////////
void callback_trigger_optical_curtain();
void callback_trigger_optical_curtain_IT();
void evaluate_optical_curtain();
bool fixed_value_optical_curtain=true;

double conveyor_pos=0;

////////////////////////////
/* UART COMMANDS DEBUG*/
////////////////////////////

//external HAL_UART_RxCpltCallback
uint8_t rxBuffer;
UART_HandleTypeDef* huart_CLI_debug;
UART_HandleTypeDef* huart_CLI_CM4_main;
UART_HandleTypeDef* huart_CLI_CM4_second;

//local HAL_UART_RxCpltCallback
uint8_t state_rx=0;
uint8_t cnt_rx;
queue_commandRx queueCommandRx;



//extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t rxBuffer_USB;
cli_char_buffer_handler cli_uart_debug;
cli_char_buffer_handler cli_uart_CM4_main;
cli_char_buffer_handler cli_uart_CM4_second;
cli_char_buffer_handler cli_usb;



//external debug_send_.....
typedef struct{
	char content[TX_SIZE_UART_DEBUG];
	char * type;
} message_debug;

const char str_start_mssg = COMMAND_HANDLER_START;
const char str_delimiter_mssg = COMMAND_HANDLER_DELIMITER;
const char str_end_mssg = COMMAND_HANDLER_END;

//local debug_send_.....
const char str_echo[] = "ECHO";
const char str_error[] = "ERRO";
const char str_success[] = "SCSS";
const char str_ack[] = "ACKN";
const char str_n_r[] = "\n\r";
int str_n_r_size = strlen(str_n_r);
const char str_tcp[] = "TCPC";


char var_debug_send_optical[20];

///////////////////////////////////
/*DEBUG_MESSAGES*/
///////////////////////////////////
void debug_send_echo(char * txBuffer);
void debug_send_error(char * txBuffer);
void debug_send_success(char * txBuffer);
void debug_send_ack(char * txBuffer);
void debug_send_tcp(char * txBuffer);
void debug_tcp_connection_state(uint8_t state);
void debug_message_sent_tcp(uint8_t state, queue_tx_rx_tcp *ms);
void debug_message_recv_tcp(char *str_conn_name, char *msgc);
void debug_init_monitor_tcp(char * str_conn_name);
void debug_lost_monitor_tcp(char * str_conn_name);
void debug_send_queue_result(osStatus_t r, char * txBuffer);
void debug_send_process(char * str_type,float optical_curtain_id, float rel_dist, char * txBuffer);
void debug_send_queue_process_result(osStatus_t r, float starting_pos_local, float relative_pos_local, char * txBuffer);
void debug_send_optical_position(float t);
void debug_send_done_discard();
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart5;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for t_RPI_watch */
osThreadId_t t_RPI_watchHandle;
const osThreadAttr_t t_RPI_watch_attributes = {
  .name = "t_RPI_watch",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for t_KEYENCE_watch */
osThreadId_t t_KEYENCE_watchHandle;
const osThreadAttr_t t_KEYENCE_watch_attributes = {
  .name = "t_KEYENCE_watch",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for t_FG1_wach */
osThreadId_t t_FG1_wachHandle;
const osThreadAttr_t t_FG1_wach_attributes = {
  .name = "t_FG1_wach",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for t_FG2_wach */
osThreadId_t t_FG2_wachHandle;
const osThreadAttr_t t_FG2_wach_attributes = {
  .name = "t_FG2_wach",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for t_FG3_wach */
osThreadId_t t_FG3_wachHandle;
const osThreadAttr_t t_FG3_wach_attributes = {
  .name = "t_FG3_wach",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for t_FG4_wach */
osThreadId_t t_FG4_wachHandle;
const osThreadAttr_t t_FG4_wach_attributes = {
  .name = "t_FG4_wach",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for t_DISCARD */
osThreadId_t t_DISCARDHandle;
const osThreadAttr_t t_DISCARD_attributes = {
  .name = "t_DISCARD",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for t_command_hand */
osThreadId_t t_command_handHandle;
const osThreadAttr_t t_command_hand_attributes = {
  .name = "t_command_hand",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for t_send_tcp */
osThreadId_t t_send_tcpHandle;
const osThreadAttr_t t_send_tcp_attributes = {
  .name = "t_send_tcp",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for t_recv_tcp */
osThreadId_t t_recv_tcpHandle;
const osThreadAttr_t t_recv_tcp_attributes = {
  .name = "t_recv_tcp",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for t_conn_tcp_RPI */
osThreadId_t t_conn_tcp_RPIHandle;
const osThreadAttr_t t_conn_tcp_RPI_attributes = {
  .name = "t_conn_tcp_RPI",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for t_conn_tcp_KEYENCE */
osThreadId_t t_conn_tcp_KEYENCEHandle;
const osThreadAttr_t t_conn_tcp_KEYENCE_attributes = {
  .name = "t_conn_tcp_KEYENCE",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for t_conn_tcp_FG1 */
osThreadId_t t_conn_tcp_FG1Handle;
const osThreadAttr_t t_conn_tcp_FG1_attributes = {
  .name = "t_conn_tcp_FG1",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for t_simulate_conveyor */
osThreadId_t t_simulate_conveyorHandle;
const osThreadAttr_t t_simulate_conveyor_attributes = {
  .name = "t_simulate_conveyor",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for t_debug_mssgs */
osThreadId_t t_debug_mssgsHandle;
const osThreadAttr_t t_debug_mssgs_attributes = {
  .name = "t_debug_mssgs",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for t_CURTAIN */
osThreadId_t t_CURTAINHandle;
const osThreadAttr_t t_CURTAIN_attributes = {
  .name = "t_CURTAIN",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal7,
};
/* Definitions for t_update_encoder */
osThreadId_t t_update_encoderHandle;
const osThreadAttr_t t_update_encoder_attributes = {
  .name = "t_update_encoder",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for t_simulate_periodic */
osThreadId_t t_simulate_periodicHandle;
const osThreadAttr_t t_simulate_periodic_attributes = {
  .name = "t_simulate_periodic",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for command_rx_queue */
osMessageQueueId_t command_rx_queueHandle;
const osMessageQueueAttr_t command_rx_queue_attributes = {
  .name = "command_rx_queue"
};
/* Definitions for RPI_queue */
osMessageQueueId_t RPI_queueHandle;
const osMessageQueueAttr_t RPI_queue_attributes = {
  .name = "RPI_queue"
};
/* Definitions for KEYENCE_queue */
osMessageQueueId_t KEYENCE_queueHandle;
const osMessageQueueAttr_t KEYENCE_queue_attributes = {
  .name = "KEYENCE_queue"
};
/* Definitions for FG1_queue */
osMessageQueueId_t FG1_queueHandle;
const osMessageQueueAttr_t FG1_queue_attributes = {
  .name = "FG1_queue"
};
/* Definitions for classifier_queue */
osMessageQueueId_t classifier_queueHandle;
const osMessageQueueAttr_t classifier_queue_attributes = {
  .name = "classifier_queue"
};
/* Definitions for orientation_queue */
osMessageQueueId_t orientation_queueHandle;
const osMessageQueueAttr_t orientation_queue_attributes = {
  .name = "orientation_queue"
};
/* Definitions for DISCARD_queue */
osMessageQueueId_t DISCARD_queueHandle;
const osMessageQueueAttr_t DISCARD_queue_attributes = {
  .name = "DISCARD_queue"
};
/* Definitions for message_debug_queue */
osMessageQueueId_t message_debug_queueHandle;
const osMessageQueueAttr_t message_debug_queue_attributes = {
  .name = "message_debug_queue"
};
/* Definitions for FG2_queue */
osMessageQueueId_t FG2_queueHandle;
const osMessageQueueAttr_t FG2_queue_attributes = {
  .name = "FG2_queue"
};
/* Definitions for FG3_queue */
osMessageQueueId_t FG3_queueHandle;
const osMessageQueueAttr_t FG3_queue_attributes = {
  .name = "FG3_queue"
};
/* Definitions for FG4_queue */
osMessageQueueId_t FG4_queueHandle;
const osMessageQueueAttr_t FG4_queue_attributes = {
  .name = "FG4_queue"
};
/* Definitions for tx_tcp_queue */
osMessageQueueId_t tx_tcp_queueHandle;
const osMessageQueueAttr_t tx_tcp_queue_attributes = {
  .name = "tx_tcp_queue"
};
/* Definitions for CURTAIN_queue */
osMessageQueueId_t CURTAIN_queueHandle;
const osMessageQueueAttr_t CURTAIN_queue_attributes = {
  .name = "CURTAIN_queue"
};
/* Definitions for timer_simulate_conveyor */
osTimerId_t timer_simulate_conveyorHandle;
const osTimerAttr_t timer_simulate_conveyor_attributes = {
  .name = "timer_simulate_conveyor"
};
/* Definitions for timer_watch_queues */
osTimerId_t timer_watch_queuesHandle;
const osTimerAttr_t timer_watch_queues_attributes = {
  .name = "timer_watch_queues"
};
/* Definitions for timer_update_encoder */
osTimerId_t timer_update_encoderHandle;
const osTimerAttr_t timer_update_encoder_attributes = {
  .name = "timer_update_encoder"
};
/* Definitions for timer_simulate_periodic_oc */
osTimerId_t timer_simulate_periodic_ocHandle;
const osTimerAttr_t timer_simulate_periodic_oc_attributes = {
  .name = "timer_simulate_periodic_oc"
};
/* Definitions for mutex_sample_conveyor */
osMutexId_t mutex_sample_conveyorHandle;
const osMutexAttr_t mutex_sample_conveyor_attributes = {
  .name = "mutex_sample_conveyor"
};
/* Definitions for mutex_encoder_hardware */
osMutexId_t mutex_encoder_hardwareHandle;
const osMutexAttr_t mutex_encoder_hardware_attributes = {
  .name = "mutex_encoder_hardware"
};
/* Definitions for mutex_encoder_cascade */
osMutexId_t mutex_encoder_cascadeHandle;
const osMutexAttr_t mutex_encoder_cascade_attributes = {
  .name = "mutex_encoder_cascade"
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART5_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
void StartDefaultTask(void *argument);
void f_RPI_watch(void *argument);
void f_KEYENCE_watch(void *argument);
void f_FG1_wach(void *argument);
void f_FG2_wach(void *argument);
void f_FG3_wach(void *argument);
void f_FG4_wach(void *argument);
void f_DISCARD(void *argument);
void f_command_handl(void *argument);
void f_send_tcp(void *argument);
void f_recv_tcp(void *argument);
void f_conn_tcp_RPI(void *argument);
void f_conn_tcp_KEYENCE(void *argument);
void f_conn_tcp_FG1(void *argument);
void f_simulate_conveyor(void *argument);
void f_debug_mssgs(void *argument);
void f_CURTAIN(void *argument);
void f_update_encoder(void *argument);
void f_simulate_periodic(void *argument);
void callback_timer_simulate_conveyor(void *argument);
void callback_timer_watch_queues(void *argument);
void callback_timer_update_encoder(void *argument);
void callback_timer_simulate_periodic_oc(void *argument);

/* USER CODE BEGIN PFP */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == OC_EXTI_PIN)
    {
    	callback_trigger_optical_curtain_IT();
    }
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART5_Init();
  MX_USART6_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  huart_CLI_debug = &huart3;
  huart_CLI_CM4_main = &huart5;
  huart_CLI_CM4_second = &huart6;

  if(USE_UART_EXT) HAL_UART_Receive_IT(huart_CLI_debug, (uint8_t*) &(cli_uart_debug.rxBuffer), 1);
  if(USE_UART_CM4_MAIN) HAL_UART_Receive_IT(huart_CLI_CM4_main, (uint8_t*) &(cli_uart_CM4_main.rxBuffer), 1);
  if(USE_UART_CM4_SECN) HAL_UART_Receive_IT(huart_CLI_CM4_second, (uint8_t*) &(cli_uart_CM4_second.rxBuffer), 1);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of mutex_sample_conveyor */
  mutex_sample_conveyorHandle = osMutexNew(&mutex_sample_conveyor_attributes);

  /* creation of mutex_encoder_hardware */
  mutex_encoder_hardwareHandle = osMutexNew(&mutex_encoder_hardware_attributes);

  /* creation of mutex_encoder_cascade */
  mutex_encoder_cascadeHandle = osMutexNew(&mutex_encoder_cascade_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of timer_simulate_conveyor */
  timer_simulate_conveyorHandle = osTimerNew(callback_timer_simulate_conveyor, osTimerPeriodic, NULL, &timer_simulate_conveyor_attributes);

  /* creation of timer_watch_queues */
  timer_watch_queuesHandle = osTimerNew(callback_timer_watch_queues, osTimerPeriodic, NULL, &timer_watch_queues_attributes);

  /* creation of timer_update_encoder */
  timer_update_encoderHandle = osTimerNew(callback_timer_update_encoder, osTimerPeriodic, NULL, &timer_update_encoder_attributes);

  /* creation of timer_simulate_periodic_oc */
  timer_simulate_periodic_ocHandle = osTimerNew(callback_timer_simulate_periodic_oc, osTimerPeriodic, NULL, &timer_simulate_periodic_oc_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of command_rx_queue */
  command_rx_queueHandle = osMessageQueueNew (30, sizeof(queue_commandRx), &command_rx_queue_attributes);

  /* creation of RPI_queue */
  RPI_queueHandle = osMessageQueueNew (3, sizeof(float), &RPI_queue_attributes);

  /* creation of KEYENCE_queue */
  KEYENCE_queueHandle = osMessageQueueNew (4, sizeof(queue_inter_stations), &KEYENCE_queue_attributes);

  /* creation of FG1_queue */
  FG1_queueHandle = osMessageQueueNew (4, sizeof(queue_inter_stations), &FG1_queue_attributes);

  /* creation of classifier_queue */
  classifier_queueHandle = osMessageQueueNew (3, sizeof(queue_stations_tcp), &classifier_queue_attributes);

  /* creation of orientation_queue */
  orientation_queueHandle = osMessageQueueNew (3, sizeof(queue_stations_tcp), &orientation_queue_attributes);

  /* creation of DISCARD_queue */
  DISCARD_queueHandle = osMessageQueueNew (200, sizeof(float), &DISCARD_queue_attributes);

  /* creation of message_debug_queue */
  message_debug_queueHandle = osMessageQueueNew (50, sizeof(message_debug), &message_debug_queue_attributes);

  /* creation of FG2_queue */
  FG2_queueHandle = osMessageQueueNew (3, sizeof(float), &FG2_queue_attributes);

  /* creation of FG3_queue */
  FG3_queueHandle = osMessageQueueNew (3, sizeof(float), &FG3_queue_attributes);

  /* creation of FG4_queue */
  FG4_queueHandle = osMessageQueueNew (3, sizeof(float), &FG4_queue_attributes);

  /* creation of tx_tcp_queue */
  tx_tcp_queueHandle = osMessageQueueNew (10, sizeof(queue_tx_rx_tcp), &tx_tcp_queue_attributes);

  /* creation of CURTAIN_queue */
  CURTAIN_queueHandle = osMessageQueueNew (60, sizeof(bool), &CURTAIN_queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of t_RPI_watch */
  t_RPI_watchHandle = osThreadNew(f_RPI_watch, NULL, &t_RPI_watch_attributes);

  /* creation of t_KEYENCE_watch */
  t_KEYENCE_watchHandle = osThreadNew(f_KEYENCE_watch, NULL, &t_KEYENCE_watch_attributes);

  /* creation of t_FG1_wach */
  t_FG1_wachHandle = osThreadNew(f_FG1_wach, NULL, &t_FG1_wach_attributes);

  /* creation of t_FG2_wach */
  t_FG2_wachHandle = osThreadNew(f_FG2_wach, NULL, &t_FG2_wach_attributes);

  /* creation of t_FG3_wach */
  t_FG3_wachHandle = osThreadNew(f_FG3_wach, NULL, &t_FG3_wach_attributes);

  /* creation of t_FG4_wach */
  t_FG4_wachHandle = osThreadNew(f_FG4_wach, NULL, &t_FG4_wach_attributes);

  /* creation of t_DISCARD */
  t_DISCARDHandle = osThreadNew(f_DISCARD, NULL, &t_DISCARD_attributes);

  /* creation of t_command_hand */
  t_command_handHandle = osThreadNew(f_command_handl, NULL, &t_command_hand_attributes);

  /* creation of t_send_tcp */
  t_send_tcpHandle = osThreadNew(f_send_tcp, NULL, &t_send_tcp_attributes);

  /* creation of t_recv_tcp */
  t_recv_tcpHandle = osThreadNew(f_recv_tcp, NULL, &t_recv_tcp_attributes);

  /* creation of t_conn_tcp_RPI */
  t_conn_tcp_RPIHandle = osThreadNew(f_conn_tcp_RPI, NULL, &t_conn_tcp_RPI_attributes);

  /* creation of t_conn_tcp_KEYENCE */
  t_conn_tcp_KEYENCEHandle = osThreadNew(f_conn_tcp_KEYENCE, NULL, &t_conn_tcp_KEYENCE_attributes);

  /* creation of t_conn_tcp_FG1 */
  t_conn_tcp_FG1Handle = osThreadNew(f_conn_tcp_FG1, NULL, &t_conn_tcp_FG1_attributes);

  /* creation of t_simulate_conveyor */
  t_simulate_conveyorHandle = osThreadNew(f_simulate_conveyor, NULL, &t_simulate_conveyor_attributes);

  /* creation of t_debug_mssgs */
  t_debug_mssgsHandle = osThreadNew(f_debug_mssgs, NULL, &t_debug_mssgs_attributes);

  /* creation of t_CURTAIN */
  t_CURTAINHandle = osThreadNew(f_CURTAIN, NULL, &t_CURTAIN_attributes);

  /* creation of t_update_encoder */
  t_update_encoderHandle = osThreadNew(f_update_encoder, NULL, &t_update_encoder_attributes);

  /* creation of t_simulate_periodic */
  t_simulate_periodicHandle = osThreadNew(f_simulate_periodic, NULL, &t_simulate_periodic_attributes);

  /* USER CODE BEGIN RTOS_THREADS */


	//Encoder
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start(&htim5);
	__HAL_TIM_SET_COUNTER(&htim4,0);
	__HAL_TIM_SET_COUNTER(&htim5,0);
	if(USE_CASCADE_BY_SOFTWARE) osTimerStart(timer_update_encoderHandle,get_ticks_from_ms(TIM_UPDT_CSCD_SOFT));

	//tcpclient_init();
	if (USE_SIMULATOR_CONVEYOR)osTimerStart(timer_simulate_conveyorHandle, get_ticks_from_ms(TIM_SIMUL_STEP)); // SIM_STEP_TIME en ms
	//osTimerStart(timer_watch_queuesHandle, get_ticks_from_ms(TIM_SLEEP_QUEUES));
	vTaskResume(t_command_handHandle);

	//Simulator_periodic
	osTimerStart(timer_simulate_periodic_ocHandle, get_ms_from_ticks(get_ticks_from_ms(TIM_OC_EVENTS_SIM)));

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 5-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 377-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC2REF;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_ACTIVE;
  sConfigOC.Pulse = 94;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 188;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 5-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 377-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  if (HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 188;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_ITR2;
  if (HAL_TIM_SlaveConfigSynchro(&htim5, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, TRIGGER_xxV_12_Pin|TRIGGER_xxV_11_Pin|TRIGGER_xxV_10_Pin|TRIGGER_xxV_9_Pin
                          |TRIGGER_24V_8_Pin|TRIGGER_24V_7_Pin|TRIGGER_24V_6_Pin|TRIGGER_24V_5_Pin
                          |TRIGGER_24V_4_Pin|TRIGGER_24V_2_Pin|TRIGGER_24V_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, INDICATOR_R_Pin|INDICATOR_G_Pin|INDICATOR_B_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SIMULATE_OPTICAL_CURTAIN_GPIO_Port, SIMULATE_OPTICAL_CURTAIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : TRIGGER_xxV_12_Pin TRIGGER_xxV_11_Pin TRIGGER_xxV_10_Pin TRIGGER_xxV_9_Pin
                           TRIGGER_24V_8_Pin TRIGGER_24V_7_Pin TRIGGER_24V_6_Pin TRIGGER_24V_5_Pin
                           TRIGGER_24V_4_Pin TRIGGER_24V_2_Pin TRIGGER_24V_1_Pin */
  GPIO_InitStruct.Pin = TRIGGER_xxV_12_Pin|TRIGGER_xxV_11_Pin|TRIGGER_xxV_10_Pin|TRIGGER_xxV_9_Pin
                          |TRIGGER_24V_8_Pin|TRIGGER_24V_7_Pin|TRIGGER_24V_6_Pin|TRIGGER_24V_5_Pin
                          |TRIGGER_24V_4_Pin|TRIGGER_24V_2_Pin|TRIGGER_24V_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : INDICATOR_R_Pin INDICATOR_G_Pin INDICATOR_B_Pin */
  GPIO_InitStruct.Pin = INDICATOR_R_Pin|INDICATOR_G_Pin|INDICATOR_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SIMULATE_OPTICAL_CURTAIN_Pin */
  GPIO_InitStruct.Pin = SIMULATE_OPTICAL_CURTAIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SIMULATE_OPTICAL_CURTAIN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_24V_1_Pin ENC_24V_2_Pin */
  GPIO_InitStruct.Pin = ENC_24V_1_Pin|ENC_24V_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_24V_3_Pin ENC_24V_4_Pin */
  GPIO_InitStruct.Pin = ENC_24V_3_Pin|ENC_24V_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */




/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();

  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 5 */

  cli_uart_debug.state_rx=0;
  cli_usb.state_rx=0;
  //USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &rxBuffer_USB);
  //USBD_CDC_ReceivePacket(&hUsbDeviceFS);

  if(ENABLE_TCP_CONNECTION_RPI) vTaskResume(t_conn_tcp_RPIHandle);
  if(ENABLE_TCP_CONNECTION_KEYENCE) vTaskResume(t_conn_tcp_KEYENCEHandle);
  if(ENABLE_TCP_CONNECTION_FG1)  vTaskResume(t_conn_tcp_FG1Handle);
  if(ENABLE_TCP_CONNECTION) vTaskResume(t_send_tcpHandle);
  if(ENABLE_TCP_CONNECTION) vTaskResume(t_recv_tcpHandle);

  unsigned long time_print_pos_op_now = xTaskGetTickCount();
  unsigned long time_print_pos_op_previous = time_print_pos_op_now;

  //unsigned long time_oc_event_now = xTaskGetTickCount();
  //unsigned long time_oc_event_previous = time_print_pos_op_now;

  unsigned long time_trigger_simul_optic = time_print_pos_op_now;
  bool optic_triggered_sim = false;

  /* Infinite loop */
  for(;;)
  {

	if(en_ackn_pos)
	{
		time_print_pos_op_now = xTaskGetTickCount();
		if(((time_print_pos_op_now - time_print_pos_op_previous)>TIM_PRINT_OPT_POS))
		{
			float t = get_current_conveyor_pos();
			debug_send_optical_position(t);
			time_print_pos_op_previous = time_print_pos_op_now;
		}
	}

	//SIMULATE PERIODIC
//	if(en_period_oc_event_sim)
//	{
//		time_oc_event_now = xTaskGetTickCount();
//		if(((time_oc_event_now - time_oc_event_previous)>TIM_OC_EVENTS_SIM))
//		{
//			trigger_simulator_oc_event();
//			time_oc_event_previous = time_oc_event_now;
//			count_total_OC_trigger_sim++;
//		}
//	}

	//START THE ENCODER CHANNELS SIMULATION
	if( !optic_triggered_sim){

		time_print_pos_op_now = xTaskGetTickCount();
		if(((time_print_pos_op_now - time_trigger_simul_optic)>1000)){
			//Simulator Encoder High frequency, starts once
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
			osDelay(get_ticks_from_ms(20));
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
			optic_triggered_sim = true;
		}
	}
	osDelay(get_ticks_from_ms(50));
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_f_RPI_watch */

void send_to_CLASSIFIER_queue(char * value){
	queue_stations_tcp local_var;
	strcpy((char *)&local_var.msg, value);
	osStatus_t r = osMessageQueuePut(classifier_queueHandle, &local_var, (uint8_t)0, 2);
	if(ENABLE_QUEUE_RESULT)debug_send_queue_result (r, "CLASSIFIER_queue_tx");
}

/* USER CODE END Header_f_RPI_watch */
void f_RPI_watch(void *argument)
{
  /* USER CODE BEGIN f_RPI_watch */
  queue_stations_tcp local_queue_class;
  queue_inter_stations local_queue_RPI_KEY;
  float starting_pos_local=0;
  float relative_pos_local=0;
  int state = 0;
  float reusable_optical_curtain;
  osStatus_t status;

  /* Infinite loop */
  for(;;)
  {
	//vTaskSuspend(NULL);
	osDelay(get_ticks_from_ms(TIM_SLEEP_QUEUES));
	if(state == 0){
		status = osMessageQueueGet(RPI_queueHandle, &starting_pos_local, (uint8_t *)NULL, 0);
		if (status == osOK){
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - starting_pos_local;
			if(ENABLE_QUEUE_RESULT)debug_send_process((char*)&str_success, starting_pos_local, relative_pos_local, (char*)"RPI_queue_rx");
			state = 1;
		}
	}else if(state == 1){
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - starting_pos_local;
		if(relative_pos_local>(RPI_POS-TOLERANCE_TRG)){
			trigger_RPI();
			debug_send_process((char*)&str_ack, starting_pos_local, relative_pos_local, (char*)"TRIGGER_RPI_pulse");

			send_tcp_to_RPI("TRIGGER_RPI_pulse"); // Used only for testing, with simulator

			state = 2;
		}
	}else if (state == 2){
		status = osMessageQueueGet(classifier_queueHandle, &local_queue_class, (uint8_t *)NULL, 0);
		if (status == osOK){
			state = 0;
			strcpy((char*)&local_queue_RPI_KEY.classifier, (char*)&local_queue_class.msg);
			local_queue_RPI_KEY.optical_curtain = starting_pos_local;
			osStatus_t r = osMessageQueuePut(KEYENCE_queueHandle, &local_queue_RPI_KEY, (uint8_t)0, 2);
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - starting_pos_local;
			if(ENABLE_QUEUE_RESULT)debug_send_queue_process_result(r, starting_pos_local, relative_pos_local, (char*)"KEYENCE_queue_tx");
		} else {
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - starting_pos_local;
			if (relative_pos_local> (RPI_POS + DIS_RELEASE_RPI)){
				state = 0;
				osStatus_t r = osMessageQueuePut(DISCARD_queueHandle, &starting_pos_local, (uint8_t)0, 2);
				reusable_optical_curtain = get_current_conveyor_pos();
				relative_pos_local =  reusable_optical_curtain - starting_pos_local;
				if(ENABLE_QUEUE_RESULT)debug_send_queue_process_result(r, starting_pos_local, relative_pos_local, (char*)"DISCARD_queue_tx");
			}
		}
	}
   }

  /* USER CODE END f_RPI_watch */
}

/* USER CODE BEGIN Header_f_KEYENCE_watch */

void send_to_ORIENTATION_queue(char * value){
	queue_stations_tcp local_var;
	strcpy((char *)&local_var.msg, value);
	osStatus_t r = osMessageQueuePut(orientation_queueHandle, &local_var, (uint8_t)0, 2);
	if(ENABLE_QUEUE_RESULT)debug_send_queue_result(r, "ORIENTATION_queue_tx");
}

/* USER CODE END Header_f_KEYENCE_watch */
void f_KEYENCE_watch(void *argument)
{
  /* USER CODE BEGIN f_KEYENCE_watch */
	queue_inter_stations local_queue_RPI_KEY;
	queue_stations_tcp local_queue_orientation;
	queue_stations_tcp local_queue_recipe;
	float relative_pos_local=0;
	int state = 0;
	float reusable_optical_curtain;
	osStatus_t status;
  /* Infinite loop */
  for(;;)
  {
	//vTaskSuspend(NULL);
	osDelay(get_ticks_from_ms(TIM_SLEEP_QUEUES));
	if(state == 0){
		status = osMessageQueueGet(KEYENCE_queueHandle, &local_queue_RPI_KEY, (uint8_t *)NULL, 0);
		if (status == osOK){
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - local_queue_RPI_KEY.optical_curtain;
			if(ENABLE_QUEUE_RESULT)debug_send_process((char*)&str_success, local_queue_RPI_KEY.optical_curtain, relative_pos_local, (char*)"KEYENCE_queue_rx");
			state = 1;
		}
	}else if(state == 1){
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - local_queue_RPI_KEY.optical_curtain;
		if(relative_pos_local>(KEYENCE_POS-DISTANCE_SEND_RECIPE_EARLIER)){
			get_recipe_from_classifier((char*) &local_queue_RPI_KEY.classifier,(char*) &local_queue_recipe.msg);
			send_tcp_to_KEYENCE((char*)&local_queue_recipe.msg);
			debug_send_process((char*)&str_success, local_queue_RPI_KEY.optical_curtain, relative_pos_local, (char*)&local_queue_recipe.msg);// Sending recipe
			state = 2;
		}

	}else if(state == 2){
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - local_queue_RPI_KEY.optical_curtain;
		if(relative_pos_local>(KEYENCE_POS-TOLERANCE_TRG)){
			trigger_KEYENCE();
			debug_send_process((char*)&str_ack, local_queue_RPI_KEY.optical_curtain, relative_pos_local, (char*)"TRIGGER_KEYENCE_pulse");
			state = 3;
		}
	}else if (state == 3){
		status = osMessageQueueGet(orientation_queueHandle, &local_queue_orientation.msg, (uint8_t *)NULL, 0);
		if (status == osOK){
			state = 0;
			strcpy((char *)&local_queue_RPI_KEY.orientation,(char *)&local_queue_orientation.msg);
			osStatus_t r = osMessageQueuePut(FG1_queueHandle, &local_queue_RPI_KEY, (uint8_t)0, 2);
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - local_queue_RPI_KEY.optical_curtain;
			if(ENABLE_QUEUE_RESULT)debug_send_queue_process_result(r, local_queue_RPI_KEY.optical_curtain, relative_pos_local, (char*)"FG1_queue_tx");
		} else {
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - local_queue_RPI_KEY.optical_curtain;
			if (relative_pos_local> (KEYENCE_POS + DIS_RELEASE_KEYENCE)){
				state = 0;
				osStatus_t r = osMessageQueuePut(DISCARD_queueHandle, &local_queue_RPI_KEY.optical_curtain, (uint8_t)0, 2);
				if(ENABLE_QUEUE_RESULT)debug_send_queue_process_result(r, local_queue_RPI_KEY.optical_curtain, relative_pos_local, (char*)"DISCARD_queue_tx");
			}
		}
	}

  }
  /* USER CODE END f_KEYENCE_watch */
}

/* USER CODE BEGIN Header_f_FG1_wach */
/**
* @brief Function implementing the t_FG1_wach thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_f_FG1_wach */
void f_FG1_wach(void *argument)
{
  /* USER CODE BEGIN f_FG1_wach */
	queue_inter_stations local_queue_KEY_FG1;
	queue_stations_tcp local_queue_comput;
	float relative_pos_local=0;
	int state = 0;
	float reusable_optical_curtain;
	osStatus_t status;
  /* Infinite loop */
  for(;;)
  {
	//vTaskSuspend(NULL);
	osDelay(get_ticks_from_ms(TIM_SLEEP_QUEUES));
	if(state == 0){
		status = osMessageQueueGet(FG1_queueHandle, &local_queue_KEY_FG1, (uint8_t *)NULL, 0);
		if (status == osOK){
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - local_queue_KEY_FG1.optical_curtain;
			if(ENABLE_QUEUE_RESULT)debug_send_process((char*)&str_success, local_queue_KEY_FG1.optical_curtain, relative_pos_local, (char*)"FG1_queue_rx");
			state = 1;
		}
	}else if(state == 1){
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - local_queue_KEY_FG1.optical_curtain;
		if(relative_pos_local>(FG1_POS-DISTANCE_SEND_RECIPE_EARLIER)){
			get_computer_from_orientation((char*) &local_queue_KEY_FG1.classifier, (char*) &local_queue_KEY_FG1.orientation, (char*) &local_queue_comput.msg);
			send_tcp_to_FG1((char*) &local_queue_comput.msg);
			debug_send_process((char*)&str_success, local_queue_KEY_FG1.optical_curtain, relative_pos_local, (char*)&local_queue_comput.msg);// Sending computer
			state = 2;
		}
	}else if(state == 2){
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - local_queue_KEY_FG1.optical_curtain;
		if(relative_pos_local>(FG1_POS-TOLERANCE_TRG)){
			trigger_FG1();
			debug_send_process((char*)&str_ack, local_queue_KEY_FG1.optical_curtain, relative_pos_local, (char*)"TRIGGER_FG1_pulse");
			state = 3;
		}
	}else if (state == 3){
		state = 0;
		osStatus_t r = osMessageQueuePut(FG2_queueHandle, &local_queue_KEY_FG1.optical_curtain, (uint8_t)0, 2);
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - local_queue_KEY_FG1.optical_curtain;
		if(ENABLE_QUEUE_RESULT)debug_send_queue_process_result(r, local_queue_KEY_FG1.optical_curtain, relative_pos_local, (char*)"FG2_queue_tx");
	}

  }
  /* USER CODE END f_FG1_wach */
}

/* USER CODE BEGIN Header_f_FG2_wach */
/**
* @brief Function implementing the t_FG2_wach thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_f_FG2_wach */
void f_FG2_wach(void *argument)
{
  /* USER CODE BEGIN f_FG2_wach */
  float starting_pos_local=0;
  float relative_pos_local=0;
  int state = 0;
  float reusable_optical_curtain;
  osStatus_t status;
  /* Infinite loop */
  for(;;)
  {
	//vTaskSuspend(NULL);
	osDelay(get_ticks_from_ms(TIM_SLEEP_QUEUES));
	if(state == 0){
		status = osMessageQueueGet(FG2_queueHandle, &starting_pos_local, (uint8_t *)NULL, 0);
		if (status == osOK){
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - starting_pos_local;
			if(ENABLE_QUEUE_RESULT)debug_send_process((char*)&str_success, starting_pos_local, relative_pos_local, (char*)"FG2_queue_rx");
			state = 1;
		}
	}else if(state == 1){
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - starting_pos_local;
		if(relative_pos_local>(FG2_POS-TOLERANCE_TRG)){
			trigger_FG2();
			debug_send_process((char*)&str_ack, starting_pos_local, relative_pos_local, (char*)"TRIGGER_FG2_pulse");
			state = 2;
		}
	}else if (state == 2){
		state = 0;
		osStatus_t r = osMessageQueuePut(FG3_queueHandle, &starting_pos_local, (uint8_t)0, 2);
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - starting_pos_local;
		if(ENABLE_QUEUE_RESULT)debug_send_queue_process_result(r, starting_pos_local, relative_pos_local, (char*)"FG3_queue_tx");
	}

  }
  /* USER CODE END f_FG2_wach */
}

/* USER CODE BEGIN Header_f_FG3_wach */
/**
* @brief Function implementing the t_FG3_wach thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_f_FG3_wach */
void f_FG3_wach(void *argument)
{
  /* USER CODE BEGIN f_FG3_wach */
  float starting_pos_local=0;
  float relative_pos_local=0;
  int state = 0;
  float reusable_optical_curtain;
  osStatus_t status;

  /* Infinite loop */
  for(;;)
  {
	//vTaskSuspend(NULL);
	osDelay(get_ticks_from_ms(TIM_SLEEP_QUEUES));
	if(state == 0){
		status = osMessageQueueGet(FG3_queueHandle, &starting_pos_local, (uint8_t *)NULL, 0);
		if (status == osOK){
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - starting_pos_local;
			if(ENABLE_QUEUE_RESULT)debug_send_process((char*)&str_success, starting_pos_local, relative_pos_local, (char*)"FG3_queue_rx");
			state = 1;
		}
	}else if(state == 1){
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - starting_pos_local;
		if(relative_pos_local>(FG3_POS-TOLERANCE_TRG)){
			trigger_FG3();
			debug_send_process((char*)&str_ack, starting_pos_local, relative_pos_local, (char*)"TRIGGER_FG3_pulse");
			state = 2;
		}
	}else if (state == 2){
		state = 0;
		osStatus_t r = osMessageQueuePut(FG4_queueHandle, &starting_pos_local, (uint8_t)0, 2);
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - starting_pos_local;
		if(ENABLE_QUEUE_RESULT)debug_send_queue_process_result(r, starting_pos_local, relative_pos_local, (char*)"FG4_queue_tx");
	}

  }
  /* USER CODE END f_FG3_wach */
}

/* USER CODE BEGIN Header_f_FG4_wach */
/**
* @brief Function implementing the t_FG4_wach thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_f_FG4_wach */
void f_FG4_wach(void *argument)
{
  /* USER CODE BEGIN f_FG4_wach */
  float starting_pos_local=0;
  float relative_pos_local=0;
  int state = 0;
  float reusable_optical_curtain;
  osStatus_t status;

  /* Infinite loop */
  for(;;)
  {
	//vTaskSuspend(NULL);
	osDelay(get_ticks_from_ms(TIM_SLEEP_QUEUES));
	if(state == 0){
		status = osMessageQueueGet(FG4_queueHandle, &starting_pos_local, (uint8_t *)NULL, 0);
		if (status == osOK){
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - starting_pos_local;
			if(ENABLE_QUEUE_RESULT)debug_send_process((char*)&str_success, starting_pos_local, relative_pos_local, (char*)"FG4_queue_rx");
			state = 1;
		}
	}else if(state == 1){
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - starting_pos_local;
		if(relative_pos_local>(FG4_POS-TOLERANCE_TRG)){
			trigger_FG4();
			debug_send_process((char*)&str_ack, starting_pos_local, relative_pos_local, (char*)"TRIGGER_FG4_pulse");
			state = 2;
		}
	}else if (state == 2){
		state = 0;
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - starting_pos_local;
		debug_send_process((char*)&str_success, starting_pos_local, relative_pos_local, (char*)"DONE!");
		count_done++;
		debug_send_done_discard();
	}

  }
  /* USER CODE END f_FG4_wach */
}

/* USER CODE BEGIN Header_f_DISCARD */
/**
* @brief Function implementing the t_DISCARD thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_f_DISCARD */
void f_DISCARD(void *argument)
{
  /* USER CODE BEGIN f_DISCARD */
  float starting_pos_local=0;
  float relative_pos_local=0;
  int state = 0;
  float reusable_optical_curtain;
  osStatus_t status = osError;

  /* Infinite loop */
  for(;;)
  {
	//vTaskSuspend(NULL);
	if(state == 0){
		status = osMessageQueueGet(DISCARD_queueHandle, &starting_pos_local, (uint8_t *)NULL, 0);
		if (status == osOK){
			reusable_optical_curtain = get_current_conveyor_pos();
			relative_pos_local =  reusable_optical_curtain - starting_pos_local;
			if(ENABLE_QUEUE_RESULT)debug_send_process((char*)&str_success, starting_pos_local, relative_pos_local, (char*)"DISCARD_queue_rx");
			state = 1;
		}
	}else if(state == 1){
		reusable_optical_curtain = get_current_conveyor_pos();
		relative_pos_local =  reusable_optical_curtain - starting_pos_local;
		if(relative_pos_local>(DISCARD_POS-TOLERANCE_TRG)){
			trigger_DISCARD();
			debug_send_process((char*)&str_ack, starting_pos_local, relative_pos_local, (char*)"TRIGGER_DISCARD_pulse");
			count_discard++;
			debug_send_done_discard();
			state = 0;
		}
	}
	osDelay(get_ticks_from_ms(TIM_SLEEP_QUEUES));
  }
  /* USER CODE END f_DISCARD */
}

/* USER CODE BEGIN Header_f_command_handl */
/**
  * @brief  Function implementing the t_command_handl thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_f_command_handl */
void f_command_handl(void *argument)
{
  /* USER CODE BEGIN f_command_handl */
  osStatus_t status;
  queue_commandRx queue_commandRx_local;
  /* Infinite loop */
  for(;;)
  {

	status = osMessageQueueGet(command_rx_queueHandle, &queue_commandRx_local, (uint8_t *)NULL, 0);
	if (status == osOK) {
		char local_message[queue_commandRx_local.len-2 + 1]; // not considering start and end delimiters
		int s = sizeof local_message;
		memcpy(local_message, queue_commandRx_local.msg + 1, s); // ignoring start
		local_message[s-1]='\0';

		char *pointer_sub_message = strtok( (char *)&local_message, COMMAND_HANDLER_DELIMITER_STR);
		char *command;
		char *value;


		int cnt = 0;
		while (pointer_sub_message != NULL){
		  if (cnt == 0) {
			  command = pointer_sub_message;
			  debug_send_echo(command);
		  }
		  else if (cnt == 1) {
			  value = pointer_sub_message;
			  debug_send_echo(value);
		  }
		  pointer_sub_message = strtok(NULL, COMMAND_HANDLER_DELIMITER_STR);
		  cnt++;
		}

		if(strcmp((char *)command, "PING") == 0){
			debug_send_ack("OK_");
		}
		else if(strcmp((char *)command, "EN_ACK_POS") == 0){
			if(strcmp(value, "true") == 0){
				en_ackn_pos = true;
			}else{
				en_ackn_pos = false;
			}
		}
		else if (strcmp((char *)command, "OC_EVENT") == 0){
			callback_trigger_optical_curtain();
		}
		else if(strcmp((char *)command, "TRIGGER_RPI") == 0){
			trigger_RPI();
		}
		else if(strcmp((char *)command, "TRIGGER_RPI_H") == 0){
			on_trigger_RPI();
		}
		else if(strcmp((char *)command, "TRIGGER_RPI_L") == 0){
			off_trigger_RPI();
		}
		else if(strcmp((char *)command, "TRIGGER_KEYENCE") == 0){
			trigger_KEYENCE();
		}
		else if(strcmp((char *)command, "TRIGGER_KEYENCE_H") == 0){
			on_trigger_KEYENCE();
		}
		else if(strcmp((char *)command, "TRIGGER_KEYENCE_L") == 0){
			off_trigger_KEYENCE();
		}
		else if(strcmp((char *)command, "TRIGGER_FG1") == 0){
			trigger_FG1();
		}
		else if(strcmp((char *)command, "TRIGGER_FG1_H") == 0){
			on_trigger_FG1();
		}
		else if(strcmp((char *)command, "TRIGGER_FG1_L") == 0){
			off_trigger_FG1();
		}
		else if(strcmp((char *)command, "TRIGGER_FG2") == 0){
			trigger_FG2();
		}
		else if(strcmp((char *)command, "TRIGGER_FG2_H") == 0){
			on_trigger_FG2();
		}
		else if(strcmp((char *)command, "TRIGGER_FG2_L") == 0){
			off_trigger_FG2();
		}
		else if(strcmp((char *)command, "TRIGGER_FG3") == 0){
			trigger_FG3();
		}
		else if(strcmp((char *)command, "TRIGGER_FG3_H") == 0){
			on_trigger_FG3();
		}
		else if(strcmp((char *)command, "TRIGGER_FG3_L") == 0){
			off_trigger_FG3();
		}
		else if(strcmp((char *)command, "TRIGGER_FG4") == 0){
			trigger_FG4();
		}
		else if(strcmp((char *)command, "TRIGGER_FG4_H") == 0){
			on_trigger_FG4();
		}
		else if(strcmp((char *)command, "TRIGGER_FG4_L") == 0){
			off_trigger_FG4();
		}
		else if(strcmp((char *)command, "TRIGGER_DISCARD") == 0){
			trigger_DISCARD();
		}
		else if(strcmp((char *)command, "TRIGGER_DISCARD_H") == 0){
			on_trigger_DISCARD();
		}
		else if(strcmp((char *)command, "TRIGGER_DISCARD_L") == 0){
			off_trigger_DISCARD();
		}
		else if(strcmp((char *)command, "TX_TCP_RPI") == 0){
			send_tcp_to_RPI(value);
		}
		else if(strcmp((char *)command, "TX_TCP_KEYENCE") == 0){
			send_tcp_to_KEYENCE(value);
		}
		else if(strcmp((char *)command, "TX_TCP_FG1") == 0){
			send_tcp_to_FG1(value);
		}
		else if (strcmp((char *)command, "CLASSIFIER") == 0){
			send_to_CLASSIFIER_queue(value);
		}
		else if (strcmp((char *)command, "ORIENTATION") == 0){
			send_to_ORIENTATION_queue(value);
		}
		else if(strcmp((char *)command, "SIMULATOR_OC_TRIGGER") == 0){
			trigger_simulator_oc_event();
		}
		else if(strcmp((char *)command, "EN_PERIOD_OC_EVENT_SIM") == 0){
			if(strcmp(value, "true") == 0){
				en_period_oc_print = true;
				en_period_oc_event_sim = true;
				sim_per_oc_end_pos = 0;
			}else{
				en_period_oc_event_sim = false;
			}
		}
		else if(strcmp((char *)command, "INDIC_R") == 0){
			if(strcmp(value, "true") == 0){
				turn_on_Indicator_R();
			}else{
				turn_off_Indicator_R();
			}
		}
		else if(strcmp((char *)command, "INDIC_G") == 0){
			if(strcmp(value, "true") == 0){
				turn_on_Indicator_G();
			}else{
				turn_off_Indicator_G();
			}
		}
		else if(strcmp((char *)command, "INDIC_B") == 0){
			if(strcmp(value, "true") == 0){
				turn_on_Indicator_B();
			}else{
				turn_off_Indicator_B();
			}
		}
		else{
			  debug_send_error("COMMAND_NOT_RECOG");
		}
	}
	osDelay(get_ticks_from_ms(TIM_SLEEP_COMMAND_H));
  }
  /* USER CODE END f_command_handl */
}

/* USER CODE BEGIN Header_f_send_tcp */


osStatus_t send_tcp_to_station(char* value, char * str_station_name, struct netconn *conn_var){
	queue_tx_rx_tcp local_var;
	strcpy((char *)&local_var.msg, value);
	local_var.conn = conn_var;
	local_var.point_conn_name = str_station_name;
	osStatus_t r = osMessageQueuePut(tx_tcp_queueHandle, &local_var, (uint8_t)0, 2);
	return r;
}

void send_tcp_to_RPI(char* value){
	osStatus_t r = send_tcp_to_station(value, (char *)&str_RPI, conn_RPI);
	if(ENABLE_QUEUE_RESULT)debug_send_queue_result(r, "TX_TCP_RPI_queue_tx");
}

void send_tcp_to_KEYENCE(char* value){
	osStatus_t r = send_tcp_to_station(value, (char *)&str_KEYENCE, conn_KEYENCE);
	if(ENABLE_QUEUE_RESULT)debug_send_queue_result(r, "TX_TCP_KEYENCE_queue_tx");
}

void send_tcp_to_FG1(char* value){
	osStatus_t r = send_tcp_to_station(value, (char *)&str_FG1, conn_FG1);
	if(ENABLE_QUEUE_RESULT)debug_send_queue_result(r, "TX_TCP_FG1_queue_tx");
}


/* USER CODE END Header_f_send_tcp */
void f_send_tcp(void *argument)
{
  /* USER CODE BEGIN f_send_tcp */
  osStatus_t status;
  queue_tx_rx_tcp ms;
  vTaskSuspend(NULL);
  /* Infinite loop */
  for(;;)
  {
	status = osMessageQueueGet(tx_tcp_queueHandle, &ms, (uint8_t *)NULL, 0);
	if (status == osOK) {
		struct tcp_pcb * tcp = ms.conn->pcb.tcp;
		uint8_t state = (uint8_t) tcp->state;

		debug_message_sent_tcp(state, &ms);

		if((state == ESTABLISHED)){
			netconn_set_nonblocking(ms.conn, false);
			netconn_write(ms.conn, &ms.msg, strlen((char *) &ms.msg), NETCONN_COPY);
			netconn_set_nonblocking(ms.conn, true);
		}
	}
	osDelay(get_ticks_from_ms(TIM_SLEEP_SEND_TCP));
  }
  /* USER CODE END f_send_tcp */
}

/* USER CODE BEGIN Header_f_recv_tcp */

void recv_tcp(char * str_conn_name, char * msgc, struct netbuf *buf, struct netconn *conn){
	if(conn!= NULL){
		if(conn->pcb.tcp->state == ESTABLISHED){
			if (netconn_recv(conn, &buf) == ERR_OK)
			{
				do
				{
					strncpy (msgc, buf->p->payload, buf->p->len);   // get the message from the server
					msgc[buf->p->len] = '\0';

					debug_message_recv_tcp( str_conn_name, msgc);

					if(conn == conn_RPI){
						send_to_CLASSIFIER_queue(msgc);
					}else if(conn == conn_KEYENCE){
						send_to_ORIENTATION_queue(msgc);
					}

					memset (msgc, '\0', BUFFER_TCP_RX_SIZE);  // clear the buffer
				}
				while (netbuf_next(buf) >0);
				netbuf_delete(buf);
			}
		}
	}
}


/* USER CODE END Header_f_recv_tcp */
void f_recv_tcp(void *argument)
{
  /* USER CODE BEGIN f_recv_tcp */
	char msgc[BUFFER_TCP_RX_SIZE];
	static struct netbuf *buf;
	vTaskSuspend(NULL);
  /* Infinite loop */
  for(;;)
  {
	recv_tcp((char *)&str_RPI, (char *)&msgc, buf, conn_RPI);
	recv_tcp((char *)&str_KEYENCE, (char *)&msgc, buf, conn_KEYENCE);
	recv_tcp((char *)&str_FG1, (char *)&msgc, buf, conn_FG1);
    osDelay(get_ticks_from_ms(TIM_SLEEP_RECV_TCP));
  }
  /* USER CODE END f_recv_tcp */
}

/* USER CODE BEGIN Header_f_conn_tcp_RPI */

void conn_init_monitor_tcp(char * str_conn_name, struct netconn **pointer_conn, ip4_addr_t* sourc_addr, unsigned short sourc_port,
		ip4_addr_t dest_addr, unsigned short dest_port){

	err_t err;
	unsigned long time_tcp_var_previous=-1;
	unsigned long time_tcp_var_now=-1;

	vTaskSuspend(NULL);
	while(1)
	{
		*pointer_conn = netconn_new(NETCONN_TCP);
		netconn_set_nonblocking(*pointer_conn, true);

		debug_init_monitor_tcp(str_conn_name);

		err = netconn_bind(*pointer_conn, sourc_addr, sourc_port);

		if (err == ERR_OK)
		{
			netconn_connect(*pointer_conn, &dest_addr, dest_port);

			time_tcp_var_previous = xTaskGetTickCount();
			time_tcp_var_now = time_tcp_var_previous;

			bool fin = false;

			while(!fin){
				struct tcp_pcb * tcp = (*pointer_conn)->pcb.tcp;
				uint8_t state = (uint8_t) tcp->state;
				//debug_tcp_connection_state(state);

				if((state == ESTABLISHED)){
					time_tcp_var_previous = time_tcp_var_now;
				}else{
					time_tcp_var_now = xTaskGetTickCount(); // reset counter if no connection

					if(((time_tcp_var_now - time_tcp_var_previous)>TIM_TCP_SLEEP_TIMEOUT)){
						fin = true;
						debug_lost_monitor_tcp(str_conn_name);
					}
				}
				osDelay(get_ticks_from_ms(TIM_TCP_SLEEP_CHECK));
			}

			netconn_close(*pointer_conn);
			netconn_prepare_delete(*pointer_conn);
			netconn_delete(*pointer_conn);
			osDelay(get_ticks_from_ms(TIM_TCP_SLEEP_ATTEMP));

		}else{
			netconn_delete(*pointer_conn);
		}
	}

}
/* USER CODE END Header_f_conn_tcp_RPI */
void f_conn_tcp_RPI(void *argument)
{
  /* USER CODE BEGIN f_conn_tcp_RPI */
	struct netconn **pointer_conn;
	ip4_addr_t* sourc_addr;
	unsigned short sourc_port;
	ip4_addr_t dest_addr;
	unsigned short dest_port;
	char * str_conn_name;

	pointer_conn = &conn_RPI;
	sourc_addr = &ipaddr;
	sourc_port = RPI_SOURC_PORT;
	IP_ADDR4(&dest_addr, RPI_IP4_1, RPI_IP4_2, RPI_IP4_3, RPI_IP4_4);
	dest_port = RPI_DEST_PORT;
	str_conn_name = (char*) &str_RPI;
  /* Infinite loop */
  for(;;)
  {
	conn_init_monitor_tcp(str_conn_name, pointer_conn, sourc_addr, sourc_port, dest_addr, dest_port);
  }
  /* USER CODE END f_conn_tcp_RPI */
}

/* USER CODE BEGIN Header_f_conn_tcp_KEYENCE */
/**
* @brief Function implementing the t_conn_tcp_KEYENCE thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_f_conn_tcp_KEYENCE */
void f_conn_tcp_KEYENCE(void *argument)
{
  /* USER CODE BEGIN f_conn_tcp_KEYENCE */
	struct netconn **pointer_conn;
	ip4_addr_t* sourc_addr;
	unsigned short sourc_port;
	ip4_addr_t dest_addr;
	unsigned short dest_port;
	char * str_conn_name;

	pointer_conn = &conn_KEYENCE;
	sourc_addr = &ipaddr;
	sourc_port = KEYENCE_SOURC_PORT;
	IP_ADDR4(&dest_addr, KEYENCE_IP4_1, KEYENCE_IP4_2, KEYENCE_IP4_3, KEYENCE_IP4_4);
	dest_port = KEYENCE_DEST_PORT;
	str_conn_name = (char*) &str_KEYENCE;
  /* Infinite loop */
  for(;;)
  {
	conn_init_monitor_tcp(str_conn_name, pointer_conn, sourc_addr, sourc_port, dest_addr, dest_port);
  }
  /* USER CODE END f_conn_tcp_KEYENCE */
}

/* USER CODE BEGIN Header_f_conn_tcp_FG1 */
/**
* @brief Function implementing the t_conn_tcp_FG1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_f_conn_tcp_FG1 */
void f_conn_tcp_FG1(void *argument)
{
  /* USER CODE BEGIN f_conn_tcp_FG1 */
	struct netconn **pointer_conn;
	ip4_addr_t* sourc_addr;
	unsigned short sourc_port;
	ip4_addr_t dest_addr;
	unsigned short dest_port;
	char * str_conn_name;

	pointer_conn = &conn_FG1;
	sourc_addr = &ipaddr;
	sourc_port = FG1_SOURC_PORT;
	IP_ADDR4(&dest_addr, FG1_IP4_1, FG1_IP4_2, FG1_IP4_3, FG1_IP4_4);
	dest_port = FG1_DEST_PORT;
	str_conn_name = (char*) &str_FG1;
  /* Infinite loop */
  for(;;)
  {
	conn_init_monitor_tcp(str_conn_name, pointer_conn, sourc_addr, sourc_port, dest_addr, dest_port);
  }
  /* USER CODE END f_conn_tcp_FG1 */
}

/* USER CODE BEGIN Header_f_simulate_conveyor */
/**
* @brief Function implementing the t_simulate_conveyor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_f_simulate_conveyor */
void f_simulate_conveyor(void *argument)
{
  /* USER CODE BEGIN f_simulate_conveyor */
  /* Infinite loop */
  for(;;)
  {
	vTaskSuspend(NULL); // wait for sTIM resume
	sim_opt_curt_mm = sim_opt_curt_mm + SIM_STEP_DISTANCE; //mm
  }
  /* USER CODE END f_simulate_conveyor */
}

/* USER CODE BEGIN Header_f_debug_mssgs */



void callback_CLI_RX_test(cli_char_buffer_handler *c){
	//Before entering this function , c->rxbuffer should be updated or a new rx
	//After this fction is excecuted, the event to wait for next rx should be updated.
	if (c->rxBuffer == COMMAND_HANDLER_START && c->state_rx==0) { // start of command
		c->cnt_rx = 0;
		c->state_rx=1;
		memset(c->queueCommandRx.msg, 0, sizeof c->queueCommandRx.msg); // reset data
	}
	else if ((c->cnt_rx > (RX_SIZE_UART_DEBUG - 1))&&(c->state_rx==1)) { // corrupted/wrong input (lack of end character)
		c->state_rx=0;
	}
	else if (c->rxBuffer == COMMAND_HANDLER_END) { // end of command detected
		if(c->state_rx==1){
			c->queueCommandRx.msg[c->cnt_rx] = c->rxBuffer;
			c->queueCommandRx.len = c->cnt_rx+1;
			osMessageQueuePut(command_rx_queueHandle, &(c->queueCommandRx), (uint8_t)0, 0);
		}
		c->state_rx=0;
	}

	// store character
	if(c->state_rx==1){
		c->queueCommandRx.msg[c->cnt_rx] = c->rxBuffer;
		c->cnt_rx++;
	}
}

//COMMAND UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == huart_CLI_debug && USE_UART_EXT)
	{
		callback_CLI_RX_test(&cli_uart_debug);
		HAL_UART_Receive_IT(huart_CLI_debug, (uint8_t*) &(cli_uart_debug.rxBuffer), 1);
	}
	if(huart == huart_CLI_CM4_main && USE_UART_CM4_MAIN)
	{
		callback_CLI_RX_test(&cli_uart_CM4_main);
		HAL_UART_Receive_IT(huart_CLI_CM4_main, (uint8_t*) &(cli_uart_CM4_main.rxBuffer), 1);
	}
	if(huart == huart_CLI_CM4_second && USE_UART_CM4_SECN)
	{
		callback_CLI_RX_test(&cli_uart_CM4_second);
		HAL_UART_Receive_IT(huart_CLI_CM4_second, (uint8_t*) &(cli_uart_CM4_second.rxBuffer), 1);
	}
}

void debug_send_echo(char * txBuffer) {
	message_debug ms_echo;
	strcpy((char *)&ms_echo.content, txBuffer);
	ms_echo.type = (char *)&str_echo;
	osMessageQueuePut(message_debug_queueHandle, &ms_echo, (uint8_t)0, 0);
}

void debug_send_error(char * txBuffer) {
	message_debug ms_error;
	strcpy((char *)&ms_error.content, txBuffer);
	ms_error.type = (char *)&str_error;
	osMessageQueuePut(message_debug_queueHandle, &ms_error, (uint8_t)0, 0);
}

void debug_send_success(char * txBuffer) {
	message_debug ms_success;
	strcpy((char *)&ms_success.content, txBuffer);
	ms_success.type = (char *)&str_success;
	osMessageQueuePut(message_debug_queueHandle, &ms_success, (uint8_t)0, 0);
}

void debug_send_ack(char * txBuffer) {
	message_debug ms_ack;
	strcpy((char *)&ms_ack.content, txBuffer);
	ms_ack.type = (char *)&str_ack;
	osMessageQueuePut(message_debug_queueHandle, &ms_ack, (uint8_t)0, 0);
}

void debug_send_tcp(char * txBuffer) {
	message_debug ms_tcp;
	strcpy((char *)&ms_tcp.content, txBuffer);
	ms_tcp.type = (char *)&str_tcp;
	osMessageQueuePut(message_debug_queueHandle, &ms_tcp, (uint8_t)0, 0);
}

void debug_send_process(char * str_type,float optical_curtain_id, float rel_dist, char * txBuffer) {
	message_debug ms_gen;
	sprintf((char *)&ms_gen.content, "ID_%.8d,RD_%.8d,%s", (int) optical_curtain_id, (int) rel_dist, txBuffer);
	ms_gen.type = str_type;
	osMessageQueuePut(message_debug_queueHandle, &ms_gen, (uint8_t)0, 0);
}

void debug_tcp_connection_state(uint8_t state){
	if((state == CLOSED)){
		debug_send_tcp("CLOSED");
	}
	else if((state == LISTEN)){
		debug_send_tcp("LISTEN");
	}
	else if((state == SYN_SENT)){
		debug_send_tcp("SYN_SENT");
	}
	else if((state == SYN_RCVD)){
		debug_send_tcp("SYN_RCVD");
	}
	else if((state == ESTABLISHED)){
		debug_send_tcp("ESTABLISHED");
	}
	else if((state == FIN_WAIT_1)){
		debug_send_tcp("FIN_WAIT_1");
	}
	else if((state == FIN_WAIT_2)){
		debug_send_tcp("FIN_WAIT_2");
	}
	else if((state == CLOSE_WAIT)){
		debug_send_tcp("CLOSE_WAIT");
	}
	else if((state == CLOSING)){
		debug_send_tcp("CLOSING");
	}
	else if((state == LAST_ACK)){
		debug_send_tcp("LAST_ACK");
	}
	else if((state == TIME_WAIT)){
		debug_send_tcp("TIME_WAIT");
	}
}

void debug_message_sent_tcp(uint8_t state, queue_tx_rx_tcp *ms){
	sprintf(smsgc_tx_tcp_buff_debug, "TCP:TX:%s:%s", ms->point_conn_name, ms->msg);
	if((state == ESTABLISHED)){
		debug_send_tcp((char *)&smsgc_tx_tcp_buff_debug);
	}else{
		debug_send_error((char *)&smsgc_tx_tcp_buff_debug);
	}
}

void debug_message_recv_tcp(char *str_conn_name, char *msgc){
	sprintf (smsgc_rx_tcp_buff_debug, "TCP:RX:%s:%s", str_conn_name, msgc);
	debug_send_tcp((char *)smsgc_rx_tcp_buff_debug);
}

void debug_init_monitor_tcp(char * str_conn_name){
	char smsgc[20]; //initialized here bcs it can be used in by many threads
	sprintf (smsgc, "%s:LOADING", str_conn_name);
	debug_send_tcp((char *)&smsgc);
}

void debug_lost_monitor_tcp(char * str_conn_name){
	char smsgc[20]; //initialized here bcs it can be used in by many threads
	sprintf (smsgc, "%s:LOST", str_conn_name);
	debug_send_tcp((char *)&smsgc);
}
void debug_send_queue_result(osStatus_t r, char * txBuffer){
	if(r == osOK) debug_send_success(txBuffer);
	else debug_send_error(txBuffer);
}

void debug_send_queue_process_result(osStatus_t r, float starting_pos_local, float relative_pos_local, char * txBuffer){
	if(ENABLE_QUEUE_RESULT){
		if(r == osOK) debug_send_process((char*)&str_success, starting_pos_local, relative_pos_local, txBuffer);
		else debug_send_process((char*)&str_error, starting_pos_local, relative_pos_local, txBuffer);
	}
}
void debug_send_optical_position(float t){
	sprintf(var_debug_send_optical, "Pos_%.2d", (int) t);
	debug_send_ack((char *)var_debug_send_optical);
}

void debug_send_done_discard(){
	char var_debug_send_optical[20];

	if(en_period_oc_print)
	{
		debug_send_ack((char *)"SIM:");

		sprintf(var_debug_send_optical, "STR_POS: %.2d", (int) sim_per_oc_start_pos);
		debug_send_ack((char *)var_debug_send_optical);

		sprintf(var_debug_send_optical, "END_POS: %.2d", (int) sim_per_oc_end_pos);
		debug_send_ack((char *)var_debug_send_optical);

		sprintf(var_debug_send_optical, "SIM_TRG: %.2d", (int) count_total_OC_trigger_sim);
		debug_send_ack((char *)var_debug_send_optical);

		debug_send_ack((char *)"<---");
	}

	sprintf(var_debug_send_optical, "TOTL_OC: %.2d", (int) count_total_OC_events);
	debug_send_ack((char *)var_debug_send_optical);

	sprintf(var_debug_send_optical, "ART_DNE: %.2d", (int) count_done);
	debug_send_ack((char *)var_debug_send_optical);

	sprintf(var_debug_send_optical, "ART_DIS: %.2d", (int) count_discard);
	debug_send_ack((char *)var_debug_send_optical);

}



void transmit_over_CLI(uint8_t* p, int l){
	if (USE_UART_EXT){
		HAL_UART_Transmit(huart_CLI_debug, p, l, 10);
	}
	if (USE_UART_CM4_MAIN){
		HAL_UART_Transmit(huart_CLI_CM4_main, p, l, 10);
	}
	if (USE_UART_CM4_SECN){
		HAL_UART_Transmit(huart_CLI_CM4_second, p, l, 10);
	}
	if (USE_USB){
		unsigned long start = xTaskGetTickCount();
		uint8_t result;
		do{
			result = CDC_Transmit_FS(p, l);
		}while(result!=USBD_OK && ((xTaskGetTickCount() - start)< 10));
	}
}



/* USER CODE END Header_f_debug_mssgs */
void f_debug_mssgs(void *argument)
{
  /* USER CODE BEGIN f_debug_mssgs */
	osStatus_t status;
	message_debug ms_p;
  /* Infinite loop */
  for(;;)
  {
	status = osMessageQueueGet(message_debug_queueHandle, &ms_p, (uint8_t *)NULL, 0);

	if (status == osOK){
		transmit_over_CLI((uint8_t *)&str_start_mssg, 1);
		if(strcmp(ms_p.type,"") != 0){
			transmit_over_CLI((uint8_t *) ms_p.type, strlen((char *)ms_p.type));
			transmit_over_CLI((uint8_t *)&str_delimiter_mssg, 1);

		}
		transmit_over_CLI((uint8_t *) ms_p.content, strlen((char *)ms_p.content));
		transmit_over_CLI((uint8_t *)&str_end_mssg, 1);
		transmit_over_CLI((uint8_t *)&str_n_r, str_n_r_size);

	}

	osDelay(get_ticks_from_ms(TIM_SLEEP_DEBUG_MSSGS));
  }
  /* USER CODE END f_debug_mssgs */
}

/* USER CODE BEGIN Header_f_CURTAIN */

void evaluate_optical_curtain(){
	float pos = get_current_conveyor_pos();
	if(((pos - last_optical_curtain) > DEAD_DISTANCE) || (last_optical_curtain ==0)){	// check if it is not in dead distance
		osStatus_t r = osMessageQueuePut(RPI_queueHandle, &pos, (uint8_t)0, 2);
		if(r == osOK) last_optical_curtain = pos;
		if(ENABLE_QUEUE_RESULT)debug_send_queue_process_result(r, pos, 0, (char*)"RPI_queue_tx");
	}else{
		osStatus_t r = osMessageQueuePut(DISCARD_queueHandle, &pos, (uint8_t)0, 2);
		if(ENABLE_QUEUE_RESULT)debug_send_queue_process_result(r, pos, 0, (char*)"DISCARD_queue_tx");
	}
}

void callback_trigger_optical_curtain(){
	//vTaskResume(t_OpticalCHandle);
	osStatus_t r = osMessageQueuePut(CURTAIN_queueHandle,&fixed_value_optical_curtain,(uint8_t)0,10);
	count_total_OC_events++;
	if(ENABLE_QUEUE_RESULT)debug_send_queue_result(r, "CURTAIN_queue_tx");

}

void callback_trigger_optical_curtain_IT(){
	osStatus_t r = osMessageQueuePut(CURTAIN_queueHandle,&fixed_value_optical_curtain,(uint8_t)0,0);
	count_total_OC_events++;
	debug_send_queue_result(r, "OC_IT");
}

/* USER CODE END Header_f_CURTAIN */
void f_CURTAIN(void *argument)
{
  /* USER CODE BEGIN f_CURTAIN */
	bool a;
	uint8_t n = 0;
	osStatus_t r;
  /* Infinite loop */
  for(;;)
  {
	//vTaskSuspend(NULL);
	r = osMessageQueueGet(CURTAIN_queueHandle,&a,&n,0);
	if( r == osOK){
		if(ENABLE_QUEUE_RESULT)debug_send_queue_result(r, "CURTAIN_queue_rx");
		evaluate_optical_curtain();
	}
	osDelay(get_ticks_from_ms(TIM_SLEEP_CURTAIN));
  }
  /* USER CODE END f_CURTAIN */
}

/* USER CODE BEGIN Header_f_update_encoder */

double getCurrentPositionMM(){
	double cascade;
	int16_t encoder_tick;
	double total;

		if (USE_CASCADE_BY_SOFTWARE == true){
			//osMutexAcquire(mutex_encoder_cascadeHandle, osWaitForever);
			cascade=counter_cascade_by_softw;
			encoder_tick = last_tick_encoder;
			//osMutexRelease(mutex_encoder_cascadeHandle);
		}else{
			cascade=__HAL_TIM_GET_COUNTER(&htim5); //counter_cascade_by_timer. Counter cascade by TIM5 slave
			encoder_tick = __HAL_TIM_GET_COUNTER(&htim4);
		}

	total = cascade*ENCODER_INTERFACE_RES_STM + (double)encoder_tick;

	return (total/ENCODER_TICK_PER_MM);
}

double getCurrentPositionCM(){
	return (getCurrentPositionMM()/10.0);
}

//Conveyor position
float get_current_conveyor_pos(){
	float var;
	if(use_simulator == true){
		var = sim_opt_curt_mm;
	}else{
		//osMutexAcquire(mutex_sample_conveyorHandle, osWaitForever);
		var = ((float)getCurrentPositionMM());
		//osMutexRelease(mutex_sample_conveyorHandle);
	}
	return var;
}

/* USER CODE END Header_f_update_encoder */
void f_update_encoder(void *argument)
{
  /* USER CODE BEGIN f_update_encoder */
  /* Infinite loop */
  for(;;)
  {
	vTaskSuspend(NULL);

	//osMutexAcquire(mutex_encoder_cascadeHandle, osWaitForever);

	//osMutexAcquire(mutex_encoder_hardwareHandle, osWaitForever);
	tick_encoder_var = __HAL_TIM_GET_COUNTER(&htim4);
	//osMutexRelease(mutex_encoder_hardwareHandle);

	//Counter cascade by Software
	if(tick_encoder_var<=0 && last_tick_encoder>16383){ //overflow
		counter_cascade_by_softw++;
	}else if(tick_encoder_var>0 && last_tick_encoder <= -16383){ //underflow
		counter_cascade_by_softw--;
	}
	last_tick_encoder=tick_encoder_var;

	//osMutexRelease(mutex_encoder_cascadeHandle);
  }
  /* USER CODE END f_update_encoder */
}

/* USER CODE BEGIN Header_f_simulate_periodic */
/**
* @brief Function implementing the t_simulate_periodic thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_f_simulate_periodic */
void f_simulate_periodic(void *argument)
{
  /* USER CODE BEGIN f_simulate_periodic */
  /* Infinite loop */
  for(;;)
  {
	vTaskSuspend(NULL);
	//START THE PERIODIC OPTICAL CURTAIN EVENT SIMULATION

	if (!en_period_oc_event_sim_last && en_period_oc_event_sim){
		sim_per_oc_start_pos = get_current_conveyor_pos();
		count_total_OC_trigger_sim = 0;
		count_total_OC_events = 0;
		count_done = 0;
		count_discard = 0;
	}

	if(en_period_oc_event_sim)
	{
		trigger_simulator_oc_event();
		count_total_OC_trigger_sim++;
		sim_per_oc_end_pos = get_current_conveyor_pos();
		en_period_oc_event_sim_last = true;
	}else{
		en_period_oc_event_sim_last = false;
	}
  }
  /* USER CODE END f_simulate_periodic */
}

/* callback_timer_simulate_conveyor function */
void callback_timer_simulate_conveyor(void *argument)
{
  /* USER CODE BEGIN callback_timer_simulate_conveyor */
	vTaskResume(t_simulate_conveyorHandle);
  /* USER CODE END callback_timer_simulate_conveyor */
}

/* callback_timer_watch_queues function */
void callback_timer_watch_queues(void *argument)
{
  /* USER CODE BEGIN callback_timer_watch_queues */
	vTaskResume(t_RPI_watchHandle);
	vTaskResume(t_KEYENCE_watchHandle);
	vTaskResume(t_FG1_wachHandle);
	vTaskResume(t_FG2_wachHandle);
	vTaskResume(t_FG3_wachHandle);
	vTaskResume(t_FG4_wachHandle);
	//vTaskResume(t_DISCARDHandle);
  /* USER CODE END callback_timer_watch_queues */
}

/* callback_timer_update_encoder function */
void callback_timer_update_encoder(void *argument)
{
  /* USER CODE BEGIN callback_timer_update_encoder */
	vTaskResume(t_update_encoderHandle);
  /* USER CODE END callback_timer_update_encoder */
}

/* callback_timer_simulate_periodic_oc function */
void callback_timer_simulate_periodic_oc(void *argument)
{
  /* USER CODE BEGIN callback_timer_simulate_periodic_oc */
	vTaskResume(t_simulate_periodicHandle);
  /* USER CODE END callback_timer_simulate_periodic_oc */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

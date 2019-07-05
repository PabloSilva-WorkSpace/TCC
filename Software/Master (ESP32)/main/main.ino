/* Fish Tank automation project - TCC -  main file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
*/


/* Headers includes */  
#include "main.h"


/* Main Data of Module */
MainData mainData;
/* Setting variables to TIMER pripheral of Module ESP32 */
hw_timer_t *timer = NULL;
timer_config_t timer_config;
/* Setting variables to UART pripheral of Module ESP32 */
uart_config_t uart_config = {.baud_rate = 19200, .data_bits = UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits = UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
uart_intr_config_t uart_intr;
static uart_isr_handle_t handle_console;
/* Variables into this scope (this file *.c) */
static int Counter1Of10ms = 0;
static int Counter2Of10ms = 0;
static int Counter3Of10ms = 0;


/* CallBack Timer runs every 10ms */ 
static void IRAM_ATTR TIMER_Group0_ISR(void *pParam)
{
  /* Simple instructions */
  Counter1Of10ms++;
  Counter2Of10ms++;
  Counter3Of10ms++;
  /* Instructions to Timer peripheral */
  TIMERG0.int_clr_timers.t0 = 1;  /* Clear the interrupt */
  TIMERG0.hw_timer[TIMER_ID].config.alarm_en = TIMER_ALARM_EN; /* Enable alarm of Timer again */
}

/*
static void IRAM_ATTR Uart2_Intr_Handler(void *pParam)
{
  Serial.println("Uart2_Intr_Handler");
  uart_disable_intr_mask(UART_ID, 0x7FFF);
  uart_clear_intr_status(UART_ID, 0x7FFF);
  uart_flush(UART_ID);
}
*/

void setup()
{
  WiFi.disconnect();
  pinMode(LED_ON_BOARD, OUTPUT);
  Serial.begin(19200);
  Serial.printf("\nSDK version using ESP object: %s", ESP.getSdkVersion()); /* Using ESP object */
  Serial.printf("\nSDK version using low level function: %s", esp_get_idf_version()); /* Using low level function */  
  /* Setting UART2 - Used to serial communication whith slaves modules */
  uart_param_config(UART_ID, &uart_config);  /* Setting communication parameters */
  uart_set_pin(UART_ID, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);  /* Setting communication pins */
  uart_driver_install(UART_ID, RX_BUF_SIZE, TX_BUF_SIZE, 0, NULL, 0);  /* Driver installation */
  uart_set_line_inverse(UART_ID, UART_INVERSE_TXD);  /* Invert level of Tx line */
  gpio_set_pull_mode(RXD_PIN, GPIO_FLOATING);  /* Turn-off pull-up and pull-down of UART RX pin */
  //uart_isr_free(UART_ID);
  //uart_isr_register(UART_ID, Uart2_Intr_Handler, NULL, ESP_INTR_FLAG_IRAM, &handle_console);
  //uart_flush(UART_ID); /* Clear RX buffer */
  
  /* Setting TIMER0 - Used to define base time (tick) of 100ms */
  timer_config.divider = 80;
  timer_config.counter_dir = TIMER_COUNT_UP;
  timer_config.counter_en = TIMER_PAUSE;
  timer_config.alarm_en = TIMER_ALARM_EN;
  timer_config.intr_type = TIMER_INTR_LEVEL;
  timer_config.auto_reload = TRUE;
  timer_init(TIMER_GROUP_0, TIMER_ID, &timer_config);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_ID, 0x00000000ULL); /* Start value to Timer */
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_ID, 10000); /* Alarm value to Timer */
  timer_enable_intr(TIMER_GROUP_0, TIMER_ID);
  timer_isr_register(TIMER_GROUP_0, TIMER_ID, TIMER_Group0_ISR, (void *) TIMER_ID, ESP_INTR_FLAG_IRAM, NULL);
  timer_start(TIMER_GROUP_0, TIMER_ID);
  /* Create Schedule Table */
  mainData.scheduleTable = Comm_appl_Create_Schedule_Table();
}


void loop()
{
  if(Counter1Of10ms >= T10ms){
    Counter1Of10ms = 0;
    Task10ms();  /* call Task10ms() */
  }
  if(Counter2Of10ms >= T500ms){
    Counter2Of10ms = 0;
    Task500ms();  /* call Task500ms() */
  }
  if(Counter3Of10ms >= T1000ms){
    Counter3Of10ms = 0;
    Task1000ms();  /* call Task1000ms() */
  }
}


void Task10ms(void)
{
  Comm_appl_FSM(&mainData);
  Comm_appl_FRM(&mainData);
}


void Task500ms(void)
{
  digitalWrite(LED_ON_BOARD, !digitalRead(LED_ON_BOARD));
  Comm_appl_Request_ChangeOf_FSM_State(&mainData, FSM_State_Send);
}


void Task1000ms(void)
{

}

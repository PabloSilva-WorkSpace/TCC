/* Fish Tank automation project - TCC -  main file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
*/


/* Headers includes */  
#include "main.h"


/* Main Data of Module */
MainData mainData;
/* Variables used to set pripherals of Modulo ESP32*/
hw_timer_t *timer = NULL;
uart_config_t uart_config = {.baud_rate = 19200, .data_bits = UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits = UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
static intr_handle_t handle_console;
/* Variables into this scope (this file *.c) */
static int Counter1Of10ms = 0;
static int Counter2Of10ms = 0;
static int Counter3Of10ms = 0;


/* CallBack Timer runs every 10ms */  
void IRAM_ATTR Timer_Interrpt_Handler()
{
  Counter1Of10ms++;
  Counter2Of10ms++;
  Counter3Of10ms++;
}


static void IRAM_ATTR UART2_Interrpt_Handler(void *arg)
{
  Serial.println("Ponto 1");
  uart_clear_intr_status(UART_ID, 0x2000);
}


void setup()
{
  Serial.begin(19200);
  pinMode(LED_ON_BOARD, OUTPUT);
  WiFi.disconnect();
  /* Setting TIMER0 - Used to define base time (tick) of 100ms */
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &Timer_Interrpt_Handler, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);
  /* Setting UART2 - Used to serial communication whith slaves modules */
  uart_param_config(UART_ID, &uart_config);  /* Setting communication parameters */
  uart_set_pin(UART_ID, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);  /* Setting communication pins */
  uart_driver_install(UART_ID, RX_BUF_SIZE, TX_BUF_SIZE, 0, NULL, 0);  /* Driver installation */
  uart_set_line_inverse(UART_ID, UART_INVERSE_TXD);  /* Invert level of Tx line */
  uart_set_mode(UART_ID, UART_MODE_UART);
  gpio_set_pull_mode(RXD_PIN, GPIO_FLOATING);  /* Turn-off pull-up and pull-down of UART RX pin */
  //uart_flush(UART_ID); /* Clear input buffer */

  uart_isr_free(UART_ID); /* Release the pre registered UART handler/subroutine */
  //uart_enable_intr_mask(UART_ID, (UART_INT_ENA_REG(2)>>UART_TX_DONE_INT_ENA_S)&UART_TX_DONE_INT_ENA_V);
  uart_enable_intr_mask(UART_ID, 0x2000);
  uart_isr_register(UART_ID, UART2_Interrpt_Handler, NULL, ESP_INTR_FLAG_IRAM, &handle_console); /* Register new UART ISR subroutine */
  uart_enable_tx_intr(UART_ID, 1, 0);
  
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
  Comm_appl_FSRM(&mainData);
}


void Task500ms(void)
{
  digitalWrite(LED_ON_BOARD, !digitalRead(LED_ON_BOARD));
  Comm_appl_Request_ChangeOf_FSRM_State(&mainData, FSRM_State_Send);
}


void Task1000ms(void)
{

}

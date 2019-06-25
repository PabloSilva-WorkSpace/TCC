/* Fish Tank automation project - TCC -  main file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */

/* Headers includes */  
#include "main.h"


/* Main Data of Module */
MainData mainData;
hw_timer_t *timer = NULL;
uart_config_t uart_config = {.baud_rate = 19200, .data_bits = UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits = UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};


/* CallBack Timer runs every 100ms */  
void IRAM_ATTR Task100ms()  
{ 
  digitalWrite(LED_ON_BOARD, !digitalRead(LED_ON_BOARD));
  Comm_appl_SendData_Request(&mainData);
}


void setup()
{
  Serial.begin(19200);
  pinMode(LED_ON_BOARD, OUTPUT);
  WiFi.disconnect();
  /* Timer create */
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &Task100ms, true);
  timerAlarmWrite(timer, 100000, true);
  timerAlarmEnable(timer);
  /* Serial Config - UART 2 */
  uart_param_config(UART_ID, &uart_config); //Setting communication parameters
  uart_set_pin(UART_ID, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); //Setting communication pins
  uart_driver_install(UART_ID, RX_BUF_SIZE * 2, 0, 0, NULL, 0); //Driver installation
  uart_set_line_inverse(UART_ID, UART_INVERSE_TXD);
  uart_set_mode(UART_ID, UART_MODE_UART);
  gpio_set_pull_mode(RXD_PIN, GPIO_FLOATING); //Turn-off pull-up and pull-down of UART RX pin
}


void loop()
{
  
}

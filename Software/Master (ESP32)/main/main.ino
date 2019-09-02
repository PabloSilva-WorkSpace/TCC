/* Fish Tank automation project - TCC -  main file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
*/


/* Headers includes */  
#include "main.h"


/* Main Data of Module */
MainData mainData;
/* Setting variables to UART pripheral of Module ESP32 */
uart_config_t uart_config = {.baud_rate = 19200, .data_bits = UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits = UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
uart_intr_config_t uart_intr;
static uart_isr_handle_t handle_console;
/* Variables into this scope (this file *.c) */


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
  //uart_flush(UART_ID); /* Clear RX buffer */
  /* Create Schedule Table */
  mainData.scheduleTable = Comm_appl_Create_Schedule_Table();
  /* Tasks create */
  xTaskCreatePinnedToCore(TaskFSRM, "TaskFSRM", 2048, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(TaskUART_TX, "TaskUART_TX", 2048, NULL, 3, NULL, 0);
}


void loop()
{
   vTaskDelay(1000/portTICK_PERIOD_MS);
}


void TaskFSRM(void* Parameters)
{
  for(;;){
    Comm_appl_FSM(&mainData);
    Comm_appl_FRM(&mainData);
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}


void TaskUART_TX(void* Parameters)
{
  for(;;){
    digitalWrite(LED_ON_BOARD, !digitalRead(LED_ON_BOARD));
    Comm_appl_Request_ChangeOf_FSM_State(&mainData, FSM_State_Send);
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}



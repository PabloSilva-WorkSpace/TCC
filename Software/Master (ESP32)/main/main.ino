/*********************************************************************************************************** 
 * Fish Tank automation project - TCC -  main file
 * Developer: 
 * 
 * ToDo[PENS] - need to improve the comments
***********************************************************************************************************/


/* Headers includes */
#include "main.h"


/* Variables into this scope (this file *.c) */
MainData_t mainData;  /* Main Data of Module */
const int WIFI_CONNECTED_BIT = BIT0;

void setup()
{
  /* Configurar hardware */
  Config_configGPIO();
  Config_configUART();
  mainData.wifi.callback = &wifi_event_handler;      /* Definição da função de callback que trata dos eventos da rede WiFi. */
  mainData.wifi.event_group = xEventGroupCreate();   /* Criação de um eventgroup para sinalização do status da rede WiFi. */
  //Config_configWIFI(mainData.wifi.callback, &mainData.wifi.event_group);
  //vTaskDelay(2000/portTICK_PERIOD_MS);

  //xEventGroupWaitBits( mainData.wifi.event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY );
  
  /* Create Schedule Table */
  mainData.uart.scheduleTable = Comm_appl_Create_Schedule_Table();
  /* Tasks create */
  xTaskCreatePinnedToCore(TaskFSRM, "TaskFSRM", 2048*2, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(TaskUART_TX, "TaskUART_TX", 2048*2, NULL, 3, NULL, 0);
}


void loop()
{
  vTaskDelay(1000/portTICK_PERIOD_MS);
}


/***************************************************************************************************************************************************************** 
  @Brief: Task responsável por processar as máquinas de estado de transmissão (Tx) e recepção (Rx) de frames.
******************************************************************************************************************************************************************/
void TaskFSRM(void* Parameters)
{
  for(;;){
    Comm_appl_FSM(&mainData.uart);
    Comm_appl_FRM(&mainData.uart);
    Comm_appl_RHM(&mainData.uart);
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}


/***************************************************************************************************************************************************************** 
  @Brief: Task responsável por alterar o estado da máquina de transmissão (Tx), com o propósito da UART transmitir frames.
******************************************************************************************************************************************************************/
void TaskUART_TX(void* Parameters)
{
  for(;;){
    digitalWrite(LED_ON_BOARD, !digitalRead(LED_ON_BOARD));
    Comm_appl_Request_ChangeOf_FSM_State(&mainData.uart, FSM_State_Send);
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}

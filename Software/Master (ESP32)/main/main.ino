/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  main file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
 * 1) Alocação de slots - Dinamica ou Estatica
 * 2) Gravar e ler dados na NVS
 * 3) Comunicação MQTT
 * 4) Embarcar arquivo de binário (Embedding Binary Data) ou Embarcar arquivo de dados (Embedding Data File)
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "General_types.h"
#include "main.h"


/********************************************************************************************************************************************************************************************************************************************************
    ### Extern Global Variables 
*********************************************************************************************************************************************************************************************************************************************************/
MainData_t mainData;                                               /* Main Data of Module */
EventGroupHandle_t gWiFi_appl_event_group = xEventGroupCreate();   /* Criação de um eventgroup para sinalização do status da rede WiFi. */


/********************************************************************************************************************************************************************************************************************************************************
    Setup function
    Description: It configure hardware and initialize variables.
*********************************************************************************************************************************************************************************************************************************************************/
void setup()
{
    mainData.wifi.callback = &wifi_event_handler;      /* Definição da função de callback que trata dos eventos da rede WiFi. */
    
    Config_configGPIO();
    Config_configUART();
    Config_configWIFI(mainData.wifi.callback, &mainData.wifi.event_group);

    vTaskDelay(2000/portTICK_PERIOD_MS);
    xEventGroupWaitBits( gWiFi_appl_event_group, WIFI_STA_CONNECTED_BIT, false, true, portMAX_DELAY );    /* Aguarda o ESP32 conectar-se a uma rede WiFi */
    
    xEventGroupSetBits( gWiFi_appl_event_group, UART_TX_ENABLE );  /* Sinalizar, ou informar, a RHM por meio deste event group que a UART esta habilitada para comunicar */
    
    /* Create Schedule Table */
    Comm_appl_Create_Schedule_Table(  &mainData.uart.scheduleTable );
    vTaskDelay(2000/portTICK_PERIOD_MS);
    /* Iniciar a aplicação/serviço MQTT Client no ESP32 */
    MQTT_appl_Start_MQTT_Client();
    vTaskDelay(2000/portTICK_PERIOD_MS);
    /* Tasks create */
    xTaskCreatePinnedToCore(Task_Comm_appl, "Task_Comm_appl", 16384, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(TaskUART_TX, "TaskUART_TX", 16384, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(Task_MQTT_appl, "Task_MQTT_appl", 16384, NULL, 1, NULL, 1);
}


/********************************************************************************************************************************************************************************************************************************************************
    Loop function
    Description: It manage call tasks.
*********************************************************************************************************************************************************************************************************************************************************/
void loop()
{
    vTaskDelay(1000/portTICK_PERIOD_MS);
}


/********************************************************************************************************************************************************************************************************************************************************
  Task FSRM
  @Brief: Task responsável por processar as máquinas de estado de transmissão (Tx) de frames, recepção (Rx) de frames e manipulação de resposta.
*********************************************************************************************************************************************************************************************************************************************************/
void Task_Comm_appl(void* Parameters)
{
    for(;;){
      Comm_appl_FSM(&mainData.uart);
      Comm_appl_FRM(&mainData.uart);
      Comm_appl_RHM(&mainData.uart);
      vTaskDelay(10/portTICK_PERIOD_MS);
    }
}


/********************************************************************************************************************************************************************************************************************************************************
  Task UART TX
  @Brief: Task responsável por alterar o estado da máquina de transmissão (Tx), com o propósito da UART transmitir frames.
*********************************************************************************************************************************************************************************************************************************************************/
void TaskUART_TX(void* Parameters)
{
    for(;;){
      digitalWrite(LED_ON_BOARD, !digitalRead(LED_ON_BOARD));
      Comm_appl_Request_ChangeOf_RHM_State(&mainData.uart, RHM_State_TxUart_Send_Request);
      vTaskDelay(500/portTICK_PERIOD_MS);
    }
}


/********************************************************************************************************************************************************************************************************************************************************
  Task UART TX
  @Brief: Task responsável por alterar o estado da máquina de transmissão (Tx), com o propósito da UART transmitir frames.
*********************************************************************************************************************************************************************************************************************************************************/
void Task_MQTT_appl(void* Parameters)
{
    for(;;){
      MQTT_appl_Send_Message();
      vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}



/*
Posts Relevantes / Bibliografia 

https://techtutorialsx.com/2019/08/15/esp32-arduino-getting-wifi-event-information/
Tutorial que ensina a manipular usar as informações de eventos do wifi do ESP32

https://www.dobitaobyte.com.br/esp32-com-mqtt-servidor-web-e-sistema-de-arquivos/
Tutorial que ensina a configurar e usar o ESP32 como cliente MQTT

https://techtutorialsx.com/2017/04/24/esp32-publishing-messages-to-mqtt-topic/
Tutorial que ensina a configurar e usar o ESP32 como cliente MQTT

https://iotdesignpro.com/projects/how-to-connect-esp32-mqtt-broker
Tutorial que ensina a configurar e usar o ESP32 como cliente MQTT

https://pubsubclient.knolleary.net/api.html
Tutorial que explica as funções da biblioteca PubSubClient, usada para a comunicação MQTT

*/

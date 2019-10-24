/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  main file
 * Developer: Pablo
 * 
 * Date: 24/10/2019
 * 
 * Características
 * 1) Schedule table alocada estaticamente
 * 
 * ToDo[PS] - need to improve the comments
 * 1) Gravar e ler dados na NVS
 * 2) Comunicação MQTT
 * 3) Rever e testar a lógica de DELETAR slots
 * 4) Implementar o código de LEITURA dos sensores e ATUALIZAÇÃO do frame correspondente
 * 5) Implementar o código de LEITURA da hora (tempo real) a partir da internet
 * 6) Implementar a junção das Tasks Task_Comm_appl e TaskUART_TX
 * 
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
    
    xEventGroupWaitBits( gWiFi_appl_event_group, WIFI_STA_CONNECTED_BIT, false, true, portMAX_DELAY );    /* Aguardar o ESP32 conectar-se a uma rede WiFi */
    
    xEventGroupSetBits( gWiFi_appl_event_group, UART_TX_ENABLE );  /* Sinalizar, ou informar, a RHM por meio deste event group que a UART esta habilitada para comunicar (transmitir e receber frames) */
    
    /* Create Schedule Table */
    Comm_appl_Create_Schedule_Table(  &mainData.uart.scheduleTable );
    vTaskDelay(2000/portTICK_PERIOD_MS);
    
    /* Iniciar a aplicação/serviço MQTT Client no ESP32 */
    MQTT_appl_Start_MQTT_Client();
    vTaskDelay(2000/portTICK_PERIOD_MS);
    
    /* Criação das Tasks */
    xTaskCreatePinnedToCore(Task_Comm_appl, "Task_Comm_appl", 16384, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(Task_Control_appl, "Task_Control_appl", 16384, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(TaskUART_TX, "TaskUART_TX", 16384, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(Task_MQTT_appl, "Task_MQTT_appl", 16384, NULL, 1, NULL, 1);
    
    digitalWrite(COMM_ENABLE_OUT, LOW);   /* Enable next module to communicate */
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
    int i = 0;
    for(;;){
      /* Chamar a máquina de estados que controla o envio de frames no barramento UART */
      Comm_appl_FSM(&mainData.uart);
      /* Chamar a máquina de estados que controla a recepção de frames no barramento UART */
      Comm_appl_FRM(&mainData.uart);
      /* Chamar a máquina de estados que controla a lógica da comunicação no barramento UART */
      Comm_appl_RHM(&mainData.uart);
      /* Lógica que opera junto com a RHM, solicita a RHM que envie o frame do slot no barramento UART */
//      if(i >= 50){
//          Comm_appl_Request_ChangeOf_RHM_State(&mainData.uart, RHM_State_TxUart_Send_Request);
//      }
      /* Bloquear a Task por 10ms, a fim de liberar o processador para executar as demais Tasks */
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
      /* Chamar a máquina de estados que controla a comunicação no barramento UART */
      Comm_appl_Request_ChangeOf_RHM_State(&mainData.uart, RHM_State_TxUart_Send_Request);
      /* Bloquear a Task por 500ms, a fim de liberar o processador para executar as demais Tasks */
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
      /* Chamar a máquina de estados que envia informações ao Broker MQTT */
      MQTT_appl_Send_Message();
      /* Bloquear a Task por 5000ms, a fim de liberar o processador para executar as demais Tasks */
      vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}


/********************************************************************************************************************************************************************************************************************************************************
  Task CONTROL
  @Brief: Task responsável por alterar o estado da máquina de transmissão (Tx), com o propósito da UART transmitir frames.
*********************************************************************************************************************************************************************************************************************************************************/
void Task_Control_appl(void* Parameters)
{
    for(;;){
      /* Chamada da máquina de estados que lê o TIPO e o VALOR de cada entrada de sensor */
      Control_appl_SMC( &mainData.control );  
      /* Atualizar os campos TIPO e VALOR referente aos sensores no frame do primeiro slot (slot de configuração) */
      mainData.uart.scheduleTable.slot[0].frame.Data[_S1_TYPE]  = mainData.control.module.sensor_1.value;
      mainData.uart.scheduleTable.slot[0].frame.Data[_S1_VALUE] = mainData.control.module.sensor_1.value;
      mainData.uart.scheduleTable.slot[0].frame.Data[_S2_TYPE]  = mainData.control.module.sensor_1.value;
      mainData.uart.scheduleTable.slot[0].frame.Data[_S2_VALUE] = mainData.control.module.sensor_1.value;
      mainData.uart.scheduleTable.slot[0].frame.Data[_S3_TYPE]  = mainData.control.module.sensor_1.value;
      mainData.uart.scheduleTable.slot[0].frame.Data[_S3_VALUE] = mainData.control.module.sensor_1.value;
      mainData.uart.scheduleTable.slot[0].frame.Data[_S4_TYPE]  = mainData.control.module.sensor_1.value;
      mainData.uart.scheduleTable.slot[0].frame.Data[_S4_VALUE] = mainData.control.module.sensor_1.value;
      mainData.uart.scheduleTable.slot[0].frame.Data[_S5_TYPE]  = mainData.control.module.sensor_1.value;
      mainData.uart.scheduleTable.slot[0].frame.Data[_S5_VALUE] = mainData.control.module.sensor_1.value;
      /* Bloquear a Task por 100ms, a fim de liberar o processador para executar as demais Tasks */
      vTaskDelay(100/portTICK_PERIOD_MS);
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

/********************************************************************************************************************************************************************************************************************************************************
   Fish Tank automation project - TCC -  main file
   Developer: Pablo

   Date: 26/10/2019

   Características
   1) Schedule table alocada estaticamente

   ToDo[PS] - need to improve the comments
   1) Gravar e ler dados na NVS
   2) Comunicação MQTT
   3) Rever e testar a lógica de DELETAR slots
   4) Implementar o código de LEITURA dos sensores e ATUALIZAÇÃO do frame correspondente
   5) Implementar o código de LEITURA da hora (tempo real) a partir da internet
   6) Implementar a junção das Tasks Task_Comm_appl e TaskUART_TX

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

/*****************************HORARIO***************************************/
// Configurações do Servidor NTP
const char* servidorNTP = "a.st1.ntp.br"; // Servidor NTP para pesquisar a hora
const int fusoHorario = -10800; // Fuso horário em segundos (-03h = -10800 seg)
const int taxaDeAtualizacao = 1800000; // Taxa de atualização do servidor NTP em milisegundos
/* Declarações de protocolos */
WiFiUDP ntpUDP; // Declaração do Protocolo UDP
NTPClient timeClient(ntpUDP, servidorNTP, fusoHorario, 60000);
/*****************************TEMPERATURA***************************************/
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
/***************************PH*****************************************/
float calibration = +29.20; //change this value to calibrate
const int analogInPin = 34;
int sensorValue = 0;
unsigned long int avgValue;
float b;
int buf[10], temp;
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
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  xEventGroupWaitBits( gWiFi_appl_event_group, WIFI_STA_CONNECTED_BIT, false, true, portMAX_DELAY );    /* Aguardar o ESP32 conectar-se a uma rede WiFi */

  xEventGroupSetBits( gWiFi_appl_event_group, UART_TX_ENABLE );  /* Sinalizar, ou informar, a RHM por meio deste event group que a UART esta habilitada para comunicar (transmitir e receber frames) */

  /* Create Schedule Table */
  Comm_appl_Create_Schedule_Table(  &mainData.uart.scheduleTable );
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  /* Iniciar a aplicação/serviço MQTT Client no ESP32 */
  MQTT_appl_Start_MQTT_Client();
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  /* Criação das Tasks */
  xTaskCreatePinnedToCore(Task_Comm_appl, "Task_Comm_appl", 16384, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(Task_Control_appl, "Task_Control_appl", 16384, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(TaskUART_TX, "TaskUART_TX", 16384, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(Task_MQTT_appl, "Task_MQTT_appl", 16384, NULL, 1, NULL, 1);

  // Iniciar cliente de aquisição do tempo
  timeClient.begin();

  digitalWrite(COMM_ENABLE_OUT, LOW);   /* Enable next module to communicate */
}


/********************************************************************************************************************************************************************************************************************************************************
    Loop function
    Description: It manage call tasks.
*********************************************************************************************************************************************************************************************************************************************************/
void loop()
{

  vTaskDelay(5000 / portTICK_PERIOD_MS);
}


/********************************************************************************************************************************************************************************************************************************************************
  Task FSRM
  @Brief: Task responsável por processar as máquinas de estado de transmissão (Tx) de frames, recepção (Rx) de frames e manipulação de resposta.
*********************************************************************************************************************************************************************************************************************************************************/
void Task_Comm_appl(void* Parameters)
{
  int i = 0;
  for (;;) {
    /* Chamar a máquina de estados que controla o envio de frames no barramento UART */
    Comm_appl_FSM(&mainData.uart);
    /* Chamar a máquina de estados que controla a recepção de frames no barramento UART */
    Comm_appl_FRM(&mainData.uart);
    /* Chamar a máquina de estados que controla a lógica da comunicação no barramento UART */
    Comm_appl_RHM(&mainData.uart);
    /* Bloquear a Task por 10ms, a fim de liberar o processador para executar as demais Tasks */
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}


/********************************************************************************************************************************************************************************************************************************************************
  Task UART TX
  @Brief: Task responsável por alterar o estado da máquina de transmissão (Tx), com o propósito da UART transmitir frames.
*********************************************************************************************************************************************************************************************************************************************************/
void TaskUART_TX(void* Parameters)
{
  for (;;) {
    digitalWrite(LED_ON_BOARD, !digitalRead(LED_ON_BOARD));
    /* Chamar a máquina de estados que controla a comunicação no barramento UART */
    Comm_appl_Request_ChangeOf_RHM_State(&mainData.uart, RHM_State_TxUart_Send_Request);
    /* Bloquear a Task por 500ms, a fim de liberar o processador para executar as demais Tasks */
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}


/********************************************************************************************************************************************************************************************************************************************************
  Task UART TX
  @Brief: Task responsável por alterar o estado da máquina de transmissão (Tx), com o propósito da UART transmitir frames.
*********************************************************************************************************************************************************************************************************************************************************/
void Task_MQTT_appl(void* Parameters)
{
  int i = 0;
  for (;;) {
    MQTT_appl_Client_Loop();
    /* Chamar a máquina de estados que envia informações ao Broker MQTT */
    if (i >= 10) {
      MQTT_appl_Send_Message();
      i = 0;
    }
    i++;
    /* Bloquear a Task por 5000ms, a fim de liberar o processador para executar as demais Tasks */
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


/********************************************************************************************************************************************************************************************************************************************************
  Task CONTROL
  @Brief: Task responsável por alterar o estado da máquina de transmissão (Tx), com o propósito da UART transmitir frames.
*********************************************************************************************************************************************************************************************************************************************************/
void Task_Control_appl(void* Parameters)
{
  int i = 0;
  int temp;
  int hour;
  for (;;) {
    /* Chamada da máquina de estados que lê o TIPO e o VALOR de cada entrada de sensor */
    //Control_appl_SMC( &mainData.control );
    if (i >= 50) {
      /* Horário */
      timeClient.update();
      hour = timeClient.getHours() * 60 + timeClient.getMinutes();
      Serial.print("Hour is: ");
      Serial.println(hour);
      /* Sensor de temperatura */
      sensors.requestTemperatures(); // Send the command to get temperature readings
      temp = sensors.getTempCByIndex(0);
      Serial.print("Temperature is: ");
      Serial.println(temp);
      /* PH */
//      for (int k = 0; k < 10; k++)
//      {
//        buf[k] = analogRead(analogInPin);
//      }
//      for (int k = 0; k < 9; k++)
//      {
//        for (int j = k + 1; j < 10; j++)
//        {
//          if (buf[k] > buf[j])
//          {
//            temp = buf[k];
//            buf[k] = buf[j];
//            buf[j] = temp;
//          }
//        }
//      }
//      avgValue = 0;
//      for (int k = 2; k < 8; i++){
//        avgValue += buf[k];
//      }
//      float pHVol = (float)avgValue * 5.0 / 1024 / 6;
//      float phValue = -5.70 * pHVol + calibration;
//      Serial.print("sensor = ");
//      Serial.println(phValue);
        i = 0;
    }
    i++;
    /* Atualiza os dados de TYPE e VALOR dos sensores */
    mainData.control.module.sensor_1.type  = 0x00;
    mainData.control.module.sensor_1.value = 24; //(byte)temp;
    mainData.control.module.hour_msb       = (byte)((hour >> 8) & 0xFF);
    mainData.control.module.hour_lsb       = (byte)((hour >> 0) & 0xFF);
    /* Bloquear a Task por 100ms, a fim de liberar o processador para executar as demais Tasks */
    vTaskDelay(100 / portTICK_PERIOD_MS);
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

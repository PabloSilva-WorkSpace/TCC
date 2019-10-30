/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  MQTT_appl file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/ 
#include "General_types.h"
#include "MQTT_appl.h"


/********************************************************************************************************************************************************************************************************************************************************
    ### Global Variables into this scope (this file *.c) 
*********************************************************************************************************************************************************************************************************************************************************/
WiFiClient Aquarium;
PubSubClient client(Aquarium);   /* Instância um cliente MQTT*/

const char * mqttServer = "soldier.cloudmqtt.com";
const int    mqttPort = 10624;
const char * mqttUser = "upivxick";
const char * mqttPassword = "3yscqKRjuck3";


void MQTT_appl_Start_MQTT_Client( void )
{
    client.setServer(mqttServer, mqttPort);
    client.setCallback(MQTT_fCallback);
    /* Aguardar até o driver WiFi do ESP32 conectar-se ao AP (Access Point)*/
    xEventGroupWaitBits( gWiFi_appl_event_group, WIFI_STA_CONNECTED_BIT, false, true, portMAX_DELAY );    /* Aguarda o ESP32 conectar-se a uma rede WiFi */
    /* Aguardar até o ESP32 (MQTT Client) conectar-se ao MQTT Broker */                                            
    if ( client.connect("ESP32Client", mqttUser, mqttPassword )) { /* Method signature: boolean connect(Client ID, Username, Password) */
        if( DEBUG )
            Serial.println("MQTT: Connected");
        /* Assinar tópicos */
        client.subscribe("Plug");    /* Assina um tópico Wildcard */
        client.subscribe("Light");   /* Assina um tópico Wildcard */
    } else {
        Serial.print("MQTT: Failed in connect on Server");
        Serial.print(client.state());
    }
}


void MQTT_appl_Send_Message( void )
{
    int i, state = 0;
    char str_plug_1[100] = "Pablo";
    char str_plug_2[100] = "Silva";
    char str_plug_3[100];
    char str_plug_4[100];
    char payloadPlug[100];
    char payloadLight[100];
    char payloadSensor[100];
     
    xEventGroupWaitBits( gWiFi_appl_event_group, WIFI_STA_CONNECTED_BIT, false, true, portMAX_DELAY );    /* Aguarda o ESP32 conectar-se a uma rede WiFi */

    if ( client.connect("ESP32Client", mqttUser, mqttPassword) ){
        xEventGroupClearBits( gWiFi_appl_event_group, UART_TX_ENABLE );   /* Sinalizar, ou informar, por meio deste event group que o ESP32 deve inibir a transmissão de frames na UART  */
        switch(state){
            case 0:{ 
                
                    /* PLUG 1*/
                    if(mainData.module_plug.plug_1.mode == _MANUAL){
                        sprintf(str_plug_1, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_1.mode, mainData.module_plug.plug_1.on_off, 99, 99 , 99, mainData.module_plug.plug_1.potency);                                                                                                 
                    }
                    else if(mainData.module_plug.plug_1.mode == _AUTO_2){
                        int time_on, time_off;
                        time_on  = ( (mainData.module_plug.plug_1.hour_on_msb << 8) & (mainData.module_plug.plug_1.hour_on_lsb << 0) );
                        time_off = ( (mainData.module_plug.plug_1.hour_off_msb << 8) & (mainData.module_plug.plug_1.hour_off_lsb << 0) );
                        sprintf(str_plug_1, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_1.mode, mainData.module_plug.plug_1.on_off, time_on, time_off, 99, mainData.module_plug.plug_1.potency);
                    }
                    else if(mainData.module_plug.plug_1.mode == _AUTO_3){
                        sprintf(str_plug_1, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_1.mode, mainData.module_plug.plug_1.on_off, mainData.module_plug.plug_1.sensor_ref, mainData.module_plug.plug_1.min_max, mainData.module_plug.plug_1.set_point, mainData.module_plug.plug_1.potency);
                    }
                    /* PLUG 2*/
                    if(mainData.module_plug.plug_2.mode == _MANUAL){
                        sprintf(str_plug_2, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_2.mode, mainData.module_plug.plug_2.on_off, 99, 99, 99, mainData.module_plug.plug_2.potency);                                                                                        
                    }
                    else if(mainData.module_plug.plug_2.mode == _AUTO_2){
                        int time_on, time_off;
                        time_on  = ( (mainData.module_plug.plug_2.hour_on_msb << 8) & (mainData.module_plug.plug_2.hour_on_lsb << 0) );
                        time_off = ( (mainData.module_plug.plug_2.hour_off_msb << 8) & (mainData.module_plug.plug_2.hour_off_lsb << 0) );
                        sprintf(str_plug_2, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_2.mode, mainData.module_plug.plug_2.on_off, time_on, time_off, 99, mainData.module_plug.plug_2.potency);
                    }
                    else if(mainData.module_plug.plug_2.mode == _AUTO_3){
                        sprintf(str_plug_2, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_2.mode, mainData.module_plug.plug_2.on_off, mainData.module_plug.plug_2.sensor_ref, mainData.module_plug.plug_2.min_max, mainData.module_plug.plug_2.set_point, mainData.module_plug.plug_2.potency);
                    }
                    /* PLUG 3 */
                    if(mainData.module_plug.plug_3.mode == _MANUAL){
                        sprintf(str_plug_3, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_3.mode, mainData.module_plug.plug_3.on_off, 99, 99, 99, mainData.module_plug.plug_3.potency);                                                                                                 
                    }
                    else if(mainData.module_plug.plug_3.mode == _AUTO_2){
                        int time_on, time_off;
                        time_on  = ( (mainData.module_plug.plug_3.hour_on_msb << 8) & (mainData.module_plug.plug_3.hour_on_lsb << 0) );
                        time_off = ( (mainData.module_plug.plug_3.hour_off_msb << 8) & (mainData.module_plug.plug_3.hour_off_lsb << 0) );
                        sprintf(str_plug_3, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_3.mode, mainData.module_plug.plug_3.on_off, time_on, time_off, 99, mainData.module_plug.plug_3.potency);
                    }
                    else if(mainData.module_plug.plug_3.mode == _AUTO_3){
                        sprintf(str_plug_3, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_3.mode, mainData.module_plug.plug_3.on_off, mainData.module_plug.plug_3.sensor_ref, mainData.module_plug.plug_3.min_max, mainData.module_plug.plug_3.set_point, mainData.module_plug.plug_3.potency);
                    }
                    /* PLUG 4 */
                    if(mainData.module_plug.plug_4.mode == _MANUAL){
                        sprintf(str_plug_4, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_4.mode, mainData.module_plug.plug_4.on_off, 99, 99, 99, mainData.module_plug.plug_4.potency);                                                                                        
                    }
                    else if(mainData.module_plug.plug_4.mode == _AUTO_2){
                        int time_on, time_off;
                        time_on  = ( (mainData.module_plug.plug_4.hour_on_msb << 8) & (mainData.module_plug.plug_4.hour_on_lsb << 0) );
                        time_off = ( (mainData.module_plug.plug_4.hour_off_msb << 8) & (mainData.module_plug.plug_4.hour_off_lsb << 0) );
                        sprintf(str_plug_4, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_4.mode, mainData.module_plug.plug_4.on_off, time_on, time_off, 99, mainData.module_plug.plug_4.potency);
                    }
                    else if(mainData.module_plug.plug_4.mode == _AUTO_3){
                        sprintf(str_plug_4, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_4.mode, mainData.module_plug.plug_4.on_off, mainData.module_plug.plug_4.sensor_ref, mainData.module_plug.plug_4.min_max, mainData.module_plug.plug_4.set_point, mainData.module_plug.plug_4.potency);
                    }
                    /* Preparar string a ser transmitida para o Broker */
                    sprintf(payloadPlug, "%s,%s,%s,%s", str_plug_1, str_plug_2, str_plug_3, str_plug_4);     
                    /* Publicar os tópicos   -   ex: client.publish("PlugApp/plug_1", payloadPlug); */
                    client.publish("PlugApp", payloadPlug);
                    state = 1;
            }
            case 1:{
                sprintf(payloadSensor, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", mainData.control.module.sensor_1.type, mainData.control.module.sensor_1.value,
                                                                                          mainData.control.module.sensor_2.type, mainData.control.module.sensor_2.value,
                                                                                          mainData.control.module.sensor_3.type, mainData.control.module.sensor_3.value,
                                                                                          mainData.control.module.sensor_4.type, mainData.control.module.sensor_4.value,
                                                                                          mainData.control.module.sensor_5.type, mainData.control.module.sensor_5.value,
                                                                                          0, 0,
                                                                                          0, 0,
                                                                                          0, 0);
                client.publish("SensorApp", payloadSensor);
                state = 0;
            }
        }
        xEventGroupSetBits( gWiFi_appl_event_group, UART_TX_ENABLE );     /* Sinalizar, ou informar, por meio deste event group que o ESP32 deve habilitar a transmissão de frames na UART  */         
    }else{
        MQTT_appl_Start_MQTT_Client();
    }
}


void MQTT_appl_Client_Loop( void )
{
    if ( client.connect("ESP32Client", mqttUser, mqttPassword) ){
        client.loop();
    }else{
        MQTT_appl_Start_MQTT_Client();
    }
}


void MQTT_fCallback(char* topic, byte* payload, unsigned int payloadLength){
    int i, j;
    char * topic_Plug  = "Plug";
    char * topic_Light = "Light";
    String message[30];
    
    i = 0;
    j = 0;
    /* Lógica que quebra a string a cada vírgula (',') produzindo uma lista de strings */
    payload[payloadLength] = '\0';
    Serial.println( " " );
    Serial.println( (char*)payload );
    for(i=0;i<=payloadLength;i++){
        if( payload[i] != ',' ){
            message[j] = message[j] + (char)payload[i];
        }else{
            j++;
        }
    }
    j++; /* Número de strings geradas a partir de payload */
    
    for(i=0; i<j; i++){
        Serial.println( message[i] );
    }
    Serial.println("Param_1:");
    Serial.println(message[_FIELD_PLUG_PARAM_1]);
    Serial.println(message[_FIELD_PLUG_PARAM_1].toInt());
    Serial.println((byte)(message[_FIELD_PLUG_PARAM_1].toInt()));
    
    /* Lógica que prepara o frame de configuração */
    if( strcmp(topic, topic_Plug) == 0 ){
        if(j == _PLUG_MESSAGE_CONFIG_SIZE){
            xEventGroupClearBits( gWiFi_appl_event_group, UART_TX_ENABLE );   /* Sinalizar, ou informar, por meio deste event group que o ESP32 esta configurando um slot da schedule table. Isso deve inibir a transmissão de frames na UART  */
            Serial.println("Topico Plug Ok");
            /* Salva os dados de configuração em memória volátil */
            mainData.module_plug.plug_1.id           = (byte)(message[_FIELD_PLUG_ID].toInt());
            mainData.module_plug.plug_1.mode         = (byte)(message[_FIELD_PLUG_MODE].toInt());
            mainData.module_plug.plug_1.on_off       = ((byte)(message[_FIELD_PLUG_MODE].toInt()) == _MANUAL) ? (byte)(message[_FIELD_PLUG_PARAM_1].toInt()) : 0x00;
            mainData.module_plug.plug_1.sensor_ref   = ((byte)(message[_FIELD_PLUG_MODE].toInt()) == _AUTO_2) ? (byte)(message[_FIELD_PLUG_PARAM_1].toInt()) : 0x00;
            mainData.module_plug.plug_1.min_max      = ((byte)(message[_FIELD_PLUG_MODE].toInt()) == _AUTO_2) ? (byte)(message[_FIELD_PLUG_PARAM_2].toInt()) : 0x00;
            mainData.module_plug.plug_1.set_point    = ((byte)(message[_FIELD_PLUG_MODE].toInt()) == _AUTO_2) ? (byte)(message[_FIELD_PLUG_PARAM_3].toInt()) : 0x00;
            mainData.module_plug.plug_1.hour_on_msb  = ((byte)(message[_FIELD_PLUG_MODE].toInt()) == _AUTO_3) ? ((byte)(message[_FIELD_PLUG_PARAM_1].toInt() >> 0) & 0xFF) : 0x00;
            mainData.module_plug.plug_1.hour_on_lsb  = ((byte)(message[_FIELD_PLUG_MODE].toInt()) == _AUTO_3) ? ((byte)(message[_FIELD_PLUG_PARAM_1].toInt() >> 8) & 0xFF) : 0x00;
            mainData.module_plug.plug_1.hour_off_msb = ((byte)(message[_FIELD_PLUG_MODE].toInt()) == _AUTO_3) ? ((byte)(message[_FIELD_PLUG_PARAM_2].toInt() >> 0) & 0xFF) : 0x00;
            mainData.module_plug.plug_1.hour_off_lsb = ((byte)(message[_FIELD_PLUG_MODE].toInt()) == _AUTO_3) ? ((byte)(message[_FIELD_PLUG_PARAM_2].toInt() >> 8) & 0xFF) : 0x00;
            /* Manipula o frame de configuração do Slave */
            /* Header Fields */
            mainData.uart.scheduleTable.slot[0].frame.SID  = 0x03;
            mainData.uart.scheduleTable.slot[0].frame.Type = 0x02;
            mainData.uart.scheduleTable.slot[0].frame.Lenght = 11;
            /* Data Fields  */                    
            mainData.uart.scheduleTable.slot[0].frame.Data[_PLUG_ID]           = mainData.module_plug.plug_1.id;
            mainData.uart.scheduleTable.slot[0].frame.Data[_PLUG_MODE]         = mainData.module_plug.plug_1.mode;
            mainData.uart.scheduleTable.slot[0].frame.Data[_PLUG_ON_OFF]       = mainData.module_plug.plug_1.on_off; 
            mainData.uart.scheduleTable.slot[0].frame.Data[_PLUG_SENSOR_ID]    = mainData.module_plug.plug_1.sensor_ref;  
            mainData.uart.scheduleTable.slot[0].frame.Data[_PLUG_MIN_MAX]      = mainData.module_plug.plug_1.min_max; 
            mainData.uart.scheduleTable.slot[0].frame.Data[_PLUG_SET_POINT]    = mainData.module_plug.plug_1.set_point; 
            mainData.uart.scheduleTable.slot[0].frame.Data[_PLUG_HOUR_ON_MSB]  = mainData.module_plug.plug_1.hour_on_msb;
            mainData.uart.scheduleTable.slot[0].frame.Data[_PLUG_HOUR_ON_LSB]  = mainData.module_plug.plug_1.hour_on_lsb;
            mainData.uart.scheduleTable.slot[0].frame.Data[_PLUG_HOUR_OFF_MSB] = mainData.module_plug.plug_1.hour_off_msb;
            mainData.uart.scheduleTable.slot[0].frame.Data[_PLUG_HOUR_OFF_LSB] = mainData.module_plug.plug_1.hour_off_lsb;
            
            xEventGroupSetBits( gWiFi_appl_event_group, UART_TX_ENABLE );     /* Sinalizar, ou informar, por meio deste event group que o ESP32 finalizou a configuração do slot da schedule table. Isso deve habilitar a transmissão de frames na UART  */
        }
    }
    else if( strcmp(topic, topic_Light) == 0 ){
        xEventGroupClearBits( gWiFi_appl_event_group, UART_TX_ENABLE );   /* Sinalizar, ou informar, por meio deste event group que o ESP32 esta configurando um slot da schedule table. Isso deve inibir a transmissão de frames na UART  */
        Serial.println("Topico Light Ok");
        /* Header Fields */
        mainData.uart.scheduleTable.slot[0].frame.SID  = 0x03;
        mainData.uart.scheduleTable.slot[0].frame.Type = 0x03;
        mainData.uart.scheduleTable.slot[0].frame.Lenght = 14;
        /* Data Fields  */
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_START_HOUR]         = ((byte)(message[_FIELD_LIGHT_START].toInt() >> 0) & 0xFF);
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_START_MIN]          = ((byte)(message[_FIELD_LIGHT_START].toInt() >> 8) & 0xFF);
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_DAY_HOUR]           = ((byte)(message[_FIELD_LIGHT_DAY].toInt() >> 0) & 0xFF);
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_DAY_MIN]            = ((byte)(message[_FIELD_LIGHT_DAY].toInt() >> 0) & 0xFF);
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_NIGHT_HOUR]         = ((byte)(message[_FIELD_LIGHT_NIGHT].toInt() >> 0) & 0xFF);
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_NIGHT_MIN]          = ((byte)(message[_FIELD_LIGHT_NIGHT].toInt() >> 0) & 0xFF);
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_SWEEP_TIME]         = (byte)message[_FIELD_LIGHT_SWEEP_TIME].toInt();
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_PWM_WHITE_DAY]      = (byte)message[_FIELD_LIGHT_PWM_WHITE_DAY].toInt();
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_PWM_WHITE_NIGHT]    = (byte)message[_FIELD_LIGHT_PWM_WHITE_NIGHT].toInt();
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_PWM_BLUE_DAY]       = (byte)message[_FIELD_LIGHT_PWM_BLUE_DAY].toInt();
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_PWM_BLUE_NIGHT]     = (byte)message[_FIELD_LIGHT_PWM_BLUE_NIGHT].toInt();
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_PWM_RED_DAY]        = (byte)message[_FIELD_LIGHT_PWM_RED_DAY].toInt();
        mainData.uart.scheduleTable.slot[0].frame.Data[_LIGHT_PWM_RED_NIGHT]      = (byte)message[_FIELD_LIGHT_PWM_RED_NIGHT].toInt();       
        xEventGroupSetBits( gWiFi_appl_event_group, UART_TX_ENABLE );     /* Sinalizar, ou informar, por meio deste event group que o ESP32 finalizou a configuração do slot da schedule table. Isso deve habilitar a transmissão de frames na UART  */
    }
}






//        Serial.println("Paylod: ");
//        Serial.println(payloadPlug);
        
        //client.publish("LightApp", payloadLight);     /* Assina um tópico Wildcard */
        //client.publish("SensorApp", payloadSensor);   /* Assina um tópico Wildcard */



//
//        sprintf(str_plug_1, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_1.mode, mainData.module_plug.plug_1.on_off, 99, 99, 99, mainData.module_plug.plug_1.potency);
//        sprintf(str_plug_2, "%d,%d,%d,%d,%d,%d", mainData.module_plug.plug_2.mode, mainData.module_plug.plug_2.on_off, 99, 99, 99, mainData.module_plug.plug_2.potency);
//        
//        sprintf(payloadPlug, "%s,%s", str_plug_1, str_plug_2);
//        client.publish("PlugApp", payloadPlug);       /* Assina um tópico Wildcard */

/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  MQTT_appl file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/ 
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
    /* Verificar se o driver Wifi do ESP32 esta configurado no modo STA (Station) */
    if(WiFi.getMode() == WIFI_MODE_STA){
        /* Aguardar até o driver WiFi do ESP32 conectar-se ao AP (Access Point)*/
        xEventGroupWaitBits( gWiFi_appl_event_group, WIFI_STA_CONNECTED_BIT, false, true, portMAX_DELAY );    /* Aguarda o ESP32 conectar-se a uma rede WiFi */
        /* Aguardar até o ESP32 (MQTT Client) conectar-se ao MQTT Broker */                                            
        while(!client.connected() && WiFi.status() == WL_CONNECTED ){
            if ( client.connect("ESP32Client", mqttUser, mqttPassword )) { /* Method signature: boolean connect(Client ID, Username, Password) */
                if( DEBUG )
                    Serial.println("MQTT: Connected");
                client.subscribe("Juventino2");   /* Assina um tópico Wildcard */
            } else {
                Serial.print("MQTT: Failed in connect on Server");
                Serial.print(client.state());
                delay(2000);
            }
        }
    }
}


void MQTT_appl_Send_Message( void )
{
    xEventGroupWaitBits( gWiFi_appl_event_group, WIFI_STA_CONNECTED_BIT, false, true, portMAX_DELAY );    /* Aguarda o ESP32 conectar-se a uma rede WiFi */
    if ( client.connect("ESP32Client", mqttUser, mqttPassword) ){
        client.publish("Juventino", "oi!");
        client.loop();
    }else{
        MQTT_appl_Start_MQTT_Client();
    }
}


void MQTT_fCallback(char* topic, byte* payload, unsigned int payloadLength){
  Serial.println("");
  Serial.println( topic );
  Serial.println( payloadLength );
  Serial.println( (char *)payload );
}

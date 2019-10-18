/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  WiFi_appl file
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

extern EventGroupHandle_t gWiFi_appl_event_group;


void MQTT_appl_Start_MQTT_Client( void )
{
    client.setServer(mqttServer, mqttPort);
    /* Verificar se o driver Wifi do ESP32 esta configurado no modo STA (Station) */
    if(WiFi.getMode() == WIFI_MODE_STA){
        /* Aguardar até o driver WiFi do ESP32 conectar-se ao AP (Access Point)*/
        while (WiFi.status() != WL_CONNECTED) {    
            if( DEBUG )
                Serial.println("Connecting to WiFi..");
            delay(500);
        }
        /* Aguardar até o ESP32 (MQTT Client) conectar-se ao MQTT Broker */                                            
        while(!client.connected()){
            if ( client.connect("ESP32Client", mqttUser, mqttPassword )) { 
                Serial.println("connected");
            } else {
                Serial.print("failed with state ");
                Serial.print(client.state());
                delay(2000);
            }
        }
    }
}





/*

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println( (char *) payload);
}


*/

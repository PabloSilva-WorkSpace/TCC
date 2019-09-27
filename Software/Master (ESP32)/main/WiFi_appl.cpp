/*********************************************************************************************************** 
 * Fish Tank automation project - TCC -  main file
 * Developer: 
 * 
 * ToDo[PENS] - need to improve the comments
***********************************************************************************************************/


/* Headers includes */ 
#include "WiFi_appl.h"


/* Global variables into this scope (this file *.c) */
static EventGroupHandle_t event_group = xEventGroupCreate();   /* Criação de um eventgroup para sinalização do status da rede WiFi. */
const int WIFI_CONNECTED_BIT = BIT0;


/*******************************************************************************************************************************
 * @brief      Função callback do WiFi, referente aos modos Station e Access Point;
 *
 * @param      ctx    The context (In this case is null)
 * @param      event  Pointer to struct that contains WiFi status (i.g.: event)
 *
 * @return     { description_of_the_return_value }
*******************************************************************************************************************************/
esp_err_t wifi_event_handler( void *ctx, system_event_t *event )
{
    static int nAttempts = 0;
    switch( event->event_id ) 
    {
        /* Evento disparado quando o stack tcp for inicializado */
        case SYSTEM_EVENT_STA_START:{
            Serial.println("Ponto 1");
            esp_wifi_connect(); /* O ESP inicia a tentativa (intenção) de conexão com o AP da rede WiFi configurada */
            break;
        }
        /* Evento disparado quando o ESP recebe um IP do roteador (conexão bem sucedida entre ESP e Roteador) */
        case SYSTEM_EVENT_STA_GOT_IP:{
            if( DEBUG )
                ESP_LOGI( "Wifi", "got ip:%s", ip4addr_ntoa( &event->event_info.got_ip.ip_info.ip ));
            xEventGroupSetBits( event_group, WIFI_CONNECTED_BIT ); /* Sinaliza por meio deste event group que a conexão WiFi foi estabelecida */
            nAttempts = 0;
            break;
        }
        /* Evento disparado quando o ESP perde a conexão com a rede WiFi ou quando a tentativa de conexão não ocorrer */
        case SYSTEM_EVENT_STA_DISCONNECTED:{
            xEventGroupClearBits( event_group, WIFI_CONNECTED_BIT ); /* Reseta o bit do event group que sinaliza o status da conexão WiFi */
            if(nAttempts < 3){
                esp_wifi_connect();   /* ESP realiza nova tentativa de conexão com o AP da rede WiFi configurada */
                nAttempts++;
                Serial.println("Ponto 2");
            } else{
                Serial.println("Não foi possivel conectar-se a esta rede");
                wifi_init_ap();   /* Configuração do driver WiFi para o modo AP, a fim de configurar o ESP para tentar conectar em outra rede WiFi */
                nAttempts = 0;
            }
            break;
        }
        /* Evento disparado quando o stack tcp for inicializado */
        case SYSTEM_EVENT_AP_START:{
            Serial.println("ESP32 inicializado em modo AP");
            if ( DEBUG )
                ESP_LOGI( "Wifi", "ESP32 Inicializado em modo Acess Point.\n" );
            break;
        }
        /* Evento disparado quando algum dispositivo no modo Station conectar-se ao AP do ESP32 */
        case SYSTEM_EVENT_AP_STACONNECTED:{
            Serial.println("Dispositivo Station conectou-se ao AP do ESP32");
            if( DEBUG )
                ESP_LOGI( "Wifi", "Dispositivo conectado ao WiFi AP.\n" );
            xEventGroupSetBits( event_group, WIFI_CONNECTED_BIT );   /* Se chegou aqui significa que o Wifi do ESP foi inicializado corretamente no modo AP. Então, sinaliza por meio do event group */
            break;
        }
        /* Evento disparado quando algum dispositivo no modo Station desconectar-se do AP do ESP32 */
        case SYSTEM_EVENT_AP_STADISCONNECTED:{
            Serial.println("Dispositivo Station desconectou-se do AP do ESP32");
            if( DEBUG )
                ESP_LOGI( "Wifi", "Dispositivo conectado ao WiFi AP.\n" );    
            xEventGroupClearBits(event_group, WIFI_CONNECTED_BIT);   /* Sinaliza, ou informa, por meio do event group que um cliente foi desconectado do AP do ESP*/
            break;
        }
        /* Evento não previsto */
        default:{
            break;
        }
    }
    return ESP_OK;
}

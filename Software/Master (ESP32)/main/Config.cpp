/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  Config file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "Config.h"


/********************************************************************************************************************************************************************************************************************************************************
    ### Global Variables into this scope (this file *.c) 
*********************************************************************************************************************************************************************************************************************************************************/
static char wifi_ssid[SSID_SIZE];
static char wifi_password[PASSWORD_SIZE];
static const char * wifi_AP_SSID = "Aquarium";
static const char * wifi_AP_PASSWORD = "Aquarium"; 

uart_config_t uart_config = {
  .baud_rate = 19200, 
  .data_bits = UART_DATA_8_BITS, 
  .parity = UART_PARITY_DISABLE, 
  .stop_bits = UART_STOP_BITS_1, 
  .flow_ctrl = UART_HW_FLOWCTRL_DISABLE };   /* Setting variables to UART pripheral of Module ESP32 */


/********************************************************************************************************************************************************************************************************************************************************
  @Brief: Setting GPIOs
*********************************************************************************************************************************************************************************************************************************************************/
void Config_configGPIO()
{
    pinMode(LED_ON_BOARD, OUTPUT);
}


/********************************************************************************************************************************************************************************************************************************************************
  @Brief: Setting UARTs
*********************************************************************************************************************************************************************************************************************************************************/
void Config_configUART()
{
    /* Setting UART0 - Used to debug */
    Serial.begin(19200);
    Serial.printf("\nSDK version using ESP object: %s", ESP.getSdkVersion()); /* Using ESP object */
    Serial.printf("\nSDK version using low level function: %s", esp_get_idf_version()); /* Using low level function */  
    /* Setting UART2 - Used to serial communication whith slaves modules */
    uart_param_config(UART_ID, &uart_config);  /* Setting communication parameters */
    uart_set_pin(UART_ID, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);  /* Setting communication pins */
    uart_driver_install(UART_ID, RX_BUF_SIZE, TX_BUF_SIZE, 0, NULL, 0);  /* Driver installation */
    uart_set_line_inverse(UART_ID, UART_INVERSE_TXD);  /* Invert level of Tx line */
    gpio_set_pull_mode(RXD_PIN, GPIO_FLOATING);  /* Turn-off pull-up and pull-down of UART RX pin */
}


/********************************************************************************************************************************************************************************************************************************************************
  @Brief: Setting Wifi
*********************************************************************************************************************************************************************************************************************************************************/
void Config_configWIFI(esp_err_t (*fCallback)(void *, system_event_t *), EventGroupHandle_t * event_group)
{
    /* Inicialização da NVS, e chama a calibração do PHY do ESP32. */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    /* Resgatar os valores do SSID e PASSWORD que foram salvos na NVS */
    if( nvs_read_ssid_password( wifi_ssid, sizeof(wifi_ssid), wifi_password, sizeof(wifi_password) ) == ESP_OK )
    {
        if( DEBUG ) {
            ESP_LOGI(TAG, "ssid: %s", wifi_ssid );
            ESP_LOGI(TAG, "password: %s", wifi_password );  
        }
    }
    else{
        strcpy (wifi_ssid,"esp32_sta");
        strcpy (wifi_password,"12345678");
    }
    /* Chamada da função que tenta inicializar o WiFi no modo STATION, usando o SSID e PASSWORD resgatados da NVS */
    wifi_init_sta( wifi_ssid, wifi_password, fCallback, event_group );
}


/********************************************************************************************************************************************************************************************************************************************************
  Descrição: Função que configura e inicializa o driver WiFi do ESP no modo Station
*********************************************************************************************************************************************************************************************************************************************************/
void wifi_init_sta( char * ssid, char * password, esp_err_t (*fCallback)(void *, system_event_t *), EventGroupHandle_t * event_group )
{
    tcpip_adapter_init();   
    /* Configurar ou registrar, uma função de callback do driver WiFi (função que irá receber os eventos da conexão WiFi) */
    ESP_ERROR_CHECK(esp_event_loop_init( fCallback, event_group ) );
    /* Inicialização dos recursos para o driver WiFi */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init( &cfg ) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    /* Declaração e definição do descritor de configuração do driver WiFi */
    wifi_config_t wifi_config;
    memcpy(wifi_config.sta.ssid, ssid, (strlen(ssid) + 1));
    memcpy(wifi_config.sta.password, password, (strlen(password) + 1));
    //wifi_config.sta.scan_method = WIFI_FAST_SCAN;
    //wifi_config.sta.listen_interval = 10;
    Serial.printf("\n ssid: %s || pass: %s \n",ssid, password);
    /* Configuração e inicialização do driver WiFi no modo STATION */
    ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_STA ) );
    ESP_ERROR_CHECK( esp_wifi_set_config( ESP_IF_WIFI_STA, &wifi_config ) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    if( DEBUG ) {
        ESP_LOGI( TAG, "wifi_init_sta finished." );
        ESP_LOGI( TAG, "connect to ap SSID:%s password:%s",  (const char*)wifi_config.sta.ssid,  (const char*)wifi_config.sta.password );
    }
}


/********************************************************************************************************************************************************************************************************************************************************
  Descrição: Função que configura e inicializa o driver WiFi do ESP no modo AP (Access Point)
*********************************************************************************************************************************************************************************************************************************************************/
void wifi_init_ap( void )
{
    int i;
        
    /* Parar o driver WiFi, o qual foi previamente configurado para operar no modo Station */
    ESP_ERROR_CHECK(esp_wifi_restore());
    vTaskDelay(3000/portTICK_PERIOD_MS);

    Serial.println("AP Mode");
    
    /* Chamada da função que inicializa o servidor DHCP e define o IP statico da rede */
    start_dhcp_server();

    /* Declaração e definição do descritor de configuração do driver WiFi */
    wifi_config_t wifi_config;
    memcpy(wifi_config.ap.ssid, (uint8_t*)wifi_AP_SSID, (strlen(wifi_AP_SSID) + 1));   /* Configuração do SSID da rede do ESP */
    memcpy(wifi_config.ap.password, (uint8_t*)wifi_AP_PASSWORD, (strlen(wifi_AP_PASSWORD) + 1));   /* Configuração do PASSWORD da rede do ESP */
    wifi_config.ap.ssid_len = strlen(wifi_AP_SSID);
    wifi_config.ap.max_connection = 5;    /* Configuração do número máximo de dispositivos conectado ao ESP */
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    /* Configuração e inicialização do driver WiFi no modo AP (ACCESS POINT) */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    if( DEBUG )
        ESP_LOGI(TAG, "Servico WiFi Acess Point configurado e inicializado. \n");
}


/********************************************************************************************************************************************************************************************************************************************************
  Descrição: Função que configura e inicializa o serviço DHCP
*********************************************************************************************************************************************************************************************************************************************************/
void start_dhcp_server( void )
{  
    tcpip_adapter_init();
    /* O serviço dhcp é interrompido pois será configurado o endereço IP do ESP32 */
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
    /* Configura o endereço IP do ESP8266 na rede */
    tcpip_adapter_ip_info_t info;
    memset(&info, 0, sizeof(info));
    IP4_ADDR(&info.ip, 192, 168, 1, 1);
    IP4_ADDR(&info.gw, 192, 168, 1, 1);
    IP4_ADDR(&info.netmask, 255, 255, 255, 0);
    ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));
    /* Start DHCP server. Portanto, o ESP32 fornecerá IPs aos clientes STA na faixa de IP configurada acima. */
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
    
    if( DEBUG )
        ESP_LOGI(TAG, "Servico DHCP configurado e inicializado. \n");
}

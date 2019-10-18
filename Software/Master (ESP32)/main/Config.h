/******************************************************************************************************************************************************************************************************************************************************** 
 * Config.h
 * 
 * ToDo[PS]-  Need to insert comments
*********************************************************************************************************************************************************************************************************************************************************/


#ifndef _CONFIG_H
#define _CONFIG_H


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "NVS.h"


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Libraries includes 
*********************************************************************************************************************************************************************************************************************************************************/ 
/* Arduino */
#include <WiFi.h>
#include "Arduino.h"
/* Utilities */
#include "string.h"
/* ESP32 Drivers - To low layer programming */
#include "driver/gpio.h"
#include "driver/timer.h"
#include "driver/uart.h"

/* Conceled */
/* Drivers */
//#include "driver/uart_select.h"
/* ESP */
//#include "esp_wifi.h"
//#include "esp_system.h"
//#include "esp_event.h"
//#include "esp_event_loop.h"
//#include "esp_log.h"
//#include "esp_event_legacy.h"
//#include "esp_intr_alloc.h"
//#include "esp_types.h"
//#include "esp_attr.h"
//#include "esp_err.h"
//#include "soc/uhci_reg.h"
//#include <dummy.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/event_groups.h"


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Defines 
*********************************************************************************************************************************************************************************************************************************************************/
#define DEBUG (1)
#define TRUE  (1)
#define FALSE (0)
#define SSID_SIZE (30) 
#define PASSWORD_SIZE (30) 
#define LED_ON_BOARD (2)
#define UART_ID (UART_NUM_2)
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#define TIMER_ID (TIMER_0)


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Data Types 
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Constants
*********************************************************************************************************************************************************************************************************************************************************/
static const int RX_BUF_SIZE = 256;
static const int TX_BUF_SIZE = 256;


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Functions Prototypes 
*********************************************************************************************************************************************************************************************************************************************************/
void Config_configGPIO( void );
void Config_configUART( void );
void Config_configWIFI( void (*fCallback)(WiFiEvent_t, WiFiEventInfo_t), EventGroupHandle_t * );
void wifi_init_sta( void );
void wifi_init_ap( void );


#endif

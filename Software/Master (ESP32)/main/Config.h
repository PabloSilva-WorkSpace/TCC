/*********************************************************************************************************** 
  * Config.h
  * 
  * ToDo[idalgo]-  need to insert comments
***********************************************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H


/* Headers includes */ 
#include "NVS.h"

/* Libraries includes */
#include "Arduino.h"  
#include "driver/gpio.h"
#include "driver/timer.h"
#include "driver/uart.h"
#include "driver/uart_select.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_event_legacy.h"
#include "esp_intr_alloc.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "soc/uhci_reg.h"
#include <WiFi.h> 
#include <dummy.h>

/* Defines */
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

/* Constants */
static const int RX_BUF_SIZE = 256;
static const int TX_BUF_SIZE = 256;

/* Functions Prototypes */
void Config_configGPIO();
void Config_configUART();
void Config_configWIFI( esp_err_t (*fCallback)(void *, system_event_t *) );
void wifi_init_sta( char * ssid, char * password, esp_err_t (*fCallback)(void *, system_event_t *) );
void wifi_init_ap( void );
void start_dhcp_server( void );


#endif

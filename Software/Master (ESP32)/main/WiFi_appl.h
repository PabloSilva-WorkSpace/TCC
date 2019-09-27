/*********************************************************************************************************** 
  * Comm_appl.h
  * 
  * ToDo[idalgo]-  need to insert comments
***********************************************************************************************************/

#ifndef _WiFi_APPL_H
#define _WiFi_APPL_H

/* Headers includes */ 
#include "Config.h"

/* Libraries includes */  
#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include <sys/socket.h>
#include <netdb.h>

/* Defines */


/* Data Types  */
typedef struct{
  esp_err_t (*callback)(void *, system_event_t *);
  EventGroupHandle_t event_group;
}WiFi_t;

/* Functions Prototypes */
esp_err_t wifi_event_handler( void *ctx, system_event_t *event );
void wifi_manager( void *pvParameters );
void http_server( struct netconn *conn );

#endif

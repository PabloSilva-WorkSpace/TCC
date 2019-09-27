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


/* Defines */


/* Data Types  */
typedef struct{
  esp_err_t (*callback)(void *, system_event_t *);
  EventGroupHandle_t event_group;
}WF_t;

/* Functions Prototypes */
esp_err_t wifi_event_handler( void *ctx, system_event_t *event );

#endif

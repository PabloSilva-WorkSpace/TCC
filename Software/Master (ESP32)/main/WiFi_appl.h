/******************************************************************************************************************************************************************************************************************************************************** 
 * WiFi_appl.h
 * 
 * ToDo[PS]-  Need to insert comments
*********************************************************************************************************************************************************************************************************************************************************/


#ifndef _WiFi_APPL_H
#define _WiFi_APPL_H


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "Config.h"


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Libraries includes 
*********************************************************************************************************************************************************************************************************************************************************/   
#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include <sys/socket.h>
#include <netdb.h>


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Defines 
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Data Types 
*********************************************************************************************************************************************************************************************************************************************************/
typedef struct{
  void (*callback)( WiFiEvent_t, WiFiEventInfo_t );
  EventGroupHandle_t event_group;
}WiFi_t;


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Constants
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Functions Prototypes 
*********************************************************************************************************************************************************************************************************************************************************/
void wifi_event_handler( WiFiEvent_t, WiFiEventInfo_t );
void wifi_manager( void *pvParameters );
void http_server( struct netconn *conn );


#endif

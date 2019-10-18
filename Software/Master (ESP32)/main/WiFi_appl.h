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
    ### Extern Global Variables
*********************************************************************************************************************************************************************************************************************************************************/
extern EventGroupHandle_t gWiFi_appl_event_group;


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Constants
*********************************************************************************************************************************************************************************************************************************************************/
const int WIFI_STA_CONNECTED_BIT = BIT0;    /* Bit do event group usado para informar que o driver WiFi do ESP32 esta configurado como STA e estabeleceu conexão com o AP */
const int WIFI_AP_CONNECTED_BIT = BIT1;    /* Bit do event group usado para informar que o driver WiFi do ESP32 esta configurado como STA e estabeleceu conexão com o AP */

const static char http_html_hdr[] = "HTTP/1.1 200 OK\nContent-type: text/html\n\n";
const char html_page[] PROGMEM = "<html>\n<head>\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" charset=\"UTF-8\">\n<style>\nbody {font-family: Arial;}\n"
                                 "form {border: 3px solid #f1f1f1;}\ninput[type=text], input[type=password] {\n  width: 100%;\n  padding: 12px 20px;\n"
                                 "margin: 8px 0;\n display: inline-block;\n  border: 1px solid #ccc;\n box-sizing: border-box;\n}\nbutton {\n background-color: #4CAF50;\n"
                                 "color: white;\n padding: 14px 20px;\n margin: 8px 0;\n border: none;\n cursor: pointer;\n width: 100%;\n}\nbutton:hover {\n"
                                 "opacity: 0.8;\n}\n.container {\n  padding: 16px;\n}\n</style>\n</head>\n<body>\n<script>\n\nfunction send() {\n"
                                 "document.getElementById(\"msg\").innerHTML = \"\";\n  var ssid = document.getElementById(\"ssid\").value;\n"
                                 "var password = document.getElementById(\"password\").value;\n\n if( ssid == '' || ssid.length < 3)\n {\n"
                                 "alert(\"O campo ssid deve ter 3 ou mais caracteres.\");\n return;\n }\n\n if( password == '' || password.length < 8)\n {\n"
                                 "alert(\"O campo password deve ter 8 ou mais caracteres.\");\n return;\n }\n\n var http = new XMLHttpRequest();\n  var url = '';\n"
                                 "var params = 'ssid='+ssid+'&password='+password;\n  http.open('POST', url, true);\n http.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');\n"
                                 "http.onreadystatechange = function() {\n if(http.readyState == 4 && http.status == 200) {\n if(http.responseText === \"1\")\n {\n"
                                 "document.getElementById(\"msg\").innerHTML = \"<b>As configurações da rede WiFi foram alteradas com sucesso! Desligue e ligue o dispositivo da rede elétrica para que as novas configurações sejam carregadas.</b>\";\n"
                                 "}\n }\n }\n http.send(params);\n}\n</script>\n\n <div align=\"center\">\n <label id=\"msg\"></label>\n </div>\n <hr>\n <div class=\"container\">\n"
                                 "<label for=\"ssid\"><b>SSID</b></label>\n <input type=\"text\" placeholder=\"Enter SSID\" name=\"ssid\" required id=\"ssid\">\n <label for=\"password\"><b>Password</b></label>\n"
                                 "<input type=\"password\" placeholder=\"Enter Password\" name=\"password\" required id=\"password\"> \n <button onclick=\"send()\">Save</button>\n </div>\n\n</body>\n</html>";


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Functions Prototypes 
*********************************************************************************************************************************************************************************************************************************************************/
void wifi_event_handler( WiFiEvent_t, WiFiEventInfo_t );
void wifi_manager( void *pvParameters );
void http_server( struct netconn *conn );


#endif

/***********************************************************************************************************
 * Config.h
 * ToDo[PENS]-  need to insert comments
************************************************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H


/* Headers includes */ 
#include "Arduino.h"


/* Defines */
#define LED_ON_BOARD (13)
#define USART_BAUDRATE (19200)


/* Functions */
void Config_configGPIO();
void Config_configUART();
void Config_configTIMER();


#endif

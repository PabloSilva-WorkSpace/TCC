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


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Libraries includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "Arduino.h"


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Defines 
*********************************************************************************************************************************************************************************************************************************************************/
#define TRUE (1)
#define FALSE (0)
#define LED_ON_BOARD (13)
#define USART_BAUDRATE (19200)


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Data Types 
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Constants
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Functions Prototypes 
*********************************************************************************************************************************************************************************************************************************************************/
void Config_configGPIO();
void Config_configUART();
void Config_configTIMER();


#endif

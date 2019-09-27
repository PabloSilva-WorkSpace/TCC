/*********************************************************************************************************** 
  * main.h
  * 
  * ToDo[idalgo]-  need to insert comments
***********************************************************************************************************/

#ifndef _MAIN_H
#define _MAIN_H

/* Headers includes */ 
#include "Comm_appl.h"  /* Declara os dados e funções referentes a comunicação UART */
#include "WiFi_appl.h"  /* Declara os dados e funções referentes a comunicação WiFi */

/* Libraries includes */  


/* Defines */
#define T10ms (1)
#define T500ms (50)
#define T1000ms (100)

/* Functions */
void TaskFSRM(void*);    /* Each 10ms */
void TaskUART_TX(void*); /* Each 500ms */

/* Constants */

/* Data Types */
typedef struct{
  Uart_t uart;
  WF_t wifi;
}MainData_t;


#endif

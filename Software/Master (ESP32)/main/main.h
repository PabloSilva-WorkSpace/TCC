/*
   main.h
   /* ToDo[idalgo]-  need to insert comments
*/

#ifndef _MAIN_H
#define _MAIN_H

/* Headers includes */ 
#include "Comm_appl.h"

/* Libraries includes */  
#include <WiFi.h> 
#include <dummy.h>
#include "driver/uart.h"
#include "driver/gpio.h"

/* Defines */
#define T10ms (1)
#define T500ms (50)
#define T1000ms (100)
#define LED_ON_BOARD (2)
#define UART_ID (UART_NUM_2)
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

/* Functions */
void Task500ms(void);
void Task1000ms(void);

/* Constants */
static const int RX_BUF_SIZE = 256; //1024
static const int TX_BUF_SIZE = 256; //0


#endif

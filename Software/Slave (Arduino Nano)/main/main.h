/*
   main.h
   /* ToDo[idalgo]-  need to insert comments
*/

#ifndef _MAIN_H
#define _MAIN_H

/* Headers includes */ 
#include "Comm_appl.h"

/* Libraries includes */  


/* Defines */
#define T10ms (1)
#define T500ms (50)
#define T1000ms (100)

#define TIMER_ID (TIMER_0)
#define FALSE (0)
#define TRUE (1)

//#define RX_BUFFER_SIZE (64)

//#define TXD_PIN (GPIO_NUM_17)
//#define RXD_PIN (GPIO_NUM_16)

/* Functions */
void Task10ms(void);
void Task500ms(void);
void Task1000ms(void);

/* Constants */
static const int RX_BUF_SIZE = 256; 
static const int TX_BUF_SIZE = 256; 


#endif

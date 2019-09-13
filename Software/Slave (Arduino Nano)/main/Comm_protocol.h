/*
   Comm_protocol.h
   /* ToDo[idalgo]-  need to insert comments
*/

#ifndef _COMM_PROTOCOL_H
#define _COMM_PROTOCOL_H

/* Headers includes */ 
#include "main.h"
#include "Comm_appl.h"

/* Libraries includes */  
#include "Arduino.h"

/* Defines */


/* Functions Prototypes */
int Comm_protocol_Frame_Send_Request(byte *, int);
int Comm_protocol_Frame_Get_Request(byte *, int);

int Comm_protocol_Get_TXFIFO_Lenght(void);
int Comm_protocol_Get_RXFIFO_Lenght(void);


#endif

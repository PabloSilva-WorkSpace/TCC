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


/* Defines */


/* Functions Prototypes */
int Comm_protocol_Frame_Send_Request(struct Frame *);
char * structToString(struct Frame *);

int Comm_protocol_Get_TXFIFO_Lenght(void);
int Comm_protocol_Get_RXFIFO_Lenght(void);


#endif

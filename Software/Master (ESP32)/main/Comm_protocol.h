/*********************************************************************************************************** 
  * Comm_protocol.h
  * 
  * ToDo[idalgo]-  need to insert comments
***********************************************************************************************************/

#ifndef _COMM_PROTOCOL_H
#define _COMM_PROTOCOL_H

/* Headers includes */
#include "Config.h"

/* Libraries includes */  

/* Defines */
#define MAX_DATA_SIZE (20)

/* Variables  */
typedef struct {
  byte Break = 0x00;          /* Break signal */
  byte Synch = 0x55;          /* Synch signal */
  byte Id_Source = 0x01;      /* ID do módulo transmissor */
  byte Id_Target = 0x00;      /* ID do módulo alvo */
  byte Lenght = 0x01;         /* Comprimento da mensagem */
  byte Type = 0x01;           /* Tipo de módulo do transmissor */
  byte SID = 0x00;            /* Identificador de serviço da mensagem */
  byte Data[MAX_DATA_SIZE];   /* Dados */
  byte Checksum = 0x00;       /* Checksum */
}Frame_t;

struct Slot{
  Frame_t frame;
  Slot *nextSlot;
};

typedef struct Slot Slot_t;

/* Functions Prototypes */
int Comm_protocol_Frame_Send_Request( Frame_t * );
char * structToString( Frame_t * );

int Comm_protocol_Get_TXFIFO_Lenght(void);
int Comm_protocol_Get_RXFIFO_Lenght(void);


#endif

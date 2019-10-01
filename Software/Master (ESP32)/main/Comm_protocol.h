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
#define _FRAME_MAX_DATA_SIZE (20)
#define _RX_BUFFER_SIZE (64)
#define _TX_BUFFER_SIZE (64)

#define _BREAK   (0U)
#define _SYNCH   (1U)
#define _SID     (2U)
#define _TYPE    (3U)
#define _ID_SRC  (4U)
#define _ID_TRG  (5U)
#define _LENGHT  (6U)
#define _DATA    (7U)

/* Variables  */
typedef struct {
  byte Break = 0x00;                 /* Break signal */
  byte Synch = 0x55;                 /* Synch signal */
  byte SID = 0x01;                   /* Identificador de serviço da mensagem */
  byte Type = 0x01;                  /* Tipo de módulo do transmissor */
  byte Id_Source = 0x01;             /* ID do módulo transmissor */
  byte Id_Target = 0xFF;             /* ID do módulo alvo */
  byte Lenght = 0x11;                /* Comprimento da mensagem */
  byte Data[_FRAME_MAX_DATA_SIZE];   /* Dados */
  byte Checksum = 0x00;              /* Checksum */
}Frame_t;

struct Slot{
  Frame_t frame;
  Slot *nextSlot;
};

typedef struct Slot Slot_t;

typedef char RxBuffer_t[_RX_BUFFER_SIZE];
typedef char TxBuffer_t[_TX_BUFFER_SIZE];

/* Functions Prototypes */
int Comm_protocol_Frame_Send_Request( TxBuffer_t *, int );
int Comm_protocol_Frame_Read_Request( TxBuffer_t *, int );

int Comm_protocol_Get_TxFIFO_Length( void );
int Comm_protocol_Get_RxFIFO_Length( void );
int Comm_protocol_Get_RxBUFFER_Length( void );


#endif

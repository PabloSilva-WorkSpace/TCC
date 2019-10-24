/******************************************************************************************************************************************************************************************************************************************************** 
 * Comm_protocol.h
 * 
 * ToDo[PS]-  Need to insert comments
*********************************************************************************************************************************************************************************************************************************************************/


#ifndef _COMM_PROTOCOL_H
#define _COMM_PROTOCOL_H


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "Config.h"


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Libraries includes 
*********************************************************************************************************************************************************************************************************************************************************/  
#include "Arduino.h"


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Defines 
*********************************************************************************************************************************************************************************************************************************************************/
#define _FRAME_MAX_DATA_SIZE (24)
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


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Data Types 
*********************************************************************************************************************************************************************************************************************************************************/
typedef struct {
  byte Break = 0x00;                /* Break signal */
  byte Synch = 0x55;                /* Synch signal */
  byte SID = 0x01;                  /* Identificador de serviço da mensagem */
  byte Type = 0x02;                 /* Tipo de módulo transmissor */
  byte Id_Source = 0x00;            /* ID do módulo transmissor */
  byte Id_Target = 0x01;            /* ID do módulo alvo */
  byte Lenght = 0x01;               /* Comprimento da mensagem */
  byte Data[_FRAME_MAX_DATA_SIZE];  /* Dados */
  byte Checksum = 0x00;             /* Checksum */
}Frame_t;

typedef byte RxBuffer_t[_RX_BUFFER_SIZE];
typedef byte TxBuffer_t[_TX_BUFFER_SIZE];


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Constants
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Functions Prototypes 
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_protocol_Frame_Send_Request(TxBuffer_t *, int);
int Comm_protocol_Frame_Read_Request(RxBuffer_t *, int);

int Comm_protocol_Get_TxFIFO_Lenght(void);
int Comm_protocol_Get_RxFIFO_Lenght(void);


#endif

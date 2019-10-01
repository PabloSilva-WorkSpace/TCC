/*********************************************************************************************************** 
  * Comm_appl.h
  * 
  * ToDo[idalgo]-  need to insert comments
***********************************************************************************************************/

#ifndef _COMM_APPL_H
#define _COMM_APPL_H

/* Headers includes */
#include "Comm_protocol.h"  /* Declara as funções de acesso a interface UART */

/* Libraries includes */  

/* Defines */

/* Data Types  */
typedef enum {
    FSM_State_Idle = 0,
    FSM_State_Send = 1,
    FSM_State_Sending = 2,
    FSM_State_Error = 3,  
}FSM_States_t;

typedef enum {
    FRM_State_Idle = 0,
    FRM_State_Receiving = 1,
    FRM_State_Received = 2,
    FRM_State_Error = 3,  
}FRM_States_t;

typedef enum {
    RHM_State_Idle = 0,
    RHM_State_Process = 1, 
}RHM_States_t;

typedef struct{
    Slot_t *scheduleTable;
    RxBuffer_t RxBuffer;
    TxBuffer_t TxBuffer;
    FSM_States_t FSM_State = FSM_State_Idle;
    FRM_States_t FRM_State = FRM_State_Idle;
    RHM_States_t RHM_State = RHM_State_Idle;
}Uart_t;


/********************************************************************************************************************************************
    Functions Prototypes 
*********************************************************************************************************************************************/
byte Comm_appl_FSM( Uart_t * );  /* FSM = Frame Send Machine ou Máquina de envio de frames */
void Comm_appl_Request_ChangeOf_FSM_State( Uart_t *, FSM_States_t );

byte Comm_appl_FRM( Uart_t * );  /* FRM = Frame Receive Machine ou Máquina de recepção de frames */
void Comm_appl_Request_ChangeOf_FRM_State( Uart_t *, FRM_States_t );

byte Comm_appl_RHM( Uart_t * );
void Comm_appl_Request_ChangeOf_RHM_State( Uart_t *, RHM_States_t );

void Comm_appl_Set_Frame_Header( Frame_t *, byte, byte, byte, byte, byte, byte, byte );
void Comm_appl_Set_Frame_Data( Frame_t *, byte *, int );
void Comm_appl_Set_Frame_Checksum( Frame_t * );

int Comm_appl_FrameToBuffer( Uart_t * );
int Comm_appl_Echo_Frame( Uart_t * );
int Comm_appl_Validate_Frame( Uart_t * );

Slot_t *Comm_appl_Create_Schedule_Table(void);
void Comm_appl_Insert_Slot(Slot_t *);
Slot_t *Comm_appl_Select_Next_Slot(Slot_t *);

#endif

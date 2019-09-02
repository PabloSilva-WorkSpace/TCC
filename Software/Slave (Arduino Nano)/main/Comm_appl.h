/*
   Comm_appl.h
   /* ToDo[idalgo]-  need to insert comments
*/

#ifndef _COMM_APPL_H
#define _COMM_APPL_H

/* Headers includes */ 
#include "Comm_protocol.h"

/* Libraries includes */  
#include "Arduino.h"

/* Defines */
#define MAX_DATA_SIZE 20
#define RX_BUFFER_SIZE (64)
#define TX_BUFFER_SIZE (64)

/* Variables  */
enum FSM_States {
  FSM_State_Idle = 0,
  FSM_State_Send = 1,
  FSM_State_Sending = 2,
  FSM_State_Error = 3,  
};

enum FRM_States {
  FRM_State_Idle = 0,
  FRM_State_Receiving = 1,
  FRM_State_Received = 2,
  FRM_State_Error = 3,  
};

struct Frame{
  byte Break = 0x00;          /* Break signal */
  byte Synch = 0x55;          /* Synch signal */
  byte Id_Source = 0x00;      /* ID do módulo transmissor */
  byte Id_Target = 0x01;      /* ID do módulo alvo */
  byte Lenght = 0x03;         /* Comprimento da mensagem */
  byte Type = 0x02;           /* Tipo de módulo do transmissor */
  byte SID = 0x01;            /* Identificador de serviço da mensagem */
  byte Data[MAX_DATA_SIZE];   /* Dados */
  byte Checksum = 0x00;       /* Checksum */
};

struct MainData{
  struct Frame frame;
  byte RxBuffer[RX_BUFFER_SIZE];
  byte TxBuffer[TX_BUFFER_SIZE];
  enum FSM_States FSM_State =  FSM_State_Idle;
  enum FRM_States FRM_State =  FRM_State_Idle;
};

/* Functions */
byte Comm_appl_FSM(struct MainData *);  /* FSM = Frame Send Machine ou Máquina de envio de frames */
void Comm_appl_Request_ChangeOf_FSM_State(struct MainData *, enum FSM_States);

byte Comm_appl_FRM(struct MainData *);  /* FRM = Frame Receive Machine ou Máquina de recepção de frames */
void Comm_appl_Request_ChangeOf_FRM_State(struct MainData *, enum FRM_States);

void Comm_appl_Set_Frame_Header(struct Frame *, byte, byte, byte, byte, byte, byte, byte);
void Comm_appl_Set_Frame_Data(struct Frame *, byte *, int);
void Comm_appl_Set_Frame_Checksum(struct Frame *);

int Comm_appl_frameToBuffer(struct MainData *);

int validacao(byte *Buffer, int lenght); /* Função de teste - Apagar*/

#endif

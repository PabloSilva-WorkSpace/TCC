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

/* Variables  */
enum FSRM_States {
  FSRM_State_Idle = 0,
  FSRM_State_Send = 1,
  FSRM_State_Error = 2,  
};

struct Frame{
  byte Break = 0x00;          /* Break signal */
  byte Synch = 0x55;          /* Synch signal */
  byte Id_Source = 0x01;      /* ID do módulo transmissor */
  byte Id_Target = 0x00;      /* ID do módulo alvo */
  byte Lenght = 0x01;         /* Comprimento da mensagem */
  byte Type = 0x01;           /* Tipo de módulo do transmissor */
  byte SID = 0x00;            /* Identificador de serviço da mensagem */
  byte Data[MAX_DATA_SIZE];   /* Dados */
  byte Checksum = 0x00;       /* Checksum */
};

struct Slot{
  struct Frame frame;
  struct Slot *nextSlot;
};

struct MainData{
  struct Slot *scheduleTable;
  enum FSRM_States FSRM_State =  FSRM_State_Idle;
};

/* Functions */
byte Comm_appl_FSRM(struct MainData *);  /* FSRM = Frame Sending and Receiving Machine ou Máquina de envio e recepção de frames */
void Comm_appl_Request_ChangeOf_FSRM_State(struct MainData *, enum FSRM_States);

void Comm_appl_Set_Frame_Header(struct Frame *, byte, byte, byte, byte, byte, byte, byte);
void Comm_appl_Set_Frame_Data(struct Frame *, byte *, int);
void Comm_appl_Set_Frame_Checksum(struct Frame *);

struct Slot *Comm_appl_Create_Schedule_Table(void);
void Comm_appl_Insert_Slot(struct Slot *);
struct Slot *Comm_appl_Select_Next_Slot(struct Slot *);

#endif

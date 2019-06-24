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
struct Frame{
  byte Break = 0x00;
  byte Id_Source = 0x01;
  byte Type = 0x01;
  byte Lenght = 0x01;
  byte SID = 0x00;
  byte Id_Target = 0x00;
  byte Data[MAX_DATA_SIZE];
  byte Cs = 0x00;  
};

enum Comm_Send_Status {
  Comm_appl_Status_Idle = 0,
  Comm_appl_Status_Send = 1,
  Comm_appl_Status_Error = 2,  
};

struct MainData{
  Frame frame;
  Comm_Send_Status Status_Send_Rqt =  Comm_appl_Status_Send;  
};

/* Functions */
byte Comm_appl_SendData_Request(MainData *pData);

#endif

/*
 *  Comm_appl.h
 *  
 *  ToDo[idalgo]-  need to insert comments
*/

#ifndef _COMM_APPL_H
#define _COMM_APPL_H


/* Headers includes */ 
#include "Comm_protocol.h"


/* Libraries includes */  
#include "Arduino.h"


/******************************************************************************************************************************************** 
    Defines 
*********************************************************************************************************************************************/
#define _FRAME_MAX_DATA_SIZE (20)
#define _RX_BUFFER_SIZE (64)
#define _TX_BUFFER_SIZE (64)

#define _CMD_TABLE_MAX_SIZE (20)
#define _CMD_CODE_FILTER_SIZE (5)

#define _BREAK   (0U)
#define _SYNCH   (1U)
#define _SID     (2U)
#define _TYPE    (3U)
#define _ID_SRC  (4U)
#define _ID_TRG  (5U)
#define _LENGHT  (6U)
#define _DATA    (7U)


/********************************************************************************************************************************************  
    Variables
*********************************************************************************************************************************************/
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


enum RHM_States {
  RHM_State_Idle = 0,
  RHM_State_Process = 1, 
};


struct Frame{
  byte Break = 0x00;                /* Break signal */
  byte Synch = 0x55;                /* Synch signal */
  byte SID = 0x01;                  /* Identificador de serviço da mensagem */
  byte Type = 0x02;                 /* Tipo de módulo transmissor */
  byte Id_Source = 0x00;            /* ID do módulo transmissor */
  byte Id_Target = 0x01;            /* ID do módulo alvo */
  byte Lenght = 0x01;               /* Comprimento da mensagem */
  byte Data[_FRAME_MAX_DATA_SIZE];  /* Dados */
  byte Checksum = 0x00;             /* Checksum */
};


struct MainData{
  struct Frame frame;
  byte RxBuffer[_RX_BUFFER_SIZE];
  byte TxBuffer[_TX_BUFFER_SIZE];
  enum FSM_States FSM_State =  FSM_State_Idle;   /* Variável de estado da máquina de enviar frame (Frame Sending Machine) */
  enum FRM_States FRM_State =  FRM_State_Idle;   /* Variável de estado da máquina de receber frame (Frame Receiving Machine) */
  enum RHM_States RHM_State =  RHM_State_Idle;   /* Variável de estado da máquina de manipular resposta (Response Handling Machine) */
};


typedef enum {
  /* Non error codes */
  KOSTIA_NOK = 0,
  KOSTIA_OK = 1,
  KOSTIA_EXECUTING = 2,
  KOSTIA_CMD_RX = 3,
  KOSTIA_DATA_RECEIVED = 4,
  /* Error codes */
  KOSTIA_ER_NO_LGN_MATCHES = -1,
  KOSTIA_ER_WRONG_BUF_SIZE = -2,
  KOSTIA_ER_LGN_REQUIRED = -3,
  KOSTIA_ER_INVALID = -4,
  KOSTIA_ER_NOT_INIT = -5,      /* Instance was not initialized */
  KOSTIA_ER_TYPE_NOTFIND = -6,  /* Type not found */
  KOSTIA_ER_CMD_NOTFIND = -7,   /* Command not found */
  KOSTIA_ER_LGN_DENIED = -8,
  KOSTIA_ER_WRONG_INDEX = -9    /* Invalid index received */
} Kostia_TRsp;


typedef struct{
  byte au08Command[_CMD_CODE_FILTER_SIZE];                       /* Command code */
  byte u08Mask;                                                  /* Command code mask, to say what part of the Kostia data stream represents the command code */
  Kostia_TRsp (*pfExecute)(byte *pCmd, struct MainData *);       /* Callback to command execute function */
}Kostia_TCmdTable;


/********************************************************************************************************************************************
    Functions Prototypes 
*********************************************************************************************************************************************/
byte Comm_appl_FSM(struct MainData *);  /* FSM = Frame Send Machine (Máquina de Envio de Frames) */
void Comm_appl_Request_ChangeOf_FSM_State(struct MainData *, enum FSM_States);

byte Comm_appl_FRM(struct MainData *);  /* FRM = Frame Receive Machine (Máquina de Recepção de Frames) */
void Comm_appl_Request_ChangeOf_FRM_State(struct MainData *, enum FRM_States);

byte Comm_appl_RHM(struct MainData *);  /* RHM = Response Handling Machine (Máquina de Tratamento de Resposta) */
void Comm_appl_Request_ChangeOf_RHM_State(struct MainData *, enum RHM_States);

void Comm_appl_Set_Frame_Header(struct Frame *, byte, byte, byte, byte, byte, byte, byte);
void Comm_appl_Set_Frame_Data(struct Frame *, byte *, int);
void Comm_appl_Set_Frame_Checksum(struct Frame *);

int  Comm_appl_FrameToBuffer(struct MainData *);
int  Comm_appl_Echo_Frame(struct MainData *);
int  Comm_appl_Validate_Frame(struct MainData *);

/* CMD  Table Functions */
static Kostia_TRsp Comm_appl_QueryID (byte *pCmd, struct MainData *);
static Kostia_TRsp Comm_appl_SetID (byte *pCmd, struct MainData *);
static Kostia_TRsp Comm_appl_RequestData (byte *pCmd, struct MainData *);
static Kostia_TRsp Comm_appl_CmdTableError(byte *pAddr, struct MainData *);
static Kostia_TRsp Comm_appl_FindCommand(byte *pAddr, struct MainData *);


#endif

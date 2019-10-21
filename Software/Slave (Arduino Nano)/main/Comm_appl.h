/******************************************************************************************************************************************************************************************************************************************************** 
 * Comm_appl.h
 * 
 * ToDo[PS]-  Need to insert comments
*********************************************************************************************************************************************************************************************************************************************************/


#ifndef _COMM_APPL_H
#define _COMM_APPL_H


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "Comm_protocol.h"


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Libraries includes 
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Defines 
*********************************************************************************************************************************************************************************************************************************************************/
#define _CMD_TABLE_MAX_SIZE (20)
#define _CMD_CODE_FILTER_SIZE (5)


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Data Types 
*********************************************************************************************************************************************************************************************************************************************************/
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
    Frame_t frame;
    RxBuffer_t RxBuffer;
    TxBuffer_t TxBuffer;
    FSM_States_t FSM_State =  FSM_State_Idle;   /* Variável de estado da máquina de enviar frame (Frame Sending Machine) */
    FRM_States_t FRM_State =  FRM_State_Idle;   /* Variável de estado da máquina de receber frame (Frame Receiving Machine) */
    RHM_States_t RHM_State =  RHM_State_Idle;   /* Variável de estado da máquina de manipular resposta (Response Handling Machine) */
}Uart_t;

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
}Kostia_Rsp_t;

typedef struct{
    byte au08Command[_CMD_CODE_FILTER_SIZE];          /* Command code */
    byte u08Mask;                                     /* Command code mask, to say what part of the Kostia data stream represents the command code */
    Kostia_Rsp_t (*pfExecute)(byte *pCmd, Uart_t *);   /* Callback to command execute function */
}Kostia_CmdTable_t;


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Constants
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Functions Prototypes 
*********************************************************************************************************************************************************************************************************************************************************/
byte Comm_appl_FSM( Uart_t * );  /* FSM = Frame Send Machine (Máquina de Envio de Frames) */
void Comm_appl_Request_ChangeOf_FSM_State( Uart_t *, FSM_States_t );

byte Comm_appl_FRM( Uart_t * );  /* FRM = Frame Receive Machine (Máquina de Recepção de Frames) */
void Comm_appl_Request_ChangeOf_FRM_State( Uart_t *, FRM_States_t );

byte Comm_appl_RHM( Uart_t * );  /* RHM = Response Handling Machine (Máquina de Tratamento de Resposta) */
void Comm_appl_Request_ChangeOf_RHM_State( Uart_t *, RHM_States_t );

void Comm_appl_Init_Slave_Data(Uart_t *, byte, byte);

void Comm_appl_Set_Frame_Header( Frame_t *, byte, byte, byte, byte, byte, byte, byte );
void Comm_appl_Set_Frame_Data( Frame_t *, byte *, int );
void Comm_appl_Set_Frame_Checksum( Frame_t * );

int Comm_appl_FrameToBuffer( Uart_t * );
int Comm_appl_Check_Frame_IsEcho( Uart_t * );
int Comm_appl_Check_Frame_IsValid( Uart_t * );

/* CMD  Table Functions */
static Kostia_Rsp_t Comm_appl_QueryID (byte *pCmd, Uart_t *);
static Kostia_Rsp_t Comm_appl_SetID (byte *pCmd, Uart_t *);
static Kostia_Rsp_t Comm_appl_ConfigSlave(byte *pCmd, Uart_t *);
static Kostia_Rsp_t Comm_appl_RequestData (byte *pCmd, Uart_t *);
static Kostia_Rsp_t Comm_appl_CmdTableError(byte *pAddr, Uart_t *);
static Kostia_Rsp_t Comm_appl_FindCommand(byte *pAddr, Uart_t *);


#endif

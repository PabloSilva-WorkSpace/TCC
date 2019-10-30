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
#include "Comm_protocol.h"  /* Declara as funções de acesso a interface UART */


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Libraries includes 
*********************************************************************************************************************************************************************************************************************************************************/ 


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Defines 
*********************************************************************************************************************************************************************************************************************************************************/
#define _CMD_TABLE_MAX_SIZE (20)
#define _CMD_CODE_FILTER_SIZE (5)

#define _SCHEDULE_TABLE_MAX_SIZE (10)


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
    RHM_State_TxUart_Send_Request = 1,
    RHM_State_RxUart_Notify_Response = 2,
    RHM_State_RxUart_Notify_Echo = 3,
    RHM_State_Process = 4,
}RHM_States_t;

typedef struct {
    Slot_t slot[_SCHEDULE_TABLE_MAX_SIZE];    /* Table of Slot_t */
    Slot_t * pSlot;                           /* Pointer to current slot */
    Slot_t * pFirstSlot;                      /* Pointer to first slot  - This slot is to config slaves */
    Slot_t * pLastSlot;                       /* Pointer to last slot */
    Slot_t * pLastSlotSent;                   /* Pointer to last slot sended*/
    int Length;                               /* Quantity of slots */
}ScheduleTable_t;

typedef struct{
    ScheduleTable_t scheduleTable;
    RxBuffer_t RxBuffer;
    TxBuffer_t TxBuffer;
    FSM_States_t FSM_State = FSM_State_Idle;
    FRM_States_t FRM_State = FRM_State_Idle;
    RHM_States_t RHM_State = RHM_State_Idle;
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
byte Comm_appl_FSM( Uart_t * );   /* FSM = Frame Send Machine ou Máquina de envio de frames */
void Comm_appl_Request_ChangeOf_FSM_State( Uart_t *, FSM_States_t );

byte Comm_appl_FRM( Uart_t * );   /* FRM = Frame Receive Machine ou Máquina de recepção de frames */
void Comm_appl_Request_ChangeOf_FRM_State( Uart_t *, FRM_States_t );

byte Comm_appl_RHM( Uart_t * );   /* RHM = Response Handling Machine (Máquina de Tratamento de Resposta) */
void Comm_appl_Request_ChangeOf_RHM_State( Uart_t *, RHM_States_t );

void Comm_appl_Set_Frame_Header( Frame_t *, byte, byte, byte, byte, byte, byte, byte );
void Comm_appl_Set_Frame_Data( Frame_t *, byte *, int );
void Comm_appl_Set_Frame_Checksum( Frame_t * );

int Comm_appl_FrameToBuffer( Uart_t * );
int Comm_appl_Check_Frame_IsEcho( Uart_t * );
int Comm_appl_Check_Frame_IsValid( Uart_t * );
void Comm_appl_Prepara_QueryID( void );

void Comm_appl_Create_Schedule_Table( ScheduleTable_t * );
void Comm_appl_Insert_Slot( ScheduleTable_t * );
byte Comm_appl_Define_Slave_ID( ScheduleTable_t * );
void Comm_appl_Delete_Slot( ScheduleTable_t * );
void Comm_appl_Reset_Slot(Slot_t *);
Slot_t *Comm_appl_Select_Next_Slot(Slot_t *);

/* CMD  Table Functions */
static Kostia_Rsp_t Comm_appl_QueryID_Callback (byte *pCmd, Uart_t *);
static Kostia_Rsp_t Comm_appl_SetID_Callback (byte *pCmd, Uart_t *);
static Kostia_Rsp_t Comm_appl_ConfigSlave_Callback (byte *pCmd, Uart_t *);
static Kostia_Rsp_t Comm_appl_RequestData_Callback (byte *pCmd, Uart_t *);
static Kostia_Rsp_t Comm_appl_CmdTableError(byte *pAddr, Uart_t *);
static Kostia_Rsp_t Comm_appl_FindCommand(byte *pAddr, Uart_t *);

#endif

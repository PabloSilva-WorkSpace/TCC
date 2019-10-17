/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  Comm_appl file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "Comm_appl.h"


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Global Variables into this scope (this file *.c)
*********************************************************************************************************************************************************************************************************************************************************/
/* Tabela de todas as mensagens de resposta disponíveis do Slaves para o ESP. Formatação da tabela: {{SID,TYPE}, byte, callback function} */
static const Kostia_CmdTable_t CmdTable_FromSlaveToMaster[] = {
    {{0x01U, 0x02U}, 0x01U, Comm_appl_QueryID_Callback},         /* Response to command: Query if slave is configured */
    {{0x02U, 0x02U}, 0x01U, Comm_appl_SetID_Callback},           /* Response to command: Set ID to slave */
//    {{0x03U, 0x01U}, 0x01U, Comm_appl_RequestData_Callback},   /* Response to command: Request slave's data */
    {{0x00U, 0x00U}, 0x00U, Comm_appl_CmdTableError}  /* Response to command: Must be the last element */
};


/********************************************************************************************************************************************************************************************************************************************************
    Função: Frame Send Machine (FSM)
    Descrição: Esta função é uma máquina de estados que controla o envio de frames
    \Parametros: struct MainData *pMainData - estrutura de dados principal
*********************************************************************************************************************************************************************************************************************************************************/
byte Comm_appl_FSM( Uart_t *pUart )
{
    switch (pUart->FSM_State){
        case FSM_State_Idle:
        {
            break;
        }
        case FSM_State_Send:   /* Start execute of frame sending on bus */
        {
            int TxBuff_Length;
            TxBuff_Length = Comm_appl_FrameToBuffer(pUart);
            Comm_protocol_Frame_Send_Request(&pUart->TxBuffer, TxBuff_Length);
            Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Sending);
            break;
        }
        case FSM_State_Sending:   /* Executing frame sending on bus */
        {
            if(Comm_protocol_Get_TxFIFO_Length() == 0){
                Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Idle);
            }
            break;
        }
        case FSM_State_Error:
        {
            /* ToDo[PS] - error handler */
            Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Idle);
            break;
        } 
        default:
        {
            /* ToDo[PS] - default handler */
        }
    }    
    return 0;
}


/********************************************************************************************************************************************************************************************************************************************************
    Função: Frame Receive Machine (FRM)
    Descrição: Esta função é uma máquina de estados que controla a recepção de frames
*********************************************************************************************************************************************************************************************************************************************************/
byte Comm_appl_FRM( Uart_t *pUart )
{
    static int RxBuff_Timeout = 0;
    static int RxBuff_Length = 0;
    static int RxBuff_Length_Previous = 0;
    switch (pUart->FRM_State){
        case FRM_State_Idle:
        {
            if(Comm_protocol_Get_RxFIFO_Length() > 0){   /* Verifica a quantidade de bytes armazenados no Rx_Buffer. Se for > 0, então significa que há bytes chegando e é necessário alterar o estado da FRM */
                Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Receiving);
            }
            break;
        }
        case FRM_State_Receiving:
        {
            RxBuff_Length = Comm_protocol_Get_RxFIFO_Length();
            if(RxBuff_Length != RxBuff_Length_Previous){
                RxBuff_Length_Previous = RxBuff_Length;
                RxBuff_Timeout = 0;
            }else{
                RxBuff_Timeout++;
            }
            if(RxBuff_Timeout >= 3){   /* Timeout de 30ms */
                RxBuff_Timeout = 0;
                RxBuff_Length_Previous = 0;
                Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Received);
            }
            break;
        }
        case FRM_State_Received:
        {
            int i, nDataRead;
            nDataRead = Comm_protocol_Frame_Read_Request(&pUart->RxBuffer, RxBuff_Length);   /* Lê a quantidade #RxBuff_Length de bytes do Rx_BUFFER, e retorna a quantidade de bytes lidos  */
            Serial.printf("\nN# data bytes buffered: %d - N# data bytes read: %d\n", RxBuff_Length, nDataRead);
            for(i=0; i<nDataRead; i++)
                Serial.printf("%02X ", (byte)pUart->RxBuffer[i]);
            if(Comm_appl_Check_Frame_IsEcho(pUart) == FALSE){       /* Verifica se o frame recebido é eco: return{0: Não é eco | 1: É eco} */
                if(Comm_appl_Check_Frame_IsValid(pUart) == TRUE){   /* Verifica se o frame recebido é válido: return{0: Não é válido | 1: É válido} */
                    Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Idle);
                    Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_RxUart_Notify_Response);
                }else{
                    Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Error);
                }
            }else{
                Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Idle);
            }
            break;
        }
        case FRM_State_Error:
        {
            /* ToDo[PENS] error handler */
            Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Idle);
            break;
        } 
        default:
        {
            /* ToDo[PENS] - error handler to FSRM */
        }
    }    
    return 0;
}


/********************************************************************************************************************************************************************************************************************************************************
    Função: Response Handling Machine (RHM)
    Descrição: Esta função é uma máquina de estados que avalia o comando de entrada, recebido da Serial Rx, e manipula uma resposta
    \Parametros: struct MainData *pMainData - estrutura de dados principal
*********************************************************************************************************************************************************************************************************************************************************/
byte Comm_appl_RHM(Uart_t *pUart)
{
    switch (pUart->RHM_State){
        case RHM_State_Idle:
        {
            break;
        }
        case RHM_State_TxUart_Send_Request:
        {
            pUart->scheduleTable.pSlot = pUart->scheduleTable.pSlot->nextSlot;
            Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Send);
            Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_Idle);
            break;
        }
        case RHM_State_RxUart_Notify_Response:
        {
            Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_Process);
            break;
        }
        case RHM_State_Process:
        {
            if(Comm_appl_FindCommand(&pUart->RxBuffer[_SID], pUart) == KOSTIA_OK){
                Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_Idle);
            }     
            break;
        }
        default:
        {
            /* ToDo[PENS] - error handler to RHM */
        }
    }
    return 0;
}


/********************************************************************************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função realiza a mudança da variável de estado da FSM
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_FSM_State( Uart_t *pUart, FSM_States_t nextState)
{
    pUart->FSM_State = nextState;
}


/********************************************************************************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função realiza a mudança da variável de estado da FRM
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_FRM_State( Uart_t *pUart, FRM_States_t nextState )
{
    pUart->FRM_State = nextState;
}


/********************************************************************************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função realiza a mudança da variável de estado da RHM
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_RHM_State( Uart_t *pUart, RHM_States_t nextState )
{
    pUart->RHM_State = nextState;
}


/********************************************************************************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função configura os campos do Frame Header
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Set_Frame_Header(Frame_t *pFrame, byte Break, byte Synch, byte SID, byte Type, byte Id_Source, byte Id_Target, byte Lenght)
{
    pFrame->Break = Break;
    pFrame->Synch = Synch;
    pFrame->SID = SID;
    pFrame->Type = Type;
    pFrame->Id_Source = Id_Source;
    pFrame->Id_Target = Id_Target;
    pFrame->Lenght = Lenght;
}


/********************************************************************************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função configura o dados do Frame Data
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Set_Frame_Data(Frame_t *pFrame, byte *Data, int Size)
{
    for(int i=0; i<Size; i++){
        pFrame->Data[i] = *(Data + i);
    }
}


/********************************************************************************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função configura o valor do Frame Checksum
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Set_Frame_Checksum(Frame_t *pFrame)
{
    pFrame->Checksum = pFrame->Type | pFrame->SID;
}


/********************************************************************************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função configura o Buffer Tx que será passado para a camada Comm_protocol transmitir na UART
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_appl_FrameToBuffer(Uart_t *pUart)
{
    int i;
    pUart->TxBuffer[_BREAK]  = pUart->scheduleTable.pSlot->frame.Break;  //(char)pUart->scheduleTable->frame.Break;
    pUart->TxBuffer[_SYNCH]  = pUart->scheduleTable.pSlot->frame.Synch;
    pUart->TxBuffer[_SID]    = pUart->scheduleTable.pSlot->frame.SID;
    pUart->TxBuffer[_TYPE]   = pUart->scheduleTable.pSlot->frame.Type;
    pUart->TxBuffer[_ID_SRC] = pUart->scheduleTable.pSlot->frame.Id_Source;
    pUart->TxBuffer[_ID_TRG] = pUart->scheduleTable.pSlot->frame.Id_Target;
    pUart->TxBuffer[_LENGHT] = pUart->scheduleTable.pSlot->frame.Lenght;
    for(i = 0; i < pUart->scheduleTable.pSlot->frame.Lenght - 1; i++){
        pUart->TxBuffer[_DATA+i] = pUart->scheduleTable.pSlot->frame.Data[i];
    }
    pUart->TxBuffer[_DATA+i] = pUart->scheduleTable.pSlot->frame.Checksum;
    return (7 + pUart->scheduleTable.pSlot->frame.Lenght);
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Verifica se o frame recebido na serial Rx é eco do frame transmitido na serial Tx.
    
    \Parameters: *pMainData - Instancia da estrutura de dados principal Main Data.
    
    \Return value: 0 - significa que não é eco
    \Return value: 1 - significa que é eco
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_appl_Check_Frame_IsEcho(Uart_t *pUart)
{
    if(pUart->RxBuffer[_ID_SRC] == pUart->TxBuffer[_ID_SRC]){
        return 1;
    }
    return 0;
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Valida se o frame recebido na serial Rx é valido. Checa os campos: Break, Synch e Checksum
    
    \Parameters: *pMainData - Instancia da estrutura de dados principal Main Data.
    
    \Return value: 0 - significa que o frame não é valido
    \Return value: 1 - significa que o frame é valido
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_appl_Check_Frame_IsValid(Uart_t *pUart)
{
    if(pUart->RxBuffer[_BREAK] != 0x00 || pUart->RxBuffer[_SYNCH] != 0x55){
        return 0;
    }
    return 1;
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Cria a tabela de agandamento do ESP. A tabela de agendamento é implementada usando o conceito de Lista Ligada.
    
    \Parameters: (void)
    
    \Return value: (Slot_t *) - Retorno de um ponteiro para o primeiro elemento da Lista Ligada, ou primeiro slot da tabela de agendamento. 
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Create_Schedule_Table(ScheduleTable_t * pScheduleTable)
{
    byte Data[] = {}; //byte (*Data)[] = {0xA9, 0x1C, 0x47};
    byte ID_Master = 0x01;
    pScheduleTable->pSlot = (Slot_t *) malloc( sizeof(Slot_t *) );  /* Alocação dinamica de memória para armazenar uma "struct Slot" */
    pScheduleTable->pFirstSlot = pScheduleTable->pSlot;
    pScheduleTable->pLastSlot = pScheduleTable->pSlot;
    pScheduleTable->pSlot->nextSlot = pScheduleTable->pSlot;
    pScheduleTable->Length = 1;
    /* Configuração inicial do primeiro slot (slot para mensagens de configuração dos slaves) */
    Comm_appl_Set_Frame_Header(&pScheduleTable->pSlot->frame, 0x00, 0x55, 0x01, 0x01, 0x01, 0xFF, sizeof(Data) + 0x01);
    Comm_appl_Set_Frame_Data(&pScheduleTable->pSlot->frame, Data, sizeof(Data));
    Comm_appl_Set_Frame_Checksum(&pScheduleTable->pSlot->frame);
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Cria a tabela de agandamento do ESP. A tabela de agendamento é implementada usando o conceito de Lista Ligada.
    
    \Parameters: (void)
    
    \Return value: (Slot_t *) - Retorno de um ponteiro para o primeiro elemento da Lista Ligada, ou primeiro slot da tabela de agendamento. 
*********************************************************************************************************************************************************************************************************************************************************/
byte Comm_appl_Define_Slave_ID( ScheduleTable_t * pScheduleTable )
{
    byte i = 0x00, ID = 0x02;
    Slot_t *pAux = pScheduleTable->pFirstSlot;
    for(i = 0; i < pScheduleTable->Length; i++){
        if( ID == pAux->frame.Id_Target ){
            ID++;
            i = 0;
        }
        pAux = Comm_appl_Select_Next_Slot(pAux);
    }
    return ID;
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Insere um novo elemento na lista lgada, ou um novo slot na tabela de agendamento. 
    
    \Parameters: (Slot_t *pCurrentSlot) - Ponteiro para uma estrutura Slot_t
    
    \Return value: (void)
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Insert_Slot( ScheduleTable_t * pScheduleTable )
{
    Slot_t *pNewSlot;
    
    pNewSlot = (Slot_t *) malloc( sizeof(Slot_t *) );  //Alocação dinamica de memória para armazenar uma "struct Slot"
    pNewSlot->nextSlot = pScheduleTable->pFirstSlot;
    pScheduleTable->pSlot = pScheduleTable->pLastSlot;
    pScheduleTable->pSlot->nextSlot = pNewSlot;
    pScheduleTable->pLastSlot = pNewSlot;
    pScheduleTable->Length++;
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Transiciona o ponteiro da tabela de agendamento para o próximo slot da tabela de agendamento.
    
    \Parameters: (Slot_t *pCurrentSlot) - Ponteiro para uma estrutura Slot_t
    
    \Return value: (Slot_t *) - Retorno de um ponteiro para o próximo elemento da lista ligada, ou próximo slot da tabela de agendamento
*********************************************************************************************************************************************************************************************************************************************************/
Slot_t *Comm_appl_Select_Next_Slot(Slot_t *pCurrentSlot)
{
    return pCurrentSlot->nextSlot;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Performs the search of the received command in the tables of the test execution.
    
    \Parameters: *psInst Kostia Cmd instance.
    
    \Return value: KOSTIA_OK
    \Return value: KOSTIA_ER_TYPE_NOTFIND
    \Return value: KOSTIA_ER_CMD_NOTFIND
*********************************************************************************************************************************************************************************************************************************************************/
static Kostia_Rsp_t Comm_appl_FindCommand(byte *pAddr, Uart_t *pUart)
{
    byte lData[_CMD_CODE_FILTER_SIZE];
    byte u08CounterCmd = 0U;
    Kostia_Rsp_t eRsp = KOSTIA_ER_TYPE_NOTFIND;

    u08CounterCmd = 0;
    lData[0] = *(pAddr+0);
    lData[1] = *(pAddr+1);

    while (CmdTable_FromSlaveToMaster[u08CounterCmd].au08Command[0] != 0){
        if((lData[0] == CmdTable_FromSlaveToMaster[u08CounterCmd].au08Command[0]) && 
           (lData[1] == CmdTable_FromSlaveToMaster[u08CounterCmd].au08Command[1])){
               eRsp = CmdTable_FromSlaveToMaster[u08CounterCmd].pfExecute(pAddr, pUart); /* Chamada da função que manipula um frame de resposta para o comando recebido */
               if(eRsp == KOSTIA_OK){
                   //Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Send);
               }
               break;
        }else{
            /* Command not find */
            eRsp = KOSTIA_ER_CMD_NOTFIND;
        }
        u08CounterCmd++;
    }
    return eRsp;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Verifica se o campo mainData.frame.Id_Source =! 0  
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
*********************************************************************************************************************************************************************************************************************************************************/
static Kostia_Rsp_t Comm_appl_QueryID_Callback(byte *pCmd, Uart_t *pUart)
{
    Serial.println("Query ID");
    if(pUart->scheduleTable.pFirstSlot->frame.Id_Source == 0x01){      
        Serial.println("True");
        pUart->scheduleTable.pFirstSlot->frame.Break = 0x00;                                                              /* Break signal */
        pUart->scheduleTable.pFirstSlot->frame.Synch = 0x55;                                                              /* Synch signal */
        pUart->scheduleTable.pFirstSlot->frame.SID = 0x02;                                                                /* Identificador de serviço da mensagem */
        pUart->scheduleTable.pFirstSlot->frame.Type = 0x01;                                                               /* Tipo de módulo transmissor */
        pUart->scheduleTable.pFirstSlot->frame.Id_Source = 0x01;                                                          /* ID do módulo transmissor */
        pUart->scheduleTable.pFirstSlot->frame.Id_Target = Comm_appl_Define_Slave_ID( &pUart->scheduleTable );            /* ID do módulo alvo */
        pUart->scheduleTable.pFirstSlot->frame.Lenght = 0x01;                                                             /* Comprimento da mensagem */
        pUart->scheduleTable.pFirstSlot->frame.Checksum = 0x00;                                                           /* Checksum */
        /* Put pSlot in last slot */
        pUart->scheduleTable.pSlot = pUart->scheduleTable.pLastSlot;
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Function to read teh HW
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
*********************************************************************************************************************************************************************************************************************************************************/
static Kostia_Rsp_t Comm_appl_SetID_Callback(byte *pCmd, Uart_t *pUart)
{
    Serial.println("Set ID");
    if(pUart->scheduleTable.pFirstSlot->frame.Id_Source == 0x01){
        Serial.println("True");
        pUart->scheduleTable.pFirstSlot->frame.Break = 0x00;                /* Break signal */
        pUart->scheduleTable.pFirstSlot->frame.Synch = 0x55;                /* Synch signal */
        pUart->scheduleTable.pFirstSlot->frame.SID = 0x01;                  /* Identificador de serviço da mensagem */
        pUart->scheduleTable.pFirstSlot->frame.Type = 0x01;                 /* Tipo de módulo transmissor */
        pUart->scheduleTable.pFirstSlot->frame.Id_Source = 0x01;            /* ID do módulo transmissor */
        pUart->scheduleTable.pFirstSlot->frame.Id_Target = 0xFF;            /* ID do módulo alvo */
        pUart->scheduleTable.pFirstSlot->frame.Lenght = 0x01;               /* Comprimento da mensagem */
        pUart->scheduleTable.pFirstSlot->frame.Checksum = 0x00;             /* Checksum */
        /* Insert new slot in schedule table */
        Comm_appl_Insert_Slot(&pUart->scheduleTable);
        /* Configuração inicial do primeiro slot (slot para mensagens de configuração dos slaves) */
        Comm_appl_Set_Frame_Header(&pUart->scheduleTable.pLastSlot->frame, 0x00, 0x55, 0x03, 0x01, 0x01, pUart->RxBuffer[_ID_SRC], 0x01);
        Comm_appl_Set_Frame_Checksum(&pUart->scheduleTable.pLastSlot->frame);
        /* Put pSlot in last slot */
        pUart->scheduleTable.pSlot = pUart->scheduleTable.pLastSlot;
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Function to read teh HW
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
*********************************************************************************************************************************************************************************************************************************************************/
static Kostia_Rsp_t Comm_appl_RequestData_Callback(byte *pCmd, Uart_t *pUart)
{
    /* Ler entrada os pinos de entrada para pegar os valores do sensor e controlar a saída. Depende do módulo. */
    return KOSTIA_NOK;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Description: Function to report error in the command table
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
*********************************************************************************************************************************************************************************************************************************************************/
static Kostia_Rsp_t Comm_appl_CmdTableError(byte *pCmd, Uart_t *pUart)
{
    /* Trata a chegada de uma mensagem não registrada */
    return KOSTIA_NOK;
}

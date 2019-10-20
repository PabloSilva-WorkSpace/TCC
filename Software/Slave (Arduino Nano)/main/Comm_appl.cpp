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
/* Tabela de todos os comandos disponíveis do ESP para o Slave. Formatação da tabela: {{SID,TYPE,ID}, byte, callback function} */
static const Kostia_CmdTable_t CmdTable_FromMasterToSlave[] = {
    {{0x01U, 0x01U, 0x01U}, 0x01U, Comm_appl_QueryID},       /* Query if slave is configured */
    {{0x02U, 0x01U, 0x01U}, 0x01U, Comm_appl_SetID},         /* Set ID to slave */
    {{0x03U, 0x01U, 0x01U}, 0x01U, Comm_appl_RequestData},   /* Request slave's data */
//  {{0x03U, 0x01U, 0x01U}, 0x01U, Comm_appl_RequestData},   /* Config slave command */
    {{0x00U, 0x00U, 0x00U}, 0x00U, Comm_appl_CmdTableError}  /* Must be the last element */
};


/******************************************************************************************************************************************************************************************************************************************************** 
    Função: Frame Send Machine (FSM)
    Descrição: Esta função é uma máquina de estados que controla o envio de frames
    \Parametros: Uart_t *pUart - estrutura de dados principal
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
            if(Comm_protocol_Get_TxFIFO_Lenght() == 0){
                Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Idle);
            }
            break;
        }
        case FSM_State_Error:
        {
            /* ToDo[PENS] - error handler */
            Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Idle);
            break;
        } 
        default:
        {
            /* ToDo[PENS] - default handler */
        }
    }
    return 0;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função: Frame Receive Machine (FRM)
    Descrição: Esta função é uma máquina de estados que controla a recepção de frames
*********************************************************************************************************************************************************************************************************************************************************/
byte Comm_appl_FRM(Uart_t *pUart)
{
    static int RxBuff_Timeout = 0;
    static int RxBuff_Length = 0;
    static int RxBuff_Length_Previous = 0;
    switch (pUart->FRM_State){
        case FRM_State_Idle:
        {
            if(Comm_protocol_Get_RxFIFO_Lenght() > 0){   /* Verifica a quantidade de bytes armazenados no Rx_Buffer. Se for > 0, então significa que há bytes chegando e é necessário alterar o estado da FRM */
                Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Receiving);
            }
            break;
        }
        case FRM_State_Receiving:
        {
            RxBuff_Length = Comm_protocol_Get_RxFIFO_Lenght();
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
        case FRM_State_Received:  /* This state get datas received in Rx serial buffer and put it in a SW buffer (pUart->RxBuffer) */
        {
            int nDataRead;
            nDataRead = Comm_protocol_Frame_Read_Request(&pUart->RxBuffer, RxBuff_Length);
            if(Comm_appl_Check_Frame_IsEcho(pUart) == FALSE){
                if(Comm_appl_Check_Frame_IsValid(pUart) == TRUE){
                    Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Idle);
                    Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_Process);
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
    \Parametros: Uart_t *pUart - estrutura de dados principal
*********************************************************************************************************************************************************************************************************************************************************/
byte Comm_appl_RHM(Uart_t *pUart)
{
    switch (pUart->RHM_State){
        case RHM_State_Idle:
        {
            break;
        }
        case RHM_State_Process:
        {
            if(Comm_appl_FindCommand(&pUart->RxBuffer[_SID], pUart) == KOSTIA_OK){
                pUart->RHM_State = RHM_State_Idle;
                Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Send);
            }else{
                pUart->RHM_State = RHM_State_Idle;
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
void Comm_appl_Request_ChangeOf_FSM_State(Uart_t *pUart, FSM_States_t nextState)
{
    pUart->FSM_State = nextState;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função realiza a mudança da variável de estado da FRM
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_FRM_State(Uart_t *pUart, FRM_States_t nextState)
{
    pUart->FRM_State = nextState;
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função realiza a mudança da variável de estado da RHM
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_RHM_State(Uart_t *pUart, RHM_States_t nextState)
{
    if(pUart->RHM_State != RHM_State_Process){
        pUart->RHM_State = nextState;
    }
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    Descrição: Esta função configura os campos do Frame Header
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Init_Slave(Uart_t *pUart, byte Type , byte Data_size)
{
    int i;
    /* Inicializar o Frame */
    Comm_appl_Set_Frame_Header(&pUart->frame, 0x00, 0x55, 0x00, Type, 0x00, 0x01, Data_size + 1); /* Function signature:  Comm_appl_Set_Frame_Header(Frame_t *pFrame, byte Break, byte Synch, byte SID, byte Type, byte Id_Source, byte Id_Target, byte Lenght) */
    for(i = 0; i < _FRAME_MAX_DATA_SIZE; i++){
        pUart->frame.Data[i] = 0x00;
    }
    Comm_appl_Set_Frame_Checksum(&pUart->frame);
    /* Inicializar os buffers que armazenam os dados TX e RX */
    for(i = 0; i < _RX_BUFFER_SIZE; i++){
        pUart->RxBuffer[i] = 0x00;
    }
    for(i = 0; i < _TX_BUFFER_SIZE; i++){
        pUart->TxBuffer[i] = 0x00;
    }
    /* Inicializar as máquinas de estado da UART */
    pUart->FSM_State = FSM_State_Idle;
    pUart->FRM_State = FRM_State_Idle;
    pUart->RHM_State = RHM_State_Idle;
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
    pUart->TxBuffer[_BREAK]  = pUart->frame.Break;
    pUart->TxBuffer[_SYNCH]  = pUart->frame.Synch;
    pUart->TxBuffer[_SID]    = pUart->frame.SID;
    pUart->TxBuffer[_TYPE]   = pUart->frame.Type;
    pUart->TxBuffer[_ID_SRC] = pUart->frame.Id_Source;
    pUart->TxBuffer[_ID_TRG] = pUart->frame.Id_Target;
    pUart->TxBuffer[_LENGHT] = pUart->frame.Lenght;
    for(i = 0; i < pUart->frame.Lenght - 1; i++){
        pUart->TxBuffer[_DATA+i] = pUart->frame.Data[i];
    }
    pUart->TxBuffer[_DATA+i] = pUart->frame.Checksum;
    return (7 + pUart->frame.Lenght);
}


/******************************************************************************************************************************************************************************************************************************************************** 
    Função
    
    Descrição: Verifica se o frame recebido na serial Rx é eco do frame transmitido na serial Tx.
    
    \Parameters: *pUart - Instancia da estrutura de dados principal Main Data.
    
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
    Função
    
    Descrição: Valida se o frame recebido na serial Rx é valido. Checa os campos: Break, Synch e Checksum
    
    \Parameters: *pUart - Instancia da estrutura de dados principal Main Data.
    
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
    lData[2] = *(pAddr+2);
    
    while (CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[0] != 0){
        if((lData[0] == CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[0]) && (lData[1] == CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[1]) && (lData[2] == CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[2])){
            eRsp = CmdTable_FromMasterToSlave[u08CounterCmd].pfExecute(pAddr, pUart); /* Chamada da função que manipula um frame de resposta para o comando recebido */
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
static Kostia_Rsp_t Comm_appl_QueryID(byte *pCmd, Uart_t *pUart)
{
    if(pUart->frame.Id_Source == 0x00){      
        pUart->frame.SID = *pCmd;                 /* Identificador de serviço da mensagem */
        pUart->frame.Checksum = 0x00;             /* Checksum */
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
static Kostia_Rsp_t Comm_appl_SetID(byte *pCmd, Uart_t *pUart)
{
    if(pUart->frame.Id_Source == 0x00){
        pUart->frame.SID = *pCmd;                                         /* Identificador de serviço da mensagem */
        pUart->frame.Id_Source = pUart->RxBuffer[_ID_TRG];                /* ID do módulo transmissor */
        pUart->frame.Checksum = 0x00;                                     /* Checksum */
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
static Kostia_Rsp_t Comm_appl_RequestData(byte *pCmd, Uart_t *pUart)
{
    if(pUart->RxBuffer[_ID_TRG] == pUart->frame.Id_Source){
        pUart->frame.SID = *pCmd;                                         /* Identificador de serviço da mensagem */
        pUart->frame.Checksum = 0x00;                                     /* Checksum */
        /* Ler os pinos de entrada para pegar os valores do sensor e controlar a saída. Depende do módulo. */
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
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

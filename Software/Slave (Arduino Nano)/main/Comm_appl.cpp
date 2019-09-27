/* Fish Tank automation project - TCC -  Comm_appl file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
*/


/* Headers includes */ 
#include "Comm_appl.h"


/* Essa constante é uma tabela de todos os comandos disponíveis do ESP para o Slave */
/* modelo {{SID,TYPE,ID}, byte, callback function} */
static const Kostia_TCmdTable CmdTable_FromMasterToSlave[] = {
    {{0x01U, 0x01U, 0x01U}, 0x01U, Comm_appl_QueryID},       /* Query if slave is configured */
    {{0x02U, 0x01U, 0x01U}, 0x01U, Comm_appl_SetID},         /* Set ID to slave */
    {{0x03U, 0x01U, 0x01U}, 0x01U, Comm_appl_RequestData},   /* Request slave's data */
    {{0x00U, 0x00U, 0x00U}, 0x00U, Comm_appl_CmdTableError}  /* Must be the last element */
};


/***********************************************************************************************************************************************************************************************
    Função: Frame Send Machine (FSM)
    Descrição: Esta função é uma máquina de estados que controla o envio de frames
    \Parametros: struct MainData *pMainData - estrutura de dados principal
***********************************************************************************************************************************************************************************************/
byte Comm_appl_FSM(struct MainData *pMainData)
{
    switch (pMainData->FSM_State){
        case FSM_State_Idle:
        {
            break;
        }
        case FSM_State_Send:
        {
            int lenght;
            lenght = Comm_appl_FrameToBuffer(pMainData);
            Comm_protocol_Frame_Send_Request(pMainData->TxBuffer, lenght);
            Comm_appl_Request_ChangeOf_FSM_State(pMainData, FSM_State_Sending);      
            break;
        }
        case FSM_State_Sending: /* Uma possibilidade para sair deste estado, talvez a ideal, é usar interrupção: Quando todos data bytes no TX FIFO forem transmitidos. */ 
        {
            if(Comm_protocol_Get_TXFIFO_Lenght() == 0){
                Comm_appl_Request_ChangeOf_FSM_State(pMainData, FSM_State_Error);
            }
            break;
        }
        case FSM_State_Error:
        {
            /* ToDo[PENS] error handler */
            Comm_appl_Request_ChangeOf_FSM_State(pMainData, FSM_State_Idle);
            break;
        } 
        default:
        {
            /* ToDo[PENS] - error handler to FSRM */
        }
    }
    return 0;
}


/***********************************************************************************************************************************************************************************************
    Função: Frame Receive Machine (FRM)
    Descrição: Esta função é uma máquina de estados que controla a recepção de frames
***********************************************************************************************************************************************************************************************/
byte Comm_appl_FRM(struct MainData *pMainData)
{
    static int RxBuff_Timeout = 0;
    static int RxBuff_Length = 0;
    static int RxBuff_Length_Previous = 0;
    switch (pMainData->FRM_State){
        case FRM_State_Idle:
        {
            if(Comm_protocol_Get_RXFIFO_Lenght() > 0){
                Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Receiving);
            }
            break;
        }
        case FRM_State_Receiving: /* O ideal é ativar este estado usando interrupção: Quando chegar um data byte e a FRM estiver em FRM_State_Idle */
        {
            RxBuff_Length = Comm_protocol_Get_RXFIFO_Lenght();
            if(RxBuff_Length != RxBuff_Length_Previous){
                RxBuff_Length_Previous = RxBuff_Length;
                RxBuff_Timeout = 0;
            }else{
                RxBuff_Timeout++;
            }
            if(RxBuff_Timeout >= 3){ /* Timeout de 30ms */
                RxBuff_Timeout = 0;
                RxBuff_Length_Previous = 0;
                Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Received);
            }
            break;                                                                                                                                              
        }
        case FRM_State_Received:  /* This state get datas received in Rx serial buffer and put it in a SW buffer (pMainData->RxBuffer) */
        {
            int i;
            i = Serial.readBytes(pMainData->RxBuffer, RxBuff_Length);
            if(Comm_appl_Echo_Frame(pMainData) == 0){
                if(Comm_appl_Validate_Frame(pMainData) == 1){
                    Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Idle);
                    Comm_appl_Request_ChangeOf_RHM_State(pMainData, RHM_State_Process);
                }else{
                    Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Error);
                }
            }else{
                Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Idle);
            }
            break;
        }
        case FRM_State_Error:
        {
            /* ToDo[PENS] error handler */
            Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Idle);
            break;
        } 
        default:
        {
            /* ToDo[PENS] - error handler to FSRM */
        }
    }    
    return 0;
}


/***********************************************************************************************************************************************************************************************
    Função: Response Handling Machine (RHM)
    Descrição: Esta função é uma máquina de estados que avalia o comando de entrada, recebido da Serial Rx, e manipula uma resposta
    \Parametros: struct MainData *pMainData - estrutura de dados principal
***********************************************************************************************************************************************************************************************/
byte Comm_appl_RHM(struct MainData *pMainData)
{
    switch (pMainData->RHM_State){
        case RHM_State_Idle:
        {
            break;
        }
        case RHM_State_Process:
        {
            if(Comm_appl_FindCommand(&pMainData->RxBuffer[_SID], pMainData) == KOSTIA_OK){
                Comm_appl_Request_ChangeOf_RHM_State(pMainData, RHM_State_Idle);
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


/**********************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função realiza a mudança da variável de estado da FSM
***********************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_FSM_State(struct MainData *pMainData, enum FSM_States nextState)
{
    pMainData->FSM_State = nextState;
}


/***********************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função realiza a mudança da variável de estado da FRM
***********************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_FRM_State(struct MainData *pMainData, enum FRM_States nextState)
{
    pMainData->FRM_State = nextState;
}


/***********************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função realiza a mudança da variável de estado da RHM
***********************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_RHM_State(struct MainData *pMainData, enum RHM_States nextState)
{
    pMainData->RHM_State = nextState;
}


void Comm_appl_Set_Frame_Header(struct Frame *pFrame, byte Break, byte Synch, byte SID, byte Type, byte Id_Source, byte Id_Target, byte Lenght)
{
    pFrame->Break = Break;
    pFrame->Synch = Synch;
    pFrame->SID = SID;
    pFrame->Type = Type;
    pFrame->Id_Source = Id_Source;
    pFrame->Id_Target = Id_Target;
    pFrame->Lenght = Lenght;
}


void Comm_appl_Set_Frame_Data(struct Frame *pFrame, byte *Data, int Size)
{
    for(int i=0; i<Size; i++){
        pFrame->Data[i] = *(Data + i);
    }
}


void Comm_appl_Set_Frame_Checksum(struct Frame *pFrame)
{
    pFrame->Checksum = pFrame->Type | pFrame->SID;
}


int Comm_appl_FrameToBuffer(struct MainData *pMainData)
{
    int i;
    pMainData->TxBuffer[_BREAK]  = pMainData->frame.Break;
    pMainData->TxBuffer[_SYNCH]  = pMainData->frame.Synch;
    pMainData->TxBuffer[_SID]    = pMainData->frame.SID;
    pMainData->TxBuffer[_TYPE]   = pMainData->frame.Type;
    pMainData->TxBuffer[_ID_SRC] = pMainData->frame.Id_Source;
    pMainData->TxBuffer[_ID_TRG] = pMainData->frame.Id_Target;
    pMainData->TxBuffer[_LENGHT] = pMainData->frame.Lenght;
    for(i = 0; i < pMainData->frame.Lenght - 1; i++){
        pMainData->TxBuffer[_DATA+i] = pMainData->frame.Data[i];
    }
    pMainData->TxBuffer[_DATA+i] = pMainData->frame.Checksum;
    return (7 + pMainData->frame.Lenght);
}


/******************************************************************************************************
    Função
    
    Descrição: Verifica se o frame recebido na serial Rx é eco do frame transmitido na serial Tx.
    
    \Parameters: *pMainData - Instancia da estrutura de dados principal Main Data.
    
    \Return value: 0 - significa que não é eco
    \Return value: 1 - significa que é eco
******************************************************************************************************/
int Comm_appl_Echo_Frame(struct MainData *pMainData)
{
    if(pMainData->RxBuffer[_ID_SRC] == pMainData->TxBuffer[_ID_SRC]){
        return 1;
    }
    return 0;
}


/******************************************************************************************************
    Função
    
    Descrição: Valida se o frame recebido na serial Rx é valido. Checa os campos: Break, Synch e Checksum
    
    \Parameters: *pMainData - Instancia da estrutura de dados principal Main Data.
    
    \Return value: 0 - significa que o frame não é valido
    \Return value: 1 - significa que o frame é valido
******************************************************************************************************/
int Comm_appl_Validate_Frame(struct MainData *pMainData)
{
    if(pMainData->RxBuffer[_BREAK] != 0x00 || pMainData->RxBuffer[_SYNCH] != 0x55){
        return 0;
    }
    return 1;
}


/******************************************************************************************************
    Função
    
    Description: Performs the search of the received command in the tables of the test execution.
    
    \Parameters: *psInst Kostia Cmd instance.
    
    \Return value: KOSTIA_OK
    \Return value: KOSTIA_ER_TYPE_NOTFIND
    \Return value: KOSTIA_ER_CMD_NOTFIND
******************************************************************************************************/
static Kostia_TRsp Comm_appl_FindCommand(byte *pAddr, struct MainData *pMainData)
{
    byte lData[_CMD_CODE_FILTER_SIZE];
    byte u08CounterCmd = 0U;
    Kostia_TRsp eRsp = KOSTIA_ER_TYPE_NOTFIND;

    u08CounterCmd = 0;
    lData[0] = *(pAddr+0);
    lData[1] = *(pAddr+1);
    lData[2] = *(pAddr+2);

    while (CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[0] != 0){
        if((lData[0] == CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[0]) && 
           (lData[1] == CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[1]) && 
           (lData[2] == CmdTable_FromMasterToSlave[u08CounterCmd].au08Command[2])){
               eRsp = CmdTable_FromMasterToSlave[u08CounterCmd].pfExecute(pAddr, pMainData); /* Chamada da função que manipula um frame de resposta para o comando recebido */
               if(eRsp == KOSTIA_OK){
                   Comm_appl_Request_ChangeOf_FSM_State(pMainData, FSM_State_Send);
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


/******************************************************************************************************
    Função
    
    Description: Verifica se o campo mainData.frame.Id_Source =! 0  
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
******************************************************************************************************/
static Kostia_TRsp Comm_appl_QueryID(byte *pCmd, struct MainData *pMainData)
{
    if(pMainData->frame.Id_Source == 0x00){      
        pMainData->frame.Break = 0x00;                /* Break signal */
        pMainData->frame.Synch = 0x55;                /* Synch signal */
        pMainData->frame.SID = 0x01;                  /* Identificador de serviço da mensagem */
        pMainData->frame.Type = 0x02;                 /* Tipo de módulo transmissor */
        pMainData->frame.Id_Source = 0x00;            /* ID do módulo transmissor */
        pMainData->frame.Id_Target = 0x01;            /* ID do módulo alvo */
        pMainData->frame.Lenght = 0x01;               /* Comprimento da mensagem */
        pMainData->frame.Checksum = 0x00;             /* Checksum */
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
}


/******************************************************************************************************
    Função
    
    Description: Function to read teh HW
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
******************************************************************************************************/
static Kostia_TRsp Comm_appl_SetID(byte *pCmd, struct MainData *pMainData)
{
    if(pMainData->frame.Id_Source == 0x00){
        pMainData->frame.Break = 0x00;                                        /* Break signal */
        pMainData->frame.Synch = 0x55;                                        /* Synch signal */
        pMainData->frame.SID = *pCmd;                                         /* Identificador de serviço da mensagem */
        pMainData->frame.Type = 0x02;                                         /* Tipo de módulo transmissor */
        pMainData->frame.Id_Source = pMainData->RxBuffer[_ID_TRG];            /* ID do módulo transmissor */
        pMainData->frame.Id_Target = 0x01;                                    /* ID do módulo alvo */
        pMainData->frame.Lenght = 0x01;                                       /* Comprimento da mensagem */
        pMainData->frame.Checksum = 0x00;                                     /* Checksum */
        return KOSTIA_OK;
    }else{
        return KOSTIA_NOK;
    }
}


/******************************************************************************************************
    Função
    
    Description: Function to read teh HW
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
******************************************************************************************************/
static Kostia_TRsp Comm_appl_RequestData(byte *pCmd, struct MainData *pMainData)
{
    /* Ler entrada os pinos de entrada para pegar os valores do sensor e controlar a saída. Depende do módulo. */
    return KOSTIA_NOK;
}


/******************************************************************************************************
    Função
    
    Description: Function to report error in the command table
    
    \Parameters: u08 *pCmd - command received from Kostia Com
    
    \Return value: Kostia_TRsp
******************************************************************************************************/
static Kostia_TRsp Comm_appl_CmdTableError(byte *pCmd, struct MainData *pMainData)
{
    /* Trata a chegada de uma mensagem não registrada */
    return KOSTIA_NOK;
}

/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  Comm_appl file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "General_types.h"
#include "Comm_appl.h"


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Global Variables into this scope (this file *.c)
*********************************************************************************************************************************************************************************************************************************************************/
/* Tabela de todas as mensagens de resposta disponíveis do Slaves para o ESP. Formatação da tabela: {{SID,TYPE}, byte, callback function} */
static const Kostia_CmdTable_t CmdTable_FromSlaveToMaster[] = {
    {{0x01U, 0x00U}, 0x01U, Comm_appl_QueryID_Callback},         /* Response to command: Query if slave is configured */
    {{0x02U, 0x00U}, 0x01U, Comm_appl_SetID_Callback},           /* Response to command: Set ID to slave */
    {{0x03U, 0x00U}, 0x01U, Comm_appl_ConfigSlave_Callback},     /* Response to command: Config slave */
    {{0x04U, 0x00U}, 0x01U, Comm_appl_RequestData_Callback},     /* Response to command: Request slave's data */
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
            Comm_appl_Prepara_QueryID();
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
                Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_RxUart_Notify_Echo);
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
    static int nAttempt = 0;
    switch (pUart->RHM_State){
        case RHM_State_Idle:
        {
            break;
        }
        case RHM_State_TxUart_Send_Request:
        {
            xEventGroupWaitBits( gWiFi_appl_event_group, UART_TX_ENABLE, false, true, portMAX_DELAY );                                                              /* Aguarda a liberação da comunicação UART */

            /* Lógica para deletar um slot se não for respondido 3x, se não for o primeiro slot. Se for o primeiro slot passa para o próximo slot */
            if( (pUart->scheduleTable.pSlot == pUart->scheduleTable.pLastSlotSent) && (pUart->scheduleTable.pSlot != pUart->scheduleTable.pFirstSlot) ){
                nAttempt++;
                if(nAttempt >= 3){
                    Comm_appl_Delete_Slot( &pUart->scheduleTable );
                    nAttempt = 0;
                }
            }
            else if( (pUart->scheduleTable.pSlot == pUart->scheduleTable.pLastSlotSent) && (pUart->scheduleTable.pSlot == pUart->scheduleTable.pFirstSlot) ){
                pUart->scheduleTable.pSlot = pUart->scheduleTable.pSlot->nextSlot;
            }
                        
            Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Send);
            Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_Idle);
            
            pUart->scheduleTable.pLastSlotSent = pUart->scheduleTable.pSlot;

            break;
        }
        case RHM_State_RxUart_Notify_Response:
        {
            pUart->scheduleTable.pSlot = pUart->scheduleTable.pSlot->nextSlot;
            Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_Process);
            break;
        }
        case RHM_State_RxUart_Notify_Echo:
        {
            /* Se o eco for referente ao serviço Config Slave (0x03), resetamos o conteúdo do primeiro slot para serviço Query ID (0x01) - Isto é uma garantia, para o caso do slave não responder (ex.: não estiver no barramento) */
            if( pUart->RxBuffer[_SID] == 0x03){
                pUart->scheduleTable.pFirstSlot->frame.SID = 0x01;                  /* Identificador de serviço da mensagem: {0x01: Query ID} */
                pUart->scheduleTable.pFirstSlot->frame.Type = 0x01;                 /* Tipo de módulo transmissor: {0x01 = Master} */
                pUart->scheduleTable.pFirstSlot->frame.Id_Source = 0x01;            /* ID do módulo transmissor: {0x01 = Master} */
                pUart->scheduleTable.pFirstSlot->frame.Id_Target = 0xFF;            /* ID do módulo alvo: {0xFF = Broadcast} */
                pUart->scheduleTable.pFirstSlot->frame.Lenght = 13;               /* Comprimento da mensagem: {(X: Dados) + (1: Checksum)} */
                pUart->scheduleTable.pFirstSlot->frame.Checksum = 0x00;             /* Checksum */
            }
            Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_Idle);
            break;
        }
        case RHM_State_Process:
        {
            if(Comm_appl_FindCommand(&pUart->RxBuffer[_SID], pUart) == KOSTIA_OK){
                pUart->RHM_State = RHM_State_Idle;
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
    if(pUart->RHM_State != RHM_State_Process){
        pUart->RHM_State = nextState;
    }
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
    Descrição: Prepara os dados do primeiro slot para transmitirem os valores dos sensores e a hora, caso o primeiro slot esteja configurado como serviço QUERY_ID
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Prepara_QueryID( void )
{
    if (mainData.uart.scheduleTable.slot[0].frame.SID == 0x01) {
        /* Atualizar os campos TIPO e VALOR referente aos sensores no frame do primeiro slot (slot de configuração) */
        mainData.uart.scheduleTable.slot[0].frame.Data[_S1_TYPE]  = mainData.control.module.sensor_1.type;
        mainData.uart.scheduleTable.slot[0].frame.Data[_S1_VALUE] = mainData.control.module.sensor_1.value;
        mainData.uart.scheduleTable.slot[0].frame.Data[_S2_TYPE]  = mainData.control.module.sensor_2.type;
        mainData.uart.scheduleTable.slot[0].frame.Data[_S2_VALUE] = mainData.control.module.sensor_2.value;
        mainData.uart.scheduleTable.slot[0].frame.Data[_S3_TYPE]  = mainData.control.module.sensor_3.type;
        mainData.uart.scheduleTable.slot[0].frame.Data[_S3_VALUE] = mainData.control.module.sensor_3.value;
        mainData.uart.scheduleTable.slot[0].frame.Data[_S4_TYPE]  = mainData.control.module.sensor_4.type;
        mainData.uart.scheduleTable.slot[0].frame.Data[_S4_VALUE] = mainData.control.module.sensor_4.value;
        mainData.uart.scheduleTable.slot[0].frame.Data[_S5_TYPE]  = mainData.control.module.sensor_5.type;
        mainData.uart.scheduleTable.slot[0].frame.Data[_S5_VALUE] = mainData.control.module.sensor_5.value;
        mainData.uart.scheduleTable.slot[0].frame.Data[_HOUR_MSB] = mainData.control.module.hour_msb;
        mainData.uart.scheduleTable.slot[0].frame.Data[_HOUR_LSB] = mainData.control.module.hour_lsb;
    }
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
    byte Data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //byte (*Data)[] = {0xA9, 0x1C, 0x47};
    byte ID_Master = 0x01;
    int i, j;
    /* Inicialização do array de slots usados na schedule table */
    for(i = 0; i < _SCHEDULE_TABLE_MAX_SIZE; i++ ){
        Comm_appl_Reset_Slot(&pScheduleTable->slot[i]);
    }
    /* Organização dos ponteiros que controlam a schedule table () */
    pScheduleTable->pSlot = &pScheduleTable->slot[0];  
    pScheduleTable->pFirstSlot = pScheduleTable->pSlot;
    pScheduleTable->pLastSlot = pScheduleTable->pSlot;
    pScheduleTable->pLastSlotSent = pScheduleTable->pFirstSlot;    /* Teste_1: deletar slot que não responder 3x */
    pScheduleTable->pSlot->nextSlot = pScheduleTable->pSlot;
    pScheduleTable->Length = 1;
    /* Configuração inicial do primeiro slot (slot para mensagens de configuração dos slaves) */
    Comm_appl_Set_Frame_Header(&pScheduleTable->pSlot->frame, 0x00, 0x55, 0x01, 0x01, 0x01, 0xFF, 13); //12 Data + 1 Checksum
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
    int i;
    Slot_t *pNewSlot;
    for(i = 0; i < _SCHEDULE_TABLE_MAX_SIZE; i++ ){
        if(pScheduleTable->slot[i].frame.Synch == 0x00){
            pNewSlot = &pScheduleTable->slot[i];
            pNewSlot->nextSlot = pScheduleTable->pFirstSlot;
            pScheduleTable->pSlot = pScheduleTable->pLastSlot;
            pScheduleTable->pSlot->nextSlot = pNewSlot;
            pScheduleTable->pLastSlot = pNewSlot;
            pScheduleTable->Length++;
            break;
        }
    }
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Insere um novo elemento na lista lgada, ou um novo slot na tabela de agendamento. 
    
    \Parameters: (Slot_t *pCurrentSlot) - Ponteiro para uma estrutura Slot_t
    
    \Return value: (void)
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Delete_Slot( ScheduleTable_t * pScheduleTable )
{
    int i;
    Slot_t * pSlotAux;   /* Duvida:: Quando declarei "static Slot_t * pSlotAux;" deu erro na execução. O ESP32 fazia reboot constantemente. Por que? */
    pSlotAux = pScheduleTable->pSlot;
    for(i = 0; i < pScheduleTable->Length - 1; i++){
        pScheduleTable->pSlot = pScheduleTable->pSlot->nextSlot;
    }
    if(pScheduleTable->pLastSlot == pScheduleTable->pSlot->nextSlot){
        pScheduleTable->pLastSlot = pScheduleTable->pSlot;
    }
    pScheduleTable->pSlot->nextSlot = pSlotAux->nextSlot;
    Comm_appl_Reset_Slot(pSlotAux);
    pScheduleTable->Length--;
    /* Posicionar o pSlot no primeiro slot depois de deletar algum slot */
    pScheduleTable->pSlot = pScheduleTable->pFirstSlot;
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Limpa todos os campos do slot passado como parâmetro.
    
    \Parameters: (void)
    
    \Return value: (Slot_t *) - Retorno de um ponteiro para o primeiro elemento da Lista Ligada, ou primeiro slot da tabela de agendamento. 
*********************************************************************************************************************************************************************************************************************************************************/
void Comm_appl_Reset_Slot(Slot_t *pSlot)
{
    int i;
    Comm_appl_Set_Frame_Header(&pSlot->frame, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    for(i = 0; i < _FRAME_MAX_DATA_SIZE; i++){
        pSlot->frame.Data[i] = 0x00;
    }
    pSlot->frame.Break = 0x00;
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

    while (CmdTable_FromSlaveToMaster[u08CounterCmd].au08Command[0] != 0){
        if(lData[0] == CmdTable_FromSlaveToMaster[u08CounterCmd].au08Command[0]){
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
    Serial.println("Query ID callback");
    if(pUart->scheduleTable.Length < _SCHEDULE_TABLE_MAX_SIZE){
        Serial.println("true");
        pUart->scheduleTable.pFirstSlot->frame.Break = 0x00;                                                              /* Break signal */
        pUart->scheduleTable.pFirstSlot->frame.Synch = 0x55;                                                              /* Synch signal */
        pUart->scheduleTable.pFirstSlot->frame.SID = 0x02;                                                                /* Identificador de serviço da mensagem */
        pUart->scheduleTable.pFirstSlot->frame.Type = 0x01;                                                               /* Tipo de módulo transmissor */
        pUart->scheduleTable.pFirstSlot->frame.Id_Source = 0x01;                                                          /* ID do módulo transmissor */
        pUart->scheduleTable.pFirstSlot->frame.Id_Target = Comm_appl_Define_Slave_ID( &pUart->scheduleTable );            /* ID do módulo alvo */
        pUart->scheduleTable.pFirstSlot->frame.Lenght = 0x01;                                                             /* Comprimento da mensagem */
        pUart->scheduleTable.pFirstSlot->frame.Checksum = 0x00;                                                           /* Checksum */
        /* Put pSlot in last slot */
        pUart->scheduleTable.pSlot = pUart->scheduleTable.pFirstSlot;   /* Teste_1: deletar slot que não responder 3x */
        return KOSTIA_OK;
    }else if(pUart->scheduleTable.Length >= _SCHEDULE_TABLE_MAX_SIZE){
        Serial.println("Overflow on schedule table ");
        return KOSTIA_OK;
    }else{
        Serial.println("else");
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
    Serial.println("Set ID callback");
    if(pUart->scheduleTable.pFirstSlot->frame.Id_Source == 0x01){
        pUart->scheduleTable.pFirstSlot->frame.Break = 0x00;                /* Break signal */
        pUart->scheduleTable.pFirstSlot->frame.Synch = 0x55;                /* Synch signal */
        pUart->scheduleTable.pFirstSlot->frame.SID = 0x01;                  /* Identificador de serviço da mensagem */
        pUart->scheduleTable.pFirstSlot->frame.Type = 0x01;                 /* Tipo de módulo transmissor */
        pUart->scheduleTable.pFirstSlot->frame.Id_Source = 0x01;            /* ID do módulo transmissor */
        pUart->scheduleTable.pFirstSlot->frame.Id_Target = 0xFF;            /* ID do módulo alvo */
        pUart->scheduleTable.pFirstSlot->frame.Lenght = 13;               /* Comprimento da mensagem */
        pUart->scheduleTable.pFirstSlot->frame.Checksum = 0x00;             /* Checksum */
        /* Insert new slot in schedule table */
        Comm_appl_Insert_Slot(&pUart->scheduleTable);
        /* Configuração inicial do primeiro slot (slot para mensagens de configuração dos slaves) */
        Comm_appl_Set_Frame_Header(&pUart->scheduleTable.pLastSlot->frame, 0x00, 0x55, 0x04, 0x01, 0x01, pUart->RxBuffer[_ID_SRC], 0x01);
        Comm_appl_Set_Frame_Checksum(&pUart->scheduleTable.pLastSlot->frame);
        /* Put pSlot in last slot */
        //pUart->scheduleTable.pSlot = pUart->scheduleTable.pLastSlot;  /* Teste_1: deletar slot que não responder 3x */
        pUart->scheduleTable.pSlot = pUart->scheduleTable.pFirstSlot;   /* Teste_1: deletar slot que não responder 3x */
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
static Kostia_Rsp_t Comm_appl_ConfigSlave_Callback(byte *pCmd, Uart_t *pUart)
{
    /* Chamar as funções que irão manipular essas respostas. Chamar as funções baseadas nos tipo de slave que respondeu */
    Serial.println("Config module callback");
    if(pUart->scheduleTable.pFirstSlot->frame.Id_Source == 0x01){
        pUart->scheduleTable.pFirstSlot->frame.SID = 0x01;                  /* Identificador de serviço da mensagem: {0x01: Query ID} */
        pUart->scheduleTable.pFirstSlot->frame.Type = 0x01;                 /* Tipo de módulo transmissor: {0x01 = Master} */
        pUart->scheduleTable.pFirstSlot->frame.Id_Source = 0x01;            /* ID do módulo transmissor: {0x01 = Master} */
        pUart->scheduleTable.pFirstSlot->frame.Id_Target = 0xFF;            /* ID do módulo alvo: {0xFF = Broadcast} */
        pUart->scheduleTable.pFirstSlot->frame.Lenght = 13;               /* Comprimento da mensagem: {(X: Dados) + (1: Checksum)} */
        pUart->scheduleTable.pFirstSlot->frame.Checksum = 0x00;             /* Checksum */
        /* Put pSlot in last slot */
        pUart->scheduleTable.pSlot = pUart->scheduleTable.pFirstSlot;       /* Retorna o ponteiro da scheduletable para o primeiro slot (Slot de configuração) */
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
    /* Trata a resposta de um módulo PLUG*/
    if( pUart->RxBuffer[_TYPE] == _TYPE_MODULE_PLUG ){        
        /* PLUG 1 */
        mainData.module_plug.plug_1.id      = pUart->RxBuffer[_DATA_D0];
        mainData.module_plug.plug_1.mode    = pUart->RxBuffer[_DATA_D1];
        mainData.module_plug.plug_1.on_off  = pUart->RxBuffer[_DATA_D2];
        mainData.module_plug.plug_1.potency = pUart->RxBuffer[_DATA_D3];
        /* PLUG 2 */
        mainData.module_plug.plug_2.id      = pUart->RxBuffer[_DATA_D4];
        mainData.module_plug.plug_2.mode    = pUart->RxBuffer[_DATA_D5];
        mainData.module_plug.plug_2.on_off  = pUart->RxBuffer[_DATA_D6];
        mainData.module_plug.plug_2.potency = pUart->RxBuffer[_DATA_D7];
        /* PLUG 3 */
        mainData.module_plug.plug_3.id      = pUart->RxBuffer[_DATA_D8];
        mainData.module_plug.plug_3.mode    = pUart->RxBuffer[_DATA_D9];
        mainData.module_plug.plug_3.on_off  = pUart->RxBuffer[_DATA_D10];
        mainData.module_plug.plug_3.potency = pUart->RxBuffer[_DATA_D11];
        /* PLUG 4 */
        mainData.module_plug.plug_4.id      = pUart->RxBuffer[_DATA_D12];
        mainData.module_plug.plug_4.mode    = pUart->RxBuffer[_DATA_D13];
        mainData.module_plug.plug_4.on_off  = pUart->RxBuffer[_DATA_D14];
        mainData.module_plug.plug_4.potency = pUart->RxBuffer[_DATA_D15];
        return KOSTIA_OK;
    }
    /* Trata a resposta de um módulo LIGHT */
    else if( pUart->RxBuffer[_TYPE] == _TYPE_MODULE_LIGHT ){   
        
        return KOSTIA_OK;
    }
    else{
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

/*********************************************************************************************************** 
 * Fish Tank automation project - TCC -  main file
 * Developer: 
 * 
 * ToDo[PENS] - need to improve the comments
***********************************************************************************************************/


/* Headers includes */ 
#include "Comm_appl.h"

/***********************************************************************************************************************************************************************************************
    Função: Frame Send Machine (FSM)
    Descrição: Esta função é uma máquina de estados que controla o envio de frames
    \Parametros: struct MainData *pMainData - estrutura de dados principal
***********************************************************************************************************************************************************************************************/
byte Comm_appl_FSM( Uart_t *pUart )
{
    switch (pUart->FSM_State){
        case FSM_State_Idle:
        {
            break;
        }
        case FSM_State_Send:
        {
            int TxBuff_Length;
            TxBuff_Length = Comm_appl_FrameToBuffer(pUart);
            Comm_protocol_Frame_Send_Request(&pUart->TxBuffer, TxBuff_Length);
            Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Sending);
            break;
        }
        case FSM_State_Sending: /* O ideal é sair deste estado usando interrupção: Quando todos data bytes no TX FIFO forem transmitidos */
        {
            if(Comm_protocol_Get_TxFIFO_Length() == 0){
                Comm_appl_Request_ChangeOf_FSM_State(pUart, FSM_State_Error);
                //Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Receiving);
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


/***********************************************************************************************************************************************************************************************
    Função: Frame Receive Machine (FRM)
    Descrição: Esta função é uma máquina de estados que controla a recepção de frames
***********************************************************************************************************************************************************************************************/
byte Comm_appl_FRM( Uart_t *pUart )
{
    static int RxBuff_Timeout = 0;
    static int RxBuff_Length = 0;
    static int RxBuff_Length_Previous = 0;
    switch (pUart->FRM_State){
        case FRM_State_Idle:
        {
            if(Comm_protocol_Get_RxBUFFER_Length() > 0){   /* Se a quantidade de bytes armazenados no Rx_Buffer for maior que 0, então significa que há bytes chegando e é necessário alterar o estado da FRM */
                Comm_appl_Request_ChangeOf_FRM_State(pUart, FRM_State_Receiving);
            }
            break;
        }
        case FRM_State_Receiving: /* O ideal é ativar este estado usando interrupção: Quando chegar um data byte e a FRM estiver em FRM_State_Idle */
        {
            RxBuff_Length = Comm_protocol_Get_RxBUFFER_Length();
            if(RxBuff_Length != RxBuff_Length_Previous){
                RxBuff_Length_Previous = RxBuff_Length;
                RxBuff_Timeout = 0;
            }else{
                RxBuff_Timeout++;
            }
            if(RxBuff_Timeout >= 3){
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
            if(Comm_appl_Echo_Frame(pUart) == FALSE){          /* Verifica se o frame recebido não é eco */
                if(Comm_appl_Validate_Frame(pUart) == TRUE){   /* Verifica se o frame recebido é válido */
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
byte Comm_appl_RHM(Uart_t *pUart)
{
    switch (pUart->RHM_State){
        case RHM_State_Idle:
        {
            break;
        }
        case RHM_State_Process:
        {
            //if(Comm_appl_FindCommand(&pMainData->RxBuffer[_SID], pMainData) == KOSTIA_OK){
                Comm_appl_Request_ChangeOf_RHM_State(pUart, RHM_State_Idle);
            //}     
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
void Comm_appl_Request_ChangeOf_FSM_State( Uart_t *pUart, FSM_States_t nextState)
{
    pUart->FSM_State = nextState;
}


/***********************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função realiza a mudança da variável de estado da FRM
***********************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_FRM_State( Uart_t *pUart, FRM_States_t nextState )
{
    pUart->FRM_State = nextState;
}


/***********************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função realiza a mudança da variável de estado da RHM
***********************************************************************************************************************************************************************************************/
void Comm_appl_Request_ChangeOf_RHM_State( Uart_t *pUart, RHM_States_t nextState )
{
    pUart->RHM_State = nextState;
}


/***********************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função configura os campos do Frame Header
***********************************************************************************************************************************************************************************************/
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


/***********************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função configura o dados do Frame Data
***********************************************************************************************************************************************************************************************/
void Comm_appl_Set_Frame_Data(Frame_t *pFrame, byte *Data, int Size)
{
    for(int i=0; i<Size; i++){
        pFrame->Data[i] = *(Data + i);
    }
}


/***********************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função configura o valor do Frame Checksum
***********************************************************************************************************************************************************************************************/
void Comm_appl_Set_Frame_Checksum(Frame_t *pFrame)
{
    pFrame->Checksum = pFrame->Type | pFrame->SID;
}


/***********************************************************************************************************************************************************************************************
    Função
    Descrição: Esta função configura o Buffer Tx que será passado para a camada Comm_protocol transmitir na UART
***********************************************************************************************************************************************************************************************/
int Comm_appl_FrameToBuffer(Uart_t *pUart)
{
    int i;
    pUart->TxBuffer[_BREAK]  = pUart->scheduleTable->frame.Break;  //(char)pUart->scheduleTable->frame.Break;
    pUart->TxBuffer[_SYNCH]  = pUart->scheduleTable->frame.Synch;
    pUart->TxBuffer[_SID]    = pUart->scheduleTable->frame.SID;
    pUart->TxBuffer[_TYPE]   = pUart->scheduleTable->frame.Type;
    pUart->TxBuffer[_ID_SRC] = pUart->scheduleTable->frame.Id_Source;
    pUart->TxBuffer[_ID_TRG] = pUart->scheduleTable->frame.Id_Target;
    pUart->TxBuffer[_LENGHT] = pUart->scheduleTable->frame.Lenght;
    for(i = 0; i < pUart->scheduleTable->frame.Lenght - 1; i++){
        pUart->TxBuffer[_DATA+i] = pUart->scheduleTable->frame.Data[i];
    }
    pUart->TxBuffer[_DATA+i] = pUart->scheduleTable->frame.Checksum;
    return (7 + pUart->scheduleTable->frame.Lenght);
}


/******************************************************************************************************
    Descrição: Verifica se o frame recebido na serial Rx é eco do frame transmitido na serial Tx.
    
    \Parameters: *pMainData - Instancia da estrutura de dados principal Main Data.
    
    \Return value: 0 - significa que não é eco
    \Return value: 1 - significa que é eco
******************************************************************************************************/
int Comm_appl_Echo_Frame(Uart_t *pUart)
{
    if(pUart->RxBuffer[_ID_SRC] == pUart->TxBuffer[_ID_SRC]){
        return 1;
    }
    return 0;
}


/******************************************************************************************************
    Descrição: Valida se o frame recebido na serial Rx é valido. Checa os campos: Break, Synch e Checksum
    
    \Parameters: *pMainData - Instancia da estrutura de dados principal Main Data.
    
    \Return value: 0 - significa que o frame não é valido
    \Return value: 1 - significa que o frame é valido
******************************************************************************************************/
int Comm_appl_Validate_Frame(Uart_t *pUart)
{
    if(pUart->RxBuffer[_BREAK] != 0x00 || pUart->RxBuffer[_SYNCH] != 0x55){
        return 0;
    }
    return 1;
}


Slot_t *Comm_appl_Create_Schedule_Table(void)
{
    byte Data[] = {}; //byte (*Data)[] = {0xA9, 0x1C, 0x47};
    Slot_t *pSlot;
    pSlot = (Slot_t *) malloc( sizeof(Slot_t *) );  /* Alocação dinamica de memória para armazenar uma "struct Slot" */
    pSlot->nextSlot = pSlot;
    Comm_appl_Set_Frame_Header(&pSlot->frame, 0x00, 0x55, 0x01, 0x01, 0x01, 0xFF, sizeof(Data) + 0x01);
    Comm_appl_Set_Frame_Data(&pSlot->frame, Data, sizeof(Data));
    Comm_appl_Set_Frame_Checksum(&pSlot->frame);
    return pSlot;
}


void Comm_appl_Insert_Slot(Slot_t *pCurrentSlot)
{
    struct Slot *pSlot, *pAuxSlot;
    pAuxSlot->nextSlot = pCurrentSlot->nextSlot;
    pSlot = (struct Slot *) malloc( sizeof(Slot_t *) );  //Alocação dinamica de memória para armazenar uma "struct Slot"
    pSlot->nextSlot = pAuxSlot->nextSlot;
    pCurrentSlot->nextSlot = pSlot;
}


Slot_t *Comm_appl_Select_Next_Slot(Slot_t *pCurrentSlot)
{
    return pCurrentSlot->nextSlot;
}

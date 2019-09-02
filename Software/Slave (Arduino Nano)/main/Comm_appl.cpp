/* Fish Tank automation project - TCC -  Comm_appl file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */


/* Headers includes */ 
#include "Comm_appl.h"


//static byte RxBuffer[RX_BUFFER_SIZE];

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
      lenght = Comm_appl_frameToBuffer(pMainData);
      Comm_protocol_Frame_Send_Request(pMainData->TxBuffer, lenght);
      Comm_appl_Request_ChangeOf_FSM_State(pMainData, FSM_State_Sending);      
      break;
    }
    case FSM_State_Sending: /* O ideal é sair deste estado usando interrupção: Quando todos data bytes no TX FIFO forem transmitidos */
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


byte Comm_appl_FRM(struct MainData *pMainData)
{
  uint8_t Data_Buffer[128];
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
      } else{
        RxBuff_Timeout++;
      }
      if(RxBuff_Timeout >= 3){ /* Timeout de 30ms */
        RxBuff_Timeout = 0;
        RxBuff_Length_Previous = 0;
        Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Received);
      }
      break;                                                                                                                                              
    }
    case FRM_State_Received:
    {
      int i;
      i = Serial.readBytes(pMainData->RxBuffer, RxBuff_Length);
      //Teste01.inicio
      if(validacao(pMainData->RxBuffer, i) == 1){
        Comm_appl_Request_ChangeOf_FSM_State(pMainData, FSM_State_Send);
      }
      else{
        Serial.println("Erro de recepcao");
      }
      //Teste01.fim
      Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Error);
      break;
    }
    case FRM_State_Error:
    {
      Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Idle);
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


int validacao(byte *Buffer, int lenght)
{
  int i;
  byte Ref[64];
  Ref[0] = 0x61;
  Ref[1] = 0x62;
  for(i=0; i<lenght; i++){
    if(Buffer[i] != Ref[i])
      return 0;
  }
  //Serial.println("Validou");
  return 1;
}


void Comm_appl_Request_ChangeOf_FSM_State(struct MainData *pMainData, enum FSM_States nextState)
{
  pMainData->FSM_State = nextState;
}


void Comm_appl_Request_ChangeOf_FRM_State(struct MainData *pMainData, enum FRM_States nextState)
{
  pMainData->FRM_State = nextState;
}


void Comm_appl_Set_Frame_Header(struct Frame *pFrame, byte Break, byte Synch, byte Id_Source, byte Id_Target, byte Lenght, byte Type, byte SID)
{
  pFrame->Break = Break;
  pFrame->Synch = Synch;
  pFrame->Id_Source = Id_Source;
  pFrame->Id_Target = Id_Target;
  pFrame->Lenght = Lenght;
  pFrame->Type = Type;
  pFrame->SID = SID;
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

/*[To Do] Implementar esta função - Incluir o campo de Data e Checksum do frame no TxBuffer*/
int Comm_appl_frameToBuffer(struct MainData *pMainData)
{
  int i;
  /*
  pMainData->TxBuffer[0] = 0x54; //pMainData->frame.Break;
  pMainData->TxBuffer[1] = 0x65;//pMainData->frame.Synch;
  pMainData->TxBuffer[2] = 0x73;//pMainData->frame.Id_Source;
  pMainData->TxBuffer[3] = 0x74;//pMainData->frame.Id_Target;
  pMainData->TxBuffer[4] = 0x65;//pMainData->frame.Lenght;
  pMainData->TxBuffer[5] = 0x20;//pMainData->frame.Type;
  pMainData->TxBuffer[6] = 0x21;//pMainData->frame.SID;
  */
  pMainData->TxBuffer[0] = pMainData->frame.Break;
  pMainData->TxBuffer[1] = pMainData->frame.Synch;
  pMainData->TxBuffer[2] = pMainData->frame.Id_Source;
  pMainData->TxBuffer[3] = pMainData->frame.Id_Target;
  pMainData->TxBuffer[4] = pMainData->frame.Lenght;
  pMainData->TxBuffer[5] = pMainData->frame.Type;
  pMainData->TxBuffer[6] = pMainData->frame.SID;
  
  return 7;
}

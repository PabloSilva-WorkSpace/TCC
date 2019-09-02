/* Fish Tank automation project - TCC -  Comm_appl file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */


/* Headers includes */ 
#include "Comm_appl.h"


byte Comm_appl_FSM(struct MainData *pMainData)   /* Frame Send Machine*/
{
  switch (pMainData->FSM_State){
    case FSM_State_Idle:
    {
      break;
    }
    case FSM_State_Send:
    {
      Comm_protocol_Frame_Send_Request(&pMainData->scheduleTable->frame);
      Comm_appl_Request_ChangeOf_FSM_State(pMainData, FSM_State_Sending);      
      break;
    }
    case FSM_State_Sending: /* O ideal é sair deste estado usando interrupção: Quando todos data bytes no TX FIFO forem transmitidos */
    {
      if(Comm_protocol_Get_TXFIFO_Lenght() == 0){
        Comm_appl_Request_ChangeOf_FSM_State(pMainData, FSM_State_Error);
        Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Receiving);
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


byte Comm_appl_FRM(struct MainData *pMainData) /* Frame Receive Machine */
{
  uint8_t Data_Buffer[128];
  static int RxBuff_Timeout = 0;
  static int RxBuff_Length = 0;
  static int RxBuff_Length_Previous = 0;
  switch (pMainData->FRM_State){
    case FRM_State_Idle:
    {
      break;
    }
    case FRM_State_Receiving: /* O ideal é ativar este estado usando interrupção: Quando chegar um data byte e a FRM estiver em FRM_State_Idle */
    {
      uart_get_buffered_data_len(UART_NUM_2, (size_t*)&RxBuff_Length);
      if(RxBuff_Length != RxBuff_Length_Previous){
        RxBuff_Length_Previous = RxBuff_Length;
        RxBuff_Timeout = 0;
      } else{
        RxBuff_Timeout++;
      }
      if(RxBuff_Timeout >= 10){
        RxBuff_Timeout = 0;
        RxBuff_Length_Previous = 0;
        Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Received);
      }
      break;
    }
    case FRM_State_Received:
    {
      int N_Data_Read;
      N_Data_Read = uart_read_bytes(UART_NUM_2, Data_Buffer, RxBuff_Length, 0);
      Serial.printf("\nN# data bytes buffered: %d - N# data bytes read: %d\n", RxBuff_Length, N_Data_Read);
      for(int i=0; i<N_Data_Read; i++)
        Serial.printf("%X ", (byte)Data_Buffer[i]);
      Comm_appl_Request_ChangeOf_FRM_State(pMainData, FRM_State_Idle);
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


struct Slot *Comm_appl_Create_Schedule_Table(void)
{
  byte Data[] = {}; //byte (*Data)[] = {0xA9, 0x1C, 0x47};
  struct Slot *pSlot;
  pSlot = (struct Slot *) malloc( sizeof(struct Slot *) );  //Alocação dinamica de memória para armazenar uma "struct Slot"
  pSlot->nextSlot = pSlot;
  Comm_appl_Set_Frame_Header(&pSlot->frame, 0x00, 0x55, 0x01, 0xFF, 0x03 + sizeof(Data), 0x01, 0x01);
  Comm_appl_Set_Frame_Data(&pSlot->frame, Data, sizeof(Data));
  Comm_appl_Set_Frame_Checksum(&pSlot->frame);
  return pSlot;
}


void Comm_appl_Insert_Slot(struct Slot *pCurrentSlot)
{
  struct Slot *pSlot, *pAuxSlot;
  pAuxSlot->nextSlot = pCurrentSlot->nextSlot;
  pSlot = (struct Slot *) malloc( sizeof(struct Slot *) );  //Alocação dinamica de memória para armazenar uma "struct Slot"
  pSlot->nextSlot = pAuxSlot->nextSlot;
  pCurrentSlot->nextSlot = pSlot;
}


struct Slot *Comm_appl_Select_Next_Slot(struct Slot *pCurrentSlot)
{
  return pCurrentSlot->nextSlot;
}

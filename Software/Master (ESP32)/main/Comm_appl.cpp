/* Fish Tank automation project - TCC -  Comm_appl file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */

/* Headers includes */ 
#include "Comm_appl.h"


byte Comm_appl_FSRM(struct MainData *pMainData)
{
  switch (pMainData->FSRM_State){
    case FSRM_State_Idle:
    {
      break;
    }
    case FSRM_State_Send:
    {
      Comm_protocol_Frame_Send_Request(&pMainData->scheduleTable->frame);
      Comm_appl_Request_ChangeOf_FSRM_State(pMainData, FSRM_State_Sending);
      break;
    }
    case FSRM_State_Sending:
    {
      Comm_appl_Request_ChangeOf_FSRM_State(pMainData, FSRM_State_Error);
      break;
    }
    case FSRM_State_Error:
    {
      /* ToDo[PENS] error handler */
      Comm_appl_Request_ChangeOf_FSRM_State(pMainData, FSRM_State_Idle);
      break;
    } 
    default:
    {
      /* ToDo[PENS] - error handler to FSRM */
    }
  }    
  return 0;
}


void Comm_appl_Request_ChangeOf_FSRM_State(struct MainData *pMainData, enum FSRM_States nextState)
{
  pMainData->FSRM_State = nextState;
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

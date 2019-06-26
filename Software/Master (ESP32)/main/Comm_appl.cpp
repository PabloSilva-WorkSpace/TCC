/* Fish Tank automation project - TCC -  Comm_appl file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */

/* Headers includes */ 
#include "Comm_appl.h"


byte Comm_appl_SendData_Request(struct MainData *pMainData)
{
    switch (pMainData->Status_Send_Rqt){
      case Comm_appl_Status_Idle:
      {
        break;
      }
      case Comm_appl_Status_Send:
      {
        Comm_protocol_Send_Frame(&pMainData->scheduleTable->frame);
        break;
      }
      case Comm_appl_Status_Error:
      {
        break;
      } 
      default:
      {
        /*ToDo[PENS] - handle error*/
      }
    }    
    return 0;
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
  pSlot = (struct Slot *) malloc( sizeof(struct Slot *) );
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
  pSlot = (struct Slot *) malloc( sizeof(struct Slot *) );
  pSlot->nextSlot = pAuxSlot->nextSlot;
  pCurrentSlot->nextSlot = pSlot;
}


struct Slot *Comm_appl_Select_Next_Slot(struct Slot *pCurrentSlot)
{
  return pCurrentSlot->nextSlot;
}

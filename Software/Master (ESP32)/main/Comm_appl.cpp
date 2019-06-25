/* Fish Tank automation project - TCC -  Comm_appl file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */

/* Headers includes */ 
#include "Comm_appl.h"



byte Comm_appl_SendData_Request(struct MainData *pData)
{
    switch (pData->Status_Send_Rqt){
      case Comm_appl_Status_Idle:
      {
        break;
      }
      case Comm_appl_Status_Send:
      {
        Comm_protocol_Send_Frame(&(pData->frame));
        //Comm_protocol_Send_Frame_v2(&(pData->frame), 8);
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


void Comm_appl_Set_Header_Frame(byte Break, byte Id_Source, byte Type, byte SID, byte Id_Target, byte Lenght)
{
  
}

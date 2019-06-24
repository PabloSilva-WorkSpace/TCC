/* Fish Tank automation project - TCC -  Comm_appl file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */

/* Headers includes */ 
#include "Comm_appl.h"



byte Comm_appl_SendData_Request(MainData *pData)
{
    char *str = "Teste\0";
    switch (pData->Status_Send_Rqt){
      case Comm_appl_Status_Idle:
      {
        break;
      }
      case Comm_appl_Status_Send:
      {
        Comm_protocol_Send_Frame(str);  
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

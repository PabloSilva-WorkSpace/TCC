/*********************************************************************************************************** 
 * Fish Tank automation project - TCC -  main file
 * Developer: 
 * 
 * ToDo[PENS] - need to improve the comments
***********************************************************************************************************/

 /* Headers includes */ 
#include "Comm_protocol.h"

int Comm_protocol_Frame_Send_Request(Frame_t *pFrame)
{
  char *str = NULL;
  int iFrame_Length, iFrame_Length_Sent;
  str = structToString(pFrame);
  iFrame_Length = pFrame->Lenght + 5;
  iFrame_Length_Sent = uart_write_bytes(UART_ID, (const char *)str, iFrame_Length);
  //iFrame_Length_Sent = uart_tx_chars(UART_ID, (const char *)str, iFrame_Length);
  free(str);
  return iFrame_Length_Sent;
}


char *structToString(Frame_t *pFrame)
{
  int i, Size;
  Size = pFrame->Lenght + 5;
  char *str = (char*) malloc(Size * sizeof(char));  //Aloca #Size bytes de memória
  *(str + 0) = (char)(pFrame->Break);
  *(str + 1) = (char)(pFrame->Synch);
  *(str + 2) = (char)(pFrame->Id_Source);
  *(str + 3) = (char)(pFrame->Id_Target);
  *(str + 4) = (char)(pFrame->Lenght);
  *(str + 5) = (char)(pFrame->Type);
  *(str + 6) = (char)(pFrame->SID);
  for(i=7; i<(7 + pFrame->Lenght - 3); i++){
    *(str + i) = (char)(pFrame->Data[i-7]);
  }
  *(str + i) = (char)(pFrame->Checksum);
  return str;
}


int Comm_protocol_Get_TXFIFO_Lenght()
{
  uint32_t *pUART2_STATUS_REG;
  pUART2_STATUS_REG = (uint32_t*)UART_STATUS_REG(2);
  return (int)(*pUART2_STATUS_REG>>16)&0xFF;
}


int Comm_protocol_Get_RXFIFO_Lenght()
{
  uint32_t *pUART2_STATUS_REG;
  pUART2_STATUS_REG = (uint32_t*)UART_STATUS_REG(2);
  return (int)(*pUART2_STATUS_REG>>0)&0xFF;
}

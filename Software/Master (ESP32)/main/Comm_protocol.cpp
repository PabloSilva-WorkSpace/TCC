/* Fish Tank automation project - TCC -  Comm_protocol file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */

 /* Headers includes */ 
#include "Comm_protocol.h"


void Comm_protocol_Frame_Send_Request(struct Frame *pFrame)
{
  char *str = NULL;
  int Size;
  str = structToString(pFrame);
  Size = pFrame->Lenght + 5;
  uart_write_bytes(UART_ID, (const char *)str, Size);
  free(str);
}


char *structToString(struct Frame *pFrame)
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

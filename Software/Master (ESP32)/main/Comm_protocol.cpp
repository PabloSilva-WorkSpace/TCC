/* Fish Tank automation project - TCC -  Comm_protocol file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */

 /* Headers includes */ 
#include "Comm_protocol.h"


void Comm_protocol_Send_Frame(struct Frame *pFrame)
{
  char *str = NULL;
  str = structToString(pFrame);
  uart_write_bytes(UART_ID, (const char *)str, 8);
  Serial.printf("sizeof(*str): %d\n", sizeof(*str));
}


char *structToString(struct Frame *pFrame)
{
  char *str = (char*) malloc(8 * sizeof(char));  //Aloca #7 bytes de memória 
  *(str + 0) = (char)(pFrame->Break);
  *(str + 1) = (char)(pFrame->Id_Source);
  *(str + 2) = (char)(pFrame->Type);
  *(str + 3) = (char)(pFrame->SID);
  *(str + 4) = (char)(pFrame->Id_Target);
  *(str + 5) = (char)(pFrame->Lenght);
  *(str + 6) = (char)(pFrame->Data[0]);
  *(str + 7) = (char)(pFrame->Cs);
  return str;
}


void Comm_protocol_Send_Frame_v2(byte *pFrame, byte frameLength)
{
  char *str = NULL;
  str = structToString_v2(pFrame, frameLength);
  uart_write_bytes(UART_ID, (const char *)str, frameLength);
  Serial.printf("sizeof(*pFrame): %d\n", sizeof(*pFrame));
}


char *structToString_v2(byte *pFrame, byte frameLength)
{
  byte i;
  char *str = (char*) malloc(frameLength * sizeof(char));  //Aloca #frameLength bytes de memória  
  for(i=0; i<frameLength; i++){
    *(str + i) = (char)*(pFrame + i);
  }
  return str;
}

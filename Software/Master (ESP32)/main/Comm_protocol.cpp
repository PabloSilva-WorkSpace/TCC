/* Fish Tank automation project - TCC -  Comm_protocol file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */

 /* Headers includes */ 
#include "Comm_protocol.h"

void Comm_protocol_Send_Frame(char *Data)
{
  uart_write_bytes(UART_ID, (const char *)Data, strlen(Data));
  Serial.println("ponto 1");  
}

 

/* Fish Tank automation project - TCC -  Comm_protocol file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */

 /* Headers includes */ 
#include "Comm_protocol.h"


/*
struct Frame{
  byte Break = 0x00;           Break signal 
  byte Synch = 0x55;           Synch signal 
  byte Id_Source = 0x01;       ID do módulo transmissor 
  byte Id_Target = 0x00;       ID do módulo alvo 
  byte Lenght = 0x01;          Comprimento da mensagem 
  byte Type = 0x01;            Tipo de módulo do transmissor 
  byte SID = 0x00;             Identificador de serviço da mensagem 
  byte Data[MAX_DATA_SIZE];    Dados 
  byte Checksum = 0x00;        Checksum 
};
*/


int Comm_protocol_Frame_Send_Request(byte *TxBuffer, int lenght)
{
  //Serial.println("Send");
  Serial.write(TxBuffer, lenght); //Serial.write(pMainData->RxBuffer, i);
  return 1;
}


int Comm_protocol_Get_TXFIFO_Lenght()
{
  return Serial.availableForWrite();
}


int Comm_protocol_Get_RXFIFO_Lenght()
{
  return Serial.available();
}

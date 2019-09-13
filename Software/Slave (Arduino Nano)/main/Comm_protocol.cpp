/* 
 *  Fish Tank automation project - TCC -  Comm_protocol file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
*/


/*********************************************************************************************
    Headers includes 
**********************************************************************************************/
#include "Comm_protocol.h"


int Comm_protocol_Frame_Send_Request(byte *TxBuffer, int lenght)
{
  return Serial.write(TxBuffer, lenght);
}


int Comm_protocol_Frame_Get_Request(byte *RxBuffer, int lenght)
{
  return Serial.readBytes(RxBuffer, lenght);
}


int Comm_protocol_Get_TXFIFO_Lenght()
{
  return Serial.availableForWrite();
}


int Comm_protocol_Get_RXFIFO_Lenght()
{
  return Serial.available();
}

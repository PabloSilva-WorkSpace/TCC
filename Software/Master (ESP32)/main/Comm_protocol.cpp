/********************************************************************************************************************************************************************************************************************************************************
 * Fish Tank automation project - TCC -  Comm_protocol file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "Comm_protocol.h"


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Esta função escreve bytes no Tx_Buffer, os quais serão posteriormente armazenados automaticamente no Tx_Fifo e enviados no pino TxD.
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_protocol_Frame_Send_Request(TxBuffer_t *pTxBuffer, int lenght)
{
    return Serial2.write((byte *)pTxBuffer, lenght);
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Esta função lê os bytes no Rx_Buffer, os quais serão posteriormente armazenados automaticamente no Tx_Fifo e enviados no pino TxD.
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_protocol_Frame_Read_Request(RxBuffer_t *pRxBuffer, int RxBuff_Length)
{
    return Serial2.readBytes((byte *)pRxBuffer, RxBuff_Length);
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Esta função lê a quantidade de bytes armazenados no Tx_FIFO (O último estágio antes do pino TxD - output), os quais ainda serão transmitidos no pino TxD.
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_protocol_Get_TxFIFO_Length()
{
    return Serial2.availableForWrite();
}


/********************************************************************************************************************************************************************************************************************************************************
    Descrição: Esta função lê a quantidade de bytes armazenados no Rx_FIFO (O primeiro estágio a partir do pino RxD - input), os quais tem sido recebidos no pino RxD.
*********************************************************************************************************************************************************************************************************************************************************/
int Comm_protocol_Get_RxFIFO_Length()
{
    return Serial2.available();
}



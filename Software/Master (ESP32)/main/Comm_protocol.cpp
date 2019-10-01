/*********************************************************************************************************** 
 * Fish Tank automation project - TCC -  main file
 * Developer: 
 * 
 * ToDo[PENS] - need to improve the comments
***********************************************************************************************************/

 /* Headers includes */ 
#include "Comm_protocol.h"


/***********************************************************************************************************************************************************************************************
    Descrição: Esta função escreve bytes no Tx_Buffer, os quais serão posteriormente armazenados automaticamente no Tx_Fifo e enviados no pino TxD.
***********************************************************************************************************************************************************************************************/
int Comm_protocol_Frame_Send_Request(TxBuffer_t *pTxBuffer, int lenght)
{
    return uart_write_bytes(UART_ID, (const char *)pTxBuffer, lenght);
}


/***********************************************************************************************************************************************************************************************
    Descrição: Esta função lê os bytes no Rx_Buffer, os quais serão posteriormente armazenados automaticamente no Tx_Fifo e enviados no pino TxD.
***********************************************************************************************************************************************************************************************/
int Comm_protocol_Frame_Read_Request(RxBuffer_t *pRxBuffer, int RxBuff_Length)
{
    int nDataReceived;
    nDataReceived = uart_read_bytes(UART_ID, (uint8_t*)pRxBuffer, RxBuff_Length, 0);
    return nDataReceived;
}


/***********************************************************************************************************************************************************************************************
    Descrição: Esta função lê a quantidade de bytes armazenados no Tx_FIFO (O último estágio antes do pino TxD - output), os quais ainda serão transmitidos no pino TxD.
***********************************************************************************************************************************************************************************************/
int Comm_protocol_Get_TxFIFO_Length()
{
    uint32_t *pUART2_STATUS_REG;
    pUART2_STATUS_REG = (uint32_t*)UART_STATUS_REG(2);
    return (int)(*pUART2_STATUS_REG>>16)&0xFF;
}


/***********************************************************************************************************************************************************************************************
    Descrição: Esta função lê a quantidade de bytes armazenados no Rx_FIFO (O primeiro estágio a partir do pino RxD - input), os quais tem sido recebidos no pino RxD.
***********************************************************************************************************************************************************************************************/
int Comm_protocol_Get_RxFIFO_Length()
{
    uint32_t *pUART2_STATUS_REG;
    pUART2_STATUS_REG = (uint32_t*)UART_STATUS_REG(2);
    return (int)(*pUART2_STATUS_REG>>0)&0xFF;
}


/***********************************************************************************************************************************************************************************************
    Descrição: Esta função lê a quantidade de bytes armazenados no Rx_BUFFER (O estágio seguinte ao Rx_FIFO), os quais foram recebidos no Rx_FIFO.
***********************************************************************************************************************************************************************************************/
int Comm_protocol_Get_RxBUFFER_Length()
{
    size_t RxBuff_length;
    uart_get_buffered_data_len(UART_ID, &(RxBuff_length));   /* Lê a quantidade de bytes armazenados no Rx_Buffer da UART definida por UART_ID e armazena em RxBuff_Length */
    return RxBuff_length;
}

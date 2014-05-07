/*
* Methods for the UARTi public interface.
*/
#ifndef UART_I_API_H
#define UART_I_API_H

UINT    UARTiConfigure      (UART_MODULE, UINT32);
void    UARTiDisable(UART_MODULE iUART);
void    UARTiWriteString(const char *ccpString, UART_MODULE iUART);
void    UARTiWriteBinaryData(unsigned char *data_block, int length, UART_MODULE iUART);

#endif


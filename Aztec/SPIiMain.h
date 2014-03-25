/*
* Private methods and attributes available only to SPIiMain.c
*/
#ifndef SPI_I_MAIN_H
#define SPI_I_MAIN_H

#define SPI_SIZEOF_RX_REPOSITORY_ARRAY	100;

#define COM_CLEAR 0

int  SPIiSendMessageAttn(char *buf, int len, int leave_attention_asserted);
void SPIiRestart(void);
int  SPIiWaitReceiveChar(char *c);
int  SPIiWaitTransmitChar(void);

#endif


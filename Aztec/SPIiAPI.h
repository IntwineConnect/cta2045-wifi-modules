/*
* Methods for the SPIi public interface.
*/
#ifndef SPI_I_API_H
#define SPI_I_API_H

#define ATTENTION_DEASSERT          0
#define ATTENTION_REMAIN_ASSERTED   1

#define RESPONSE_RECEIVED           0
#define RESPONSE_NOT_RECEIVED       1
#define RESPONSE_NOT_VERIFIED       2

#define TRANSMIT_SUCCESSFUL         0
#define TRANSMIT_NOT_SUCCESSFUL     1

void    SPIiInitialize(int control_reg_val);
int     SPIiSendMessage(char *buf, int len);
int     SPIiSendMessageEntek(char *sendbuf, int len, char *userbuf, int *userlen);

#endif


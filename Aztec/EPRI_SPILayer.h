
/**
 * File:: EPRI_SPILayer.h
 * Desc: SPI level processing header
 * Author: Jordan Murray
 * Date: 5/10/2016
 */

#include "MCI_Common.h"

#ifndef __EPRI_SPILAYER_H
 #define __SPI_UARTLAYER_H


#define MIN_TIME_BEFORE_CLOCK_US   20     //us between assertion of both flow control signals and the beginning of clocked data transfer
#define MIN_TIME_AFTER_CLOCK_US    20     //us between end of clocked data and deassertion of flow control signals

#define INTER_MESSAGE_TIMEOUT_MS   100    // inter message delay 100 ms
#define MESSAGE_INIT_TIMEOUT_MS    100    // messsage initiation failure timeout  100 ms
#define APP_ACK_WAIT_TIME_MS       100
#define LINK_ACK_DELAY_MS          20
#define MESSAGE_RETRY_DELAY_MS     500
#define CS_ASSERT_TIMEOUT_MS       10   //maximum permissible delay before an SPI master responds to ATTN assetion


#define TX_NUM_TRIES    3

#define OVERRIDE_OPCODE1        0x11
#define OPSTATE_OPCODE1         0x13
#define IDLE_NORMAL             0x00
#define RUNNING_NORMAL          0x01

#define INTWINE_MSG             0xF0
#define EPRI_READ_CMD           0xF0
#define EPRI_WRITE_CMD          0xF1
#define EPRI_SAVE_CMD           0xF2

#define SPI_ENABLE      1
#define SPI_DISABLE     0


void EPRI_SPI_TimeMonitor_Callback(void);
void EPRI_SPI_init();
void SPI_MCI_Sync_Callback(void);
void SPI_Message_Timeout_Callback(void);
void SPI_MCI_Wait_Callback(void);
BOOL MCI_IsSendingSPI(void);
MCIResponse MCISendSPI(unsigned char * msg);



void MCISendAsyncSPI(unsigned char * msg);
void SPIrxMessageHandler(MCIResponse * lastSentPacket);
void SPIrxbuffer_initialize();
void SPI3_ISR(void);

#endif
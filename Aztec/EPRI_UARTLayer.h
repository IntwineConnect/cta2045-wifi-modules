/**
 * File: EPRI_UARTLayer.h
 * Desc: UART level processing functionality for EPRI project
 *
 */
 
#ifndef INCLUDE_EPRI_UART_H
 #define INCLUDE_EPRI_UART 1
#endif
 
#define DLL_ACK_NAK_MIN_TIME_OUT_MS 40      // Data ink-layer ACK.  40-200ms
#define DLL_ACK_NAK_MAX_TIME_OUT_MS 200     // Data ink-layer ACK.  40-200ms
#define AL_RESPONSE_MIN_TIME_OUT_MS 100     // App layer response.  100ms-3s
#define AL_RESPONSE_MAX_TIME_OUT_MS 3000    // App layer response.  100ms-3s
#define INTER_MESSAGE_MIN_DLY_MS    100     // Inter message delay. 100ms-1s
#define MESSAGE_RETRY_DLY_MS        500     // Retry delay

#define RX_BUF_SIZE 300
#define TX_MSG_SIZE 10

#define TX_NUM_TRIES    3

#define OVERRIDE_OPCODE1    0x11
#define OPSTATE_OPCODE1     0x13
#define IDLE_NORMAL         0x00
#define RUNNING_NORMAL      0x01

#define INTWINE_MSG    0xF0
#define EPRI_READ_CMD  0xF0
#define EPRI_WRITE_CMD 0xF1
#define EPRI_SAVE_CMD  0xF2

typedef struct
{
   UINT8   numTries;            // Number of times the command was sent before it completed.
   UINT8   numBytesReceived;    // Total bytes received in last message. Includes Checksum
   UINT8   LLResponseValid;     // flag that indicates we have something
   UINT8   LLResponse[11];      // the received packet
   UINT8   AppResponseValid;    // flag that indicates we have something
   UINT8   AppResponse[11];     // the received packet
} RSResponse;


void EPRI_TimeMonitor_Callback(void);
void EPRI_UART_init();
void MCI_Sync_Callback(void);
void Message_Timeout_Callback(void);
BOOL SendTimeSync(int weekday, int hour);
BOOL SendQueryOpState(unsigned char OSCnt);
BOOL MCI_IsSending();

// function to transmit RS485 packets. in EPRI_UARTLayer.c
RSResponse MCISend(unsigned char * msg);
void       MCISendAsync(unsigned char * msg);
void rxMessageHandler(RSResponse * lastSentPacket);
void rxbuffer_initialize();
void UART2_ISR(void);


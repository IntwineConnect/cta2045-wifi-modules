/**
 * EPRI_UARTLayer.c
 *
 * Intwine Connect 11/22/2011
 * Written by Joe Logan, Osmisys LLC
 *
 * This file handles UART level processing for the EPRI OpenADR demo.
 */


#include <uart.h>
#include <string.h>

#include "HardwareProfile.h"

#include "TimeMonitor.h"
#include "UARTiAPI.h"
#include "INTiAPI.h"
#include "EPRI_Queue.h"
#include "EPRI_UARTLayer.h"

int OK_TO_READ_232 = 0;
int position_counter = 0;
int numBytes = 0;
unsigned char RxBuf[RX_BUF_SIZE];       // receive buffer
unsigned char rxmessage[RX_BUF_SIZE];   // RxBuf double buffer.
unsigned char txmessage[TX_MSG_SIZE];   // Current TX message. Reused for Retries.
int RxBytesReceived = 0;

int OverRide = 0;
int OptOutEvent = 0;
int OptInEvent = 0;
int IdleNormal;
int RunningNormal;

unsigned char AppAckMsg[RX_BUF_SIZE] = { 0x08, 0x01, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00};
unsigned char TimeSyncMsg[8] = { 0x08, 0x01, 0x00, 0x02, 0x16, 0x00, 0x00, 0x00};
unsigned char QueryOpState[8] = { 0x08, 0x01, 0x00, 0x02, 0x12, 0x00, 0x00, 0x00};

RSResponse  AsyncTxRSBuf;
RSResponse  AsyncRxRSBuf;
RSResponse  NullRSBuf;

volatile enum _TxMsgState
{
    TX_IDLE = 0,
    TX_SEND_CMD,
    TX_WAIT_LL_ACK,
    TX_WAIT_APP_ACK,
    TX_WAIT_LL_ACK_DLY,
    TX_SEND_LL_ACK,
    TX_WAIT_RETRY_DLY,
} TxMsgState;

volatile enum _RxMsgState
{
    RX_IDLE = 0,
    RX_CMD_RECEIVED,
    RX_WAIT_LL_ACK_DLY,
    RX_SEND_LL_ACK,
    RX_WAIT_APP_ACK_DLY,
    RX_SEND_APP_ACK,
    RX_RECEIVE_LL_ACK
} RxMsgState;

// global variables
unsigned char dl_ack[1] = {0x06};
unsigned char dl_nak[2] = {0x15, 0x03};

UINT16 MakeChecksum(unsigned char * message, int len);
void MCI_Wait_Callback();
void parse_epri_buf(unsigned char *msg_buf, int *param_val, unsigned char *param_buf, int msg_len);
void App_Ack(unsigned char * message, int len);

/**
 * init function
 */
void EPRI_UART_init()
{

    INTiRegisterUART2RxCallbackFunction(UART2_ISR);

    rxbuffer_initialize();
    position_counter = 0;
    OK_TO_READ_232 = 1;
    
    NullRSBuf.numTries = 0;
    NullRSBuf.numBytesReceived = 0;
    NullRSBuf.LLResponseValid = 0;
    NullRSBuf.AppResponseValid = 0;

    OverRide = 0;
    IdleNormal = 0;
    RunningNormal = 0;
}


void EPRI_UART_write(unsigned char *message, int length )
{

	UARTEnable(UART2, UART_DISABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX)); 
	UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_TX)); 

    LATDSET = BIT_14;   // enable RS-485 TX output
    UARTiWriteBinaryData(message, length, UART2);
    LATDCLR = BIT_14;

	UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

}

/*
* Function:			void UART2_ISR(void)
* Description:		ISR for UART2
* Creation Date:	10/19/2011	
* Author:			Robert Scaccia
*/
void UART2_ISR(void)
{
	unsigned char i;

   // check receiver
    if (INTGetFlag(INT_U2RX) && INTGetEnable(INT_U2RX))	 // something in the receive buffer?
    {
        while (UARTReceivedDataIsAvailable(UART2))
        {
            i = UARTGetDataByte(UART2);
            if (OK_TO_READ_232) // if we are past bootup
            {
                // Call back detect 20ms idle time on RS-485 bus.
                // Per the MCI V2 spec, section 14.1.4 this indicates 
                // "start of a new message".
                TimeMonitorRegisterI(2, 20, MCI_Sync_Callback);

                RxBuf[position_counter] = i;
                RxBuf[position_counter + 1] = '\0';
                position_counter++;
            }
        }
    }
}

/**
 * Construct and send a MCI packet via the UART, blocking.
 *
 * @param msgtype1 Msg type byte, MSB
 * @param msgtype2 msg type byte, LSB
 * @param payload the message payload
 */
RSResponse MCISend(unsigned char * msg)
{

// Message Sequence:
//   1. Send command via MCISendAsync
//   2. Block till TX_IDLE


    if (MCI_IsSending())
        return NullRSBuf;

    MCISendAsync(msg);

    while (TxMsgState != TX_IDLE);
    
    return AsyncTxRSBuf;
}

/**
 * Construct and send a MCI packet via the UART, non-blocking
 * Call is non-blocking with timeouts specified MCI 2.0 spec.
 *
 * @param msgtype1 Msg type byte, MSB
 * @param msgtype2 msg type byte, LSB
 * @param payload the message payload
 */
void MCISendAsync(unsigned char * msg)
{

// Message Sequence:
//   1. Set TxMsgState, then Send command
// Handled by callbacks:
//   2. Link Layer ACK/NAK received in MCI_Sync_Callback. 
//   3. Application Layer ACK/NAK received in MCI_Sync_Callback.
//   4. Wait delay, send Link Layer ACK/NAK from MCI_Wait_Callback
//   5. Wait intermessage delay then return to TX_IDLE from MCI_Wait_Callback
//   6. Retry message if bad or no link-layer ack

    int i = 0;

    if (RxMsgState != RX_IDLE)
        return;

    if (TxMsgState == TX_IDLE)
    {
    // First Try - clear buffer
        AsyncTxRSBuf.numTries = 1;
        AsyncTxRSBuf.numBytesReceived = 0;
        AsyncTxRSBuf.LLResponseValid = 0;
        AsyncTxRSBuf.AppResponseValid = 0;
    }
    else
        AsyncTxRSBuf.numTries++;

    TxMsgState = TX_SEND_CMD;

    // 2nd two bytes of message are length of payload
    int payloadLen = msg[2] * 256 + msg[3];

    for (i = 0; i < payloadLen+4; i++)
    {
	    txmessage[i] = msg[i];
    }
    // add 4 to payload length to get message length
    MakeChecksum(txmessage, (payloadLen + 4));

    // Send initial message to start Tx State Machine
    // Set timeout also
    EPRI_UART_write(txmessage, (payloadLen + 6));
    TimeMonitorRegisterI(1, DLL_ACK_NAK_MAX_TIME_OUT_MS, Message_Timeout_Callback);

    TxMsgState = TX_WAIT_LL_ACK;
}

/**
 * Check to see if transmit is in progress
 *
 * @param msgtype1 Msg type byte, MSB
 * @param msgtype2 msg type byte, LSB
 * @param payload the message payload
 */
BOOL MCI_IsSending()
{
    if (TxMsgState == TX_IDLE)
        return FALSE;
    else
        return TRUE;

}

/**
 * Send a Data Layer ACK via the UART.
 */
void DL_Ack()
{
    EPRI_UART_write(dl_ack, 1);
}

/**
 * Send a Data Layer NACK via the UART.
 */
void DL_Nak()
{
    EPRI_UART_write(dl_nak, 2);
}

/**
 * Send a App Layer ACK via the UART.
 */
void App_Ack(unsigned char * message, int len)
{
    EPRI_UART_write(message, len);
}

/**
 * Send a Time Sync to SGD, non-blocking
 */
BOOL SendTimeSync(int weekday, int hour)
{

    if (MCI_IsSending())
        return FALSE;

    TimeSyncMsg[5] = (weekday << 4) + hour;
    MCISendAsync(TimeSyncMsg);

    return TRUE;

}

/**
 * Send a "Query Operational State" message to SGD, non-blocking
 */
BOOL SendQueryOpState(unsigned char OSCnt)
{

    if (MCI_IsSending())
        return FALSE;

    QueryOpState[5] = OSCnt;
    MCISendAsync(QueryOpState);

    return TRUE;
}

/**
 * Check the checksum of a received message.
 *
 * @param message[] the entire message, including checksum bits.
 * @param len the length of the string.
 * @return 1 for good checksum, 0 for bad checksum
 */

int ChecksumDecode(unsigned char * message, int len)
{
    // see MCI-V1-6.pdf page 68 for explanation
    int check1 = 0xAA;
    int check2 = 0;
    int checktemp = 0;

    int count = 0;
    while (count < len)
    {
        checktemp = message[count] & 0xFF;
        check1 = (check1 + checktemp) % 255;
        check2 = (check2 + check1) % 255;
        count++;
    }

    if ((check1 == 0) && (check2 == 0))
    {
        return 1;  // yay
    }
    else
    {
        return 0; // nay
    }
}

/**
 * Compute the checksum of an outgoing message. It is up to the user to append
 * the Int returned to the end of the string.
 *
 * @param message[] the message type bytes and payload.
 * @param lan string length.
 * @return checksum as two byte (integer) value
 */
UINT16 MakeChecksum(unsigned char * message, int len)
{
    // see MCI-V1-6.pdf page 68 for explanation
    //putsUART("in MakeChecksum \r\n");

    int check1 = 0xAA;
    int check2 = 0;
    int checktemp = 0;
    int msb = 0;
    int lsb = 0;

    int count = 0;
    while (count < len)
    {
        checktemp = message[count] & 0xFF;
        check1 = (check1 + checktemp) % 255;
        check2 = (check2 + check1) % 255;
        count++;
    }

    msb = 255 - ((check1 + check2) % 255);
    lsb = 255 - ((check1 + msb) % 255);
    message[len] =  (char) msb;
    message[len+1] =  (char) lsb;

    return (msb << 8) | lsb;
}

/**
 * handle a received packet.
 */
void rxMessageHandler(RSResponse * lastSentPacket)
{

    int len = rxmessage[2] * 256;
    len += rxmessage[3];
    len += 6;


    if ((TxMsgState != TX_IDLE) && !RxBytesReceived)
    // Timeout in MCISend
    {
        lastSentPacket->numBytesReceived = 0;
        lastSentPacket->LLResponseValid = 0;
        return;
    }

    // check for data link ack or nack (we dont ack acks)
    if (rxmessage[0] == 0x06 && rxmessage[1] == 0)
    {

        lastSentPacket->numBytesReceived = numBytes;
        lastSentPacket->LLResponse[0] = 0x06;
        lastSentPacket->LLResponse[1] = '\0';
        lastSentPacket->LLResponseValid = 1;
    }
    // nack code is 0x15
    else if (rxmessage[0] == 0x15 && rxmessage[2] == 0)
    {
        lastSentPacket->numBytesReceived = numBytes;
        lastSentPacket->LLResponse[0] = rxmessage[0];
        lastSentPacket->LLResponse[1] = rxmessage[1];
        lastSentPacket->LLResponse[2] = '\0';
        lastSentPacket->LLResponseValid = 1;
    }
    // if it wasn't a data link message...
    else
    {
        // check the checksum

        if (ChecksumDecode(rxmessage, len) == 0) // bad shape
        {
            // nack
            DL_Nak();
        }
        else // Message good
        {
            if ( rxmessage[0] < 0xF0 && (TxMsgState != TX_IDLE))
			// Save the application response
			// In most cases, it should be an application ack
            // put our received packet in lastSentPacket and throw the flag
            {
                int y;
                for (y = 0; y < 11; y++)
                {
                   lastSentPacket->AppResponse[y] = rxmessage[y];
                }
                lastSentPacket->numBytesReceived = numBytes;
                lastSentPacket->AppResponseValid = 1;

                // data link ack
                DL_Ack();

                // Stats for PGE hourly Load Factor
                // IdleNormal & RunningNormal cleared hourly by MCI_One_Second_Callback()
                if (rxmessage[4] == OPSTATE_OPCODE1)
                {
                    if (rxmessage[5] == IDLE_NORMAL)
                        IdleNormal++;
                    else if (rxmessage[5] == RUNNING_NORMAL)
                        RunningNormal++;
                }

            }
            else if (RxMsgState == RX_SEND_LL_ACK)
            {
                DL_Ack();

            }
            else if (RxMsgState == RX_SEND_APP_ACK)
            {

                if (rxmessage[0] < 0xF0)
                {

                    // Per Chuck Thomas use override as define in CEA-2045 Draft v08
                    // OptOutEvent and OptInEvent get set on change of state and 
                    // get cleared in OpenADRClient.c
                    if (rxmessage[4] == OVERRIDE_OPCODE1)
                    {
                        if (!OverRide && rxmessage[5])
                            OptOutEvent = 1;
                        else if (OverRide && !rxmessage[5])
                            OptInEvent = 1;
                         
                        OverRide = (int) rxmessage[5];
                    }
    
                    AppAckMsg[2] = 0;
                    AppAckMsg[3] = 0x02;
                    AppAckMsg[4] = 0x03;
                    AppAckMsg[5] = rxmessage[4];
                    MakeChecksum(AppAckMsg, 6);

                    App_Ack(AppAckMsg, 8);

                }
                else if (rxmessage[0] == INTWINE_MSG)
                {
//                    EPRI_Config( rxmessage, len );
                }

            }

        }
    }

    numBytes = 0;
    RxBytesReceived = 0;
    return;
}

// initialize buffer
void rxbuffer_initialize()
{
     int i;

    for (i = 0; i < RX_BUF_SIZE; i++)
    {
        RxBuf[i] = 0;
    }


}

// Message Sync indicator
// Indicates message receive is complete by 20ms with no received characters
// Callback initiated from UART2InterruptServiceRoutine
// RxBuf copied into rxmessage as doubled-buffered.
// Must be done processing rxmessage before next callback.
// 
void MCI_Sync_Callback()
{

    int i;

    numBytes = position_counter;
    for (i = 0; i < position_counter; i++)
    {
        rxmessage[i] = RxBuf[i];
        RxBuf[i] = 0;
    }
    rxmessage[i] = 0;

    numBytes = position_counter;
    position_counter = 0;
    RxBytesReceived = 1;

    switch (TxMsgState)
    {
        case TX_WAIT_LL_ACK:
            TimeMonitorCancelI(1);              // Cancel Message_Timeout_Callback
            rxMessageHandler(&AsyncTxRSBuf);
            if (AsyncTxRSBuf.LLResponseValid && AsyncTxRSBuf.LLResponse[0] == 0x06 && AsyncTxRSBuf.LLResponse[1] == 0x00)
            // Link-Layer ACK OK. Wait for Application ACK
            {
                TxMsgState = TX_WAIT_APP_ACK;
                TimeMonitorRegisterI(1, AL_RESPONSE_MAX_TIME_OUT_MS, Message_Timeout_Callback);
            }
            else
            // LL NAK or worse. Set quick timeout for end-of-message clean-up 
            {
                TimeMonitorRegisterI(1, 9, Message_Timeout_Callback);
            }
            break;

        case TX_WAIT_APP_ACK:
            TimeMonitorCancelI(1);              // Cancel Message_Timeout_Callback
            TxMsgState = TX_WAIT_LL_ACK_DLY;
            TimeMonitorRegisterI(3, DLL_ACK_NAK_MIN_TIME_OUT_MS, MCI_Wait_Callback);
            break;

        default:
            break;
    }

    if (TxMsgState == TX_IDLE)
    {
        switch (RxMsgState)
        {
            case RX_IDLE:

                AsyncRxRSBuf.numBytesReceived = 0;
                AsyncRxRSBuf.LLResponseValid = 0;
                AsyncRxRSBuf.AppResponseValid = 0;

                RxMsgState = RX_CMD_RECEIVED;
                rxMessageHandler(&AsyncRxRSBuf);    // send ACK/NAK

                RxMsgState = RX_WAIT_LL_ACK_DLY;
                TimeMonitorRegisterI(1, AL_RESPONSE_MAX_TIME_OUT_MS, Message_Timeout_Callback);
                TimeMonitorRegisterI(3, DLL_ACK_NAK_MIN_TIME_OUT_MS, MCI_Wait_Callback);
                break;

            case RX_RECEIVE_LL_ACK:

                TimeMonitorCancelI(1);              // Cancel Message_Timeout_Callback
                RxMsgState = RX_IDLE;
                break;

             default:
                break;
        }
    }
    return;
}

// MCI delayed callback. Used for delaying LL ACK/NAK and App ACK/NAK
// 
void MCI_Wait_Callback()
{

    switch (TxMsgState)
    {
        case TX_WAIT_LL_ACK_DLY:
            TxMsgState = TX_SEND_LL_ACK;
            rxMessageHandler(&AsyncTxRSBuf);    // ACK/NAK Sent from here
            TxMsgState = TX_IDLE;
            break;

        case TX_WAIT_RETRY_DLY:
            MCISendAsync(txmessage);
            break;

        default:
            break;
    }

    switch (RxMsgState)
    {
        case RX_WAIT_LL_ACK_DLY:
            RxMsgState = RX_SEND_LL_ACK;
            rxMessageHandler(&AsyncRxRSBuf);

            RxMsgState = RX_WAIT_APP_ACK_DLY;
            TimeMonitorRegisterI(4, AL_RESPONSE_MIN_TIME_OUT_MS, MCI_Wait_Callback);
            break;

        case RX_WAIT_APP_ACK_DLY:
            RxMsgState = RX_SEND_APP_ACK;
            rxMessageHandler(&AsyncRxRSBuf);

            RxMsgState = RX_RECEIVE_LL_ACK;
            break;

        default:
            break;
    }

}

// Unsolicted Message Timeout
// 
void Message_Timeout_Callback()
{
    if (TxMsgState != TX_IDLE)
    {
        if (AsyncTxRSBuf.numTries == TX_NUM_TRIES)
        {
        // Retries expired, back to idle
            TxMsgState = TX_IDLE;
        }
        else
        {
        // Resend
            TxMsgState = TX_WAIT_RETRY_DLY;
            TimeMonitorRegisterI(3, MESSAGE_RETRY_DLY_MS, MCI_Wait_Callback);
        }
    }

    if (RxMsgState != RX_IDLE)
    {
        RxMsgState = RX_IDLE;
    }
}
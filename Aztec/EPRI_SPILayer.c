
/**
 * File:: EPRI_SPILayer.c
 * Desc: SPI level processing functionality for EPRI project
 * Author: Jordan Murray
 * Date: 5/10/2016
 */
#ifdef DC_CEA2045

#include <string.h>

#include "HardwareProfile.h"

#include "TimeMonitor.h"

#include "INTiAPI.h"
#include "spi.h"
#include "MCI_Common.h"
#include "EPRI_SPILayer.h"

#include "debugging.h"

int OK_TO_READ_SPI = 0;
int SPI_position_counter = 0;
int SPI_numBytes = 0;
unsigned char SPIRxBuf[RX_BUF_SIZE];
unsigned char SPIrxmessage[RX_BUF_SIZE];
unsigned char SPItxmessage[TX_MSG_SIZE];
int SPIRxBytesReceived = 0;

int SPI_OverRide = 0;
int SPI_OptOutEvent = 0;
int SPI_OptInEvent = 0;
int SPI_IdleNormal;
int SPI_RunningNormal;


MCIResponse AsyncTxSPIBuf;
MCIResponse AsyncRxSPIBuf;
MCIResponse NullSPIBuf;

volatile enum _TxSPIMsgState
{
    TX_IDLE = 0,
    TX_SEND_CMD,
    TX_WAIT_LL_ACK,
    TX_WAIT_APP_ACK,
    TX_WAIT_LL_ACK_DLY,
    TX_SEND_LL_ACK,
    TX_WAIT_RETRY_DLY
} TxSPIMsgState;

volatile enum _RxSPIMsgState
{
    RX_IDLE = 0,
    RX_CMD_RECEIVED,
    RX_WAIT_LL_ACK_DLY,
    RX_SEND_LL_ACK,
    RX_WAIT_APP_ACK_DLY,
    RX_SEND_APP_ACK,
    RX_RECEIVE_LL_ACK
} RxSPIMsgState;

volatile enum _SPIBusState
{
    BUS_IDLE,
    MASTER_WAITING,
    SLAVE_WAITING,
    MASTER_READY,
    SLAVE_READY
} SPIBusState;


UINT16 MakeChecksumSPI(unsigned char * message, int len);
void MCI_Wait_Callback();
void App_Ack(unsigned char * message, int len);
/** 
 * init function
 */
void EPRI_SPI_init()
{
    
    INTiRegisterSPI3CallbackFunction(SPI3_ISR);
    
    SPIrxbuffer_initialize();
    SPI_position_counter = 0;
    OK_TO_READ_SPI = 1;
    
    NullSPIBuf.numTries = 0;
    NullSPIBuf.numBytesReceived = 0;
    NullSPIBuf.LLResponseValid = 0;
    NullSPIBuf.AppResponseValid = 0;
    
    SPI_OverRide = 0;
    SPI_IdleNormal = 0;
    SPI_RunningNormal = 0;
    
}

void EPRI_SPI_startup(void)
{
    //SpiChnOpen(SPI_CHANNEL3, SPI_CON_SLVEN | SPI_CON_MODE8 | SPI_CON_ON, 1024);
    SpiChnOpen(SPI_CHANNEL3, SPI_CONFIG_ENHBUF | SPI_CONFIG_ON, 1024);
}

void EPRI_SPI_shutdown(void)
{
    SpiChnClose(SPI_CHANNEL3);
}

/**
 * Interrupt handler for chip select assertion
 */

void __ISR(_CHANGE_NOTICE_VECTOR, ipl6) ChipSelect_ISR(void)
{
    INTClearFlag(INT_CN);
    if(SPI_CS_IO)    //transition was a rising edge
    {
        SPI_ATTN_INACTIVE
        SPIBusState = BUS_IDLE;
    }
    else
    {     
        SPI_ATTN_ASSERT
        SPIBusState = SLAVE_READY;
        
        //start SPI to prepare to receive data
        EPRI_SPI_startup();
        
    }
}


/**
 * void BusMasterCheck(void)
 * 
 * checks to see if the CTA2045 bus master is aware that a slave wants to send
 * if the chip select pin is pulled low, set the SPIBusState to MASTER_READY
 * 
 * Jordan Murray   May 17, 2016
 */
void BusMasterCheck()
{
    if(SPI_CS_IO == 0)
    {
        SPIBusState == MASTER_READY;
    }
    else
    {
        SPIBusState == SLAVE_WAITING;
    }
}



void EPRI_SPI_write(unsigned char *message, int length)
{
    //ASSERT ATTN PIN
    SPI_ATTN_ASSERT
    SPIBusState = SLAVE_WAITING;
    

    //wait for SCG to be ready - set callback in case the SCG doesn't respond
    TimeMonitorRegisterI(1,CS_ASSERT_TIMEOUT_MS, SPI_Message_Timeout_Callback);
    /* temporary comment 
    while(SPIBusState == SLAVE_WAITING)  //the timeout callback will change the state to something else
    {
        BusMasterCheck();
    }
     */
    SPIBusState = MASTER_READY;
    
    if(SPIBusState == MASTER_READY)
    {   
        SPI3BUF = 0;            //clear SPI data buffer
        
        //start SPI channel 3    
        EPRI_SPI_startup();
                
        //call function to write data
        SPI3WriteBinaryData(message, length);
         SPIBusState == BUS_IDLE;
        while(1);
        
        //stop and reset SPI channel 3
        EPRI_SPI_shutdown();
    }

        
    SPI_ATTN_INACTIVE


}

/*
 *Function:  void SPI3_ISR(void)
 *Description: ISR for SPI3
 *Creation Data:  5/11/2016
 *Author: Jordan Murray
 */
void SPI3_ISR(void)
{
    unsigned char i;
    
    //check receiver
    if(INTGetFlag(INT_SPI3RX) && INTGetEnable(INT_SPI3RX))  //something in the receive buffer?
    {
        while(!SpiChnRxBuffEmpty(SPI_CHANNEL3))        //while there is data in the RxBuffer, keep reading from the common buffer
        {
            i = SpiChnGetC(SPI_CHANNEL3);
                    if(OK_TO_READ_SPI)
                    {
                        TimeMonitorRegisterI(2, 20, SPI_MCI_Sync_Callback);
                        
                        SPIRxBuf[SPI_position_counter] = i;
                        SPIRxBuf[SPI_position_counter + 1] = '/0';
                        SPI_position_counter++;
                    }
        }
    }
}



/**
 * Construct and send an MCI packet via the UART, blocking
 * 
 */

MCIResponse MCISendSPI(unsigned char * msg)
{
    if (MCI_IsSendingSPI())
        return NullSPIBuf;
    
    MCISendAsyncSPI(msg);
    
    while(TxSPIMsgState != TX_IDLE);
    
    return AsyncTxSPIBuf;
}

/*
 * construct and send an MCI packet via SPI3, non-blocking
 * call is non-blocking with timeouts specified in MCI 2.0 spec
 * 
 */
void MCISendAsyncSPI(unsigned char * msg)
{
    int i = 0;
    
    if(RxSPIMsgState != RX_IDLE)
        return;
    
    if(TxSPIMsgState == TX_IDLE)
    {
        //first try - clear buffer
        AsyncTxSPIBuf.numTries = 1;
        AsyncTxSPIBuf.numBytesReceived = 0;
        AsyncTxSPIBuf.LLResponseValid = 0;
        AsyncTxSPIBuf.AppResponseValid = 0;
    }
    else
        AsyncTxSPIBuf.numTries++;
    
    TxSPIMsgState = TX_SEND_CMD;
    
    //2nd two bytes of message are length of payload
    int payloadLen = msg[2] * 256 + msg[3];
    
    for(i = 0; i< payloadLen+4; i++)
    {
        SPItxmessage[i] = msg[i];
    }
    //add 4 to payload length to get message length
    MakeChecksumSPI(SPItxmessage, (payloadLen + 4));
    
    //send initial message to start Tx state machine
    EPRI_SPI_write(SPItxmessage, (payloadLen + 6));
    TimeMonitorRegisterI(1,INTER_MESSAGE_TIMEOUT_MS, SPI_Message_Timeout_Callback);
    
    TxSPIMsgState = TX_WAIT_LL_ACK;
}

/*
 * Check to see if transmit is in progress
 * 
 */
BOOL MCI_IsSendingSPI()
{
    if(TxSPIMsgState == TX_IDLE)
        return FALSE;
    else
        return TRUE;
}


/*
 * Send a time sync to SGD, non-blocking
 * 
 */
BOOL SendTimeSyncSPI(int weekday, int hour)
{
    if(MCI_IsSendingSPI())
        return FALSE;
    
    TimeSyncMsg[5] = (weekday << 4) + hour;
    MCISendAsyncSPI(TimeSyncMsg);
    
    return TRUE;
}

/**
 * Check the checksum of a received message.
 *
 * @param message[] the entire message, including checksum bits.
 * @param len the length of the string.
 * @return 1 for good checksum, 0 for bad checksum
 */

int ChecksumDecodeSPI(unsigned char * message, int len)
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
UINT16 MakeChecksumSPI(unsigned char * message, int len)
{
    // see MCI-V1-6.pdf page 68 for explanation
    //putsUART("in MakeChecksumSPI \r\n");

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
void SPIrxMessageHandler(MCIResponse * lastSentPacket)
{
    int len = SPIrxmessage[2] * 256;
    len += SPIrxmessage[3];
    len += 6;
    
    if((TxSPIMsgState != TX_IDLE) && !SPIRxBytesReceived)  //timeout in MCISend
    {
        lastSentPacket->numBytesReceived = 0;
        lastSentPacket->LLResponseValid = 0;
        return;
    }
    
    //check for data link ack or nak (we don't ack acks)
    if(SPIrxmessage[0] == 0x06 && SPIrxmessage[1] == 0x00)
    {
        lastSentPacket->numBytesReceived = SPI_numBytes;
        lastSentPacket->LLResponse[0] = 0x06;
        lastSentPacket->LLResponse[1] = 0x00;
        lastSentPacket->LLResponse[2] = '\0';
        lastSentPacket->LLResponseValid = 1;
    }
    
    //nack code is 0x15
    else if(SPIrxmessage[0] == 0x15 && SPIrxmessage[2] == 0)
    {
        lastSentPacket->numBytesReceived = SPI_numBytes;
        lastSentPacket->LLResponse[0] = SPIrxmessage[0];
        lastSentPacket->LLResponse[1] = SPIrxmessage[1];
        lastSentPacket->LLResponse[2] = SPIrxmessage[2];
        lastSentPacket->LLResponseValid = '\0';
        lastSentPacket->LLResponseValid = 1;
    }
    
    //if it wasn't a data link message...
    else
    {
        //check the checksum
        if(ChecksumDecodeSPI(SPIrxmessage,len) == 0) //bad checksum
        {
            DL_Nak();
        }
        else                                      //good checksum
        {
            if(SPIrxmessage[0] < 0xF0 && (TxSPIMsgState != TX_IDLE))
            {
                int y;
                for(y = 0; y < 11; y++)
                {
                    lastSentPacket->AppResponse[y] = SPIrxmessage[y];
                }
                lastSentPacket->numBytesReceived = SPI_numBytes;
                lastSentPacket->AppResponseValid = 1;
                
                //data link ack
                DL_Ack();
                
                //stats for PGE hourly Load Factor
                //SPI_IdleNormal & SPI_RunningNormal cleared hourly by MCI_One_Second_Callback()
                if(SPIrxmessage[4] == OPSTATE_OPCODE1)
                {
                    if(SPIrxmessage[5] == IDLE_NORMAL)
                        SPI_IdleNormal++;
                    else if (SPIrxmessage[5] == RUNNING_NORMAL)
                        SPI_RunningNormal++;
                }
                
            }
            else if(RxSPIMsgState == RX_SEND_LL_ACK)
            {
                DL_Ack();
            }
            else if(RxSPIMsgState == RX_SEND_APP_ACK)
            {
                if(SPIrxmessage[0] < 0xF0)
                {
                    //per Chuck Thomas use override as defined in CTA-2045 draft V08
                    //SPI_OptOutEvent and SPI_OptInEvent get set on change of state and
                    //get cleared in OpenADRClient.c
                    if(SPIrxmessage[4] == OVERRIDE_OPCODE1)
                    {
                        if(!SPI_OverRide && SPIrxmessage[5])
                            SPI_OptOutEvent = 1;
                        else if(SPI_OverRide && !SPIrxmessage[5])
                            SPI_OptInEvent = 1;
                        
                        SPI_OverRide = (int) SPIrxmessage[5];
                    }
                    
                    AppAckMsg[2] = 0x00;
                    AppAckMsg[3] = 0x02;
                    AppAckMsg[4] = 0x03;
                    AppAckMsg[5] = SPIrxmessage[4];
                    MakeChecksumSPI(AppAckMsg, 6);
                    
                    App_Ack(AppAckMsg, 8);                    
                }
                else if(SPIrxmessage[0] == INTWINE_MSG)
                {
                    //EPRI_Config(SPIrxmessage,len);
                }
            }
        }
    }
    
    SPI_numBytes = 0;
    SPIRxBytesReceived = 0;
    return;
}

//initialize_buffer
void SPIrxbuffer_initialize()
{
    int i;
    
    for(i = 0; i < RX_BUF_SIZE; i++)
    {
        SPIRxBuf[i] = 0;
    }
}

/**
 * Message Sync Indicator
 * Indicates that a message receive is complete by 20ms
 * callback initiated from UART2InterruptServiceRoutine.
 * SPIRxBuf copied into SPIrxmessage as double-buffered.
 * Must be done processing SPIrxmessage before next callback.
 */
void SPI_MCI_Sync_Callback()
{
    int i;
    
    SPI_numBytes = SPI_position_counter;
    for(i = 0; i < SPI_position_counter; i++)
    {
        SPIrxmessage[i] = SPIRxBuf[i];
        SPIRxBuf[i] = 0;
    }
    SPIrxmessage[i] = 0;
    
    SPI_numBytes = SPI_position_counter;
    SPI_position_counter = 0;
    SPIRxBytesReceived = 1;
    
    switch(TxSPIMsgState)
    {
        case TX_WAIT_LL_ACK:
            TimeMonitorCancelI(1);             //Cancel SPI_Message_Timeout_Callback
            SPIrxMessageHandler(&AsyncTxSPIBuf);
            if(AsyncTxSPIBuf.LLResponseValid && AsyncTxSPIBuf.LLResponse[0] == 0x06 && AsyncTxSPIBuf.LLResponse[1] == 0x00)
            //link layer ACK OK. Wait for application ack
            {
                TxSPIMsgState = TX_WAIT_APP_ACK;
                TimeMonitorRegisterI(1, APP_ACK_WAIT_TIME_MS, SPI_Message_Timeout_Callback);
            }
            else //LL NAK or worse. Set quick timeout for end of message clean up
            {
                TimeMonitorRegisterI(1,9,SPI_Message_Timeout_Callback);
            }
            break;
        case TX_WAIT_APP_ACK:
            TimeMonitorCancelI(1);         // cancel timeout callback
            TxSPIMsgState = TX_WAIT_LL_ACK_DLY;
            TimeMonitorRegisterI(3, LINK_ACK_DELAY_MS, SPI_MCI_Wait_Callback);
            break;
        default:
            break;
    }
    
    if(TxSPIMsgState == TX_IDLE)
    {
        switch(RxSPIMsgState)
        {
            case RX_IDLE:
                AsyncRxSPIBuf.numBytesReceived = 0;
                AsyncRxSPIBuf.LLResponseValid = 0;
                AsyncRxSPIBuf.AppResponseValid = 0;
                
                RxSPIMsgState = RX_CMD_RECEIVED;
                SPIrxMessageHandler(&AsyncRxSPIBuf);    // send ACK/NAK
                
                RxSPIMsgState = RX_WAIT_LL_ACK_DLY;
                TimeMonitorRegisterI(1, APP_ACK_WAIT_TIME_MS, SPI_Message_Timeout_Callback);
                TimeMonitorRegisterI(3, LINK_ACK_DELAY_MS, SPI_MCI_Wait_Callback);
                break;
            case RX_RECEIVE_LL_ACK:
                TimeMonitorCancelI(1);          //Cancel message timeout callback
                RxSPIMsgState = RX_IDLE;
                break;
            default:
                break;                       
        }
    }
    return;
}

//MCI delayed callback. Used for delaying LL ACK/NAK and App ACK/NAK
void SPI_MCI_Wait_Callback()
{
    switch(TxSPIMsgState)
    {
        case TX_WAIT_LL_ACK_DLY:
            TxSPIMsgState = TX_SEND_LL_ACK;
            SPIrxMessageHandler(&AsyncTxSPIBuf);   //ACK/NAK sent from here
            TxSPIMsgState = TX_IDLE;
            break;
        case TX_WAIT_RETRY_DLY:
            MCISendAsyncSPI(SPItxmessage);
            break;
        default:
            break;
    }
    
    switch(RxSPIMsgState)
    {
        case RX_WAIT_LL_ACK_DLY:
            RxSPIMsgState = RX_SEND_LL_ACK;
            SPIrxMessageHandler(&AsyncRxSPIBuf);
            
            RxSPIMsgState = RX_WAIT_APP_ACK_DLY;
            TimeMonitorRegisterI(4, APP_ACK_WAIT_TIME_MS, SPI_MCI_Wait_Callback);
            break;
        case RX_WAIT_APP_ACK_DLY:
            RxSPIMsgState = RX_SEND_APP_ACK;
            SPIrxMessageHandler(&AsyncRxSPIBuf);
            
            RxSPIMsgState = RX_RECEIVE_LL_ACK;
            break;
        default:
            break;
    }
}

//unsolicited message timeout callback
void SPI_Message_Timeout_Callback()
{
    if(TxSPIMsgState != TX_IDLE)
    {
        if(AsyncTxSPIBuf.numTries == TX_NUM_TRIES)
        {
            //retries expired, back to idle
            TxSPIMsgState = TX_IDLE;
        }
        else
        {
            //resend
            TxSPIMsgState = TX_WAIT_RETRY_DLY;
            TimeMonitorRegisterI(3, MESSAGE_RETRY_DELAY_MS, SPI_MCI_Wait_Callback);            
        }
    }
    
    if(RxSPIMsgState != RX_IDLE)
    {
        RxSPIMsgState = RX_IDLE;
    }
    
    if(SPIBusState != BUS_IDLE)
    {
        SPIBusState == BUS_IDLE;
    }
}



SPI3WriteBinaryData(unsigned char *data_block, int length)
{
    while(length)
    {
        while(SpiChnIsBusy(3));               //wait until SPI is ready
        SpiChnPutC(3, *data_block++);
        while(SpiChnIsBusy(3));               //block until send is finished
        length--;
    }
}

#endif
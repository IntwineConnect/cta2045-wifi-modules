
/**
 * File:: EPRI_SPILayer.c
 * Desc: SPI level processing functionality for EPRI project
 * Author: Jordan Murray
 * Date: 5/10/2016
 */
#if defined(DC_CEA2045)

#include "EPRI_SPILayer.h"
#include <string.h>

#include <xc.h>
#include <sys/attribs.h>

#include "HardwareProfile.h"

#include "TimeMonitor.h"

#define RX_BUF_SIZE 300
#define TX_MSG_SIZE 30

//#include "EPRI_SPILayer.h"

#include "plib.h"
#include "MCI_Common.h"
#include "BasicDR.h"
//#include "Compiler.h"

#define MESSAGE_TYPE_LENGTH 2 
#define PAYLOAD_LENGTH_LENGTH 2
#define CHECKSUM_LENGTH 2


enum PhysicalLayerStateEnum
{
    PHY_LAYER_INIT = 0,
    PHY_LAYER_IDLE,
    PHY_LAYER_RECEIVE,
    PHY_LAYER_TRANSMIT,
    PHY_LAYER_TRANSMIT_WAIT_TO_DEASSERT
};
typedef enum PhysicalLayerStateEnum PhysicalLayerStateEnum;
typedef struct _PhysicalLayerInternals {
    volatile PhysicalLayerStateEnum state;
    volatile PhysicalLayerStateEnum nextState;
    volatile PhysicalLayerStateEnum lastState;
    volatile int rxRequest;     
    volatile int rxLinkLayerDone;
    volatile int rxComplete;    
    volatile int txRequest;     
    volatile int txLinkLayerDone; 
    volatile int txComplete;   
    volatile int txTimeout;
    volatile int timerTimeout;  
    volatile int txLastByteLoaded;
    volatile int txLastByteTransferred;
} PhysicalLayerInternals;
volatile PhysicalLayerInternals physicalLayerInternals;

// Handy defines to keep the state machine code clean-ish
#define ENTERING_SPI_PHYSICAL_LAYER_TASK_STATE (physicalLayerInternals.state != physicalLayerInternals.lastState)
#define EXITING_SPI_PHYSICAL_LAYER_TASK_STATE (physicalLayerInternals.nextState != physicalLayerInternals.state)

// Handshakes for Physical_Link_Layer_Task
volatile int SPI_CS_RISING_EDGE = 0;         // Always written 1 form the change notification ISR; always read and reset to 0 from SPI_Physical_Layer_Task

#define SPI_PHYSICAL_LAYER_TIME_MONITOR_IDX 12
void SPI_Physical_Layer_Timeout_Callback(){
    physicalLayerInternals.timerTimeout = 1;
}
#define SPI_START_PHYSICAL_LAYER_TIMER(timeout) { TimeMonitorDisableInterrupt(); physicalLayerInternals.timerTimeout = 0; TimeMonitorRegisterI(SPI_PHYSICAL_LAYER_TIME_MONITOR_IDX,timeout,SPI_Physical_Layer_Timeout_Callback); }
#define SPI_CANCEL_PHYSICAL_LAYER_TIMER { TimeMonitorCancelI(SPI_PHYSICAL_LAYER_TIME_MONITOR_IDX); }

enum LinkLayerStateEnum
{
    LINK_LAYER_INIT = 0,                // Valid transitions out: LINK_LAYER_IDLE
    LINK_LAYER_IDLE,                    // Valid transitions out: LINK_LAYER_RECEIVE_MESSAGE_TYPE, LINK_LAYER_TRANSMIT
    LINK_LAYER_RECEIVE_MESSAGE_TYPE,    // Valid transitions out: LINK_LAYER_RECEIVE_PAYLOAD_LENGTH, LINK_LAYER_TIMEOUT
    LINK_LAYER_RECEIVE_PAYLOAD_LENGTH,  // Valid transitions out: LINK_LAYER_RECEIVE_PAYLOAD, LINK_LAYER_TIMEOUT
    LINK_LAYER_RECEIVE_PAYLOAD,         // Valid transitions out: LINK_LAYER_RECEIVE_CHECKSUM, LINK_LAYER_TIMEOUT
    LINK_LAYER_RECEIVE_CHECKSUM,        // Valid transitions out: LINK_LAYER_RECEIVE_DELAY_ACK_NAK, LINK_LAYER_TIMEOUT
    LINK_LAYER_RECEIVE_DELAY_ACK_NAK,        // Valid transitions out: LINK_LAYER_TIMEOUT
    LINK_LAYER_RECEIVE_REPLY_ACK_NAK,
    LINK_LAYER_TRANSMIT_MESSAGE,
    LINK_LAYER_TRANSMIT_WAIT_FOR_ACK_NAK,
    LINK_LAYER_TRANSMIT_WAIT_FOR_RETRY_TIMER,
    LINK_LAYER_TIMEOUT
};
typedef enum LinkLayerStateEnum LinkLayerStateEnum;

typedef struct _LinkLayerInternals {
   volatile LinkLayerStateEnum state;
   volatile LinkLayerStateEnum nextState;
   volatile LinkLayerStateEnum lastState;
   volatile int enableReceivingBytes;
   volatile int rxBytePosition;
   volatile unsigned char volatile rxMessage[RX_BUF_SIZE];
   volatile int parsedRxMessageLength;
   volatile int checksumValid;
   volatile int txBytePosition;
   volatile unsigned char volatile txMessage[TX_MSG_SIZE];
   volatile int txMessageLength;
   volatile int txMessageResponseReceived;
   volatile int txMessageRetries;
   volatile int txMessageRetryCounter;
   volatile LinkLayerStateEnum txMessageRetryTargetState;
   volatile int timerTimeout;
   volatile int linkLayerReplyTimeout;  
} LinkLayerInternals;                  // This struct contains state variables that persist from call to call of Link_Layer_Task
volatile LinkLayerInternals linkLayerInternals; // The only code that ever writes and updates this variable is Link_Layer_Task and the SPI RX ISR

// Handy defines to keep the state machine code clean-ish
#define ENTERING_SPI_LINK_LAYER_TASK_STATE (linkLayerInternals.state != linkLayerInternals.lastState)
#define EXITING_SPI_LINK_LAYER_TASK_STATE (linkLayerInternals.nextState != linkLayerInternals.state)

// Handshakes for SPI_Link_Layer_Task
volatile int SGD_TO_UCM_MESSAGE_REQUEST = 0; // Always written 1 from change notification ISR; always read and reset to 0 from SPI_Link_Layer_Task
volatile int UCM_TO_SGD_MESSAGE_REQUEST = 0; // Always written 1 from MCISendAsyncSPI; always read and reset to 0 from SPI_Link_Layer_Task
volatile int UCM_TO_SGD_ACK_NAK_REQUEST = 0; // Always written 1 from Link_Layer_Send_ACK_NAK; always reset to 0 from SPI_Link_Layer_Task

#define SPI_LINK_LAYER_TIME_MONITOR_IDX 11
void SPI_Link_Layer_Timeout_Callback(){
    linkLayerInternals.timerTimeout = 1;
}
#define SPI_START_LINK_LAYER_TIMER(timeout) { TimeMonitorDisableInterrupt(); linkLayerInternals.timerTimeout = 0; TimeMonitorRegisterI(SPI_LINK_LAYER_TIME_MONITOR_IDX,timeout,SPI_Link_Layer_Timeout_Callback); }
#define SPI_CANCEL_LINK_LAYER_TIMER { TimeMonitorCancelI(SPI_LINK_LAYER_TIME_MONITOR_IDX); }

#define SPI_LINK_LAYER_REPLY_TIME_MONITOR_IDX 13
void SPI_Link_Layer_Reply_Timeout_Callback(){
    linkLayerInternals.linkLayerReplyTimeout = 1;
}
#define SPI_START_LINK_LAYER_REPLY_TIMER(timeout) { TimeMonitorDisableInterrupt(); linkLayerInternals.linkLayerReplyTimeout = 0; TimeMonitorRegisterI(SPI_LINK_LAYER_REPLY_TIME_MONITOR_IDX,timeout,SPI_Link_Layer_Reply_Timeout_Callback); }
#define SPI_CANCEL_LINK_LAYER_REPLY_TIMER { TimeMonitorCancelI(SPI_LINK_LAYER_REPLY_TIME_MONITOR_IDX); }

#define TIME_MIN_WAIT_TIME_TO_SEND_ACK_NAK_IN_MS 40                 // T_ma, min
#define TIME_MAX_WAIT_TIME_FOR_ACK_NAK_IN_MS 500                    // T_ma, max
#define TIME_MIN_WAIT_TIME_TO_SEND_RESPONSE_AFTER_ACK_NAK_IN_MS 100 // T_ar, min
#define TIME_MIN_WAIT_TIME_TO_SEND_ANOTHER_MESSAGE_IN_MS 100        // T_im, min
#define TIME_MAX_MESSAGE_DURATION_IN_MS 500                         // T_ml, max
#define TIME_MAX_WAIT_TIME_FOR_PARTNER_IN_MS 100                    // Per 15.3.1.9

/**
 * Interrupt handler for chip select assertion
 */
void __ISR(_CHANGE_NOTICE_VECTOR, ipl6) ChipSelect_ISR(void)
{ 
    if(INTGetFlag(INT_CN) && INTGetEnable(INT_CN)){
        mCNClearIntFlag();
        if(SPI_CS_IO)
        {
            // If we se CS go high (this was a low to high transition) and 
            // we're currently receiving or transmitting, disable the SPI TX
            // and RX interrupts and handshake this info to the physical
            // layer state machine
            if(physicalLayerInternals.state == PHY_LAYER_RECEIVE) {
                INTEnable(INT_SOURCE_SPI_TX(SPI_CHANNEL3), INT_DISABLED);
                INTClearFlag(INT_SOURCE_SPI_TX(SPI_CHANNEL3));
            } else if(physicalLayerInternals.state == PHY_LAYER_TRANSMIT) {
                INTEnable(INT_SOURCE_SPI_RX(SPI_CHANNEL3), INT_DISABLED);
                INTClearFlag(INT_SOURCE_SPI_RX(SPI_CHANNEL3));
            }
            SPI_CS_RISING_EDGE = 1;            
        }
        else
        {
            // CS was asserted - this means the SGD wants to send us a message
            if( (linkLayerInternals.state == LINK_LAYER_IDLE || linkLayerInternals.state == LINK_LAYER_TRANSMIT_WAIT_FOR_ACK_NAK) && physicalLayerInternals.state == PHY_LAYER_IDLE){
                SGD_TO_UCM_MESSAGE_REQUEST = 1;
            }
        }
    }
}

void SPI3_RX_ISR(void)
{
    unsigned char i;
    if(INTGetFlag(INT_SPI3RX) && INTGetEnable(INT_SPI3RX))
    {   
        if(physicalLayerInternals.state == PHY_LAYER_RECEIVE){
            while( SPI3STAT & 0x1 ){
                // Always read the byte, even if we don't care about it (or
                // can't fit it in the buffer) - this helps avoid SPI RX error
                // interrupts.
                i = SPI3BUF;
                if(linkLayerInternals.rxBytePosition + 1 < RX_BUF_SIZE){
                    linkLayerInternals.rxMessage[linkLayerInternals.rxBytePosition] = i;
                    linkLayerInternals.rxBytePosition += 1;
                    linkLayerInternals.rxMessage[linkLayerInternals.rxBytePosition] = '/0';
                }
            }
        }
    } 
}

void SPI3_TX_ISR(void)
{
    if(INTGetFlag(INT_SPI3TX) && INTGetEnable(INT_SPI3TX))  //something in the receive buffer?
    {   
        if(physicalLayerInternals.state == PHY_LAYER_TRANSMIT){
            if(!physicalLayerInternals.txLastByteLoaded &&
               linkLayerInternals.txBytePosition < linkLayerInternals.txMessageLength && 
               ((SPI3STAT >> 3) & 0x1)){      // in enhanced buffer mode, SPI3STAT[1] will be 0 when there is room in buffer, 1 when the buffer is full
                if(linkLayerInternals.txBytePosition < TX_MSG_SIZE){
                    SPI3BUF = linkLayerInternals.txMessage[linkLayerInternals.txBytePosition++];
                }
            } else if(physicalLayerInternals.txLastByteLoaded){
                // This is the interrupt after we transmitted the very last byte. 
                // We're completely finished transmitting data.
                INTEnable(INT_SOURCE_SPI_TX(SPI_CHANNEL3), INT_DISABLED);
                physicalLayerInternals.txLastByteTransferred = 1;
            }
            if(linkLayerInternals.txBytePosition >= linkLayerInternals.txMessageLength) {
                // We're completely finished loading data for transfer.
                // Not necessarily done transmitting the last byte of the message!
                physicalLayerInternals.txLastByteLoaded = 1;
            }
            
        }
    }
}

int SPI_Physical_Layer_Task(void){ 
    unsigned char throwAway;
    int continueProcessing;

    // By default, the next state is the current state. This avoids duplicating this line of code in every state.
    physicalLayerInternals.nextState = physicalLayerInternals.state;

    switch(physicalLayerInternals.state){
        case PHY_LAYER_INIT:
            // Initialize pins and peripherals
            // * Set SCLK, Input
            
            SPI_MISO_TRIS = 0;  // MISO is SDO, Output
            SPI_MOSI_TRIS = 1;  // MOSI is SDI, Input
            SPI_SCLK_TRIS = 1;  // SCLK is Input
            SPI_CS_TRIS = 1;    // CS is input

            SPI_ATTN_INACTIVE   // ATTENTION is three-state. INACTIVE deasserts and tristates the line.

            // * Register ISRs
            INTiRegisterSPI3RxCallbackFunction(SPI3_RX_ISR);
            INTiRegisterSPI3TxCallbackFunction(SPI3_TX_ISR);

            SpiChnEnable(3, 0);
            SpiChnConfigure(3, SPI_CONFIG_SLVEN|SPI_CONFIG_CKE_REV);
            
            INTEnable(INT_SOURCE_SPI_RX(SPI_CHANNEL3), INT_DISABLED);
            INTEnable(INT_SOURCE_SPI_RX(SPI_CHANNEL3), INT_DISABLED);

            physicalLayerInternals.rxRequest = 0;
            physicalLayerInternals.txRequest = 0;
            
            SPI_CANCEL_PHYSICAL_LAYER_TIMER

            physicalLayerInternals.nextState = PHY_LAYER_IDLE;
            break;
        case PHY_LAYER_IDLE:
            // Wait for a RX interrupt or a TX command                
            if(physicalLayerInternals.rxRequest){
                physicalLayerInternals.rxComplete = 0;
                physicalLayerInternals.rxLinkLayerDone = 0;
                linkLayerInternals.rxBytePosition = 0;
                SPI_CS_RISING_EDGE = 0;
                physicalLayerInternals.nextState = PHY_LAYER_RECEIVE;
            }

            if(physicalLayerInternals.txRequest){
                physicalLayerInternals.txComplete = 0;
                physicalLayerInternals.txLastByteLoaded = 0;
                physicalLayerInternals.txLastByteTransferred = 0;
                physicalLayerInternals.txTimeout = 0;
                physicalLayerInternals.txLinkLayerDone = 0;
                linkLayerInternals.txBytePosition = 0;
                SPI_CS_RISING_EDGE = 0;
                physicalLayerInternals.nextState = PHY_LAYER_TRANSMIT;
            }

            break;
        case PHY_LAYER_TRANSMIT:
            // Precondition: Link Layer wants to send a message
            if(ENTERING_SPI_PHYSICAL_LAYER_TASK_STATE){
                physicalLayerInternals.txRequest = 0;
                SPI3STATCLR = 1 << 8;
                SPI3STATCLR = 1 << 6;
//                    IEC0SET = BIT_28;
                SpiChnEnable(3, 1);
                SPI3BUF = linkLayerInternals.txMessage[linkLayerInternals.txBytePosition++];                
                INTEnable(INT_SOURCE_SPI_TX(SPI_CHANNEL3), INT_ENABLED);
                SPI_ATTN_ASSERT
                SPI_START_PHYSICAL_LAYER_TIMER(TIME_MAX_WAIT_TIME_FOR_PARTNER_IN_MS)
            }

            // We don't do anything here - the SPI TX interrupt handler will be pulling bytes from the Link Layer until the entire message is transmitted...

            if(!SPI_CS_IO){
                // Our partner asserted chip select - disable the timer
                SPI_CANCEL_PHYSICAL_LAYER_TIMER
            }

            if(physicalLayerInternals.timerTimeout && SPI_CS_IO){
                // Timed out waiting for our partner to assert CS
                physicalLayerInternals.txTimeout = 1;
                // Link layer state machine must assert txLinkLayerDone and we'll go back to idle below
            }

            // Exit condition: Link Layer has transmitted the entire message
            // Exit condition: SGD deasserts CS
            if(physicalLayerInternals.txLastByteTransferred || SPI_CS_RISING_EDGE){ 
                physicalLayerInternals.nextState = PHY_LAYER_TRANSMIT_WAIT_TO_DEASSERT;
            }
                
            if(physicalLayerInternals.txLinkLayerDone){
                // We were abnormally canceled from transmitting from the link
                // layer. Probably due to a link layer timeout.
                physicalLayerInternals.nextState = PHY_LAYER_IDLE;
                physicalLayerInternals.txComplete = 1;
            }

            if(EXITING_SPI_PHYSICAL_LAYER_TASK_STATE && physicalLayerInternals.nextState == PHY_LAYER_IDLE){
                SPI_ATTN_INACTIVE
                SpiChnEnable(3, 0);
                INTEnable(INT_SOURCE_SPI_TX(SPI_CHANNEL3), INT_DISABLED);
            }
            break;
        case PHY_LAYER_TRANSMIT_WAIT_TO_DEASSERT:
            if(ENTERING_SPI_PHYSICAL_LAYER_TASK_STATE){
                // 19.2 kbps is roughly 2 bytes per millisecond
                // We have to wait for one full byte to transfer at the slowest
                // speed of 19.2 kbps.
                // To be safe, I choose a delay of 2ms
                SPI_START_PHYSICAL_LAYER_TIMER(2)
            }
            
            if(physicalLayerInternals.timerTimeout){
                physicalLayerInternals.nextState = PHY_LAYER_IDLE;
                physicalLayerInternals.txComplete = 1;
            }
            
            if(EXITING_SPI_PHYSICAL_LAYER_TASK_STATE){
                SPI_ATTN_INACTIVE
                SpiChnEnable(3, 0);
                INTEnable(INT_SOURCE_SPI_TX(SPI_CHANNEL3), INT_DISABLED);
            }
            break;
        case PHY_LAYER_RECEIVE:
            // Precondition: CS was asserted by SGD
            if(ENTERING_SPI_PHYSICAL_LAYER_TASK_STATE){
                physicalLayerInternals.rxRequest = 0; // This acknowledges the RX request
                SPI3STATCLR = 1 << 8;
                SPI3STATCLR = 1 << 6;
                while((SPI3STAT >> 0) & 0x1){
                    throwAway = SPI3BUF;
                }
                INTEnable(INT_SOURCE_SPI_RX(SPI_CHANNEL3), INT_ENABLED);
                SpiChnEnable(3, 1);
                SPI_ATTN_ASSERT
            }

            // We don't do anything here - the SPI RX interrupt handler will be pushing bytes to the Link Layer until the Link Layer has received a full message...

            // Exit condition: Link Layer has received a the entire message
            if(physicalLayerInternals.rxLinkLayerDone){
                physicalLayerInternals.nextState = PHY_LAYER_IDLE;
                physicalLayerInternals.rxComplete = 1;
            }

            // Exit condition: SGD deasserts CS
            if(SPI_CS_RISING_EDGE){
                physicalLayerInternals.nextState = PHY_LAYER_IDLE;
                physicalLayerInternals.rxComplete = 1;
            }

            if(EXITING_SPI_PHYSICAL_LAYER_TASK_STATE){
                SPI_ATTN_INACTIVE
                SpiChnEnable(3, 0);
                INTEnable(INT_SOURCE_SPI_RX(SPI_CHANNEL3), INT_DISABLED);
            }
            break;
        default:
            Nop();
            break;
    }
    continueProcessing = physicalLayerInternals.state != physicalLayerInternals.nextState;
    physicalLayerInternals.lastState = physicalLayerInternals.state;
    physicalLayerInternals.state = physicalLayerInternals.nextState;
    
    return continueProcessing;
}

/**
 * Check the checksum of a received message.
 *
 * @param message[] the entire message, including checksum bits.
 * @param len the length of the string.
 * @return 1 for good checksum, 0 for bad checksum
 */
int IsChecksumValid(unsigned char * message, int len)
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

int SPI_Link_Layer_Task(void){
    int continueProcessing;
    
    // By default, the next state is the current state. This avoids duplicating this line of code in every state.
    linkLayerInternals.nextState = linkLayerInternals.state;

    switch(linkLayerInternals.state){
        case LINK_LAYER_INIT:
            // * Init variables
            SGD_TO_UCM_MESSAGE_REQUEST = 0;
            UCM_TO_SGD_MESSAGE_REQUEST = 0;
            UCM_TO_SGD_ACK_NAK_REQUEST = 0;

            linkLayerInternals.rxBytePosition = 0;            
            linkLayerInternals.txBytePosition = 0;
            linkLayerInternals.txMessageRetries = 3;

            SPI_CANCEL_LINK_LAYER_TIMER
            
            linkLayerInternals.nextState = LINK_LAYER_IDLE;
            break;
        case LINK_LAYER_IDLE:
            // Exit condition: MCISendAsyncSPI has been called and has a message to transmit
            if(UCM_TO_SGD_MESSAGE_REQUEST == 1 && physicalLayerInternals.state == PHY_LAYER_IDLE){
                linkLayerInternals.nextState = LINK_LAYER_TRANSMIT_MESSAGE;
                linkLayerInternals.txMessageRetryTargetState = LINK_LAYER_TRANSMIT_MESSAGE;
                linkLayerInternals.txMessageRetryCounter = linkLayerInternals.txMessageRetries;
            }

            // Exit condition: The port change ISR indicates that the SGD wants to send us a message
            // Or we got back to this case and CS was still low...
            if(SGD_TO_UCM_MESSAGE_REQUEST == 1 && physicalLayerInternals.state == PHY_LAYER_IDLE){
                linkLayerInternals.nextState = LINK_LAYER_RECEIVE_MESSAGE_TYPE;
            }

            // Exit condition: the app layer wants to send an ACK / NAK
            if(UCM_TO_SGD_ACK_NAK_REQUEST == 1 && physicalLayerInternals.state == PHY_LAYER_IDLE){
                linkLayerInternals.nextState = LINK_LAYER_RECEIVE_REPLY_ACK_NAK;
                linkLayerInternals.txMessageRetryTargetState = LINK_LAYER_RECEIVE_REPLY_ACK_NAK;
                linkLayerInternals.txMessageRetryCounter = linkLayerInternals.txMessageRetries;
            }

            break;
        case LINK_LAYER_RECEIVE_MESSAGE_TYPE:
            if(ENTERING_SPI_LINK_LAYER_TASK_STATE){
                SGD_TO_UCM_MESSAGE_REQUEST = 0;
                linkLayerInternals.rxBytePosition = 0;
                linkLayerInternals.parsedRxMessageLength = 0;
                linkLayerInternals.checksumValid = 0;
                physicalLayerInternals.rxRequest = 1;
                
                // Note that all future read states depend on this timer being
                // set. Be careful if you change it or remove it.
                SPI_START_LINK_LAYER_TIMER(TIME_MAX_MESSAGE_DURATION_IN_MS)
            }

            // Exit condition: we have received the message type
            //      path 1: the message type is an ACK/NAK, go to idle
            //      path 2: go to LINK_LAYER_RECEIVE_PAYLOAD_LENGTH
            if(linkLayerInternals.rxBytePosition >= MESSAGE_TYPE_LENGTH){
                // Did a wild ACK / NAK appear? This really shouldn't happen, 
                // but we'll be relaxed for our SGD and just go back to idle, 
                // pretending it never happened.
                if(linkLayerInternals.rxMessage[0] == 0x06 && linkLayerInternals.rxMessage[1] == 0x00){
                    // ACK
                    linkLayerInternals.txMessageResponseReceived = 1;
                    linkLayerInternals.nextState = LINK_LAYER_IDLE;
                }else if(linkLayerInternals.rxMessage[0] == 0x15){
                    // NAK
                    linkLayerInternals.txMessageResponseReceived = 1;
                    linkLayerInternals.nextState = LINK_LAYER_IDLE;
                } else {
                    // Message type received, carry on.
                    linkLayerInternals.nextState = LINK_LAYER_RECEIVE_PAYLOAD_LENGTH;
                }
            }

            // Exit condition: timeout
            //      path 1: go to LINK_LAYER_ERROR
            if(linkLayerInternals.timerTimeout){
                linkLayerInternals.nextState = LINK_LAYER_TIMEOUT;
            }

            if(EXITING_SPI_LINK_LAYER_TASK_STATE && (linkLayerInternals.nextState == LINK_LAYER_IDLE || linkLayerInternals.nextState == LINK_LAYER_TIMEOUT)){
                physicalLayerInternals.rxLinkLayerDone = 1;
            }

            break;
        case LINK_LAYER_RECEIVE_PAYLOAD_LENGTH:  // Valid transitions out: LINK_LAYER_RECEIVE_PAYLOAD, LINK_LAYER_TIMEOUT
            // This state can timeout if the timer that was set in 
            // LINK_LAYER_RECEIVE_MESSAGE_TYPE lapses
            
            // Exit condition: we have received the message length
            //      path 1: the length is too long for us to support, go send a NAK
            //      path 2: go to LINK_LAYER_RECEIVE_PAYLOAD
            if(linkLayerInternals.rxBytePosition >= MESSAGE_TYPE_LENGTH + PAYLOAD_LENGTH_LENGTH){
                linkLayerInternals.parsedRxMessageLength = ((linkLayerInternals.rxMessage[2] << 8) | linkLayerInternals.rxMessage[3]) & 0x1FFF; // Only the bottom 13 bits of the payload length are valid
                if(linkLayerInternals.parsedRxMessageLength > RX_BUF_SIZE){
                    linkLayerInternals.txMessage[0] = 0x15; // NAK
                    linkLayerInternals.txMessage[1] = 0x02; // Invalid length
                    linkLayerInternals.txMessageLength = 2;
                    linkLayerInternals.nextState = LINK_LAYER_RECEIVE_DELAY_ACK_NAK;
                } else {
                    linkLayerInternals.nextState = LINK_LAYER_RECEIVE_PAYLOAD;
                }
            }

            // Exit condition: timeout
            //      path 1: go to LINK_LAYER_ERROR
            if(linkLayerInternals.timerTimeout){
                linkLayerInternals.nextState = LINK_LAYER_TIMEOUT;
            }

            if(EXITING_SPI_LINK_LAYER_TASK_STATE && (linkLayerInternals.nextState == LINK_LAYER_RECEIVE_DELAY_ACK_NAK || linkLayerInternals.nextState == LINK_LAYER_TIMEOUT)){
                physicalLayerInternals.rxLinkLayerDone = 1;
            }

            break;
        case LINK_LAYER_RECEIVE_PAYLOAD:         // Valid transitions out: LINK_LAYER_RECEIVE_CHECKSUM, LINK_LAYER_TIMEOUT
            // This state can timeout if the timer that was set in 
            // LINK_LAYER_RECEIVE_MESSAGE_TYPE lapses
            
            // Exit condition: the entire payload has been received
            //      path 1: go to LINK_LAYER_RECEIVE_CHECKSUM
            if(linkLayerInternals.rxBytePosition >= linkLayerInternals.parsedRxMessageLength + MESSAGE_TYPE_LENGTH + PAYLOAD_LENGTH_LENGTH){
                linkLayerInternals.nextState = LINK_LAYER_RECEIVE_CHECKSUM;
            }

            // Exit condition: timeout
            //      path 1: go to LINK_LAYER_ERROR
            if(linkLayerInternals.timerTimeout){
                linkLayerInternals.nextState = LINK_LAYER_TIMEOUT;
            }

            if(EXITING_SPI_LINK_LAYER_TASK_STATE && linkLayerInternals.nextState == LINK_LAYER_TIMEOUT){
                physicalLayerInternals.rxLinkLayerDone = 1;
            }

            break;
        case LINK_LAYER_RECEIVE_CHECKSUM:        // Valid transitions out: LINK_LAYER_RECEIVE_DELAY_ACK_NAK, LINK_LAYER_TIMEOUT
            // This state can timeout if the timer that was set in 
            // LINK_LAYER_RECEIVE_MESSAGE_TYPE lapses
            
            // Exit condition: we received the checksum bytes
            //      path 1: Send an ACK
            //      path 2: Send a NAK
            if(linkLayerInternals.rxBytePosition >= linkLayerInternals.parsedRxMessageLength + MESSAGE_TYPE_LENGTH + PAYLOAD_LENGTH_LENGTH + CHECKSUM_LENGTH){
                linkLayerInternals.checksumValid = IsChecksumValid(linkLayerInternals.rxMessage, linkLayerInternals.parsedRxMessageLength + MESSAGE_TYPE_LENGTH + PAYLOAD_LENGTH_LENGTH + CHECKSUM_LENGTH);
                if(linkLayerInternals.checksumValid){
                    // If the checksum was valid, send a 2 byte reply of either ACK or NAK
                    // Also pass the message onto the basic DR or intermediate DR layers.
                    linkLayerInternals.txMessageLength = 2;
                    linkLayerInternals.nextState = LINK_LAYER_RECEIVE_DELAY_ACK_NAK;
                    if(linkLayerInternals.rxMessage[0] == 0x08 && linkLayerInternals.rxMessage[1] == 0x01){
                        if(linkLayerInternals.rxMessage[2] == 0x00 && linkLayerInternals.rxMessage[3] == 0x00){
                            // This was a zero-payload message, which indicates "message supported query"
                            // Messages of type 0x08 0x01 (basic demand response, required) are supported; reply with Link Layer ACK
                            // ToDo: if specific op codes are unsupported, call into the IntermediateDR layer to ask for support here
                            linkLayerInternals.txMessage[0] = 0x06; // ACK
                            linkLayerInternals.txMessage[1] = 0x00;
                        } else {
                            // Payload length is non-zero, this is a real message
                            linkLayerInternals.txMessage[0] = 0x06; // ACK
                            linkLayerInternals.txMessage[1] = 0x00;
                            BasicDRMessageHandler(linkLayerInternals.rxMessage); 
                        }
                    } else if(linkLayerInternals.rxMessage[0] == 0x08 && linkLayerInternals.rxMessage[1] == 0x02){
                        if(linkLayerInternals.rxMessage[2] == 0x00 && linkLayerInternals.rxMessage[3] == 0x00){
                            // This was a zero-payload message, which is a "message supported query"
                            // Messages of type 0x08 0x02 are supported; reply with Link Layer ACK
                            // ToDo: if specific op codes are unsupported, call into the IntermediateDR layer to ask for support here
                            linkLayerInternals.txMessage[0] = 0x06; // ACK
                            linkLayerInternals.txMessage[1] = 0x00;
                        } else {
                            // Payload length is non-zero, this is a real message
                            linkLayerInternals.txMessage[0] = 0x06; // ACK
                            linkLayerInternals.txMessage[1] = 0x00;
                            IntermediateDRMessageHandler(linkLayerInternals.rxMessage);
                        }
                    } else if(linkLayerInternals.rxMessage[0] == 0x08 && linkLayerInternals.rxMessage[1] == 0x03){
                        // ToDo: In the future, this is where we should call back the link layer message handler... (Baud Rate, max payload length, etc...))
                        // For now, unsupported.
                        linkLayerInternals.txMessage[0] = 0x15; // NAK
                        linkLayerInternals.txMessage[1] = 0x06; // Unsupported message type
                    } else {
                        linkLayerInternals.txMessage[0] = 0x15; // NAK
                        linkLayerInternals.txMessage[1] = 0x06; // Unsupported message type                        
                    }
                } else {
                    // The checksum was bad - reply with a link-layer NAK. 
                    linkLayerInternals.txMessageLength = 2;
                    linkLayerInternals.nextState = LINK_LAYER_RECEIVE_DELAY_ACK_NAK;
                    linkLayerInternals.txMessage[0] = 0x15; // NAK
                    linkLayerInternals.txMessage[1] = 0x03; // Checksum error                                                                                                                                                                                                                                                                                                                                                                     
                }
            }

            // Exit condition: timeout
            //      path 1: go to LINK_LAYER_ERROR
            if(linkLayerInternals.timerTimeout){
                linkLayerInternals.nextState = LINK_LAYER_TIMEOUT;
            }

            if(EXITING_SPI_LINK_LAYER_TASK_STATE && (linkLayerInternals.nextState == LINK_LAYER_TIMEOUT || linkLayerInternals.nextState == LINK_LAYER_RECEIVE_DELAY_ACK_NAK)){
                physicalLayerInternals.rxLinkLayerDone = 1;
            }

            break;
        case LINK_LAYER_RECEIVE_DELAY_ACK_NAK:        // Valid transitions out: LINK_LAYER_IDLE, LINK_LAYER_TIMEOUT
            if(ENTERING_SPI_LINK_LAYER_TASK_STATE){
                UCM_TO_SGD_ACK_NAK_REQUEST = 0;                
                
                SPI_START_LINK_LAYER_TIMER(TIME_MIN_WAIT_TIME_TO_SEND_ACK_NAK_IN_MS)
            }

            if(linkLayerInternals.timerTimeout){
                linkLayerInternals.nextState = LINK_LAYER_RECEIVE_REPLY_ACK_NAK;
            }
            break;
        case LINK_LAYER_RECEIVE_REPLY_ACK_NAK:
            // Precondition: linkLayerInternals.txMessage and 
            // linkLayerInternals.txMessageLength have been set with ACK or 
            // NAK + NAK Code                               

            // Timeout for this state is handled by the physical layer's 
            // txTimeout flag
            
            if(ENTERING_SPI_LINK_LAYER_TASK_STATE){
                linkLayerInternals.txBytePosition = 0;
                linkLayerInternals.txMessageResponseReceived = 0;
                physicalLayerInternals.txRequest = 1;
            }
            
            // We don't do anything here - the Physical Layer state machine will enable the TX ISR, and it will pick up our TX message
           
            if(physicalLayerInternals.txComplete){   
                linkLayerInternals.nextState = LINK_LAYER_IDLE;
            }

            if(physicalLayerInternals.txTimeout){
                linkLayerInternals.nextState = LINK_LAYER_TIMEOUT;
            }
            
            if(EXITING_SPI_LINK_LAYER_TASK_STATE){
                physicalLayerInternals.txLinkLayerDone = 1;
            }

            break;
        case LINK_LAYER_TRANSMIT_MESSAGE:   // Valid transitions out: LINK_LAYER_TRANSMIT_PAYLOAD, LINK_LAYER_TIMEOUT            
            // Precondition: linkLayerInternals.txMessage is set with message contents and the appropriate checksum has been inserted into txMessage
            // Precondition: linkLayerInternals.txMessageLength has been set
            
            // Timeout for this state is handled by the physical layer's 
            // txTimeout flag

            if(ENTERING_SPI_LINK_LAYER_TASK_STATE){
                linkLayerInternals.txMessageRetryCounter--;
                UCM_TO_SGD_MESSAGE_REQUEST = 0;
                
                linkLayerInternals.txBytePosition = 0;
                linkLayerInternals.txMessageResponseReceived = 0;

                // This kicks off the physical layer to do the work:
                physicalLayerInternals.txComplete = 0;
                physicalLayerInternals.txRequest = 1;
            }

            // We don't do anything here - the Physical Layer state machine will enable the TX ISR, and it will pick up our TX message, reporting when it's complete or timed out...
            
            if(physicalLayerInternals.txComplete){   
                linkLayerInternals.nextState = LINK_LAYER_TRANSMIT_WAIT_FOR_ACK_NAK;
            }

            // Exit condition: timeout
            //      path 1: go to LINK_LAYER_TIMEOUT
            if(physicalLayerInternals.txTimeout){
                linkLayerInternals.nextState = LINK_LAYER_TIMEOUT;
            }

            if(EXITING_SPI_LINK_LAYER_TASK_STATE){
                physicalLayerInternals.txLinkLayerDone = 1;
            }

            break;
        case LINK_LAYER_TRANSMIT_WAIT_FOR_ACK_NAK:
            if(ENTERING_SPI_LINK_LAYER_TASK_STATE){
                linkLayerInternals.rxBytePosition = 0;
                linkLayerInternals.parsedRxMessageLength = 0;
                linkLayerInternals.checksumValid = 0;
                
                // Reset the SPI_CS_IO rising edge detector.
                SGD_TO_UCM_MESSAGE_REQUEST = 0;

                SPI_START_LINK_LAYER_TIMER(TIME_MAX_WAIT_TIME_FOR_ACK_NAK_IN_MS)
            }
            
            // This waits for the ISR to do notice a falling edge on SPI_CS_IO
            // Prefer to do this on an edge, but level is adequate.
            if(SGD_TO_UCM_MESSAGE_REQUEST){
                SGD_TO_UCM_MESSAGE_REQUEST = 0;
                physicalLayerInternals.rxRequest = 1;
            }

            // Exit condition: we have received bytes that may be the checksum
            //      path 1: the message type is an ACK/NAK, go to idle
            //      path 2: checksum failed on recipient side - retry
            //      path 3: another NAK, do not retry
            if(linkLayerInternals.rxBytePosition >= MESSAGE_TYPE_LENGTH){
                linkLayerInternals.txMessageResponseReceived = 1;
                if(linkLayerInternals.rxMessage[0] == 0x06 && linkLayerInternals.rxMessage[1] == 0x00){
                    // ACK
                    linkLayerInternals.nextState = LINK_LAYER_IDLE;
                }else if(linkLayerInternals.rxMessage[0] == 0x15 && linkLayerInternals.rxMessage[1] == 0x03){
                    // NAK that indicates a checksum failure - we should retry sending our message again
                    // Note: The LINK_LAYER_TIMEOUT state has state entry logic to initiate the TX retry if possible
                    linkLayerInternals.nextState = LINK_LAYER_TIMEOUT;
                }else if(linkLayerInternals.rxMessage[0] == 0x15){
                    // Another NAK but wasn't a checksum failure; various reasons that are not addressed with a retry
                    linkLayerInternals.nextState = LINK_LAYER_IDLE;
                }
            }

            // Exit condition: the checksum hasn't been received within the required time window
            //      path 1: timeout error and potentially retry
            if(linkLayerInternals.timerTimeout){
                // Note: The LINK_LAYER_TIMEOUT state has state entry logic to initiate the TX retry if possible
                linkLayerInternals.nextState = LINK_LAYER_TIMEOUT;
            }

            if(EXITING_SPI_LINK_LAYER_TASK_STATE){
                physicalLayerInternals.rxLinkLayerDone = 1;
            }

            break;
        case LINK_LAYER_TRANSMIT_WAIT_FOR_RETRY_TIMER:
            if(ENTERING_SPI_LINK_LAYER_TASK_STATE){
                // * ToDo: Choose a random delay between 100 mS to 2000 mS (for now, 250 is fine))
                SPI_START_LINK_LAYER_TIMER(250)
                        
                // I'm really not a huge fan of setting these here, 
                // but without introducing an additional state or an additional
                // handshake, this must be done:
                physicalLayerInternals.txComplete = 0;
                physicalLayerInternals.txTimeout = 0;
            }

            if(linkLayerInternals.timerTimeout && 
               physicalLayerInternals.state == PHY_LAYER_IDLE){ // the PHY_LAYER_IDLE compare is superfluous, but safe.
                linkLayerInternals.nextState = linkLayerInternals.txMessageRetryTargetState;
            }
            break;
        case LINK_LAYER_TIMEOUT:
            if(linkLayerInternals.lastState == LINK_LAYER_TRANSMIT_MESSAGE ||
               linkLayerInternals.lastState == LINK_LAYER_TRANSMIT_WAIT_FOR_ACK_NAK ||
               linkLayerInternals.lastState == LINK_LAYER_RECEIVE_DELAY_ACK_NAK){
                // Check and see if there are retries left - and try if possible
                if(linkLayerInternals.txMessageRetryCounter == 0){
                    linkLayerInternals.nextState = LINK_LAYER_IDLE;
                } else {
                    linkLayerInternals.nextState = LINK_LAYER_TRANSMIT_WAIT_FOR_RETRY_TIMER;
                }
            } else {
                // We get here if one of the multiple receive states timeout
                linkLayerInternals.nextState = LINK_LAYER_IDLE;
            }
            break;
        default:
            break;
    }

    continueProcessing = linkLayerInternals.state != linkLayerInternals.nextState;
    linkLayerInternals.lastState = linkLayerInternals.state;
    linkLayerInternals.state = linkLayerInternals.nextState;
    return continueProcessing;
}

void SPI_Driver_Task(void){
    int morePhysicalWork;
    int moreLinkWork;
    
    // Run the state machines as greedily as possible as long as there is logic
    // to execute. As long as both tasks change states within a call, we'll
    // call them again. As soon as they both stop changing states, then we 
    // yield to the scheduler.
    do {
        morePhysicalWork = SPI_Physical_Layer_Task();
        moreLinkWork = SPI_Link_Layer_Task();
    } while (morePhysicalWork || moreLinkWork);
}


/**
 * Compute the checksum of an outgoing message. This also modifies the message
 * payload to include the checksum
 *
 * @param message[] the message type bytes and payload.
 * @param lan string length.
 * @return checksum as two byte (integer) value
 */
UINT16 MakeChecksumSPI(volatile unsigned char * message, int len)
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

MCIResponse SyncResponse;

// Blocking/synchronous call
MCIResponse MCISendSPI(unsigned char * msg)
{
    int finished = 0;
    int sent = 0;
    int timeout = 0;
    
    int expectLinkLayerReply = msg[0] == 0x08 && (msg[1] == 0x01 || msg[1] == 0x02 || msg[1] == 0x03);
    
    // These timeout values could be more intelligently calculated based on the bit rate, max message length, etc.
    // They really serve as a mechanism to not starve the rest of the system from cycles until we end up implementing
    // a threading mechanism or RTOS of some sort. 
    if(expectLinkLayerReply){
        // MAX MESSAGE DURATION (TX) + MAX WAIT FOR ACK/NAK + ACK / NAK + MAX MESSAGE DURATION (RX) --> END
        SPI_START_LINK_LAYER_REPLY_TIMER(TIME_MAX_WAIT_TIME_FOR_ACK_NAK_IN_MS + TIME_MAX_MESSAGE_DURATION_IN_MS * 2)
    } else {
        // MAX MESSAGE DURATION (TX) + MAX WAIT FOR ACK/NAK + ACK / NAK        
        SPI_START_LINK_LAYER_REPLY_TIMER(TIME_MAX_WAIT_TIME_FOR_ACK_NAK_IN_MS + TIME_MAX_MESSAGE_DURATION_IN_MS)
    }
    
    do {
        if(linkLayerInternals.state == LINK_LAYER_IDLE && physicalLayerInternals.state == PHY_LAYER_IDLE && !sent){
            MCISendAsyncSPI(msg);
            sent = 1;
        }
        SPI_Driver_Task();
        if(expectLinkLayerReply){
            // TX was ACK'd or NAK'd, then the expected link layer reply was fully received...
            finished = linkLayerInternals.txMessageResponseReceived && linkLayerInternals.checksumValid;
        } else {
            finished = linkLayerInternals.txMessageResponseReceived;
        }
        timeout = linkLayerInternals.linkLayerReplyTimeout;
    } while(!finished && !timeout);

    if(timeout){
        // Just kick everything back to the beginning and reset everything into
        // a known state.
        physicalLayerInternals.state = PHY_LAYER_INIT;
        linkLayerInternals.state = LINK_LAYER_INIT;
        SPI_Driver_Task();
    }
    
    SPI_CANCEL_LINK_LAYER_REPLY_TIMER
    
    SyncResponse.LLResponse[0] = linkLayerInternals.rxMessage[0];
    SyncResponse.LLResponse[1] = linkLayerInternals.rxMessage[1];
    
    return SyncResponse;
}

/*
 * construct and send a MCI packet via SPI3, non-blocking
 * call is non-blocking with timeouts specified in MCI 2.0 spec
 * 
 */
void MCISendAsyncSPI(unsigned char * msg)
{
    int i;
    if(linkLayerInternals.state == LINK_LAYER_IDLE){
        int payloadLen = ((msg[2] << 8) | msg[3]) & 0x1FFF; // Only the bottom 13 bits of the payload length are valid
        for(i=0; i < payloadLen + MESSAGE_TYPE_LENGTH + PAYLOAD_LENGTH_LENGTH; i++){
            linkLayerInternals.txMessage[i] = msg[i];
        }
        
        MakeChecksumSPI(linkLayerInternals.txMessage, payloadLen + MESSAGE_TYPE_LENGTH + PAYLOAD_LENGTH_LENGTH);
        linkLayerInternals.txMessageLength = payloadLen + MESSAGE_TYPE_LENGTH + PAYLOAD_LENGTH_LENGTH + CHECKSUM_LENGTH;
        UCM_TO_SGD_MESSAGE_REQUEST = 1;
    } else {
        // Can't really do anything here except drop the request, unless
        // we implement a request queue.
    }
}

void SPI_Driver_Send_ACK_NAK(unsigned char * msg)
{
    int i;
    if(linkLayerInternals.state == LINK_LAYER_IDLE){
        linkLayerInternals.txMessage[0] = msg[0];
        linkLayerInternals.txMessage[1] = msg[1]; // Checksum error
        linkLayerInternals.txMessageLength = 2;
        UCM_TO_SGD_ACK_NAK_REQUEST = 1;
    } else {
        // Wat do?
    }
}


/*
 * Send a time sync to SGD, non-blocking
 * 
 */
//BOOL SendTimeSyncSPI(int weekday, int hour)
//{
//    if(linkLayerInternals.state == LINK_LAYER_IDLE){
//        TimeSyncMsg[5] = (weekday << 4) + hour;
//        MCISendAsyncSPI(TimeSyncMsg);
//        return TRUE;
//    } else {
//        return FALSE; // BH: ToDo: look for all other callers and verify this policy change.
//    }
//
//}

#endif
/*
*	SPIi is a driver interface for the SPI ports.
*/

#include <stdlib.h>
#include <spi.h>
#include <int.h>
#include <plib.h>
#include <system.h>
#include <int_5xx_6xx_7xx.h>
#include <string.h>
#include "IntwineCodeBaseMethods.h"
#include "COMuAPI.h"
#include "SPIiAPI.h"
#include "SPIiMain.h"
#include "PRODuAPI.h"
#include "OSCiAPI.h"
#include "p32mx675f512l.h"

// High water for receiving a character back was about 600
#define SPI_CHARACTER_RECEIVE_LOOP_TIMEOUT  2000
#define SPI_CHARACTER_TRANSMIT_LOOP_TIMEOUT 2000

//#define TESTING_SPI_PORT

// Notes about what can be done in the future to make this more modular:
// these will make this code more generic
// - client to setup gpio for attention and select lines
// - client to provide callback for asserting attention
// - client to provide callback for de-asserting attention

int     SpiDoMasterSlaveExample (int nCycles);
void    SpiInitDevice           (int chn, int isMaster, int frmEn, int frmMaster, unsigned int uiDivider);

// some definitions

#define    MIN_SPI_TXFER_SIZE        8        	// min number of words per transfer
#define    MAX_SPI_TXFER_SIZE        512        // max number of words per transfer

#define    EXTRA_CHARACTERS_TO_PURGE_SPI_BUS    2
void SPIiPurgeBus(void);

void SpiInitDevice(int chn, int isMaster, int frmEn, int frmMaster, unsigned int uiDivider)
{
    unsigned int    config=SPI_CON_MODE8|SPI_CON_SMP|SPI_CON_ON;    // SPI configuration word
    if(isMaster)
    {
        config|=SPI_CON_MSTEN;
    }
    if(frmEn)
    {
        config|=SPI_CON_FRMEN;
        if(!frmMaster)
        {
            config|=SPI_CON_FRMSYNC;
        }
    }
    SpiChnOpen(chn, config, uiDivider);    // divide fpb by iDivider, configure the I/O ports. Not using SS in this example
}


/*
*	Function Name:	void SPIiInitialize(void)
*	Description:	Initialization of the selected SPI.
*	Initiator: 		Robert Scaccia
*	Date: 			10/10/2011
*/
int control_reg_val_save_for_restart = 0;
void SPIiInitialize(int control_reg_val)
{

#ifdef TESTING_SPI_PORT
    SpiChnSetBrg(SPI_CHANNEL3, SpiBrgVal(GetSystemClock(), 23100));
    SpiInitDevice(SPI_CHANNEL3, 1, 0, 1, 1024);     //23.1KHz

    //SpiChnOpen(SPI_CHANNEL3, SPI_OPEN_MSTEN | SPI_OPEN_CKP_HIGH | SPI_OPEN_MODE8 | SPI_OPEN_TBE_EMPTY | SPI_OPEN_RBF_FULL | SPI_OPEN_ON, GetSystemClock());	
    //SpiChnChangeMode(SPI_CHANNEL3, TRUE, TRUE, FALSE);																						
#else

// Library call - produces same results as hand-written code below
//    SpiChnOpen(SPI_CHANNEL3, SPI_OPEN_SLVEN | SPI_OPEN_CKE_REV | SPI_OPEN_MODE8, GetSystemClock());

    // SPI3CON - bits
    // 3    2    2    1    1    1    0    0
    // 1    7    3    9    5    1    7    3
    // 000x xxxx xxxx xxxx xxxx xxxx xxxx xxxx - no Framed SPI support
    // xxx0 0000 0000 00xx xxxx xxxx xxxx xxxx - reserved
    // xxxx xxxx xxxx xx0x xxxx xxxx xxxx xxxx - no Framed SPI support
    // xxxx xxxx xxxx xxx0 xxxx xxxx xxxx xxxx - reserved
    // xxxx xxxx xxxx xxxx 1xxx xxxx xxxx xxxx - Enabled
    // xxxx xxxx xxxx xxxx x0xx xxxx xxxx xxxx - No Freeze
    // xxxx xxxx xxxx xxxx xx0x xxxx xxxx xxxx - Continue in idle mode
    // xxxx xxxx xxxx xxxx xxx0 xxxx xxxx xxxx - SDOx control by SPI module
    // xxxx xxxx xxxx xxxx xxxx 00xx xxxx xxxx - 8 bit data width
    // xxxx xxxx xxxx xxxx xxxx xx0x xxxx xxxx - Input sampled at middle
    // xxxx xxxx xxxx xxxx xxxx xxx1 xxxx xxxx - CKE = 1 (clock edge, 1 = change from active to idle)
    // xxxx xxxx xxxx xxxx xxxx xxxx 0xxx xxxx - SSEN = 0
    // xxxx xxxx xxxx xxxx xxxx xxxx x0xx xxxx - CKP = 0 (clock polarity, 0 = idle low, active high)
    // xxxx xxxx xxxx xxxx xxxx xxxx xx0x xxxx - MSTEN = 0 (slave)
    // xxxx xxxx xxxx xxxx xxxx xxxx xxx0 0000 - reserted

    // Notes:
    // INTWINE_CONNECTED_LOAD_CONTROL  0000 0000 0000 0000 1000 0001 0000 0000 = 0x00008100
    // INTWINE_PROGRAMMABLE_THERMOSTAT 0000 0000 0000 0000 1000 0001 1000 0000 = 0x00008180
    SPI3CON = control_reg_val;
    control_reg_val_save_for_restart = control_reg_val;

    // 1) Configure attention line - RB2 as output
    // 2) Configure select line    - RD14 as input
    // 1)
    TRISBCLR = BIT_2;
    LATBSET = BIT_2;

    // 2)
    TRISDSET = BIT_14;

#endif    
}


/*
*	Function Name:	void SPIiSendMessageAttn(char *buf, int len, int leave_attention_asserted)
*	Description:	Method to send a synchronous (non-interrupt) message, leaves attention asserted
*	Initiator: 		Bill Barnett
*	Date: 			11/25/2011
*/
int SPIiSendMessageAttn(char *buf, int len, int leave_attention_asserted)
{
    char *ptr = buf, cJunkChar; 
    int char_count = 0;

    // SPI test code
    // 4) Byte in SPI transmit buffer
    // 5) Assert attention
    // 6) Wait for tx buffer to empty and rx buffer to fill
    // 7) 
    // 8) De-assert attention
    // 9) Read character


    // 4)
    while(SPI3STATbits.SPIRBF)
    {
        cJunkChar = SPI3BUF;  // clear buffer and overflow
    }

    // 5) load first character
    SPI3BUF = *ptr++;
    char_count = 1;

    // 6) assert attention
    LATBCLR = BIT_2;

    while(char_count <= len)
    {
        if(SPIiWaitTransmitChar() != TRANSMIT_SUCCESSFUL)
        {
            return(RESPONSE_NOT_RECEIVED);
        }

        // load next character
        SPI3BUF = *ptr++;
        char_count++;

        if(SPIiWaitReceiveChar(&cJunkChar) == RESPONSE_NOT_RECEIVED)
        {
            return(RESPONSE_NOT_RECEIVED);
        }

    }

    SPIiPurgeBus();

    // 7
    if(!leave_attention_asserted)
    {
        LATBSET = BIT_2;
    }

    return(RESPONSE_RECEIVED);
}

/*
*	Function Name:	void SPIiSendMessage(char *buf, int len)
*	Description:	Method to send a synchronous (non-interrupt) message
*	Initiator: 		Bill Barnett
*	Date: 			11/24/2011
*/
int SPIiSendMessage(char *buf, int len)
{
    return(SPIiSendMessageAttn(buf, len, 0));
}


#define TIMEOUT_CHARACTERS_START        50
#define TIMEOUT_CHARACTERS_MESSAGE      50

/*
*	Function Name:	int SPIiSendMessageEntek(char *sendbuf, int len, char *userbuf, int *userlen)
*	Description:	Entek-specific method to send a message and recieve and Entek-protocol response
*	Initiator: 		Bill Barnett
*	Date: 			11/25/2011
*/
int SPIiSendMessageEntek(char *sendbuf, int len, char *userbuf, int *userlen)
{
    char *ptr, cReceiveChar; 
    char receive_buf[100];

    int timeout_char_count = 0;

    char dummy_char = 0xAA;
    char start_char = 0x51;
    int start_char_received = 0;
    int received_char_count = 0;

    int message_size = 0;
    int message_complete = 0;

    int purge_count = 0;

    char checksum, *chkptr;
    int ic;

    // Steps to sendind a message and receiving a response
    // 1) Send the original message, but leave attention asserted so the entek will send response
    // 2) Setup: load dummy char and loop waiting on start character
    // 3) Process characters while waiting for start character
    // 4) Process remaining portion or message, retrieving length in the process
    // 5) Purge, de-assert attention and check character count
    // 6) Verify the validation code

    // 1)
    if(SPIiSendMessageAttn(sendbuf, len, 1) == RESPONSE_NOT_RECEIVED)
    {
        return(RESPONSE_NOT_RECEIVED);
    }

    // 2)
    memset(receive_buf, '$', 100);
    ptr = receive_buf;
    SPI3BUF = dummy_char;

    // 3)
    while((timeout_char_count <= TIMEOUT_CHARACTERS_START) && (!start_char_received))
    {
        if(SPIiWaitTransmitChar() != TRANSMIT_SUCCESSFUL)
        {
            return(RESPONSE_NOT_RECEIVED);
        }

        // load next character
        SPI3BUF = dummy_char;
        timeout_char_count++;

        // wait for character to come back
        if(SPIiWaitReceiveChar(&cReceiveChar) == RESPONSE_NOT_RECEIVED)
        {
            return(RESPONSE_NOT_RECEIVED);
        }

        if(cReceiveChar == start_char)
        {
            start_char_received = 1;
            *ptr++ = cReceiveChar;
            received_char_count = 1;
        }
    }

    if(!start_char_received)
    {
        LATBSET = BIT_2;
        SPIiRestart();      // Since the fail return is not based on SPIiWaitReceiveChar
        return(RESPONSE_NOT_RECEIVED);
    }

    // 4)
    SPI3BUF = dummy_char;
    timeout_char_count = 0;
    while((timeout_char_count <= TIMEOUT_CHARACTERS_MESSAGE) && (!message_complete))
    {
        if(SPIiWaitTransmitChar() != TRANSMIT_SUCCESSFUL)
        {
            return(RESPONSE_NOT_RECEIVED);
        }

        // load next character
        SPI3BUF = dummy_char;
        timeout_char_count++;

        // wait for character to come back
        if(SPIiWaitReceiveChar(&cReceiveChar) == RESPONSE_NOT_RECEIVED)
        {
            return(RESPONSE_NOT_RECEIVED);
        }

        *ptr++ = cReceiveChar;
        received_char_count++;

        // set message size
        if(message_size == 0)
        {
            message_size = ((int) cReceiveChar) + 2;  // size does not include Header or Length bytes
        }

        if(received_char_count >= message_size)
        {
            message_complete = 1;
        }

    }

    SPIiPurgeBus();

    LATBSET = BIT_2;

    purge_count++;  //bp

    if(timeout_char_count >= TIMEOUT_CHARACTERS_MESSAGE)
    {
        SPIiRestart();      // Since the fail return is not based on SPIiWaitReceiveChar
        return(RESPONSE_NOT_RECEIVED);
    }

    // 6)
    checksum = 0;
    chkptr = receive_buf;
    for(ic=0; ic<message_size-1; ic++)
    {
        checksum ^= *chkptr++;
    }
    
    if(*chkptr != checksum)
    {
        return(RESPONSE_NOT_VERIFIED);
    }

    memcpy(userbuf, receive_buf, message_size);
    return(RESPONSE_RECEIVED);

}

void SPIiPurgeBus(void)
{
    int i;
    char cJunkChar;

    for(i=0; i<EXTRA_CHARACTERS_TO_PURGE_SPI_BUS; i++)
    {
        // wait for character to come back, don't care about return value
        // here as we are just purging, simply return
        SPIiWaitReceiveChar(&cJunkChar);

    }
}


int SPIiWaitReceiveChar(char *c)
{
    int rbf_count=0;
    static int rbf_high_water=0;
    char cReceiveChar;

    rbf_count = 0;
    while(SPI3STATbits.SPIRBF == 0)
    {
        if(++rbf_count > rbf_high_water)
        {
            rbf_high_water = rbf_count;
        }

        if(rbf_count > SPI_CHARACTER_RECEIVE_LOOP_TIMEOUT)
        {
            // failed, clear user's character, deassert attention, 
            *c = 0;
            LATBSET = BIT_2;

            SPIiRestart();

            return(RESPONSE_NOT_RECEIVED);
        }
    }

    cReceiveChar = SPI3BUF;
    *c = cReceiveChar;

    return(RESPONSE_RECEIVED);
}

void SPIiRestart(void)
{

    // close and re-open the channel
    SpiChnClose(SPI_CHANNEL3);
    SPIiInitialize(control_reg_val_save_for_restart);
}

int SPIiWaitTransmitChar(void)
{
    int tbe_count=0;
    static int tbe_high_water=0;

    tbe_count = 0;
    while(SPI3STATbits.SPITBE == 0)
    {
        if(++tbe_count > tbe_high_water)
        {
            tbe_high_water = tbe_count;
        }

        if(tbe_count > SPI_CHARACTER_TRANSMIT_LOOP_TIMEOUT)
        {
            // failed, clear user's character, deassert attention, 
            LATBSET = BIT_2;

            SPIiRestart();

            return(TRANSMIT_NOT_SUCCESSFUL);
        }
    }

    return(TRANSMIT_SUCCESSFUL);
}



/**
 * LinkLayerMessaging.c
 * 
 * Contains code for handling exclusively link layer message exchanges
 *  
 * Jordan Murray May 2016
 */

#include "LinkLayerMessaging.h"
#include "MCI_Common.h"
#ifdef DC_CEA2045
 #include "EPRI_SPILayer.h"
#else
 #include "EPRI_UARTLayer.h"
#endif


/**supported link layer message types
 * first two bytes: 0x0803 standard prefix for MCI link layer message
 * second two bytes: 
 *   first 3 bits: reserved, 0
 *   next 13 bits: payload length
 * third two bytes: message opcodes
 * last two bytes: checksum (blank for now)
*/
unsigned char LINKMSG_RequestDifferentPowerMode[8] = {0x08, 0x03, 0x00, 0x02, 0x16, 0x00, 0x00, 0x00 };
unsigned char LINKMSG_RequestDifferentBitRate[8] = {0x08, 0x03, 0x00, 0x02, 0x17, 0x00, 0x00, 0x00};
unsigned char LINKMSG_QueryMaximumPayloadLength[8] = {0x08, 0x03, 0x00, 0x02, 0x18, 0x00, 0x00, 0x00};
unsigned char LINKMSG_ResponseMaximumPayloadLength[8] = {0x08, 0x03, 0x00, 0x02, 0x19, 0x00, 0x00, 0x00};
unsigned char LINKMSG_QueryGetSGDSlotNumber[8] = {0x08, 0x03, 0x00, 0x02, 0x1A, 0x00, 0x00, 0x00};
unsigned char LINKMSG_QueryGetAvailableSlotNumbers[8] = {0x08, 0x03, 0x00, 0x02, 0x1C, 0x00, 0x00, 0x00};
unsigned char LINKMSG_SendNextCommandToSlot[8] = {0x08, 0x03, 0x00, 0x02, 0x1E, 0x00, 0x00, 0x00};

int MAXIMUM_PAYLOAD_LENGTH_INDICATOR[14] = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 1280, 1500, 2048, 4096};
extern int MAX_PAYLOAD_SGD;

volatile enum _LL_MsgState LL_MsgState;

void SendSendNextCommandToSlot(unsigned char slotNumber)
{
    unsigned char messageBuffer[8];
    memcpy(messageBuffer,LINKMSG_SendNextCommandToSlot,8);
    
    messageBuffer[6] = slotNumber;
    MCISendNeutral(messageBuffer);
    LL_MsgState = LL_WAITING_SEND_NEXT_COMMAND_TO_SLOT;
}

void SendQueryGetAvailableSlotNumbers(void)
{    
    LL_MsgState = LL_WAITING_RESPONSE_GET_AVAILABLE_SLOT_NUMBER;
    MCISendNeutral(LINKMSG_QueryGetAvailableSlotNumbers);
}

void SendQueryGetSGDSlotNumber(void)
{
    LL_MsgState = LL_WAITING_RESPONSE_GET_SGD_SLOT_NUMBER;
    MCISendNeutral(LINKMSG_QueryGetSGDSlotNumber);    
}

void SendQueryMaximumPayloadLength(void)
{
    LL_MsgState = LL_WAITING_RESPONSE_MAXIMUM_PAYLOAD_LENGTH;
    MCISendNeutral(LINKMSG_QueryMaximumPayloadLength);    
}

void SendResponseMaximumPayloadLength(void)
{
    unsigned char messageBuffer[8];
    memcpy(messageBuffer,LINKMSG_ResponseMaximumPayloadLength,8);      
    
    /* See Table 7-2 of the CTA spec for details on message length encoding 
       Using 0x06 = 128 bit max payload here */
    messageBuffer[5] = 0x06;   //payload length   
    MCISendNeutral(messageBuffer);
}

void SendRequestDifferentPowerMode(unsigned char mode)
{
    unsigned char messageBuffer[8];
    memcpy(messageBuffer,LINKMSG_RequestDifferentPowerMode,8);
    
    messageBuffer[5] = mode;
    LL_MsgState = LL_WAITING_REQUEST_DIFFERENT_POWER_MODE;
    MCISendNeutral(messageBuffer);
}

void SendRequestDifferentBitRate(unsigned char bri)
{
    unsigned char messageBuffer[8];
    memcpy(messageBuffer,LINKMSG_RequestDifferentBitRate,8);
    
    messageBuffer[5] = bri;
    LL_MsgState == LL_WAITING_REQUEST_DIFFERENT_BIT_RATE;
    MCISendNeutral(messageBuffer);
}

void LinkLayerAckHandler(unsigned char * msg)
{
   
    switch(LL_MsgState)
    {
        case LL_IDLE:
            //what is this ack regarding? there is no outstanding link layer message
            //probably a response to an application ack
            
            break;
        case LL_WAITING_REQUEST_DIFFERENT_POWER_MODE:
            if(msg[0] == 0x06)
            {
                
            }
            else if(msg[0] == 0x15)
            {
                
            }
            break;
        case LL_WAITING_REQUEST_DIFFERENT_BIT_RATE:
            if(msg[0] == 0x06)
            {
                LL_MsgState = LL_IDLE;
            }
            else if(msg[0] == 0x15)
            {
                
            }
            break;
        case LL_WAITING_RESPONSE_MAXIMUM_PAYLOAD_LENGTH:
            if(msg[0] == 0x06)
            {
                
            }
            else if(msg[0] == 0x15)
            {
                
            }
            break;
        case LL_WAITING_RESPONSE_GET_SGD_SLOT_NUMBER:
            if(msg[0] == 0x06)
            {
                
            }
            else if(msg[0] == 0x15)
            {
                
            }
            break;
        case LL_WAITING_RESPONSE_GET_AVAILABLE_SLOT_NUMBER:
            if(msg[0] == 0x06)
            {
                
            }
            else if(msg[0] == 0x15)
            {
                
            }
            break;
        case LL_WAITING_SEND_NEXT_COMMAND_TO_SLOT:
            if(msg[0] == 0x06)
            {
                
            }
            else if(msg[0] == 0x15)
            {
                
            }
            break;                   
    }
}

void LinkLayerNakHandler(unsigned char * nakCode)
{
        
    switch(LL_MsgState)
    {
        case LL_IDLE:
            //what is this nak regarding? there is no outstanding link layer message
            break;
        case LL_WAITING_REQUEST_DIFFERENT_POWER_MODE:
            
            break;
        case LL_WAITING_REQUEST_DIFFERENT_BIT_RATE:
            break;
        case LL_WAITING_RESPONSE_MAXIMUM_PAYLOAD_LENGTH:
            break;
        case LL_WAITING_RESPONSE_GET_SGD_SLOT_NUMBER:
            break;
        case LL_WAITING_RESPONSE_GET_AVAILABLE_SLOT_NUMBER:
            break;
        case LL_WAITING_SEND_NEXT_COMMAND_TO_SLOT:
            break;                   
    }
}

void LinkLayerMessageHandler(unsigned char * msg)
{
    unsigned char opcode1 = msg[4];
    unsigned char opcode2 = msg[5];
    
    if(opcode1 == QUERY_MAXIMUM_PAYLOAD_LENGTH && opcode2 == 0x00)    //handler for maximum payload length query
    {
        // Got a request for MAX playload length...let's respond!
        SendResponseMaximumPayloadLength();      
    }
    else if(opcode1 == RESPONSE_MAXIMUM_PAYLOAD_LENGTH)
    {
        
        if(LL_MsgState == LL_WAITING_RESPONSE_MAXIMUM_PAYLOAD_LENGTH) //if we were expecting a payload length response
        {
            DL_Ack();            
            MAX_PAYLOAD_SGD = MAXIMUM_PAYLOAD_LENGTH_INDICATOR[opcode2];
        }
        else
        {
            DL_Nak(NO_REASON);
        }
        // = opcode2;
    }
    else if(opcode1 == REQUEST_DIFFERENT_BIT_RATE)   //handler for bit rate change request
    {
        if(opcode2 != 0x00)
        {
            DL_Nak(REQUEST_NOT_SUPPORTED);
        }
    }    
    else   //unsupported message type
    {
        DL_Nak(UNSUPPORTED_MESSAGE_TYPE);
    }
       
}




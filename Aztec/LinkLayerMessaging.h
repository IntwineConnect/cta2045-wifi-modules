/* 
 * File:   LinkLayerMessaging.h
 * Author: jmm72
 *
 * Created on May 19, 2016, 4:34 PM
 */

/**
 * LINK LAYER NAK CODE DEFINITIONS
 */
#define NO_REASON                   0x00
#define INVALID_BYTE                0x01
#define INVALID_LENGTH              0x02
#define CHECKSUM_ERROR              0x03
#define MESSAGE_TIMEOUT             0x05
#define UNSUPPORTED_MESSAGE_TYPE    0x06
#define REQUEST_NOT_SUPPORTED       0x07

/**
 * Data link message opcode definitions
 */
#define REQUEST_DIFFERENT_POWER_MODE            0x16
#define REQUEST_DIFFERENT_BIT_RATE              0x17
#define QUERY_MAXIMUM_PAYLOAD_LENGTH            0x18
#define RESPONSE_MAXIMUM_PAYLOAD_LENGTH         0x19
#define QUERY_GET_SGD_SLOT_NUMBER               0x1a
#define RESPONSE_SLOT_NUMBER                    0x1b
#define QUERY_GET_AVAILABLE_SLOT_NUMBERS        0x1c
#define RESPONSE_AVAILABLE_SLOT_NUMBERS         0x1d
#define SEND_NEXT_COMMAND_TO_SLOT               0x1e


volatile enum _LL_MsgState
{
    LL_IDLE,
    LL_WAITING_REQUEST_DIFFERENT_POWER_MODE,
    LL_WAITING_REQUEST_DIFFERENT_BIT_RATE,
    LL_WAITING_RESPONSE_MAXIMUM_PAYLOAD_LENGTH,
    LL_WAITING_RESPONSE_GET_SGD_SLOT_NUMBER,
    LL_WAITING_RESPONSE_GET_AVAILABLE_SLOT_NUMBER,
    LL_WAITING_SEND_NEXT_COMMAND_TO_SLOT,
};

extern volatile enum _LL_MsgState LL_MsgState;

/**supported link layer message types
 * first two bytes: 0x0803 standard prefix for MCI link layer message
 * second two bytes: 
 *   first 3 bits: reserved, 0
 *   next 13 bits: payload length
 * third two bytes: message opcodes
 * last two bytes: checksum (blank for now)
*/
unsigned char LINKMSG_RequestDifferentPowerMode[8];
unsigned char LINKMSG_RequestDifferentBitRate[8];
unsigned char LINKMSG_QueryMaximumPayloadLength[8];
unsigned char LINKMSG_ResponseMaximumPayloadLength[8];
unsigned char LINKMSG_QueryGetSGDSlotNumber[8];
unsigned char LINKMSG_QueryGetAvailableSlotNumbers[8];
unsigned char LINKMSG_SendNextCommandToSlot[8];


void LinkLayerMessageHandler(unsigned char * msg);
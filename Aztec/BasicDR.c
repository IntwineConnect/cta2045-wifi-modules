/**
 * BasicDR.c
 * 
 * contains code related to receiving/sending DR messages
 */
#include "MCI_Common.h"
#include "HardwareProfile.h"
#include "BasicDR.h"
#include <math.h>

#include "DeviceInfo.h"

volatile enum _RelayMsgState
{
    RLY_IDLE,
    RLY_WAITING_END_SHED,
            RLY_ACKED_END_SHED,
    RLY_WAITING_SHED,
            RLY_ACKED_SHED,
    RLY_WAITING_REQUEST_POWER_LEVEL,
            RLY_ACKED_REQUEST_POWER_LEVEL,
    RLY_WAITING_PRESENT_RELATIVE_PRICE,
            RLY_ACKED_PRESENT_RELATIVE_PRICE,
    RLY_WAITING_NEXT_PERIOD_RELATIVE_PRICE,
            RLY_ACKED_NEXT_PERIOD_RELATIVE_PRICE,
    RLY_WAITING_TIME_IN_PRICE_PERIOD,
            RLY_ACKED_TIME_IN_PRICE_PERIOD,
    RLY_WAITING_CRITICAL_PEAK_EVENT,
            RLY_ACKED_CRITICAL_PEAK_EVENT,
    RLY_WAITING_GRID_EMERGENCY,
            RLY_ACKED_GRID_EMERGENCY,
    RLY_WAITING_GRID_GUIDANCE,
            RLY_ACKED_GRID_GUIDANCE,
    RLY_WAITING_LOAD_UP,
            RLY_ACKED_LOAD_UP,
    RLY_WAITING_OP_STATE,
            RLY_ACKED_OP_STATE,
    RLY_WAITING_OUTSIDE_COMM_GOOD,
            RLY_ACKED_OUTSIDE_COMM_GOOD,
    RLY_WAITING_OUTSIDE_COMM_LOST,
            RLY_ACKED_OUTSIDE_COMM_LOST,
    RLY_WAITING_SIMPLE_TIME_SYNC,
            RLY_ACKED_SIMPLE_TIME_SYNC,
    RLY_WAITING_PENDING_EVENT,
            RLY_ACKED_PENDING_EVENT,
    RLY_WAITING_PENDING_EVENT_TYPE,
            RLY_ACKED_PENDING_EVENT_TYPE,
    RLY_WAITING_GET_COMMODITY_READ,
            RLY_ACKED_GET_COMMODITY_READ,
    RLY_WAITING_SET_COMMODITY_READ,
            RLY_ACKED_SET_COMMODITY_READ,
    RLY_WAITING_START_AUTONOMOUS_CYCLING,
            RLY_ACKED_START_AUTONOMOUS_CYCLING,
    RLY_WAITING_TERMINATE_AUTONOMOUS_CYCLING,
            RLY_ACKED_TERMINATE_AUTONOMOUS_CYCLING,
    RLY_WAITING_SET_TEMPERATURE_OFFSET,
            RLY_ACKED_SET_TEMPERATURE_OFFSET,
    RLY_WAITING_GET_TEMPERATURE_OFFSET,
            RLY_ACKED_GET_TEMPERATURE_OFFSET,
    RLY_WAITING_GET_PRESENT_TEMPERATURE,
            RLY_ACKED_GET_PRESENT_TEMPERATURE,
    RLY_WAITING_GET_SET_POINT,
            RLY_ACKED_GET_SET_POINT,
    RLY_WAITING_SET_SET_POINT,
            RLY_ACKED_SET_SET_POINT,
    RLY_WAITING_GET_ENERGY_PRICE,
            RLY_ACKED_GET_ENERGY_PRICE,
    RLY_WAITING_SET_ENERGY_PRICE,
            RLY_ACKED_SET_ENERGY_PRICE,
    RLY_WAITING_INFO_REQUEST,
            RLY_ACKED_INFO_REQUEST
            
} RelayMsgState;

unsigned char ExpectedAckType;
unsigned char override = 0;

//storage for commodity data
CommodityReadData commodityResponse[10];
unsigned char nOptions = 0;
DeviceInformation DeviceInfo;

// sends a "comm good" packet to the end device
MCIResponse TransmitCommGoodPacket()
{
    return MCISendNeutral(OutsideCommGood);    
}

// sends a "comm bad" packet to the end device
MCIResponse TransmitCommBadPacket()
{
    return MCISendNeutral(OutsideCommLost);    
}

void IntermediateDRMessageHandler(unsigned char *msg)
{
    unsigned char opcode1 = msg[4];
    unsigned char opcode2 = msg[5];
    unsigned char responseCode = msg[6];
    int mlen = msg[2] << 8 |  msg[3];
    
    ResponseReadyFlag = TRUE;
    httpCode = HandleIDRResponseCode(responseCode);
    //is the message Get Commodity Read Reply message?
    if(opcode1 == GET_COMMODITY_READ_CODE && opcode2 == GET_COMMODITY_READ_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_GET_COMMODITY_READ)
        {
            int i;
            int nCommodities = mlen/13;
            RelayMsgState = RLY_ACKED_GET_COMMODITY_READ;
            
            //this structure array holds response information for all commodity types in the message
            CommodityReadData commodityDataArray[nCommodities];
            commodityDataArray[0].nCommodities = nCommodities;
            //figure out the length of the commodity data size for malloc
            //int datasize = nCommodities*sizeof(CommodityReadData);
            
            //put data from message buffer into structure for each commodity type
            for(i = 0; i < nCommodities; i++)
            {
                commodityDataArray[i].commodityCode = msg[7+i*13];
                commodityDataArray[i].instantaneousRate = msg[8+i*13] << 40 | msg[9+i*13] << 32 | msg[10+i*13] << 24 | msg[11+i*13] << 16 | msg[12+i*13] << 8 | msg[13+i*13];
                commodityDataArray[i].cumulativeAmount = msg[14+i*13] << 40 | msg[15+i*13] << 32 | msg[16+i*13] << 24 | msg[17+i*13] << 16 | msg[18+i*13] << 8 | msg[19+i*13];
            }
            
            /*zero memory in the commodity data buffer before copying so that no old data is left in case
             the length of valid data field is ignored elsewhere*/
            memset(commodityResponse,0,COMMODITY_DATA_BUFFER_LENGTH*sizeof(CommodityReadData));
            //copy received data to buffer
            memcpy(commodityResponse, commodityDataArray, COMMODITY_DATA_BUFFER_LENGTH*sizeof(CommodityReadData));  
            
        }
    }
    else if(opcode1 == INFO_REQUEST_CODE && opcode2 == INFO_REQUEST_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_INFO_REQUEST)
        {
            RelayMsgState = RLY_ACKED_INFO_REQUEST;
            unsigned char mnBuffer[17];
            unsigned char srBuffer[17];
            unsigned char ctaBuffer[3];
            memcpy(mnBuffer,&msg[20],16);
            mnBuffer[16] ='\0';
            memcpy(srBuffer,&msg[36],16);
            srBuffer[16] = '\0';
            memcpy(ctaBuffer,&msg[7],2);
            ctaBuffer[3] = '\0';
            
            DeviceInfo.CTAver = (unsigned short int) atoi(ctaBuffer);
            DeviceInfo.vendorID = msg[9] << 8 | msg[10];
            DeviceInfo.deviceType = msg[11] << 8 | msg[12];
            DeviceInfo.deviceRev = msg[13] << 8 | msg[14];
            DeviceInfo.capbmp = msg[15] << 24 | msg[16] << 16 | msg[17] << 8 | msg[18];
            DeviceInfo.modelNumber = atoll(mnBuffer);
            DeviceInfo.serialNumber = atoll(srBuffer);
            DeviceInfo.firmwareYear = msg[52];
            DeviceInfo.firmwareMonth = msg[53];
            DeviceInfo.firmwareDay = msg[54];
            DeviceInfo.firmwareMajor = msg[55];
            DeviceInfo.firmwareMinor = msg[56];
        }
    }
    else if(opcode1 == INFO_REQUEST_CODE && opcode2 == INFO_REQUEST_CODE) //if the message is a request for info from the UCM
    {
        unsigned char msg[59];
        int temp;
        short int stemp;

        //IDR response
        msg[0] = 0x08;
        msg[1] = 0x02;
        //payload length
        msg[2] = 0x00;
        msg[3] = 0x35; //53
        //begin payload
        msg[4] = INFO_REQUEST_CODE;
        msg[5] = INFO_REQUEST_REPLY_CODE;
        msg[6] = 0x00;
        //cta version
        msg[7] = 0x00;
        msg[8] = 0x00;
        //vendor ID
        stemp = AztecUCMInfo.vendorID;
        ReverseByteOrder(stemp, sizeof(stemp));
        memcpy(&msg[9],&stemp,sizeof(stemp));
        //device type
        stemp = AztecUCMInfo.deviceType;
        ReverseByteOrder(&stemp, sizeof(stemp));
        memcpy(&msg[11],&stemp,sizeof(stemp));
        //device revision
        stemp = AztecUCMInfo.deviceRev;
        ReverseByteOrder(&stemp, sizeof(stemp));
        memcpy(&msg[13],&stemp,sizeof(stemp));
        //capability bitmap
        temp = AztecUCMInfo.capbmp;
        ReverseByteOrder(&temp,sizeof(temp));
        memcpy(&msg[15], &temp, sizeof(temp));
        msg[19] = 0x00;  //reserved
        snprintf(&msg[20], 16, "%016llu", AztecUCMInfo.modelNumber);
        snprintf(&msg[36], 16,"%016llu", AztecUCMInfo.serialNumber);
        msg[52] = AztecUCMInfo.firmwareYear;
        msg[53] = AztecUCMInfo.firmwareMonth;
        msg[54] = AztecUCMInfo.firmwareDay;
        msg[55] = AztecUCMInfo.firmwareMajor;
        msg[56] = AztecUCMInfo.firmwareMinor;
        //checksum fields
        msg[57] = 0xff;
        msg[58] = 0xff;

        httpCode = 500;
        MCISendNeutral(msg);       

    }
    else if(opcode1 == SET_COMMODITY_READ_CODE && opcode2 == SET_COMMODITY_READ_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_SET_COMMODITY_READ )
        {
            RelayMsgState = RLY_ACKED_SET_COMMODITY_READ;
        }
    }
    else if(opcode1 == START_AUTONOMOUS_CYCLING_CODE && opcode2 == START_AUTONOMOUS_CYCLING_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_START_AUTONOMOUS_CYCLING)
        {
            RelayMsgState = RLY_ACKED_START_AUTONOMOUS_CYCLING;
        }
    }
    else if(opcode1 == TERMINATE_AUTONOMOUS_CYCLING_CODE && opcode2 == TERMINATE_AUTONOMOUS_CYCLING_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_TERMINATE_AUTONOMOUS_CYCLING)
        {
            RelayMsgState = RLY_ACKED_TERMINATE_AUTONOMOUS_CYCLING;
        }
    }
    else if(opcode1 == GET_SET_POINT_CODE && opcode2 == GET_SET_POINT_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_GET_SET_POINT)
        {            
            setpoint2 = -32768;
            
            deviceType = msg[7] << 8 | msg[8];
            units = msg[9];
            setpoint1 = msg[10] << 8 | msg[11];
            if(mlen > 8)
            {
                setpoint2 = msg[12] << 8 | msg[13];
            }
            
            RelayMsgState = RLY_ACKED_GET_SET_POINT;
        }
    }
    else if(opcode1 == SET_SET_POINT_CODE && opcode2 == SET_SET_POINT_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_SET_SET_POINT)
        {            
            RelayMsgState = RLY_ACKED_SET_SET_POINT;            
        }
    }
    else if(opcode1 == SET_TEMPERATURE_OFFSET_CODE && opcode2 == SET_TEMPERATURE_OFFSET_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_SET_TEMPERATURE_OFFSET)
        {
            RelayMsgState = RLY_ACKED_SET_TEMPERATURE_OFFSET;
        }
    }
    else if(opcode1 == GET_TEMPERATURE_OFFSET_CODE && opcode2 == GET_TEMPERATURE_OFFSET_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_GET_TEMPERATURE_OFFSET)
        {
            currentOffset = msg[7];
            units = msg[8];
            RelayMsgState = RLY_ACKED_GET_TEMPERATURE_OFFSET;
        }
    }
    else if(opcode1 == GET_PRESENT_TEMPERATURE_CODE && opcode2 == GET_PRESENT_TEMPERATURE_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_GET_PRESENT_TEMPERATURE)
        {            
            deviceType = msg[7] << 8 | msg[8];
            units = msg[9];
            setpoint1 = msg[10] << 8 | msg[11];
            
            RelayMsgState = RLY_ACKED_GET_PRESENT_TEMPERATURE;
        }
    }    
    else if(opcode1 == SET_ENERGY_PRICE_CODE && opcode2 == SET_ENERGY_PRICE_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_SET_ENERGY_PRICE) {
            RelayMsgState = RLY_ACKED_SET_ENERGY_PRICE;
        }
    }
    else if(opcode1 == GET_ENERGY_PRICE_CODE && opcode2 == GET_ENERGY_PRICE_REPLY_CODE)
    {
        if(RelayMsgState == RLY_WAITING_GET_ENERGY_PRICE)
        {
            currentPrice = msg[7] << 8 | msg[8];
            currencyCode = msg[9] << 8 | msg[10];
            digitsAfterPoint = msg[11];
                                   
            if(mlen > 8)
            {
                expirationTime = msg[12] << 24 | msg[13] << 16 | msg[14] << 8 | msg[15];
                if(mlen > 12)
                {
                    nextPrice = msg[16] << 24 | msg[17] << 16 | msg[18] << 8 | msg[19];
                    nOptions = 2;
                }
                else
                {
                    nOptions = 1;
                }
            }
            else
            {
                nOptions = 0;
            }
        }
    }
    else
    {           
        httpCode = 404;
    }
}

void BasicDRMessageHandler(unsigned char * msg)
{
    unsigned char opcode1 = msg[4];
    unsigned char opcode2 = msg[5];
    
    ResponseReadyFlag = TRUE;
   
    if(opcode1 == CUSTOMER_OVERRIDE_CODE) //customer override
    {
        if(opcode2 == 0)
        {
            override = 0;
        }
        else
        {
            override = 1;
            TimeMonitorRegisterI(10,OVERRIDE_DURATION,OverrideTimeoutCallback);
        }
    }
    else if (opcode1 == RESPONSE_OP_STATE_CODE) //opstate response
    {
        if(RelayMsgState == RLY_WAITING_OP_STATE)
        {
            RelayMsgState = RLY_ACKED_OP_STATE;
            //this switch statement will be redone when the REST API documentation for this message type is completed
            codeByte = opcode2;
            switch(opcode2)
            {
                case IDLE_NORMAL:
                    httpCode = 200;
                    codeByte = IDLE_NORMAL;
                    break;
                case RUNNING_NORMAL:
                    httpCode = 200;
                    codeByte = RUNNING_NORMAL;
                    break;
                case RUNNING_CURTAILED:
                    httpCode = 200;
                    codeByte = RUNNING_CURTAILED;
                    break;
                case RUNNING_HEIGHTENED:
                    httpCode = 200;
                    codeByte = RUNNING_HEIGHTENED;
                    break;
                case IDLE_CURTAILED:
                    httpCode = 200;
                    codeByte = IDLE_CURTAILED;
                    break;
                case SGD_ERROR_CONDITION:
                    httpCode = 200;
                    codeByte = SGD_ERROR_CONDITION;
                    break;
                case IDLE_HEIGHTENED:
                    httpCode = 200;
                    codeByte = IDLE_HEIGHTENED;
                    break;
                case CYCLING_ON:
                    httpCode = 200;
                    codeByte = CYCLING_ON;
                    break;
                case CYCLING_OFF:
                    httpCode = 200;
                    codeByte = CYCLING_OFF;
                    break;
                case VARIABLE_FOLLOWING:
                    httpCode = 200;
                    codeByte = VARIABLE_FOLLOWING;
                    break;
                case VARIABLE_NOT_FOLLOWING:
                    httpCode = 200;
                    codeByte = VARIABLE_NOT_FOLLOWING;
                    break;
                case IDLE_OPTED_OUT:
                    httpCode = 200;
                    codeByte = IDLE_OPTED_OUT;
                    break;
                case RUNNING_OPTED_OUT:
                    httpCode = 200;
                    codeByte = RUNNING_OPTED_OUT;
                    break;
                default:
                    httpCode = 400;
                    codeByte = 0;
                    break;                    
            }
        }
    }
    else if(opcode1 == APP_ACK_CODE) //application ack
    {
        //ack indicates success, set httpCode accordingly
        httpCode = 200;
        
        if(opcode2 == END_SHED_CODE && RelayMsgState == RLY_WAITING_END_SHED)
        {
            RelayMsgState = RLY_ACKED_END_SHED;
        }
        else if(opcode2 == SHED_CODE && RelayMsgState == RLY_WAITING_SHED)
        {
            RelayMsgState = RLY_ACKED_SHED;            
        }
        else if(opcode2 == LOAD_UP_CODE && RelayMsgState == RLY_WAITING_LOAD_UP)
        {
            RelayMsgState = RLY_ACKED_LOAD_UP;            
        }
        else if(opcode2 == REQUEST_POWER_LEVEL_CODE && RelayMsgState == RLY_WAITING_REQUEST_POWER_LEVEL)
        {
            RelayMsgState = RLY_ACKED_REQUEST_POWER_LEVEL;
        }
        else if(opcode2 == PRESENT_RELATIVE_PRICE_CODE && RelayMsgState == RLY_WAITING_PRESENT_RELATIVE_PRICE)
        {
            RelayMsgState = RLY_ACKED_PRESENT_RELATIVE_PRICE;
        }
        else if(opcode2 == NEXT_PERIOD_RELATIVE_PRICE_CODE && RelayMsgState == RLY_WAITING_NEXT_PERIOD_RELATIVE_PRICE)
        {
            RelayMsgState = RLY_ACKED_NEXT_PERIOD_RELATIVE_PRICE;
        }
        else if(opcode2 == TIME_IN_PRICE_PERIOD_CODE && RelayMsgState == RLY_WAITING_TIME_IN_PRICE_PERIOD)
        {
            RelayMsgState = RLY_ACKED_TIME_IN_PRICE_PERIOD;
        }
        else if(opcode2 == CRITICAL_PEAK_EVENT_CODE && RelayMsgState == RLY_WAITING_CRITICAL_PEAK_EVENT)
        {
            RelayMsgState = RLY_ACKED_CRITICAL_PEAK_EVENT;
        }
        else if(opcode2 == GRID_EMERGENCY_CODE && RelayMsgState == RLY_WAITING_GRID_EMERGENCY)
        {
            RelayMsgState = RLY_ACKED_GRID_EMERGENCY;
        }
        else if(opcode2 == GRID_GUIDANCE_CODE && RelayMsgState == RLY_WAITING_GRID_GUIDANCE)
        {
            RelayMsgState = RLY_ACKED_GRID_GUIDANCE;
        }
        else if(opcode2 == OUTSIDE_COMM_CODE && RelayMsgState == RLY_WAITING_OUTSIDE_COMM_GOOD)
        {
            RelayMsgState = RLY_ACKED_OUTSIDE_COMM_GOOD;
        }
        else if(opcode2 == OUTSIDE_COMM_CODE && RelayMsgState == RLY_WAITING_OUTSIDE_COMM_LOST)
        {
            RelayMsgState = RLY_ACKED_OUTSIDE_COMM_LOST;
        }
        else if(opcode2 == SIMPLE_TIME_SYNC_CODE && RelayMsgState == RLY_WAITING_SIMPLE_TIME_SYNC)
        {
            RelayMsgState = RLY_ACKED_SIMPLE_TIME_SYNC;
        }
        else if(opcode2 == PENDING_EVENT_CODE && RelayMsgState == RLY_WAITING_PENDING_EVENT)
        {
            RelayMsgState = RLY_ACKED_PENDING_EVENT;
        }
        else if(opcode2 == PENDING_EVENT_TYPE_CODE && RelayMsgState == RLY_WAITING_PENDING_EVENT_TYPE)
        {
            RelayMsgState = RLY_ACKED_PENDING_EVENT_TYPE;
        }
        else if(opcode2 == GET_COMMODITY_READ_CODE && RelayMsgState == RLY_WAITING_GET_COMMODITY_READ)
        {
            if(RelayMsgState)
            {
                RelayMsgState = RLY_ACKED_GET_COMMODITY_READ;
            }                
        }
        else
        {
            //there must be a mismatch between the expected response type and what has been received
            httpCode = 500;
        }
        
    }
    else if(opcode1 == APP_NAK_CODE) //application nak
    {
        
        switch (opcode2) 
        {
            case APPNAK_NO_REASON:
                httpCode = 401;
                break;
            case APPNAK_OPCODE1_NOT_SUPPORTED:
                httpCode = 501;
                break;
            case APPNAK_OPCODE2_INVALID:
                httpCode = 404;
                break;
            case APPNAK_BUSY:
                httpCode = 403;
                break;
            case APPNAK_LENGTH_INVALID:
                httpCode = 414;
                break;
            default:
                httpCode = 500;
                break;
        }
        
    }
    
}



/**
 * This will cause a send function to stop blocking and return a failure code if
 * there is no timely response from the MCI layer.
 */
void RelayTimeoutCallback(void)
{    
    httpCode = 500;
    ResponseReadyFlag = TRUE;
}

/**
 * this terminates a customer override after a period of time has passed
 * 
 */
void OverrideTimeoutCallback(void)
{
    override = 0;
}

/**
 * Checks a flag to determine if the MCI layer has received an application response
 * from the SCG. The return value is used to decide whether to continue blocking or not.
 */
BOOL CheckDataValid(void)
{
    if(ResponseReadyFlag == FALSE)
    {
        return FALSE;
    }
    else
    {
        LED1_ON()
        LED1_OFF()
        return TRUE;
    }
}

void BlockUntilReady(void)
{    
    BOOL RelayDataValid = FALSE;
    
    //set up a callback to ensure that the function returns
    TimeMonitorRegisterI(8,RELAY_TIMEOUT_PERIOD,RelayTimeoutCallback);
            
    //block until an application response has been received and processed
    while(!RelayDataValid)
    {
        RelayDataValid = CheckDataValid();
    }
    
    TimeMonitorCancelI(8);          // cancel timeout callback in case we got here without it
    ResponseReadyFlag = FALSE;      // reset data valid flag
    RelayMsgState = RLY_IDLE;
}

RelayMsg SendShedCommand( int eventDuration)
{
    RelayMsg retval;
    unsigned char messageBuffer[8];    
    unsigned char opcode2 = 0;
    
    memcpy(messageBuffer, ShedCommand,8);  
    
    opcode2 = MakeDurationByte(eventDuration);
    messageBuffer[5] = opcode2;
    
    RelayMsgState = RLY_WAITING_SHED;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);    
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

RelayMsg SendEndShedCommand(void)
{
    RelayMsg retval;
    RelayMsgState = RLY_WAITING_END_SHED;
    //the second opcode byte isn't used by the End Shed command
    
    httpCode = 500;
    MCISendNeutral(EndShedCommand);    
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

/**
 * SendRequestForPowerLevel(int percent, int produce)
 * brief: Sends the Basic DR request for an SCG to produce/consume a percentage of 
 * its maximum power. The percent argument is converted to 7 bit precision, with 
 * the MSB of the second opcode byte being used to specify whether the device is
 * to produce or consume power
 * 
 * @param percent: the percentage of rated power that will be consumed
 * @param produce: 1 if the device should be producing power, 0 if consuming power
 */
RelayMsg SendRequestForPowerLevel(int percent)
{
    RelayMsg retval;
    int produce = 0;
    unsigned char opcode2 = 0x00;
    unsigned char messageBuffer[8];               
    memcpy(messageBuffer, RequestPowerLevel,8);
    
    if(percent < 0)
    {
        produce = 1;
        percent = -1*percent;
    }
    //limit percent value to 100%
    
    if(percent > 100)
    {
        percent = 100;
    }
     
    percent = (int) (1.27*(float)percent);
            
    opcode2 = (unsigned char) ((produce << 7) | percent); 
    
    
    
    messageBuffer[5] = opcode2;
    
    RelayMsgState = RLY_WAITING_REQUEST_POWER_LEVEL;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

RelayMsg SendPresentRelativePrice(double rpi) 
{
    RelayMsg retval;
    unsigned char opcode2 = 0x00;
    unsigned char messageBuffer[8];    
    memcpy(messageBuffer, PresentRelativePrice,8);
    
    
    //input bounds checking
    if(rpi > 9.79)
    {
        rpi = 9.79;
    }
    else if(rpi < 0.0)
    {
        rpi = 0.0;
    }

    opcode2 = (unsigned char) floor(-31.0 + 0.5*sqrtf(3844.0+(4.0*(63.0+8192.0*rpi))));    //calculate byte value per section 8.2.2 of CTA spec
    messageBuffer[5] = opcode2;
    
    RelayMsgState = RLY_WAITING_PRESENT_RELATIVE_PRICE;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);
        
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

RelayMsg SendNextPeriodRelativePrice(double rpi) 
{
    RelayMsg retval;
    unsigned char opcode2 = 0x00;
    unsigned char messageBuffer[8];    
    memcpy(messageBuffer, NextPeriodRelativePrice,8);
    
    
    //input bounds checking
    if(rpi > 9.79)
    {
        rpi = 9.79;
    }
    else if(rpi < 0.0)
    {
        rpi = 0.0;
    }

    opcode2 = (unsigned char) floor(-31.0 + 0.5*sqrtf(3844.0+(4.0*(63.0+8192.0*rpi))));    //calculate byte value per section 8.2.2 of CTA spec
    messageBuffer[5] = opcode2;
    
    RelayMsgState = RLY_WAITING_NEXT_PERIOD_RELATIVE_PRICE;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);
        
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

RelayMsg SendTimeRemainingInPresentPricePeriod(int eventDuration)
{
    RelayMsg retval;
    unsigned char messageBuffer[8];    
    memcpy(messageBuffer, TimeRemainingInPresentPricePeriod,8);   
    unsigned char opcode2 = 0;
    
    opcode2 = MakeDurationByte(eventDuration);
    messageBuffer[5] = opcode2;
    
    RelayMsgState = RLY_WAITING_TIME_IN_PRICE_PERIOD;
    ExpectedAckType = TIME_IN_PRICE_PERIOD_CODE;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);    
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

RelayMsg SendCriticalPeakEvent(int eventDuration)
{
    RelayMsg retval;
    unsigned char messageBuffer[8];    
    memcpy(messageBuffer, CriticalPeakEvent,8);   
    unsigned char opcode2 = 0;
    
    opcode2 = MakeDurationByte(eventDuration);
    messageBuffer[5] = opcode2;
    
    RelayMsgState = RLY_WAITING_CRITICAL_PEAK_EVENT;
    ExpectedAckType = CRITICAL_PEAK_EVENT_CODE;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);    
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

RelayMsg SendGridEmergency(int eventDuration)
{
    RelayMsg retval;
    unsigned char messageBuffer[8];    
    memcpy(messageBuffer, GridEmergency,8);   
    unsigned char opcode2 = 0;
    
    opcode2 = MakeDurationByte(eventDuration);
    messageBuffer[5] = opcode2;
    
    RelayMsgState = RLY_WAITING_GRID_EMERGENCY;
    ExpectedAckType = GRID_EMERGENCY_CODE;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);    
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

RelayMsg SendLoadUp(int eventDuration)
{
    RelayMsg retval;
    unsigned char messageBuffer[8];    
    memcpy(messageBuffer, LoadUp,8);   
    unsigned char opcode2 = 0;
    
    messageBuffer[5] = MakeDurationByte(eventDuration);
    
    RelayMsgState = RLY_WAITING_LOAD_UP;
    ExpectedAckType = LOAD_UP_CODE;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);    
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

RelayMsg SendTimeSync(int day, int hour)
{
    RelayMsg retval;
    unsigned char messageBuffer[8];
    memcpy(messageBuffer, TimeSync, 8);
    unsigned char opcode2 = 0;
    
    //input checking
    if(day < 0)
    {
        day = 0;
    }
    else if(day > 6)
    {
        day = 6;
    }
    
    if(hour < 0)
    {
        hour = 0;
    }
    else if(hour > 24)
    {
        hour = 24;
    }
    
    opcode2 = day << 5 | hour;        
    messageBuffer[5] = opcode2;
    
    RelayMsgState = RLY_WAITING_SIMPLE_TIME_SYNC;
    ExpectedAckType = SIMPLE_TIME_SYNC_CODE;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    
    return retval;
}

RelayMsg SendQueryOpState(void)
{
    RelayMsg retval;
    RelayMsgState = RLY_WAITING_OP_STATE;
    ExpectedAckType = QUERY_OP_STATE_CODE;
    
    httpCode = 500;
    MCISendNeutral(QueryOpState);
        
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = codeByte;
    return retval;
}

DeviceInfoRelayMsg SendInfoRequest()
{
    DeviceInfoRelayMsg retval;
    RelayMsgState = RLY_WAITING_INFO_REQUEST;
    ExpectedAckType = INFO_REQUEST_CODE;
    
    httpCode = 500;
    MCISendNeutral(InfoRequest);
    
    BlockUntilReady();
    
    retval.DevInfo = DeviceInfo;
    retval.httpCode = httpCode;
    retval.responseCode = codeByte;
    return retval;
    
}

RelayMsg SendOutsideCommGood(void)
{
    RelayMsg retval;
    RelayMsgState = RLY_WAITING_OUTSIDE_COMM_GOOD; 
    ExpectedAckType = OUTSIDE_COMM_CODE;
    
    httpCode = 500;
    MCISendNeutral(OutsideCommGood);       
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}


RelayMsg SendOutsideCommLost(void)
{
    RelayMsg retval;
    RelayMsgState = RLY_WAITING_OUTSIDE_COMM_LOST;
    ExpectedAckType = OUTSIDE_COMM_CODE;
    
    httpCode = 500;
    MCISendNeutral(OutsideCommLost);    
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

RelayMsg SendSetCommodityRead(unsigned char code, long long rate, long long amount  )
{
    RelayMsg retval;
    RelayMsgState = RLY_WAITING_SET_COMMODITY_READ;
    ExpectedAckType = SET_COMMODITY_READ_CODE;
    unsigned char messageBuffer[21];
    memcpy(messageBuffer,SetCommodityRead,21);
    
    //set the commodity code
    messageBuffer[6] = code;
    
    //set the rate
    messageBuffer[7] = rate >> 40 | BYTE1;
    messageBuffer[8] = rate >> 32 | BYTE1;
    messageBuffer[9] = rate >> 24 | BYTE1;
    messageBuffer[10] = rate >> 16 | BYTE1;
    messageBuffer[11] = rate >> 8 | BYTE1;
    messageBuffer[12] = rate | BYTE1;
    
    //set the cumulative amount
    messageBuffer[13] = amount >> 40 | BYTE1;
    messageBuffer[14] = amount >> 32 | BYTE1;
    messageBuffer[15] = amount >> 24 | BYTE1;
    messageBuffer[16] = amount >> 16 | BYTE1;
    messageBuffer[17] = amount >> 8 | BYTE1;
    messageBuffer[18] = amount | BYTE1;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();
    
    retval.codeByte = responseCode;
    return retval;
}

// TODO: No API yet implemented
CommodityRelayMsg SendGetCommodityRead(unsigned char measured, 
                                        unsigned char commodityCode)
{
    CommodityRelayMsg retval;
    RelayMsgState = RLY_WAITING_GET_COMMODITY_READ;
    ExpectedAckType = GET_COMMODITY_READ_CODE;
    
    if(commodityCode >= 0 && commodityCode <= 7) // if the request is specific
    {
        unsigned char messageBuffer[9];  
        memcpy(messageBuffer, GetCommodityRead, 9);  
        
        if(measured != 0)
        {
            measured = 128;
        }
        commodityCode = commodityCode | measured;
        
        messageBuffer[6] = commodityCode;
        
        httpCode = 500;
        MCISendNeutral(messageBuffer);
    }
    else //if the request is general (if there was a commodity code that wasn't good, we'll pretend it's not there)
    {
        httpCode = 500;
        MCISendNeutral(GetCommodityReadGeneral);
    }
    
    BlockUntilReady();
    
    //transfer data to structure
    retval.nCommodities = nCommodities;             // number of commodities for which we have data
    retval.httpCode = httpCode;
    retval.responseCode = responseCode;
    retval.commodityResponse = commodityResponse;
    return retval;
}

RelayMsg SendStartAutonomousCycling(UINT32 ID,         //event ID 32 bit uint
                                    UINT32 startTime, // in Unix seconds since 1/1/2000 (946702800 seconds after the epoch)
                                    UINT16 duration,           //in minutes
                                    UINT8 dutyCycle,          //as a percentage
                                    UINT8 startRand,          //in minutes
                                    UINT8 endRand,            //in minutes
                                    UINT8 crit                //reserved for future use
                                    )
{
    RelayMsg retval;
    unsigned char messageBuffer[22];
    memcpy(messageBuffer, StartAutonomousCycling, 22);
    
    //convert to network byte ordering
    ReverseByteOrder(&ID,4);
    ReverseByteOrder(&startTime,4);
    ReverseByteOrder(&duration,2);
    //construct packet
    memcpy(&messageBuffer[6],&ID,4);
    memcpy(&messageBuffer[10],&startTime,4);
    memcpy(&messageBuffer[14],&duration, 2);
    
    messageBuffer[16] = dutyCycle;
    messageBuffer[17] = startRand;
    messageBuffer[18] = endRand;
    messageBuffer[19] = crit;  
    
    httpCode = 500;    
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = responseCode;
    return retval;
}

// TODO: no API yet implemented
TempOffsetRelayMsg SendGetTemperatureOffset(void)
{
    TempOffsetRelayMsg retval;
    
    httpCode = 500;
    MCISendNeutral(GetTemperatureOffset);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.responseCode = responseCode;
    retval.currentOffset = currentOffset;
    retval.units = units;
    return retval;
}

// TODO: no API yet implemented
RelayMsg SendSetTemperatureOffset(unsigned char currentOffset,  //offset to be applied in degrees F or C
                                unsigned char units)            //0 for F, 1 for C
{
    RelayMsg retval;
    unsigned char messageBuffer[10];
    memcpy(messageBuffer, SetTemperatureOffset, 10);
    
    //0 means 0, everything else means 1
    if(units != 0)
    {
        units = 1;
    }
    
    messageBuffer[6] = currentOffset;
    messageBuffer[7] = units;
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = responseCode;
    return retval;
}

RelayMsg SendTerminateAutonomousCycling(UINT32 ID,
                                        UINT8 endRand)
{
    RelayMsg retval;
    unsigned char messageBuffer[13];
    memcpy(messageBuffer,TerminateAutonomousCycling,13);
    
    ReverseByteOrder(&ID,4);
    
    memcpy(&messageBuffer[6],&ID,4);
    
    messageBuffer[10] = endRand;    
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = responseCode;
    return retval;
}

TempSetpointRelayMsg SendGetSetPoint(void)
{
    TempSetpointRelayMsg retval;

    RelayMsgState = RLY_WAITING_GET_SET_POINT;
    ExpectedAckType = GET_SET_POINT_CODE;
       
    httpCode = 500;
    MCISendNeutral(GetSetPoint);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.responseCode = responseCode;
    retval.units = units;
    retval.setpoint1 = setpoint1;
    retval.setpoint2 = setpoint2;
    return retval;
}

RelayMsg SendSetSetPoint(UINT16 deviceType,
                        UINT8 units,
                        UINT16 setpoint1,
                        UINT16 setpoint2)
{
    RelayMsg retval;
    unsigned char messageBuffer[15];
    memcpy(messageBuffer,SetSetPoint,15);
    
    ReverseByteOrder(&deviceType,2);
    ReverseByteOrder(&setpoint1,2);
    ReverseByteOrder(&setpoint2,2);
    
    memcpy(&messageBuffer[6],&deviceType,2);
    memcpy(&messageBuffer[8],&units,1);
    memcpy(&messageBuffer[9],&setpoint1,2);
    memcpy(&messageBuffer[11],&setpoint2,2);
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();    
    
    retval.httpCode = httpCode;
    retval.codeByte = responseCode;
    return retval;
}

TempSetpointRelayMsg SendGetPresentTemperature(void)
{
    TempSetpointRelayMsg retval;

    RelayMsgState = RLY_WAITING_GET_PRESENT_TEMPERATURE;
    ExpectedAckType = GET_PRESENT_TEMPERATURE_REPLY_CODE;
       
    httpCode = 500;
    MCISendNeutral(GetPresentTemperature);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.responseCode = responseCode;
    retval.units = units;
    retval.setpoint1 = setpoint1;
    return retval;
}


// TODO: no API yet implemented
EnergyPriceRelayMsg SendGetEnergyPrice(void)
{
    EnergyPriceRelayMsg retval;
    
    httpCode = 500;
    MCISendNeutral(GetEnergyPrice);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.responseCode = responseCode;
    retval.currentPrice = currentPrice;
    retval.currencyCode = currencyCode;
    retval.digitsAfterPoint = digitsAfterPoint;
    retval.expirationTime = expirationTime;
    retval.nextPrice = nextPrice;
    return retval;
}

// TODO: no API yet implemented
RelayMsg SendSetEnergyPrice(UINT16 currentPrice,
                            UINT16 currencyCode,
                            UINT8 digitsAfterPoint,
                            UINT32 expirationTime,
                            UINT32 nextPrice)
{
    RelayMsg retval;
    unsigned char messageBuffer[21];
    memcpy(messageBuffer,SetEnergyPrice,21);
    
    ReverseByteOrder(&currentPrice,2);
    ReverseByteOrder(&currencyCode,2);
    ReverseByteOrder(&expirationTime,4);
    ReverseByteOrder(&nextPrice,4);
    
    memcpy(&messageBuffer[6], &currentPrice, 2);
    memcpy(&messageBuffer[8], &currencyCode, 2);
    messageBuffer[10] = digitsAfterPoint;
    memcpy(&messageBuffer[11], &expirationTime, 4);
    memcpy(&messageBuffer[15], &nextPrice, 4);
    
    httpCode = 500;
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = responseCode;
    return retval;
}

short int HandleIDRResponseCode(unsigned char code)
{
    short int http;
    switch(code)
    {
        case IDRR_SUCCESS:
            http = 200;
            break;
        case IDRR_COMMAND_NOT_IMPLEMENTED:
            http = 501;
            break;
        case IDRR_BAD_VALUE:
            http = 400;
            break;
        case IDRR_COMMAND_LENGTH_ERROR:
            http = 414;
            break;
        case IDRR_RESPONSE_LENGTH_ERROR:
            http = 414;
            break;
        case IDRR_BUSY_CODE:
            http = 401;
            break;
        case IDRR_OTHER_ERROR:
            http = 403;
            break;
        default:
            http = 500;
            break;
    }
    return http;
}

unsigned char MakeDurationByte(int eventDuration)
{
    unsigned char opcode2 = 0;
    float temp;
    
    
    temp = ceil(sqrtf(.5*((float)eventDuration)));
    
    if(eventDuration > 255)
    {
            eventDuration = 255;
    }
    else if(eventDuration < 0)
    {
            eventDuration = 0;
    }
    
    opcode2 = (unsigned char) temp;    
    
    return opcode2;
}

void ReverseByteOrder(void *ptr, int length)
{
    unsigned char buffer;
    int i;
    
    for(i = 0; i < (int)length/2; i++)
    {
        memcpy(&buffer,ptr+i,1);
        memcpy(ptr+i,ptr+length-1-i,1);
        memcpy(ptr+length-1-i, &buffer,1);
    }
}
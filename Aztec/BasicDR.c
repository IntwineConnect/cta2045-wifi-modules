/**
 * BasicDR.c
 * 
 * contains code related to receiving/sending DR messages
 */
#include "MCI_Common.h"
#include "HardwareProfile.h"
#include "BasicDR.h"
#include <math.h>

#include "debugging.h"

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
    RLY_WAITING_GET_SET_POINT,
            RLY_ACKED_GET_SET_POINT,
    RLY_WAITING_SET_SET_POINT,
            RLY_ACKED_SET_SET_POINT,
    RLY_WAITING_GET_ENERGY_PRICE,
            RLY_ACKED_GET_ENERGY_PRICE,
    RLY_WAITING_SET_ENERGY_PRICE,
            RLY_ACKED_SET_ENERGY_PRICE
            
} RelayMsgState;

//storage for commodity data
CommodityReadData commodityResponse[10];

void IntermediateDRMessageHandler(unsigned char *msg)
{
    unsigned char opcode1 = msg[4];
    unsigned char opcode2 = msg[5];
    int mlen = msg[2]*256 + msg[3];
    
    ResponseReadyFlag == TRUE;
    
    //is the message Get Commodity Read message?
    if(opcode1 == GET_COMMODITY_READ_CODE)
    {
        //is the message a Get Commodity Read Reply?
        if(opcode2 == GET_COMMODITY_READ_REPLY_CODE && RelayMsgState == RLY_WAITING_GET_COMMODITY_READ)
        {
            int i = 1;
            int nCommodities = mlen/13;
            RelayMsgState = RLY_ACKED_GET_COMMODITY_READ;
            
            responseCode = msg[6];
            //set the http code in accordance with the intermediate DR response code
            if(responseCode == SUCCESS)
            {
                httpCode = 200;
            }
            else
            {
                httpCode = 400;  //update this to be more informative once REST API spec is updated
            }
            //this structure array holds response information for all commodity types in the message
            CommodityReadData commodityDataArray[nCommodities];
            //figure out the length of the commodity data size for malloc
            //int datasize = nCommodities*sizeof(CommodityReadData);
            
            
            //put data from message buffer into structure for each commodity type
            for(i == 0; i <= nCommodities; i++)
            {
                commodityDataArray[i].commodityCode = msg[7+i*13];
                commodityDataArray[i].instantaneousRate = msg[8+i*13] << 40 | msg[9+i*13] << 32 | msg[10+i*13] << 24 | msg[11+i*13] << 16 | msg[12+i*13] << 8 | msg[13+i*13];
                commodityDataArray[i].cumulativeAmount = msg[14+i*13] << 40 | msg[15+i*13] << 32 | msg[16+i*13] << 24 | msg[17+i*13] << 16 | msg[18+i*13] << 8 | msg[19+i*13];;
            }
            
            /*zero memory in the commodity data buffer before copying so that no old data is left in case
             the length of valid data field is ignored elsewhere*/
            memset(commodityResponse,0,COMMODITY_DATA_BUFFER_LENGTH*sizeof(CommodityReadData));
            //copy received data to buffer
            memcpy(commodityResponse, commodityDataArray, COMMODITY_DATA_BUFFER_LENGTH*sizeof(CommodityReadData));  
            
            ResponseReadyFlag = TRUE;
        }
    }
    else if(opcode1 == START_AUTONOMOUS_CYCLING_CODE)
    {
        if(opcode2 == START_AUTONOMOUS_CYCLING_REPLY_CODE && RelayMsgState == RLY_WAITING_START_AUTONOMOUS_CYCLING)
        {
            unsigned char responseCode = msg[6];
            RelayMsgState = RLY_ACKED_START_AUTONOMOUS_CYCLING;
            if(responseCode == SUCCESS)
            {
                httpCode = 200;
            }
            else
            {
                httpCode = 400;
            }
            ResponseReadyFlag = TRUE;
        }
    }
    else if(opcode1 == TERMINATE_AUTONOMOUS_CYCLING_CODE)
    {
        //if this is a response and we are expecting a response to this message type
        if(opcode2 == TERMINATE_AUTONOMOUS_CYCLING_REPLY_CODE && RelayMsgState == RLY_WAITING_TERMINATE_AUTONOMOUS_CYCLING)
        {
            unsigned char responseCode = msg[6];
            RelayMsgState = RLY_ACKED_TERMINATE_AUTONOMOUS_CYCLING;
            if(responseCode == SUCCESS)
            {
                httpCode = 200;
            }
            else
            {
                httpCode = 400;
            }
        }
    }
    else if(opcode1 == SET_TEMPERATURE_OFFSET_CODE)
    {
        if(opcode2 == SET_TEMPERATURE_OFFSET_REPLY_CODE && RelayMsgState == RLY_WAITING_SET_TEMPERATURE_OFFSET)
        {
            
            
        }
    }
    else if(opcode1 == GET_TEMPERATURE_OFFSET_CODE)
    {
        if(opcode2 == GET_TEMPERATURE_OFFSET_REPLY_CODE && RelayMsgState == RLY_WAITING_GET_TEMPERATURE_OFFSET)
        {
            
        }
    }
    else if(opcode1 == SET_SET_POINT_CODE)
    {
        if(opcode2 == SET_SET_POINT_REPLY_CODE && RelayMsgState == RLY_WAITING_SET_SET_POINT)
        {
            
        }
    }
    else if(opcode1 == GET_SET_POINT_CODE)
    {
        if(opcode2 == GET_SET_POINT_REPLY_CODE && RelayMsgState == RLY_WAITING_GET_SET_POINT)
        {
            
        }
    }
    else if(opcode1 == SET_ENERGY_PRICE_CODE && RelayMsgState == RLY_WAITING_SET_ENERGY_PRICE)
    {
        if(opcode2 == SET_ENERGY_PRICE_REPLY_CODE)
        {
            
        }
    }
    else if(opcode1 == GET_ENERGY_PRICE_CODE)
    {
        if(opcode2 == GET_ENERGY_PRICE_REPLY_CODE && RelayMsgState == RLY_WAITING_GET_ENERGY_PRICE)
        {
            
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
        else if(opcode2 == PENDING_EVENT_TYPE_CODE && RelayMsgState == RLY_WAITING_PENDING_EVENT)
        {
            RelayMsgState = RLY_ACKED_PENDING_EVENT;
        }
        else if(opcode2 == PENDING_EVENT_TYPE_CODE && RelayMsgState == RLY_WAITING_PENDING_EVENT_TYPE)
        {
            RelayMsgState = RLY_ACKED_PENDING_EVENT_TYPE;
        }
        else if(opcode2 == GET_COMMODITY_READ_CODE && RelayMsgState == RLY_WAITING_GET_COMMODITY_READ)
        {
            RelayMsgState = RLY_ACKED_GET_COMMODITY_READ;
        }
        else
        {
            //there must be a mismatch between the expected response type and what has been received
            httpCode = 400;
            DL_Nak(0xa0);
        }
        
    }
    else if(opcode1 == APP_NAK_CODE) //application nak
    {
        httpCode = 400;
        if(opcode2 == END_SHED_CODE && RelayMsgState == RLY_WAITING_END_SHED)
        {
            RelayMsgState = RLY_ACKED_END_SHED;
        }
        else if(opcode2 == SHED_CODE && RelayMsgState == RLY_WAITING_SHED)
        {
            RelayMsgState = RLY_ACKED_SHED;
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
        else if(opcode2 == PENDING_EVENT_TYPE_CODE && RelayMsgState == RLY_WAITING_PENDING_EVENT)
        {
            RelayMsgState = RLY_ACKED_PENDING_EVENT;
        }
        else if(opcode2 == PENDING_EVENT_TYPE_CODE && RelayMsgState == RLY_WAITING_PENDING_EVENT_TYPE)
        {
            RelayMsgState = RLY_ACKED_PENDING_EVENT_TYPE;
        }
        else if(opcode2 == GET_COMMODITY_READ_CODE && RelayMsgState == RLY_WAITING_GET_COMMODITY_READ)
        {
            RelayMsgState = RLY_ACKED_GET_COMMODITY_READ;
        }
        else
        {
            //there must be a mismatch between the expected response type and what has been received
            httpCode = 400;
            DL_Nak(0xa4);
        }
    }
    
}

/**
 * This will cause a send function to stop blocking and return a failure code if
 * there is no timely response from the MCI layer.
 */
void RelayTimeoutCallback(void)
{
    httpCode = 400;  
    ResponseReadyFlag = TRUE;
    LED2_ON()
    //LED1_OFF()
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
        return TRUE;
    }
}

void BlockUntilReady(void)
{    
    BOOL RelayDataValid = FALSE;
    
    //set up a callback to ensure that the function returns
    TimeMonitorRegisterI(8,RELAY_TIMEOUT_PERIOD,RelayTimeoutCallback);
    LED1_ON()
            
    //block until an application response has been received and processed
    while(!RelayDataValid)
    { 
        RelayDataValid = CheckDataValid();
    }    
    
    TimeMonitorCancelI(4);          // cancel timeout callback in case we got here without it
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
    MCISendNeutral(messageBuffer);    
    
    BlockUntilReady();
    LED2_ON()
    retval.httpCode = httpCode;
    retval.codeByte = DEFAULT_RETURN_CODE;
    return retval;
}

RelayMsg SendEndShedCommand(void)
{
    RelayMsg retval;
    RelayMsgState = RLY_WAITING_END_SHED;
    //the second opcode byte isn't used by the End Shed command
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
RelayMsg SendRequestForPowerLevel(double percent, int produce)
{
    RelayMsg retval;
    unsigned char opcode2 = 0x00;
    unsigned char messageBuffer[8];               
    memcpy(messageBuffer, RequestPowerLevel,8);
    
    //limit percent value to 100%
    if(percent > 100.0)
    {
        percent = 100.0;
    }
    else if (percent < 0.0)
    {
        percent = 0.0;
    }    
    percent = 1.27 * percent;

        
    opcode2 = (unsigned char) percent; 
    
    if(produce != 0)
    {
        opcode2 = opcode2 | (1 << 7);   //set MSB according to specifications
    }
    
    messageBuffer[5] = opcode2;
    
    RelayMsgState = RLY_WAITING_REQUEST_POWER_LEVEL;
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
    if(rpi > 10.0)
    {
        rpi = 10.0;
    }
    else if(rpi < 0.0)
    {
        rpi = 0.0;
    }

    opcode2 = (unsigned char) floor(-31 + .5*sqrtf(3844+(4*(63+8192*rpi))));    //calculate byte value per section 8.2.2 of CTA spec
    messageBuffer[5] = opcode2;
    
    RelayMsgState = RLY_WAITING_PRESENT_RELATIVE_PRICE;
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
    MCISendNeutral(QueryOpState);
        
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = codeByte;
    return retval;
}

RelayMsg SendOutsideCommGood(void)
{
    RelayMsg retval;
    RelayMsgState = RLY_WAITING_OUTSIDE_COMM_GOOD; 
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
    MCISendNeutral(OutsideCommLost);    
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte - DEFAULT_RETURN_CODE;
    return retval;
}

CommodityRelayMsg SendGetCommodityRead(unsigned char measured, 
                                        unsigned char commodityCode)
{
    CommodityRelayMsg retval;
    RelayMsgState = RLY_WAITING_GET_COMMODITY_READ;
    
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
        
        MCISendNeutral(messageBuffer);
    }
    else //if the request is general (if there was a commodity code that wasn't good, we'll pretend it's not there)
    {
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
    
    
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = responseCode;
    return retval;
}

TempOffsetRelayMsg SendGetTemperatureOffset(void)
{
    TempOffsetRelayMsg retval;
    
    MCISendNeutral(GetTemperatureOffset);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.responseCode = responseCode;
    retval.currentOffset = currentOffset;
    retval.units = units;
    return retval;
}

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
    
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = responseCode;
    return retval;
}

TempSetpointRelayMsg SendGetSetPoint(void)
{
    TempSetpointRelayMsg retval;
    
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
    memcpy(&messageBuffer[9],&setpoint1,2);
    memcpy(&messageBuffer[11],&setpoint2,2);
    
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();    
    
    retval.httpCode = httpCode;
    retval.codeByte = responseCode;
    return retval;
}

EnergyPriceRelayMsg SendGetEnergyPrice(void)
{
    EnergyPriceRelayMsg retval;
    
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
    
    MCISendNeutral(messageBuffer);
    
    BlockUntilReady();
    
    retval.httpCode = httpCode;
    retval.codeByte = responseCode;
    return retval;
}

unsigned char MakeDurationByte(int eventDuration)
{
    unsigned char opcode2 = 0;
    
    eventDuration = sqrtf(.5*eventDuration);
    
    if(eventDuration > 256)
    {
            eventDuration = 256;
    }
    else if(eventDuration < 0)
    {
            eventDuration = 0;
    }
    
    opcode2 = (unsigned char) ceil(eventDuration);
    
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
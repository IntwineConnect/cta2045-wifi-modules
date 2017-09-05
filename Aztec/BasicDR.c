/**
 * BasicDR.c
 * 
 * contains code related to receiving/sending basic DR messages
 */
#include "MCI_Common.h"
#include "HardwareProfile.h"
#include "BasicDR.h"
#include <math.h>


//when a UCM has sent a message that 
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
            RLY_ACKED_PENDING_EVENT_TYPE
            
} RelayMsgState;


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
                    break;
                case RUNNING_NORMAL:
                    httpCode = 200;
                    break;
                case RUNNING_CURTAILED:
                    httpCode = 200;
                    break;
                case RUNNING_HEIGHTENED:
                    httpCode = 200;
                    break;
                case IDLE_CURTAILED:
                    httpCode = 200;
                    break;
                case SGD_ERROR_CONDITION:
                    httpCode = 200;
                    break;
                case IDLE_HEIGHTENED:
                    httpCode = 200;
                    break;
                case CYCLING_ON:
                    httpCode = 200;
                    break;
                case CYCLING_OFF:
                    httpCode = 200;
                    break;
                case VARIABLE_FOLLOWING:
                    httpCode = 200;
                    break;
                case VARIABLE_NOT_FOLLOWING:
                    httpCode = 200;
                    break;
                case IDLE_OPTED_OUT:
                    httpCode = 200;
                    break;
                case RUNNING_OPTED_OUT:
                    httpCode = 200;
                    break;
                default:
                    httpCode = 400;
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
        return TRUE;
    }
}

void BlockUntilReady(void)
{    
    BOOL RelayDataValid = FALSE;
    
    //set up a callback to ensure that the function returns
    TimeMonitorRegisterI(4,RELAY_TIMEOUT_PERIOD,RelayTimeoutCallback);
    
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
    
//    BlockUntilReady();  // BH: It's unclear to me how this will ever return...
    
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
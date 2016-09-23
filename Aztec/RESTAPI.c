#include "RESTAPI.h"

/*this function checks to see if any of received message's headers are those
 associated with the REST API and processes them if they are*/

MCI_MESSAGE_TYPE eventType;

int parseEventName(char *buffer)
{
    MCI_MESSAGE_TYPE retval = 1;
    if(memcmp(buffer,"critical_peak", 13) == 0)
    {
        eventType = CRITICAL_PEAK_EVENT;
    }
    else if(memcmp(buffer, "shed", 4 ) == 0)
    {
        eventType = SHED;
    }    
    else if(memcmp(buffer,"grid_emergency", 14) == 0)
    {
        eventType = GRID_EMERGENCY;
    }
    else if(memcmp(buffer,"change_level", 12) == 0)
    {
        eventType = REQUEST_POWER_LEVEL;
    }
    else if(memcmp(buffer,"normal", 6) == 0)
    {
        eventType = END_SHED;
    }
    else if(memcmp(buffer,"load_up", 7) == 0)
    {
        eventType = LOAD_UP;
    }
    else if(memcmp(buffer,"get_commodity", 13) == 0)
    {
        eventType = GET_COMMODITY_READ;
    }
    else if(memcmp(buffer,"set_commodity", 13) == 0)
    {
        eventType = SET_COMMODITY_READ;
    }
    else if(memcmp(buffer,"start_cycling", 13) == 0)
    {
        eventType = START_AUTONOMOUS_CYCLING;
    }
    else if(memcmp(buffer,"stop_cycling", 12) == 0)
    {
        eventType = TERMINATE_AUTONOMOUS_CYCLING;
    }
    else if(memcmp(buffer,"opt_out", 7) == 0)
    {
        eventType = OPT_OUT;
    }
    else
    {
        //if we end up here, we've got an unsupported type
        eventType = UNKNOWN_TYPE;
        retval = 0;
    }
    
    return retval;  
        
}


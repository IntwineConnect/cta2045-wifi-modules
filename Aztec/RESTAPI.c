#include "RESTAPI.h"

/*this function checks to see if any of received message's headers are those
 associated with the REST API and processes them if they are*/

MCI_MESSAGE_TYPE eventType;

int parseEventName(char *buffer)
{
    MCI_MESSAGE_TYPE retval = 1;
    
    if(!memcmp(buffer, "shed"))
    {
        eventType = SHED;
    }
    else if(!memcmp(buffer,"critical_peak"))
    {
        eventType = CRITICAL_PEAK_EVENT;
    }
    else if(!memcmp(buffer,"grid_emergency"))
    {
        eventType = GRID_EMERGENCY;
    }
    else if(!memcmp(buffer,"change_level"))
    {
        eventType = REQUEST_POWER_LEVEL;
    }
    else if(!memcmp(buffer,"normal"))
    {
        eventType = END_SHED;
    }
    else
    {
        //if we end up here, we've got an unsupported type
        eventType = UNKNOWN_TYPE;
        retval = 0;
    }
    
    return retval;  
        
}


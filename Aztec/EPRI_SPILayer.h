/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _EPRI_SPILAYER_H    /* Guard against multiple inclusion */
#define _EPRI_SPILAYER_H

#include "MCI_Common.h"

void SPI_Driver_Task();
void SPI_Driver_Send_ACK_NAK(unsigned char * msg);
MCIResponse MCISendSPI(unsigned char * msg);

void MCISendAsyncSPI(unsigned char * msg);

#endif /* _EPRI_SPILAYER_H */

/* *****************************************************************************
 End of File
 */

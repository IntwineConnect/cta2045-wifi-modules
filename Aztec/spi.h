#ifndef SPI_H
#define SPI_H

void spiEnable(unsigned char spi_channel);
void WriteMeterChip(BYTE addr, BYTE numbits, DWORD value);
DWORD ReadMeterChip(BYTE addr, BYTE numbits);

#endif

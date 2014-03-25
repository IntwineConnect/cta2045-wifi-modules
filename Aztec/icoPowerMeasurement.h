#ifndef ICOPOWERMEASUREMENT_H
#define ICOPOWERMEASUREMENT_H

#ifdef INTWINE_CONNECTED_OUTLET

#include "GenericTypeDefs.h"

void PowerMeasurementInit(void);
void PowerMeasurementTask(void);

void ResetCumulativeEnergy(void);

#endif

#endif

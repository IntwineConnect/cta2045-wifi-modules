  #include "HardwareProfile.h"

#include "Metrics.h"
#include "eeprom.h"

#define INTWINE_METRICS_VERSION          1

void ClearMetrics(void);            // Clear data and write
void RestoreMetrics(void);          // Run restore/startup algorithm
void WriteMetrics(void);            // Write current contents to next block

typedef struct
{
    int MagicNumber;           // 0xBADCAFE2
    int MetricsVersion;        // Start at 1
    int SequenceNumber;        // Start at 1

} INTWINE_METRICS_HEADER;

typedef struct
{
    INTWINE_METRICS_HEADER Header;

    int iMetricsData[METRICS_MAX_INDEX];

} INTWINE_METRICS_VER_1;

static INTWINE_METRICS_VER_1 iMetric1 = 
{
    { 0, INTWINE_METRICS_VERSION },  // use invalid magic number
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
};

// Defines and globals

#define WEAR_BLOCKS             8           // 8 ..
#define WEAR_BLOCK_SIZE         0x1000      // .. wear blocks of 4K each

static int iWearAverageSequenceNumber  = 1;    // Increments forever
static int iWearAverageIndex           = 0;    // Current index

void SetMetrics(METRICS_INDEX index, int value)
{
    // Steps to working with a new device state
    // 1) Determine if good index and if the value has changed
    // 2) If it has:
    // 2a) Set the new value
    // 2b) Write the new structure
    // 2c) Set read required flag so that next 

    RestoreMetrics();  // Runs once at startup

    // 1)
    if(index < METRICS_MAX_INDEX)
    {
        if(value != iMetric1.iMetricsData[index])
        {
            // 2a)
            iMetric1.iMetricsData[index] = value;

            // 2b)
            WriteMetrics();
        }
    }
}

int GetMetrics(METRICS_INDEX index)
{
    int retval = 0;

    // Steps to working with a new device state
    // 1) Determine if good index
    // 2) Return value

    RestoreMetrics();  // Runs once at startup

    // 1)
    if(index < METRICS_MAX_INDEX)
    {
        retval = iMetric1.iMetricsData[index];
    }
    return(retval);
}

void ClearMetrics(void)
{
    int i, size;

    // Clear/initialize device state data and nv
    // 1) Clear the device state structure
    // 2) Erase/Write structure

    // 1)
    size = sizeof(INTWINE_METRICS_VER_1);
    memset((char *)&iMetric1.Header.MagicNumber, 0, size);

    for(i=0; i<WEAR_BLOCKS; i++)
    {
        WriteMetrics(); // Will set header, seqeunce and write to next block
    }
}

void IncrementMetrics(METRICS_INDEX index)
{
    int value;
    value = GetMetrics(index);
    SetMetrics(index, value+1);
}

void RestoreMetrics(void)
{
    int iHighWaterSequenceNumber = 0;
    int iBlockToUse = 0;
    int i, address, size;
    static int restore_metrics = 1;

    // One time only
    if(restore_metrics == 0)
    {
        return;
    }
    restore_metrics = 0;

    // Steps to restoring metrics
    // 1) Read each block looking for valid block with highest sequence number and version
    // 2) Never initialized, clear
    // 3) Restore that block to the structure in use

    // 1)
    size = sizeof(INTWINE_METRICS_VER_1);
    for(i=0; i<WEAR_BLOCKS; i++)
    {
        address = EEPROM_ADDRESS_METRICS_INFO + (i * WEAR_BLOCK_SIZE);
        EEReadBlock(address, size, (char *)&iMetric1.Header.MagicNumber);
        if(iMetric1.Header.MagicNumber == MAGIC_NUMBER_METRICS_INFO)
        {
            if(iMetric1.Header.SequenceNumber > iHighWaterSequenceNumber)
            {
                iHighWaterSequenceNumber = iMetric1.Header.SequenceNumber;
                iBlockToUse = i;
            }
        }
    }

    if(iHighWaterSequenceNumber == 0)
    {
        // 2)
        ClearMetrics();
    }
    else
    {
        // 3)
        address = EEPROM_ADDRESS_METRICS_INFO + (iBlockToUse * WEAR_BLOCK_SIZE);
        EEReadBlock(address, size, (char *)&iMetric1.Header.MagicNumber);
        // double check magic
        if(iMetric1.Header.MagicNumber == MAGIC_NUMBER_METRICS_INFO)
        {
            iWearAverageSequenceNumber  = iMetric1.Header.SequenceNumber;
            iWearAverageIndex           = iBlockToUse;
        }
        else
        {
            // for some reason magic does not match
            ClearMetrics();
        }
    }
}


// Assumes iMetric1 structure contains the data to write
void WriteMetrics(void)
{
    int size, address;

    // 1) Get size
    // 2) Move to next wear block
    // 3) Write it

    // 1)
    size = sizeof(INTWINE_METRICS_VER_1);

    // 2)
    iWearAverageIndex++;
    if(iWearAverageIndex >= WEAR_BLOCKS)
    {
        iWearAverageIndex = 0;
    }

    // 3)
    address = EEPROM_ADDRESS_METRICS_INFO + (iWearAverageIndex * WEAR_BLOCK_SIZE);
    iMetric1.Header.MagicNumber    = MAGIC_NUMBER_METRICS_INFO;
    iMetric1.Header.MetricsVersion = INTWINE_METRICS_VERSION;
    iMetric1.Header.SequenceNumber = ++iWearAverageSequenceNumber;
    EEEraseWrite4kBlock(address, size, (char *)&iMetric1.Header.MagicNumber);
}


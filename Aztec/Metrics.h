#ifndef METRICS_H
#define METRICS_H

typedef enum
{
    METRICS_MODULE_STARTUP_COUNT                  = 0,  // slot 1
    METRICS_DFW_LAST_TRY_FAILED                   = 1,  // slot 2
    METRICS_DFW_SUCCESS_COUNT                     = 2,  // slot 3
    METRICS_DFW_RECEIVE_CHECKSUM_FAIL             = 3,  // slot 4
    METRICS_DFW_VERIFY_CHECKSUM_FAIL              = 4,  // slot 5
    METRICS_DFW_SERVER_TIMEOUT_FAIL               = 5,  // slot 6
    METRICS_DFW_BAD_MAGIC_NUMBER_FAIL             = 6,  // slot 7
    METRICS_DFW_VERIFY_LENGTH_FAIL                = 7,  // slot 8
    METRICS_RADIO_ACK_RECOVERY                    = 8,  // slot 9
    METRICS_00000000_MRIDS                        = 9,  // slot 10
    METRICS_FIRST_10                              = 10,

    METRICS_EXTRA_CIDS_CLOSED                     = 10, // slot 11
    METRICS_FREED_PACKETS                         = 11, // slot 13

    METRICS_RCON_REGISTER                         = 12, // slot 12 (keep this one last if possible)

    METRICS_COUNTERS_END,

    WATCHDOG_METRICS_START                        = 20,

    WATCHDOG_PROVISIONING_FAILED                  = 20, // slot 1
    WATCHDOG_PROVISIONING_UNEXPECTED_RADIO_RESULT = 21, // slot 2
    WATCHDOG_PROVISIONING_COMPLETE                = 22, // slot 3
    WATCHDOG_RADIO_STARTUP_FAILED                 = 23, // slot 4
    WATCHDOG_ASSOCIATION_FAILED                   = 24, // slot 5
    WATCHDOG_MAC_ADDRESS_READ_FAILURE             = 25, // slot 6
    WATCHDOG_TCP_INVALID_FLUSH_BINARY             = 26, // slot 7
    WATCHDOG_ASSERT                               = 27, // slot 8
    WATCHDOG_CB_NO_ATTRIBUTE                      = 28, // slot 9
    WATCHDOG_CB_WRONG_DATATYPE                    = 29, // slot 10
    WATCHDOG_METRICS_FIRST_10                     = 30,

    WATCHDOG_CB_NULL_POINTER_OR_BAD_ID            = 30, // slot 11
    WATCHDOG_TOO_MANY_CIDS                        = 31, // slot 12
    WATCHDOG_TIME_MONITOR_OVERFLOW                = 32, // slot 13
    WATCHDOG_THERM_CS_HELD_LOW                    = 33, // slot 14
    WATCHDOG_THERM_UNEXPECTED_MSG                 = 34, // slot 15
    WATCHDOG_DFW_COMPLETE                         = 35, // slot 16
    WATCHDOG_SPI3_ERROR                           = 36, // slot 17
    WATCHDOG_RADIO_INTERFACE_ERROR                = 37, // slot 18
    WATCHDOG_BULK_DATA_COUNTER_CORRUPTED          = 38, // slot 19
    WATCHDOG_NON_NUMERIC_BULK_SIZE                = 39, // slot 20
    WATCHDOG_METRICS_SECOND_10                    = 40,

    WATCHDOG_NO_PACKETS_TO_COMMANDEER             = 40, // slot 21
    WATCHDOG_REAL_WATCHDOG_TIMEOUT                = 41,
    WATCHDOG_OVERSIZED_ASCII_BUFFER               = 42,
    WATCHDOG_OVERSIZED_BINARY_BUFFER              = 43,

    WATCHDOG_METRICS_END,                               // keep this last

    METRICS_MAX_INDEX                             = 50,

} METRICS_INDEX;

void SetMetrics(METRICS_INDEX index, int value);
int  GetMetrics(METRICS_INDEX index);
void IncrementMetrics(METRICS_INDEX index);

#endif  //#ifndef METRICS_H




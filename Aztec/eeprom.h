#ifndef EEPROM_H
#define EEPROM_H

#define EEPROM_ADDRESS_METRICS_INFO             0xE0000     // Allow for all of 0xE to be 16 wear leveling blocks

#define EEPROM_ADDRESS_CB_KEY_INFO              0xF0000
#define EEPROM_ADDRESS_PROVISIONING_INFO        0xF1000
#define EEPROM_ADDRESS_POWER_MEASUREMENT_INFO   0xF2000
#define EEPROM_ADDRESS_DEVICE_SAVE_STATE_INFO   0xF3000
#define EEPROM_ADDRESS_EVENT_Q_INFO             0xF4000
#define EEPROM_ADDRESS_ICO_CAL_INFO             0xF9000     // F9 because CTL put the info here on PIC24

#define MAGIC_NUMBER_CB_KEY_INFO                0x39923958
#define MAGIC_NUMBER_PROVISIONING_INFO          0xBADCAFE1
#define MAGIC_NUMBER_POWER_MEASUREMENT_INFO     0x1BADBABE
#define MAGIC_NUMBER_DEVICE_SAVE_STATE_INFO     0xBADCAFE2
#define MAGIC_NUMBER_METRICS_INFO               0x0BADCAFE  // Reset with f/w 414
#define MAGIC_NUMBER_EVENT_Q_INFO               0x30466958
#define MAGIC_NUMBER_ICO_CAL_INFO               0xDEADBEEF

#endif

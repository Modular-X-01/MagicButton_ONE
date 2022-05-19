#ifndef __APP_BATT_H
#define __APP_BATT_H

#include <Arduino.h>

void app_batt_init(void);
uint8_t app_batt_percent();
uint16_t app_batt_volatge();
bool app_batt_charged();
void app_batt_config_wakeup();
void app_batt_deinit(void);

#endif
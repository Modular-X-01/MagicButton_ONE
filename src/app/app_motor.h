#ifndef __APP_MOTOR_H
#define __APP_MOTOR_H
#include <stdint.h>
void app_motor_init(void);

void app_motor_deinit(void);

void app_motor_on(void);

void app_motor_off(void);

void app_motor_once(uint32_t ms); //ms

#endif

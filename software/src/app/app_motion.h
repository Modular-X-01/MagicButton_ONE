#ifndef __APP_MOTION_H
#define __APP_MOTION_H
#include <stdint.h>
#include "board_def.h"

#include "Wire.h"


#if HW_ACC == ACC_KXTJ3_1057
#include "kxtj3-1057.h"
extern KXTJ3 myIMU;
#elif HW_ACC == ACC_QMA7981
#include "qma7981.h"
extern QMA7981 myIMU;
#elif HW_ACC ==ACC_LSM6DS3TR
#include <Adafruit_LSM6DS3.h> 
#endif



#define AXIS_THRESHOLD 0.6

typedef enum
{
    DIR_NONE = 0,
    DIR_UP = 0x01,
    DIR_DOWN = 0x02,
    DIR_LEFT = 0x04,
    DIR_RIGHT = 0x08,

} direction_t;

typedef struct
{
    float x;
    float y;
    float z;

    int16_t raw_x;
    int16_t raw_y;
    int16_t raw_z;

} motion_t;

bool app_motion_init(float sampleRate, uint8_t accelRange);

void app_motion_deinit(void);

void app_motion_get(motion_t *data);

void app_motion_sleep();
void app_motion_wakeup();
uint8_t app_getDirection();

#endif
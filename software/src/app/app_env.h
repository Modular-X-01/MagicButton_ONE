#ifndef __APP_ENV_H
#define __APP_ENV_H
#include <stdint.h>
#include <Adafruit_AHTX0.h>
#include "Wire.h"

extern Adafruit_AHTX0 aht;
 


typedef struct{ 
    float temperature;
    float humidity;
}env_t;

bool app_env_init(void);

void app_env_deinit(void);

void app_env_get(env_t *data); 

#endif
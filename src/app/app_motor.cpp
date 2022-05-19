#include "app_motor.h"
#include <Arduino.h>

#include "board_def.h"

#ifndef PIN_MOTOR
    #define PIN_MOTOR -1
#endif

 
SoftwareTimer motorTimer;

void app_motor_init(void)
{
    app_motor_off();
    pinMode(PIN_MOTOR, OUTPUT);
}

void app_motor_deinit(void)
{

    nrf_gpio_cfg_default(PIN_MOTOR);
}

void app_motor_on(void)
{
    digitalWrite(PIN_MOTOR, HIGH);
}

void app_motor_off(void)
{
    digitalWrite(PIN_MOTOR, LOW);
}

static void motor_timer_callback(TimerHandle_t xTimerID)
{
    // freeRTOS timer ID, ignored if not used
    (void)xTimerID;
    app_motor_off();
    motorTimer.~SoftwareTimer();
}
TimerHandle_t motor_timerhandle;
void app_motor_once(uint32_t ms) //ms
{

    motorTimer.begin(ms, motor_timer_callback, NULL, false);
    // // Start the timer
    if (!motorTimer.start())
    {
        // tft.println("timer start fail");
        return;
    }
    app_motor_on();
}



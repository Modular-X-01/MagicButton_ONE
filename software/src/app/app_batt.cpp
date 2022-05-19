
#include "app_batt.h"

#include <Arduino.h>

#define PIN_BATT PIN_VBAT
#define PIN_CHARGE -1
#define BATT_FACTOR (2.0)                    //使用1M:1M分压
static float mv_per_lsb = 3000.0F / 4096.0F; // 12-bit ADC with 3.0V input range

void app_batt_init(void)
{
  analogReference(AR_INTERNAL_3_0);
  // Set the resolution to 12-bit (0..4095)
  analogReadResolution(12); // Can be 8, 10, 12 or 14
  if (PIN_CHARGE >= 0)
  {
    pinMode(PIN_CHARGE, INPUT);
  }
}

void app_batt_deinit(void)
{
  if (PIN_CHARGE >= 0)
  {
    nrf_gpio_cfg_default(PIN_CHARGE);
  }
  nrf_gpio_cfg_default(PIN_BATT);
}

//https://github.com/G6EJD/LiPo_Battery_Capacity_Estimator/blob/master/ReadBatteryCapacity_LIPO.ino
uint8_t app_batt_percent()
{
  uint8_t percentage = 100;

  float voltage = ((float)analogRead(PIN_BATT) * mv_per_lsb * BATT_FACTOR) / 1000.0; //
  percentage = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;
  if (voltage > 4.19)
    percentage = 100;
  else if (voltage <= 3.50)
    percentage = 0;
  return percentage;
}
uint16_t app_batt_volatge() //mV
{
  uint16_t voltage = (analogRead(PIN_BATT) * mv_per_lsb * BATT_FACTOR) / 1;
  return voltage;
}

bool app_batt_charged()
{
  if (digitalRead(PIN_CHARGE) == LOW)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void app_batt_config_wakeup()
{
  pinMode(PIN_CHARGE, INPUT_PULLUP_SENSE); // this pin (WAKE_HIGH_PIN) is pulled down and wakes up the feather when externally connected to 3.3v.
}

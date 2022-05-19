
#include "app_env.h"

Adafruit_AHTX0 aht;
 
bool app_env_init()
{
    if (!aht.begin())
        {
                // u8g2log.println("Could not find AHT? Check wiring");
                return false;
        }
        // u8g2log.println("AHT10 or AHT20 found");
        return true;
        
}


void app_env_get(env_t *data)
{
    sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data

        data->temperature = temp.temperature;
        data->humidity = humidity.relative_humidity;
        // u8g2log.print("Temperature: ");
        // u8g2log.print(temp.temperature);
        // u8g2log.println(" degrees C");
        // u8g2log.print("Humidity: ");
        // u8g2log.print(humidity.relative_humidity);
        // u8g2log.println("% rH");
     
     

}


void app_env_deinit()
{
   
    Wire.end();
}
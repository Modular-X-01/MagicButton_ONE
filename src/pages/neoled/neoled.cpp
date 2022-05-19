#include "neoled.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_batt.h"
#include "app/app_timeout.h"
#include "app/app_motion.h"

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel neo = Adafruit_NeoPixel(4, PIN_NEOLED, NEO_RGB + NEO_KHZ800);

// Adafruit_AHTX0 aht;
static uint8_t num = 0;
static bool flag_start = false;

static TaskHandle_t handle;
uint8_t style = 0;
typedef enum
{
    T_RANDOM = 0,
    T_FULLRGB,
    T_ACC,

    T_MAX,
} neotype_t;
const char *type_str[]={
    "random",
    "fullRGB",
    "accel",

};
#include <math.h>
static void randomColor(void *para)
{
    uint16_t h = 0;
    while (1)
    {
        for (int i = 0; i < neo.numPixels(); i++)
        { // For each pixel...

            // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
            // Here we're using a moderately bright green color:
            switch (style % T_MAX)
            {
            case T_RANDOM:
                neo.setPixelColor(i, neo.Color(random(256), random(256), random(256)));
                break;
            case T_FULLRGB:
                neo.setPixelColor(i, neo.ColorHSV(h, 200, 100));
                h += 256;
                break;

            case T_ACC:
                motion_t m;
                double axis;
                app_motion_get(&m);

                 axis = pow(m.x,2)+pow(m.y,2);
                uint16_t h;
                uint8_t s;
                uint8_t v;
                h = map(axis, 0, 16.0, 0, 65535);
                s = map(m.y, -1.0, 1.0, 0, 255);
                v = map(m.z, -1.0, 1.0, 0, 255);

                neo.setPixelColor(i, neo.ColorHSV(h, 200, 100));
                break;
            default:
                break;
            }
            //

            neo.show(); // Send the updated pixel colors to the hardware.

            delay(50); // Pause before next pass through loop
        }
    }
}

static void init(void *data)
{

    pinMode(PIN_NEOPWR, OUTPUT);
    digitalWrite(PIN_NEOPWR, LOW);
    neo.begin();
    neo.clear();
    app_motion_init(25, 2);

    randomSeed(analogRead(PIN_VBAT));
    //创建定时获取任务
    BaseType_t res = xTaskCreate(randomColor, "random", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle);
    if (res == pdPASS)
    {
    }
}

static void task()
{ //显示当前电量和电压
    uint16_t voltage;
    uint8_t percent;
    style++;
    u8g2.setFontPosTop();
    u8g2.setFont(u8g2_font_5x7_tr);
    voltage = app_batt_volatge();
    percent = app_batt_percent();

    u8g2.clearBuffer();

    u8g2.drawStr(0, 0, "NeoLED");

    u8g2.setCursor(0, 16);
    u8g2.print(voltage);
    u8g2.println("mV");
    u8g2.setCursor(0, 32);
    u8g2.print(percent);
    u8g2.println("%");
    u8g2.printf("  %s",type_str[(style % T_MAX)]);
    
    u8g2.sendBuffer();
    
}
static void enter(void *data)
{
    u8g2.clear();
    u8g2.setFontPosTop();
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(0, 0, "NeoLED");

    u8g2.sendBuffer();

    task();

    manager_setBusy(false);
}

static void loop(void *data)
{
    KEY_TYPE key = app_key_get();
    switch (key)
    {
    case KEY0_SHORT:
        task();
        break;                    //计数开始
    case KEY0_LONG:               //长按
        manager_switchToParent(); //进入父项目 //退出
        break;
    default:
        break;
    }
}

static void exit(void *data)
{
    manager_setBusy(true);
    vTaskDelete(handle);

    neo.clear();
    neo.~Adafruit_NeoPixel();
    nrf_gpio_cfg_default(31+6);
    nrf_gpio_cfg_default(15);
    app_motion_deinit();

    
}
#include "img.h"
page_t page_neoled = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Neoled",
    .title_cn = " 灯效",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = true,
    .wakeup_btn_effect_enable = false,
    .acc_enable=false,

};
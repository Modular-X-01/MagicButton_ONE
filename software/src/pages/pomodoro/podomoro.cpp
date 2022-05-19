#include "podomoro.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_timeout.h"

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel led = Adafruit_NeoPixel(4, PIN_NEOLED, NEO_RGB + NEO_KHZ800);
static bool flag_start = false;

static TaskHandle_t handle;

#define TIME_POROMODO (25 * 60)
#define TIME_BREAK (5 * 60)

TimerHandle_t xTimer_onesec, xTimer_action;
static uint32_t cnt = 0;
typedef enum
{
    POROMODO = 0,
    BREAK = 1,

} action_t;

static action_t action;

static void colorstate(void *para)
{
    action_t action = *(action_t *)(para);
    pinMode(PIN_NEOPWR, OUTPUT);
    digitalWrite(PIN_NEOPWR, LOW);
    led.begin();
    led.clear();
    while (1)
    {

        for (int i = 0; i < led.numPixels(); i++)
        { // For each pixel...

            // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
            // Here we're using a moderately bright green color:
            if (action == BREAK)
                led.setPixelColor(i, led.Color(0, 255, 0));
            else
                led.setPixelColor(i, led.Color(17, 100, 100));

            led.show(); // Send the updated pixel colors to the hardware.

            delay(200); // Pause before next pass through loop
        }
        for (int i = 0; i < led.numPixels(); i++)
        { // For each pixel...

            // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
            // Here we're using a moderately bright green color:
            led.setPixelColor(i, led.Color(0, 0, 0));

            led.show(); // Send the updated pixel colors to the hardware.

            delay(200); // Pause before next pass through loop
        }
        led.clear();

        nrf_gpio_cfg_default(2);
        nrf_gpio_cfg_default(15);
        vTaskDelete(NULL);
    }
}

static void dispNum(uint32_t t, bool state, action_t action)
{
    u8g2.clear();
    u8g2.setFontPosTop();
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_5x7_tr);
    switch (action)
    {
    case POROMODO:
        u8g2.drawStr(0, 0, "Podomoro");
        break;
    case BREAK:
        u8g2.drawStr(0, 0, "Break");
        break;
    default:
        break;
    }

    u8g2.setFont(u8g2_font_logisoso20_tr);
    char buffer[10];
    sprintf(buffer, "%02d:%02d", t / 60, t % 60);
    u8g2.drawStr(0, 14, buffer);

    // u8g2.drawFrame(10, 40, 30, 16);

    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_roentgen_nbp_tr);
    if (state == false)
    {
        u8g2.drawStr(10, 40, "START");
    }
    else
    {
        u8g2.drawStr(13, 40, "STOP");
    }
    u8g2.sendBuffer();
}

static void onesecCallback(TimerHandle_t xTimer)
{
    if (flag_start)
        cnt--;

    if (cnt == 0)
    {

        //任务

        if (action == POROMODO)
        {
            flag_start = false;
            cnt = TIME_BREAK;
            action = BREAK;
        }
        else
        {
            flag_start = false;
            cnt = TIME_POROMODO;
            action = POROMODO;
        }

        if (handle != NULL)
        {
            vTaskDelete(handle);
            handle = NULL;
            led.clear();
            nrf_gpio_cfg_default(2);
            nrf_gpio_cfg_default(15);
        }
        BaseType_t res = xTaskCreate(colorstate, "color", SCHEDULER_STACK_SIZE_DFLT, &action, TASK_PRIO_LOW, &handle);
        if (res == pdPASS)
        {
        }
    }
    if (app_timeout_getState() == false)
    {

        dispNum(cnt, flag_start, action);
    }
}

static void init(void *data)
{
    randomSeed(analogRead(PIN_VBAT));
    //创建定时获取任务
    xTimer_onesec = xTimerCreate("onesec", 1000, pdTRUE, (void *)0, onesecCallback);
    cnt = TIME_POROMODO;
    action = POROMODO;
    flag_start = false;
}

static void enter(void *data)
{

    dispNum(TIME_POROMODO, false, POROMODO);

    u8g2.sendBuffer();

    manager_setBusy(false);
}

static void event()
{
    flag_start = !flag_start;
    if (flag_start)
    {
        xTimerStart(xTimer_onesec, 0);
    }
    else
    {
        xTimerStop(xTimer_onesec, 0);
    }
    dispNum(cnt, flag_start, action);
}

static void loop(void *data)
{

    KEY_TYPE key = app_key_get();
    switch (key)
    {
    case KEY0_SHORT:

        event();

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
    xTimerStop(xTimer_onesec, 0);
    xTimerDelete(xTimer_onesec, 0);
    if (handle != NULL)
    {
        vTaskDelete(handle);
        handle = NULL;
    }
    led.clear();
    nrf_gpio_cfg_default(2);
    nrf_gpio_cfg_default(15);

    manager_setBusy(true);
}
#include "img.h"
page_t page_podomoro = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Podomoro",
    .title_cn = "番茄钟",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = true,
    .wakeup_btn_effect_enable = false,

};
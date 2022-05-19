#include "camera.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_timeout.h"
#include "bluetooth.h"
#include "bluetooth_no.h"
#include <bluefruit.h>

extern BLEHidAdafruit blehid;
static TaskHandle_t handle;
 
static void update(void *para)
{
    bool old_state = false;
    bool new_state = false;
    while (1)
    {
         
        if (Bluefruit.connected())
        {
            new_state = true;
        }
        else
        {
            new_state = false;
        }
        if (new_state != old_state  &&app_timeout_getState()==false)
        {
            old_state = new_state;
            u8g2.clear();
            u8g2.setFontPosBottom();
            if (new_state)
            {

                u8g2.drawXBM((OLED_WIDTH - bluetooth_width) / 2, 0, bluetooth_width, bluetooth_height, bluetooth_bits);
                u8g2.drawStr(0, OLED_HEIGHT, "Connected");
            }
            else
            {

                u8g2.drawXBM((OLED_WIDTH - bluetooth_no_width) / 2, 0, bluetooth_no_width, bluetooth_no_height, bluetooth_no_bits);
                u8g2.drawStr(0, OLED_HEIGHT, "Disconnected");
            }

            u8g2.sendBuffer();
        }
        delay(200);
    }
}

static void init(void *data)
{

    //创建定时获取任务
    BaseType_t res = xTaskCreate(update, "camera", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle);
    if (res == pdPASS)
    {
    }
    Bluefruit.Advertising.start(0);
}

static void enter(void *data)
{

    u8g2.setFontPosBottom();
    u8g2.setFont(u8g2_font_5x8_tr);
    for (uint8_t i = 0; i < OLED_HEIGHT; i += 1)
    {
        u8g2.clearBuffer();
        {
            u8g2.drawXBM((OLED_WIDTH - bluetooth_no_width) / 2, OLED_HEIGHT - i, bluetooth_no_width, bluetooth_no_height, bluetooth_no_bits);
            u8g2.drawStr(0, 2 * OLED_HEIGHT - i, "Disconnected");
        }
        u8g2.sendBuffer();
    }

    manager_setBusy(false);
}

static void event()
{
    if (Bluefruit.connected())
    {
        blehid.consumerKeyPress(HID_USAGE_CONSUMER_VOLUME_DECREMENT);

        // Delay a bit between reports
        delay(10);

        // Send key release
        blehid.consumerKeyRelease();
    }
    {
    }
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

    vTaskDelete(handle);
    Bluefruit.disconnect(0);
    for (uint8_t i = 0; i < 10; i++)
    {
        Bluefruit.Advertising.stop();
        delay(50);
    }

    manager_setBusy(true);
}
#include "img.h"
page_t page_camera = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Camera",
    .title_cn = "自拍器",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = true,
    .wakeup_btn_effect_enable = true,

};
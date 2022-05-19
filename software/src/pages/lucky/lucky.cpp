#include "lucky.h"
#include "board_def.h"
#include "app/app_key.h"

// Adafruit_AHTX0 aht;
static uint8_t num = 0;
static bool flag_start = false;

static TaskHandle_t handle;

static void dispNum(void *para)
{
    char buffer[4];
    u8g2.setFont(u8g2_font_7Segments_26x42_mn);
    u8g2.setFontPosTop();
    int32_t width = u8g2.getStrWidth("0");
    int8_t height = u8g2.getMaxCharHeight();

    while (1)
    {
        if (flag_start == true)
        {

            num = random(10);
            snprintf(buffer, 4, "%d", num);
            u8g2.clearBuffer();
            u8g2.drawStr((OLED_WIDTH - width) / 2, (OLED_HEIGHT - height) / 2, buffer);
            u8g2.sendBuffer();
        }
        else
        {
        }

        delay(50);
    }
}

static void init(void *data)
{
    randomSeed(analogRead(PIN_VBAT));
    //创建定时获取任务
    BaseType_t res = xTaskCreate(dispNum, "lucky", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle);
    if (res == pdPASS)
    {
    }
}

static void enter(void *data)
{

    
    u8g2.setFont(u8g2_font_7Segments_26x42_mn);
    u8g2.setFontPosTop();
    int32_t width = u8g2.getStrWidth("0");
    int8_t height = u8g2.getMaxCharHeight();

     
    u8g2.clearBuffer();
    u8g2.drawStr((OLED_WIDTH - width) / 2, (OLED_HEIGHT - height) / 2, "0");
    u8g2.sendBuffer();

     

    manager_setBusy(false);
}

static void event()
{
    flag_start = !flag_start;
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
    manager_setBusy(true);
}
#include "img.h"
page_t page_lucky = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Lucky",
    .title_cn = " 幸运数",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = false,
    .wakeup_btn_effect_enable = false,

};
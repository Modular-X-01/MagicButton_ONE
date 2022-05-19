#include "singleselector.h"
#include "board_def.h"
#include "app/app_key.h"

// Adafruit_AHTX0 aht;

static bool flag_start = false;

static TaskHandle_t handle;

static void disp(void *para)
{
   
    u8g2.setFont(u8g2_font_logisoso42_tr);
    u8g2.setFontPosTop();
    int32_t width = u8g2.getStrWidth("A");
    int8_t height = u8g2.getMaxCharHeight();
    uint8_t num = 0;
    char * selector[4]={"A","B","C","D"};
    

    while (1)
    {
        if (flag_start == true)
        {

            num = random(4);

             
             
            u8g2.clearBuffer();
            u8g2.drawStr((OLED_WIDTH - width) / 2, (OLED_HEIGHT - height) / 2+8, selector[num]);
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
    BaseType_t res = xTaskCreate(disp, "single", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle);
    if (res == pdPASS)
    {
    }
}

static void enter(void *data)
{
    u8g2.clear();
    u8g2.setFontPosTop();
    u8g2.setFont(u8g2_font_logisoso18_tr);
    u8g2.drawStr(0, 0, "Start");

    u8g2.setFont(u8g2_font_logisoso42_tr);

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
page_t page_singleselector = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "SingleSelector",
    .title_cn = " 单项选择",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = true,
    .wakeup_btn_effect_enable = true,

};
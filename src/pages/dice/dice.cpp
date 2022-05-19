#include "dice.h"
#include "board_def.h"
#include "app/app_key.h"
#include "dice-1.h"
#include "dice-2.h"
#include "dice-3.h"
#include "dice-4.h"
#include "dice-5.h"
#include "dice-6.h"
static const uint8_t *dice_img[] = {dice_1_bits, dice_2_bits, dice_3_bits, dice_4_bits, dice_5_bits, dice_6_bits};
static const uint8_t img_nums = ARRAY_SIZE(dice_img);
// Adafruit_AHTX0 aht;
static uint8_t num = 0;
static bool flag_start = false;

static TaskHandle_t handle;

static void dispNum(void *para)
{
    int x = 0,y=0;
    while (1)
    {
        if (flag_start == true)
        {

            num = random(img_nums) % img_nums;

            u8g2.clearBuffer();
            
            x = random(OLED_WIDTH-48);
             

            u8g2.drawXBM(x, 0, 48, 48, dice_img[num]);

            u8g2.sendBuffer();
        }
        else
        {
        }

        delay(100);
    }
}

static void init(void *data)
{
    randomSeed(millis());
    //创建定时获取任务
    BaseType_t res = xTaskCreate(dispNum, "lucky", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle);
    if (res == pdPASS)
    {
    }
}

static void enter(void *data)
{
    u8g2.clear();
    u8g2.setFontPosTop();
    u8g2.setFont(u8g2_font_logisoso18_tr);
    u8g2.drawStr(5, 15, "Start");

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
page_t page_dice = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "dice",
    .title_cn = "骰子",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = false,
    .wakeup_btn_effect_enable = false,

};
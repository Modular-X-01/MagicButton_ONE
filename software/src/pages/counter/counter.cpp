#include "counter.h"
#include "app/app_motion.h"
#include "board_def.h"
#include "app/app_key.h"
#include <math.h>
static uint32_t cnt = 0;
static char *buffer;
#define BUF_LENGTH 20
bool f_exit;
static void init(void *data)
{

    buffer = (char *)malloc(BUF_LENGTH);
    memset(buffer, 0, BUF_LENGTH);

    f_exit = false;
}

static void enter(void *data)
{
    u8g2.clear();
    u8g2.setFontPosTop();

    u8g2.setFont(u8g2_font_logisoso42_tn);

    memset(buffer, 0, BUF_LENGTH);
    snprintf(buffer, BUF_LENGTH, "%d", cnt);

    u8g2.clearBuffer();
    int32_t width = u8g2.getStrWidth(buffer);
    int8_t height = u8g2.getMaxCharHeight();

    u8g2.drawStr((OLED_WIDTH - width) / 2, (OLED_HEIGHT - height) / 2 + 4, buffer);
    u8g2.sendBuffer();

    manager_setBusy(false);
}

static void task()
{
    cnt++;

    memset(buffer, 0, BUF_LENGTH);
    snprintf(buffer, BUF_LENGTH, "%d", cnt);

    if (cnt / 10000)
    {
        u8g2.setFont(u8g2_font_logisoso18_tn);
    }
    else if (cnt / 1000)
    {
        u8g2.setFont(u8g2_font_logisoso24_tn);
    }
    else if (cnt / 100) //3位数
    {
        u8g2.setFont(u8g2_font_logisoso30_tn);
    }
    else
    {
        u8g2.setFont(u8g2_font_logisoso42_tn);
    }

    u8g2.clearBuffer();
    int32_t width = u8g2.getStrWidth(buffer);
    int8_t height = u8g2.getMaxCharHeight();
    u8g2.drawStr((OLED_WIDTH - width) / 2, ((OLED_HEIGHT - height) / 2) + 4, buffer);
    u8g2.sendBuffer();
}
static double axis_max = 0;
static double axis = 0;
static void loop(void *data)
{
    KEY_TYPE key = app_key_get();
    switch (key)
    {
    case KEY0_UP:
        task();
        break; //计数开始

    case KEY0_LONG: //长按
        manager_switchToParent(); //进入父项目 //退出
        // app_motion_init(50, 16);
        // f_exit = true;
        break;
    default:
        break;
    }

    //长按后且一直按着，开始准备退出，
    // if (f_exit)
    // {

    //     if (digitalRead(PIN_SW01))
    //     {
    //         f_exit = false;
    //         app_motion_deinit();
    //     }
    //     else
    //     {

    //         motion_t motion;
    //         app_motion_get(&motion);
    //         axis = sqrt(pow(motion.x, 2) + pow(motion.y, 2) + pow(motion.z, 2));
    //         if (axis > axis_max)
    //         {
    //             axis_max = axis;
    //         }
    //         if (((int)axis) > MAX_ACC)
    //         {
    //             app_motion_deinit();
    //             manager_switchToParent(); //进入父项目 //退出
    //         }
    //     }
    // }
    // u8g2.setFont(u8g2_font_4x6_mr);
    // memset(buffer, 0, BUF_LENGTH);
    // snprintf(buffer, BUF_LENGTH, "%.2lf,%.2lf,%d", axis_max, axis, (uint8_t)f_exit);
    // u8g2.drawStr(0, 0, buffer);
    // u8g2.sendBuffer();
}

static void exit(void *data)
{

    manager_setBusy(true);

    free(buffer);
}

#include "img.h"
page_t page_counter = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Counter",
    .title_cn = "计数器",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = true,
    .wakeup_btn_effect_enable = true,

};

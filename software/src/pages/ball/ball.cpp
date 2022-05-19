#include "ball.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_motion.h"

#define CIRCLE_R 3

motion_t motion_max, motion_old;

static double axis_max;//总的加速度

static void display_ball(int16_t x, int16_t y)
{

    u8g2.clearBuffer();

    u8g2.drawHLine(0, 24, 64);
    u8g2.drawVLine(32, 0, 48);

    u8g2.drawFilledEllipse(x, y, CIRCLE_R, CIRCLE_R);

    u8g2.sendBuffer();
}

static void init(void *data)
{

    app_motion_init(50, 16);
    motion_max.x = 0;
    motion_max.y = 0;
    motion_max.z = 0;
    axis_max = 0;
}

static void enter(void *data)
{
    u8g2.clear();
    
    u8g2.setFont(u8g2_font_5x7_mr);
    u8g2.setFontPosTop();

    display_ball(24, 32);

    manager_setBusy(false);
}

static void loop(void *data)
{
    motion_t motion;
    double axis;
    app_motion_get(&motion);
    axis = sqrt(pow(motion.x, 2) + pow(motion.y, 2) + pow(motion.z, 2));

    if(axis>axis_max)
    {
        axis_max = axis;
    }

    if (abs(motion.x) > motion_max.x)
    {
        motion_max.x = abs(motion.x);
    }
    if (abs(motion.y) > motion_max.y)
    {
        motion_max.y = abs(motion.y);
    }
    if (abs(motion.z) > motion_max.z)
    {
        motion_max.z = abs(motion.z);
    }

    int16_t x = motion.x * 32 + 32;
    int16_t y = motion.y * 24 + 24;

    display_ball(x, y);
    char buffer[10];
    sprintf(buffer, "x:%.2f", motion.x);
    u8g2.drawStr(0, 10, buffer);
    sprintf(buffer, "y:%.2f", motion.y);
    u8g2.drawStr(0, 30, buffer);
    sprintf(buffer, "z:%.2f", motion.z);
    u8g2.drawStr(0, 40, buffer);

    //max
     
    sprintf(buffer, "s:%.2lf", axis_max);
    u8g2.drawStr(33, 0, buffer);
    sprintf(buffer, "x:%.2lf", motion_max.x);
    u8g2.drawStr(33, 10, buffer);
    sprintf(buffer, "y:%.2lf", motion_max.y);
    u8g2.drawStr(33, 30, buffer);
    sprintf(buffer, "z:%.2lf", motion_max.z);
    u8g2.drawStr(33, 40, buffer);


    u8g2.sendBuffer();

    KEY_TYPE key = app_key_get();
    switch (key)
    {
    case KEY0_SHORT:
        // event();
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
    app_motion_deinit();
}
#include "img.h"
page_t page_ball = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Ball",
    .title_cn = "小球",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = false,
    .wakeup_btn_effect_enable = false,

};

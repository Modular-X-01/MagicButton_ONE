#include "pages.h"
#include "manager/manager.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_motion.h"

#include "pages/counter/counter.h"
#include "pages/env/env.h"
#include "pages/lucky/lucky.h"
#include "pages/precise/precise.h"
#include "pages/camera/camera.h"
#include "pages/stopwatch/stopwatch.h"
#include "pages/speed/speed.h"
#include "pages/singleselector/singleselector.h"
#include "pages/shotcut/shotcut.h"
#include "pages/neoled/neoled.h"
#include "pages/bird/bird.h"
#include "pages/clock/clock.h"
#include "pages/update/update.h"
#include "pages/bili/bili.h"
#include "pages/pomodoro/podomoro.h"
#include "pages/ball/ball.h"
#include "pages/dice/dice.h"
#include "pages/morse/morse.h"
#include "pages/kuai/kuai.h"
//需要添加自己的功能页面
page_t *pages[] = {
    &page_main,
    &page_clock,
    &page_counter,
    &page_podomoro,
    &page_morse,
    &page_ball,
    &page_dice,
    &page_kuai,
    &page_env,
    &page_lucky,
    &page_precise,
    &page_camera,
    &page_stopwatch,
    &page_speed,
    &page_singleselector,
    &page_shotcut,
    &page_neoled,
    &page_bili,
    &page_bird,
    // &page_update,

};

static const uint8_t pages_num = ARRAY_SIZE(pages);

static uint8_t page_index = 1;
// static uint8_t page_old = 1;

static uint32_t cnt = 0;
// static char *buffer;
static uint8_t key_type;
static void init(void *data)
{
    manager.index = 0;
    app_motion_init(25, 2);
}

static void enter(void *data)
{

    uint16_t width = pages[page_index]->icon_width;
    uint16_t height = pages[page_index]->icon_height;
    u8g2.clear();
    u8g2.setDrawColor(1);
    u8g2.setFontPosBottom();
    u8g2.setFont(u8g2_font_blipfest_07_tn);
    // u8g2.drawStr(0, OLED_HEIGHT, pages[page_index]->title_en);

    if (pages[page_index]->icon)
    {
        u8g2.drawXBM((OLED_WIDTH - width) / 2, 0, width, height, pages[page_index]->icon);
    }

    char buffer[10];
    sprintf(buffer, "%02d", page_index);
    u8g2.drawStr(0, OLED_HEIGHT, buffer);

    u8g2.sendBuffer();
    delay(500);

    manager_setBusy(false);
}

static void event()
{
    uint16_t width;
    uint16_t height;

    uint16_t width_old;
    uint16_t height_old;
    uint8_t page_old = page_index;

    width_old = pages[page_old]->icon_width;
    height_old = pages[page_old]->icon_height;

    if (page_index < (pages_num - 1))
    {
        page_index++;
    }
    else
    {
        page_index = 1; //由于父页面在索引0，因此更改为1 作为第一个子页面
    }

    width = pages[page_index]->icon_width;
    height = pages[page_index]->icon_height;

    for (uint8_t i = 0; i < 64; i += 4)
    {
        u8g2.clearBuffer();
        // u8g2.drawStr(0, OLED_HEIGHT, pages[page_index]->title_en);

        if (pages[page_old]->icon)
        {
            u8g2.drawXBM((OLED_WIDTH - width_old) / 2 - i, 0, width_old, height_old, pages[page_old]->icon);
        }

        if (pages[page_index]->icon)
        {
            u8g2.drawXBM(OLED_WIDTH - i + ((OLED_WIDTH - width) / 2), 0, width, height, pages[page_index]->icon);
        }

        char buffer[10];
        sprintf(buffer, "%02d", page_index);
        u8g2.drawStr(0, OLED_HEIGHT, buffer);

        u8g2.sendBuffer();
    }

    // u8g2.clearBuffer();
    // // u8g2.drawStr(0, OLED_HEIGHT, pages[page_index]->title_en);

    // if (pages[page_index]->icon)
    // {
    //     u8g2.drawXBM((OLED_WIDTH - width) / 2, 0, width, height, pages[page_index]->icon);
    // }

    // char buffer[10];
    // sprintf(buffer, "%d", page_index);
    // u8g2.drawStr(0, OLED_HEIGHT, buffer);

    // u8g2.sendBuffer();
}

static void switchNext()
{

    page_index++;
    if (page_index == pages_num)
        page_index = 1;
}

static void switchPrev()
{

    page_index--;
    if (page_index == 0)
        page_index = pages_num - 1;
}
#include "app/app_timeout.h"
static void acc_event()
{
    static uint8_t page_old = 1;
    uint8_t dir = app_getDirection();
    // if (dir & DIR_RIGHT)
    // {
    //     delay(200);
    //     if (app_getDirection() == DIR_NONE)
    //     {
    //         switchNext();
    //     }
    //     app_timeout_reset();
    // }
   if (dir & DIR_LEFT)
    {
        delay(200);
        if (app_getDirection() == DIR_NONE)
        {
            switchPrev();
        }
        app_timeout_reset();
    }

    if (page_old != page_index)
    {
        page_old = page_index;

        uint16_t width;
        uint16_t height;

        uint16_t width_old;
        uint16_t height_old;

        width_old = pages[page_old]->icon_width;
        height_old = pages[page_old]->icon_height;

        width = pages[page_index]->icon_width;
        height = pages[page_index]->icon_height;
        // if (dir == DIR_RIGHT)
        // {
        //     for (uint8_t i = 0; i < 64; i += 4)
        //     {
        //         u8g2.clearBuffer();
        //         // u8g2.drawStr(0, OLED_HEIGHT, pages[page_index]->title_en);

        //         if (pages[page_old]->icon)
        //         {
        //             u8g2.drawXBM((OLED_WIDTH - width_old) / 2 - i, 0, width_old, height_old, pages[page_old]->icon);
        //         }

        //         if (pages[page_index]->icon)
        //         {
        //             u8g2.drawXBM(OLED_WIDTH - i + ((OLED_WIDTH - width) / 2), 0, width, height, pages[page_index]->icon);
        //         }

        //         char buffer[10];
        //         sprintf(buffer, "%02d", page_index);
        //         u8g2.drawStr(0, OLED_HEIGHT, buffer);

        //         u8g2.sendBuffer();
        //     }
        // }
        // else 
        if(dir & DIR_LEFT)
        {
            for (uint8_t i = 0; i < 8; i += 1)
            {
                u8g2.clearBuffer();
                // u8g2.drawStr(0, OLED_HEIGHT, pages[page_index]->title_en);

                if (pages[page_old]->icon)
                {
                    u8g2.drawXBM(8, 0, width_old, height_old, pages[page_old]->icon);
                }

                if (pages[page_index]->icon)
                {
                    u8g2.drawXBM(i , 0, width, height, pages[page_index]->icon);
                }

                char buffer[10];
                sprintf(buffer, "%02d", page_index);
                u8g2.drawStr(0, OLED_HEIGHT, buffer);

                u8g2.sendBuffer();
            }
        }
    }
}

static void loop(void *data)
{
    // if (digitalRead(PIN_SW01) == LOW)
    // {
    //     event(); //展示各个项目名称和图标
    //     delay(100);
    // }
    KEY_TYPE key = app_key_get();
    switch (key)
    {
    case KEY0_SHORT:
        event(); //展示各个项目名称和图标
        break;
    case KEY0_LONG:

        manager.index = page_index;
        manager_switchToChild(); //进入子项目
        break;
    default:
        break;
    }

    //加速度进行左右展示菜单
    acc_event();
}

static void exit(void *data)
{

    manager_setBusy(true);
    app_motion_deinit();
    // free(buffer);
}

page_t page_main = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "main",
    .title_cn = "主页",
    .icon = NULL,
    .icon_width = 0,
    .icon_height = 0,
    .sleep_enable = true,
    .wakeup_btn_effect_enable = false,
    .acc_enable = true,

};

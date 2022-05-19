
#include "speed.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_timeout.h"

static uint32_t cnt;
static float speed;
static bool flag_start;
static bool flag_stop;
static bool flag_update;
static TaskHandle_t handle;
static uint32_t time_start;
static uint32_t time_stop;

static void dispSpeed(uint32_t cnt)
{
    char buffer[10];
    u8g2.clearBuffer();
    uint16_t x, y;
    

    u8g2.setFont(u8g2_font_logisoso24_tn);
    snprintf(buffer, 10, "%d", cnt);
    int32_t width = u8g2.getStrWidth(buffer);
    int8_t height = u8g2.getMaxCharHeight();

    x = (OLED_WIDTH - width) / 2;
    y = 0;//(OLED_HEIGHT - height) / 2;
    u8g2.drawStr(x, y, buffer);

    memset(buffer, 0, 10);
    u8g2.setFont(u8g2_font_sirclive_tr);
    snprintf(buffer, 10, "%.1f/s", cnt*1.0 /10.0);

    int32_t width_ms = u8g2.getStrWidth(buffer);
    int32_t height_ms = u8g2.getMaxCharHeight();

    u8g2.drawStr(OLED_WIDTH - width_ms, OLED_HEIGHT-height_ms, buffer);
    u8g2.sendBuffer();
}

static void update(void *para)
{

    while (1)
    {
        if (flag_start == true)
        {
            // 开始刷屏
            

            dispSpeed(cnt);
            if(flag_stop==true)
            {
                flag_start=false;
                 vTaskSuspend(handle);
            }
        }
       
        delay(5);
    }
}

SoftwareTimer speed_timer;
static void speed_timer_callback(TimerHandle_t xTimerID)
{
    // freeRTOS timer ID, ignored if not used
    (void)xTimerID;
    flag_stop = true;
}
static void init(void *data)
{

    cnt = 0;

    flag_start = false;
    flag_stop = false;
    flag_update = false;

    //创建定时获取任务
    BaseType_t res = xTaskCreate(update, "speed", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle);
    if (res == pdPASS)
    {
    }
    speed_timer.begin(10 * 1000, speed_timer_callback, NULL, false); //10s手速
}

static void enter(void *data)
{
    u8g2.clear();
    u8g2.setFontPosTop();
    dispSpeed(0);
    manager_setBusy(false);
}

static void event_start()
{

    speed_timer.start();
}

static void event_stop()
{
}

static void loop(void *data)
{
    KEY_TYPE key = app_key_get();
    switch (key)
    {
    case KEY0_DOWN: //计时开始

        if (flag_start == false)
        {
            flag_start = true;
            event_start();
        }



        break;

    case KEY0_UP: //计时结束
        if (flag_stop == false)
        {
            if (flag_start==true)
            {
                cnt++;
            }
        }


        break;

    case KEY0_DOUBLE: //重置所有结果

        // init();
        // u8g2.clear();
        break;
    case KEY0_LONG:               //长按
        manager_switchToParent(); //进入父项目 //退出
        break;
    default:
        break;
    }
}

static void exit(void *data)
{


    speed_timer.~SoftwareTimer();
    vTaskDelete(handle);
    manager_setBusy(true);
}
#include "img.h"
page_t page_speed = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Speed",
    .title_cn = "拼手速",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = false,
    .wakeup_btn_effect_enable = false,

};
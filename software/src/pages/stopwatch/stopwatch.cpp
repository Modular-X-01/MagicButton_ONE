#include "stopwatch.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_timeout.h"

static uint32_t record[6];
static uint8_t id;
static bool flag_start;
static bool flag_update;
static TaskHandle_t handle;
static uint32_t time_start;
static uint32_t time_stop;
// Adafruit_AHTX0 aht;
// static uint32_t cnt = 0;
// static char *buffer;

// static sensors_event_t *humidity, *temp;
// static bool flag_tempOrHumidity;
// static TaskHandle_t handle;
// static TaskHandle_t handle_switchdisp;

static void dispMs(uint32_t ms)
{
    char buffer[10];
    u8g2.clearBuffer();
    uint16_t x, y;

    u8g2.setFont(u8g2_font_logisoso24_tn);
    snprintf(buffer, 10, "%01d:%02d", ms / 1000 / 60, ms / 1000 % 60);
    int32_t width = u8g2.getStrWidth(buffer);
    int8_t height = u8g2.getMaxCharHeight();

    x = (OLED_WIDTH - width) / 2;
    y = (OLED_HEIGHT - height) / 2;
    u8g2.drawStr(x, y, buffer);

    memset(buffer, 0, 10);
    u8g2.setFont(u8g2_font_sirclive_tn);
    snprintf(buffer, 10, "%03d", ms % 1000);

    int32_t width_ms = u8g2.getStrWidth(buffer);
    // height = u8g2.getMaxCharHeight();

    u8g2.drawStr(x + width - width_ms, y + height, buffer);
    u8g2.sendBuffer();
}

static void update(void *para)
{

    while (1)
    {
        if (flag_start == true)
        {
            // 开始刷屏
            uint32_t time_consume;

            time_consume = millis() - time_start;

            dispMs(time_consume);
        }
        else
        {
            if (flag_update == true)
            {
                flag_update = false;

                dispMs(time_stop - time_start);
            }
        }
        delay(5);
    }
}

static void init(void *data)
{

    id = 0;
    memset(record, 0, 6 * 4);
    flag_start = false;
    flag_update = false;

    //创建定时获取任务
    BaseType_t res = xTaskCreate(update, "stopwatch", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle);
    if (res == pdPASS)
    {
    }
}

static void enter(void *data)
{
    u8g2.clear();
    u8g2.setFontPosTop();
    dispMs(0);
    manager_setBusy(false);
}

static void event_start()
{
    time_start = millis();
}

static void event_stop()
{
    time_stop = millis();
}

static void loop(void *data)
{
    KEY_TYPE key = app_key_get();
    switch (key)
    {
    case KEY0_DOWN: //计时开始

        if (flag_start == false)
        {
            event_start();
            flag_start = true;
        }
        else
        {
            flag_start = false;
            flag_update = true;
            event_stop();
        }

        //禁止睡眠
        app_timeout_stop();

        break;

    case KEY0_UP: //计时结束
        //禁止睡眠
        app_timeout_stop();
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
    // 重启睡眠
    app_timeout_reset();
    vTaskDelete(handle);
    manager_setBusy(true);
}
#include "img.h"
page_t page_stopwatch = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "StopWatch",
    .title_cn = "秒表",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = true,
    .wakeup_btn_effect_enable = true,
    .acc_enable = false,

};
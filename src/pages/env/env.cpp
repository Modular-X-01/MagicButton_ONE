#include "env.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_env.h"

static uint32_t cnt = 0;
static char *buffer;

static sensors_event_t *humidity, *temp;
static bool flag_tempOrHumidity;
static TaskHandle_t handle;
static TaskHandle_t handle_switchdisp;
 

static void display_Value()
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_9x15B_mr);
    u8g2.setCursor(0,0);
    u8g2.printf("T:%0.1fc",temp->temperature); 
    u8g2.setCursor(0,24);
    u8g2.printf("H:%0.0f%%",humidity->relative_humidity); 
    u8g2.sendBuffer();
}

static void getValue(void *para)
{
    while (1)
    {
        Wire.begin();
        aht.getEvent(humidity, temp); // populate temp and humidity objects with fresh data
        display_Value();
        Wire.end();
        delay(1000 * 10);
        
    }
}
 
static void init(void *data)
{
    humidity = (sensors_event_t *)malloc(sizeof(sensors_event_t));
    temp = (sensors_event_t *)malloc(sizeof(sensors_event_t));
    flag_tempOrHumidity = true;
    
    if (!aht.begin())
    {
        u8g2log.println("Could not find AHT? Check wiring");
        return;
        // while (1)
        //     delay(10);
    }

    u8g2log.println("AHT10 or AHT20 found");

    aht.getEvent(humidity, temp); // populate temp and humidity objects with fresh data

    // 创建定时获取任务
    BaseType_t res = xTaskCreate(getValue, "env", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle);
    if (res == pdPASS)
    {
    }

    // res = xTaskCreate(switchDisplay, "switch", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle_switchdisp);
    // if (res == pdPASS)
    // {
    // }
}

static void enter(void *data)
{
    u8g2.clear();
    u8g2.setFontPosTop();
    display_Value();

 

    manager_setBusy(false);
}

static void event()
{
    //按键切换温湿度
    u8g2.setFont(u8g2_font_logisoso20_tr);

    u8g2.clearBuffer();
    int32_t width = u8g2.getStrWidth(buffer);
    int8_t height = u8g2.getMaxCharHeight();
    u8g2.drawStr((OLED_WIDTH - width) / 2, (OLED_HEIGHT - height) / 2, buffer);
    u8g2.sendBuffer();
}

static void loop(void *data)
{
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

    vTaskDelete(handle);
    // vTaskDelete(handle_switchdisp);

    aht.~Adafruit_AHTX0();
    Wire.end();

    free(temp);
    free(humidity);
}
#include "img.h"
page_t page_env = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Environment",
    .title_cn = "温湿度计",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = true,
    .wakeup_btn_effect_enable = false,

};

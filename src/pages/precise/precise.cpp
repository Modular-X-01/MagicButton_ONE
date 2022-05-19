#include "precise.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_motion.h"

// Adafruit_AHTX0 aht;
// static uint32_t cnt = 0;
// static char *buffer;

// static sensors_event_t *humidity, *temp;
// static bool flag_tempOrHumidity;
// static TaskHandle_t handle;
// static TaskHandle_t handle_switchdisp;

static void init(void *data)
{
    // humidity = (sensors_event_t *)malloc(sizeof(sensors_event_t));
    // temp = (sensors_event_t *)malloc(sizeof(sensors_event_t));
    // flag_tempOrHumidity = true;
    // if (!aht.begin())
    // {
    //     Serial.println("Could not find AHT? Check wiring");
    //     while (1)
    //         delay(10);
    // }
    // Serial.println("AHT10 or AHT20 found");

    // aht.getEvent(humidity, temp); // populate temp and humidity objects with fresh data

    // //创建定时获取任务
    // BaseType_t res = xTaskCreate(getValue, "env", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle);
    // if (res == pdPASS)
    // {
    // }

    //  res = xTaskCreate(switchDisplay, "switch", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle_switchdisp);
    // if (res == pdPASS)
    // {
    // }

    app_motion_init(50,16);
}
// #include <U8g2wqy.h>
static void enter(void *data)
{u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function
    u8g2.clear();
    u8g2.setFontPosTop();
    u8g2.setFont(u8g2_font_7x13_tr);
    u8g2.drawStr(0,0,"Please");
    u8g2.drawStr(10,16,"hold");
    u8g2.drawStr(20,32,"button");
    //  u8g2.setFont(u8g2_font_wqy16_t_gb2312);  // use chinese2 for all the glyphs of "你好世界"
    //  u8g2.setCursor(0, 10);
    // u8g2.print("石头剪刀布");
    // u8g2.setCursor(0, 22);
    // u8g2.print("做饭洗碗洗衣");
    // u8g2.setCursor(0, 34);
    // u8g2.print("拖地买菜闲坐");
    u8g2.sendBuffer();
    manager_setBusy(false);
}

static uint32_t time_start;

static void event_start()
{
    time_start = millis();

}

static void event_stop()
{
    int32_t time_consume;
    char buffer[10];
    time_consume = millis()-time_start;

    u8g2.setFont(u8g2_font_logisoso18_tn);


  
    snprintf(buffer, 10, "%d", time_consume);

    u8g2.clearBuffer();
    int32_t width = u8g2.getStrWidth(buffer);
    int8_t height = u8g2.getMaxCharHeight();
     
     
    u8g2.drawStr((OLED_WIDTH - width) / 2, 15, buffer);


    u8g2.setFont(u8g2_font_sirclive_tr);
    u8g2.drawStr(40, 40, "ms");
    u8g2.sendBuffer();

}

static void loop(void *data)
{
    double axis;
    motion_t motion;
    app_motion_get(&motion);
    axis = sqrt(pow(motion.x, 2) + pow(motion.y, 2) + pow(motion.z, 2));

    if (((int)axis) > MAX_ACC)
    {
        manager_switchToParent(); //进入父项目 //退出
    }
    
    KEY_TYPE key = app_key_get();
    switch (key)
    {
    case KEY0_DOWN://计时开始
        event_start();
        break; 

    case KEY0_UP://计时结束
         event_stop();
        break;
    case KEY0_LONG:               //长按
        // manager_switchToParent(); //进入父项目 //退出
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
page_t page_precise = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Precise",
    .title_cn = "时间测量者",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = false,
    .wakeup_btn_effect_enable = false,
    .acc_enable = true,

};
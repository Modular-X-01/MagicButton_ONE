#include "morse.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_motion.h"

#include "MorseEnDecoder.h"
morseDecoder morseInput(PIN_SW01, MORSE_KEYER, MORSE_ACTIVE_LOW);

static void display_humidity()
{
}

static void init(void *data)
{
    app_key_stopLoop();
    app_key_clean();
    morseInput.setspeed(6);
    app_motion_init(50, 16);
}

static void enter(void *data)
{
    u8g2.clear();
    u8g2.setFontPosTop();
    u8g2.setFont(u8g2_font_10x20_mr);
    u8g2.setCursor(5, 15);
    u8g2.println("Morse");
    u8g2.sendBuffer();

    u8g2.setFont(u8g2_font_fub30_tr);

    manager_setBusy(false);
}

static void loop(void *data)
{
    static uint8_t y = 0;
    double axis;
    motion_t motion;
    app_motion_get(&motion);
    axis = sqrt(pow(motion.x, 2) + pow(motion.y, 2) + pow(motion.z, 2));

    if (((int)axis) > MAX_ACC)
    {
        manager_switchToParent(); //进入父项目 //退出
    }

    // KEY_TYPE key = KEY_NONE;
    // switch (key)
    // {
    // case KEY0_SHORT:
    //     // event();
    //     break;                    //计数开始
    // case KEY0_LONG:               //长按
    //     manager_switchToParent(); //进入父项目 //退出
    //     break;
    // default:
    //     break;
    // }

    // Needs to call these once per loop
    morseInput.decode();

    // Morse input:
    // If a character is decoded from the input, write it to serial output
    if (morseInput.available())
    {
        char receivedMorse = morseInput.read();
        u8g2.clearBuffer();
        u8g2.setCursor(20, 5);

        u8g2.print(receivedMorse);

        // A little error checking
        if (receivedMorse == '#')
        {
            u8g2.println("< ERROR:too many morse signals! >");
        }

        u8g2.sendBuffer();
    }
}

static void exit(void *data)
{
    manager_setBusy(true);
    app_motion_deinit();
    app_key_startLoop();
    
}
#include "img.h"
page_t page_morse = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Morse",
    .title_cn = "莫尔斯码",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = false,
    .wakeup_btn_effect_enable = false,

};

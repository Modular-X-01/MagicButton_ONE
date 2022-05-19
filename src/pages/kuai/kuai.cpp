// 休眠：rundll32.exe powrProf.dll,SetSuspendState

// 休眠：shutdown -h

// 睡眠：rundll32.exe powrprof.dll,SetSuspendState 0,1,0

// 关机：shutdown -s -t 0

#include "kuai.h"
#include <BLE52_Mouse_and_Keyboard.h>
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_timeout.h"
// #include "bluetooth.h"
// #include "bluetooth_no.h"
#include <bluefruit.h>
#include "app/app_motion.h"

extern BLEHidAdafruit blehid;
 
typedef enum
{
    CMD_SAVE = 0,
    CMD_COPY,
    CMD_PASTE,
    CMD_CUT,
    CMD_ENTER,
    CMD_ESC,
    CMD_MAX
} cmd_t;
const char *cmd_str[] = {
    "save",
    "copy",
    "paste",
    "cut",
    "enter",
    "esc",
};
static int8_t cmd_index = 0;
static void switchNext()
{

    cmd_index++;
    if (cmd_index == CMD_MAX)
        cmd_index = 0;
}

static void switchPrev()
{

    cmd_index--;
    if (cmd_index < 0)
        cmd_index = CMD_MAX - 1;
}

static void drawCmd(uint8_t index)
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
    u8g2.drawGlyph(0, 0, 74);
    u8g2.setFont(u8g2_font_8x13_tr);
    u8g2.drawStr(5, 20, cmd_str[index]);
    u8g2.sendBuffer();
}

static void init(void *data)
{
    Keyboard.begin();
    app_motion_init(50, 2); 
 
    Bluefruit.Advertising.start(0);
}

static void enter(void *data)
{

    u8g2.setFontPosBottom();
    u8g2.setFont(u8g2_font_5x8_tr);
    for (uint8_t i = 0; i < OLED_HEIGHT; i += 1)
    {
        u8g2.clearBuffer();

        {
            // u8g2.drawXBM((OLED_WIDTH - bluetooth_no_width) / 2, OLED_HEIGHT - i, bluetooth_no_width, bluetooth_no_height, bluetooth_no_bits);
            u8g2.drawStr(0, 2 * OLED_HEIGHT - i, "Disconnected");
        }
        u8g2.sendBuffer();
    }

    manager_setBusy(false);
}

static void event()
{
    if (Bluefruit.connected())
    {
        // blehid.consumerKeyPress(HID_USAGE_CONSUMER_VOLUME_DECREMENT);

        // // Delay a bit between reports
        // delay(10);

        // // Send key release
        // blehid.consumerKeyRelease();
        switch (cmd_index)
        {
        case CMD_SAVE: //save
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('s');
            Keyboard.release('s');
            Keyboard.release(KEY_LEFT_CTRL);
            break;
        case CMD_COPY: //save
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('c');
            Keyboard.release('c');
            Keyboard.release(KEY_LEFT_CTRL);
            break;
        case CMD_PASTE: //save
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('v');
            Keyboard.release('v');
            Keyboard.release(KEY_LEFT_CTRL);
            break;
        case CMD_CUT: //save
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('x');
            Keyboard.release('x');
            Keyboard.release(KEY_LEFT_CTRL);
            break;
        case CMD_ENTER: //save
            Keyboard.press(HID_KEY_ENTER);
            Keyboard.release(HID_KEY_ENTER);
            break;
        case CMD_ESC: //save
            Keyboard.press(KEY_ESC);
            Keyboard.release(KEY_ESC);

            break;

        default:
            break;
        }
    }
}

static void loop(void *data)
{
    static int8_t index_old = 0;
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

    static bool old_state = false;
    static bool new_state = false;

    if (Bluefruit.connected())
    {
        new_state = true;
        uint8_t dir = app_getDirection();
        if (dir & DIR_RIGHT)
        {
            delay(200);
            if (app_getDirection() == DIR_NONE)
            {
                switchNext();
            }
            app_timeout_reset();
        }
        else if (dir & DIR_LEFT)
        {
            delay(200);
            if (app_getDirection() == DIR_NONE)
            {
                switchPrev();
            }
            app_timeout_reset();
        }

        if (index_old != cmd_index)
        {
            index_old = cmd_index;
            drawCmd(cmd_index);
        }
    }
    else
    {
        new_state = false;
    }
    if (new_state != old_state)
    {
        old_state = new_state;

        u8g2.setFontPosTop();
        drawCmd(cmd_index);
        if (new_state)
        {
            // u8g2.drawXBM((OLED_WIDTH - bluetooth_width) / 2, 0, bluetooth_width, bluetooth_height, bluetooth_bits);
            // u8g2.drawStr(0, OLED_HEIGHT, "Connected");
            u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
            u8g2.drawGlyph(0, 0, 74);
        }
        else
        {
            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_5x8_tr);
            // u8g2.drawXBM((OLED_WIDTH - bluetooth_no_width) / 2, 0, bluetooth_no_width, bluetooth_no_height, bluetooth_no_bits);
            u8g2.drawStr(0, 20, "Disconnected");
        }

        u8g2.sendBuffer();
    }

    // BLE_MAX_CONNECTION
}

static void exit(void *data)
{

    // vTaskDelete(handle);
    app_motion_deinit();
    // Keyboard.end();
    Bluefruit.disconnect(0);
    for (uint8_t i = 0; i < 10; i++)
    {
        Bluefruit.Advertising.stop();
        delay(50);
    }

    manager_setBusy(true);
}
#include "img.h"
page_t page_kuai = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "kuai",
    .title_cn = "快捷键",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
    .sleep_enable = true,
    .wakeup_btn_effect_enable = true,
    .acc_enable = true,

};
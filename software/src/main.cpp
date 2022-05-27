#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include "manager/manager.h"
#include "app/app_key.h"
#include "app/app_motor.h"
#include "app/app_timeout.h"
#include "app/app_motion.h"
#include "app/app_env.h"

// KXTJ3 myIMU(0x0E); // Address can be 0x0E or 0x0F

#include "Button2.h"
// #include <SPI.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
#include <U8g2lib.h>
#include <U8g2wqy.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <BLE52_Mouse_and_Keyboard.h>

#include "board_def.h"

#include "Adafruit_TinyUSB.h"
#include "app/app_batt.h"



//字体定制
// https://github.com/larryli/u8g2_wqy/wiki/CustomFont

#include "bmp.h"

// Adafruit_SSD1306 OLED01(128, 48, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS01);
// const u8g2_cb_t *rotation, uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE
U8G2_SSD1306_64X48_ER_F_4W_HW_SPI u8g2(U8G2_R0, OLED_CS, OLED_DC, OLED_RESET); // EastRising 0.66" OLED breakout board, Uno: A4=SDA, A5=SCL, 5V powered

/////////////////////////////////////////////////////////////////

#include "logo.h"
#include "upgrade.h"
#include <bluefruit.h>
extern BLEHidAdafruit blehid;
extern BLEDis bledis;
typedef volatile uint32_t REG32;
#define pREG32 (REG32 *)

#define DEVICE_ID_HIGH (*(pREG32(0x10000060)))
#define DEVICE_ID_LOW (*(pREG32(0x10000064)))
#define MAC_ADDRESS_HIGH (*(pREG32(0x100000a8)))
#define MAC_ADDRESS_LOW (*(pREG32(0x100000a4)))

static void timeout_timer_callback(TimerHandle_t xTimerID);

U8G2LOG u8g2log;

// setup the terminal (U8G2LOG) and connect to u8g2 for automatic refresh of the display
// The size (width * height) depends on the selected font and the display

// assume 5x7 font
#define U8LOG_WIDTH 12 // 64/5=12
#define U8LOG_HEIGHT 6 //  48/7=6
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(4, PIN_NEOLED, NEO_RGB + NEO_KHZ800);
static void loop2();
static TaskHandle_t handle;
bool flag_lowpower = false;
void getBatt(void *para)
{
        uint8_t percent = 0;

        while (1)
        {
                percent = app_batt_percent();
                if (percent <= 10)
                {
                        flag_lowpower = false;
                }
                else
                {
                        flag_lowpower = true;
                }
                delay(2 * 60 * 1000);
        }
}

#include "time.h"

static struct tm time_struct, m_tm_return_time;
static time_t newtime;
void set_time(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second)
{

        time_struct.tm_year = year - 1900;
        time_struct.tm_mon = month;
        time_struct.tm_mday = day;
        time_struct.tm_hour = hour;
        time_struct.tm_min = minute;
        time_struct.tm_sec = second;
        newtime = mktime(&time_struct);
}
static SoftwareTimer timer_date;
struct tm *get_time(void)
{
        time_t return_time = newtime;
        m_tm_return_time = *localtime(&return_time);
        return &m_tm_return_time;
}

void updateTime(TimerHandle_t xTimer )
{     
    newtime++; 
}

void app_time_init()
{
        //创建定时获取任务 

    
    timer_date.begin(1000, updateTime); 
    timer_date.start();
}

void adv_config(void)
{
        // Advertising packet
        Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
        Bluefruit.Advertising.addTxPower();
        Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);

        // Include BLE HID service
        Bluefruit.Advertising.addService(blehid);

        // There is enough room for the dev name in the advertising packet
        Bluefruit.Advertising.addName();

        /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
        Bluefruit.Advertising.restartOnDisconnect(true);
        Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
        Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
}
void ble_config()
{
        char buffer[20];
        sprintf(buffer, "BB-%08X", DEVICE_ID_LOW);
        Bluefruit.begin();
        Bluefruit.setTxPower(4); // Check bluefruit.h for supported values

        // Configure and Start Device Information Service
        bledis.setManufacturer("ModularStudio");
        bledis.setModel(buffer);
        bledis.begin();

        /* Start BLE HID
   * Note: Apple requires BLE device must have min connection interval >= 20m
   * ( The smaller the connection interval the faster we could send data).
   * However for HID and MIDI device, Apple could accept min connection interval 
   * up to 11.25 ms. Therefore BLEHidAdafruit::begin() will try to set the min and max
   * connection interval to 11.25  ms and 15 ms respectively for best performance.
   */
        blehid.begin();

        // Set callback for set LED from central
        // blehid.setKeyboardLedCallback(set_keyboard_led);

        /* Set connection interval (min, max) to your perferred value.
   * Note: It is already set by BLEHidAdafruit::begin() to 11.25ms - 15ms
   * min = 9*1.25=11.25 ms, max = 12*1.25= 15 ms 
   */
        /* Bluefruit.Periph.setConnInterval(9, 12); */
        Bluefruit.Periph.setConnInterval(9, 16); // min = 9*1.25=11.25 ms, max = 16*1.25=20ms

        adv_config();

        Bluefruit.Advertising.stop();
}
void setup()
{
        sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
        sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
        ble_config();
        char buffer[20];

 
        pinMode(PIN_SW01, INPUT_PULLUP);
 

        u8g2.begin();
        u8g2.setContrast(1000);

        //LOGO begin
        for (uint8_t i = 1; i <= 24; i++)
        {
                u8g2.clearBuffer();
                // u8g2.drawXBMP(0, 48 - i - 1, logo_width, i, logo_bits);

                u8g2.drawXBMP(0, 24 - i, logo_width, i, logo_bits);
                u8g2.drawXBMP(0, 24, logo_width, i, &logo_bits[logo_width / 8 * (48 - i)]);
                u8g2.sendBuffer();
                delay(20);
        }

        if (digitalRead(PIN_SW01) == 0)
        {
                //进入ota模式
                u8g2.clearBuffer();
                u8g2.drawXBMP((OLED_WIDTH - upgrade_width) / 2, (OLED_HEIGHT - upgrade_height) / 2, upgrade_width, upgrade_height, upgrade_bits);
                u8g2.sendBuffer();
                delay(3000);
                // enterUf2Dfu();
                enterOTADfu();
        }

        delay(1000);

        // u8g2.noDisplay();
        // delay(3000);

        // u8g2.display();
        // u8g2.display();
        u8g2.setFont(u8g2_font_5x7_tr); // set the font for the terminal window
        u8g2log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
        u8g2log.setLineHeightOffset(1); // set extra space between lines in pixel, this can be negative
        u8g2log.setRedrawMode(1);       // 0: Update screen with newline, 1: Update screen for every char

        

        u8g2.clearBuffer();
        sprintf(buffer, "BB-%08X", DEVICE_ID_LOW);
        u8g2log.println(buffer);
        u8g2log.println("----------");
        delay(100);


        app_batt_init();
        uint8_t percent = app_batt_percent();
        u8g2log.printf("Batt: %d%%", percent);
        u8g2log.println();
        delay(100);

        // acc
        if (app_motion_init(6.25, 2) == false)
        {
                u8g2log.print("Failed to initialize IMU.\n");
        }
        else
        {
                u8g2log.println("IMU initialized.\n");
                app_motion_deinit();
        }

        delay(100);

        //humi&temp
        if (app_env_init() == false)
        {
                u8g2log.println("Could not find AHT? Check wiring");
                // while (1)
                //         delay(10);
        }
        else
        {
                u8g2log.println("AHT10 or AHT20 found");

                env_t data;
                app_env_get(&data);
                u8g2log.print("Temperature: ");
                u8g2log.print(data.temperature);
                u8g2log.println(" degrees C");delay(100);
                u8g2log.print("Humidity: ");
                u8g2log.print(data.humidity);
                u8g2log.println("% rH");delay(100);

                app_env_deinit();
        }
        delay(100);

        //创建定时获取任务
        BaseType_t res = xTaskCreate(getBatt, "battery", SCHEDULER_STACK_SIZE_DFLT, (void *)NULL, TASK_PRIO_LOW, &handle);
        if (res == pdPASS)
        {
        }
        u8g2log.println("set time ");
        set_time(2021, 9, 10, 14, 31, 0);
        app_time_init();
 
        u8g2log.println("manager_init ");
        manager_init();
        u8g2log.println("manager_init done");
        app_timeout_init(timeout_timer_callback);
        app_motor_init();
        app_key_init();

        u8g2log.println("done ");
 

        // enterOTADfu();
        pinMode(PIN_NEOPWR, OUTPUT);
        digitalWrite(PIN_NEOPWR, LOW);
        neopixel.begin();
        neopixel.clear();
        uint32_t color;
        if (percent <= 30)
        {
                color = neopixel.Color(0xff, 0, 0);
        }
        else if (percent <= 50)
        {
                color = neopixel.Color(0xff, 0xff, 0);
        }
        else if (percent <= 80)
        {
                color = neopixel.Color(0, 0xff, 0);
        }
        else
        {
                color = neopixel.Color(0, 0xff, 0xff);
        }
        for (int i = 0; i < 4; i++)
        { // For each pixel...

                // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
                // Here we're using a moderately bright green color:
                // neopixel.setPixelColor(i, neopixel.Color(255-i*32, i*64-1,i*32-1));
                neopixel.setPixelColor(i, color);

                neopixel.show(); // Send the updated pixel colors to the hardware.

                delay(50); // Pause before next pass through loop
        }
        delay(1000); // Pause before next pass through loop
        // Scheduler.startLoop(loop2);
        neopixel.~Adafruit_NeoPixel();

        nrf_gpio_cfg_default(31+6);
        nrf_gpio_cfg_default(15);
}
 
static bool flag_exit_sleep = false;
void loop()
{
        if (app_timeout_getState())
        {
                if (manager.cur->acc_enable && manager.cur->sleep_enable)
                {
                        Wire.begin();
                        app_motion_wakeup();
                        
                }
                if (manager.cur->wakeup_btn_effect_enable == false)
                {
                        if (digitalRead(PIN_SW01) == 0)
                        {

                                SPI.begin();
                                SPI.setClockDivider(SPI_CLOCK_DIV8);
                                // u8g2.begin();
                                // Wire.begin();
                                u8g2.display();
                                flag_exit_sleep = true;
                        }
                        else
                        {
                                if (flag_exit_sleep)
                                {

                                        app_timeout_SetState(false);
                                        app_key_startLoop();
                                        app_timeout_reset();
                                        app_key_clean();
                                        flag_exit_sleep = false;
                                }
                              
                        }
                }
                else
                {
                        if (digitalRead(PIN_SW01) == 0)
                        {

                                SPI.begin();
                                SPI.setClockDivider(SPI_CLOCK_DIV8);
                                // u8g2.begin();
                                // Wire.begin();
                                u8g2.display();
                                app_key_startLoop();
                                app_timeout_SetState(false);
                                app_timeout_reset();
                        }
                }

                
        }
        else
        {
                manager_loop();
                if (manager.cur->sleep_enable == false)
                {
                        app_timeout_SetState(false);
                        app_timeout_reset();
                }
        }
}

static void timeout_timer_callback(TimerHandle_t xTimerID)
{
        // freeRTOS timer ID, ignored if not used
        (void)xTimerID;
        app_timeout_stop();

        u8g2.noDisplay();

        app_timeout_SetState(true);
        app_key_stopLoop();
        app_key_clean();

        SPI.end();

        if (manager.cur->acc_enable && manager.cur->sleep_enable)
        {
                app_motion_sleep();
                Wire.end();
        }
        // Wire.end();

        // if (usb.mounted())
        // {
        // }
        // else
        // {
        //   // // 关闭qspi外设
        //   // nrfx_qspi_uninit();
        //   // *(volatile uint32_t *)0x40029010ul = 1ul;
        //   // *(volatile uint32_t *)0x40029054ul = 1ul;
        // }

        // digitalWrite(PIN_WIRE_SCL, 1);
        // digitalWrite(PIN_WIRE_SDA, 1);

        // pinMode(PIN_WIRE_SCL, OUTPUT);
        // pinMode(PIN_WIRE_SDA, OUTPUT);

        // systemOff(PIN_SW01, 0);
}

void test_sleep()
{

        uint32_t tt = millis();
        uint32_t tt_old = tt;
        bool flag_slp = false;
        while (1)
        {
                if (digitalRead(PIN_SW01) == 0)
                {
                        if (flag_slp)
                        {
                                u8g2.display();
                                flag_slp = false;
                                delay(100);
                        }
                }
                if ((millis() - tt) >= 3000)
                {
                        tt = millis();
                        if (flag_slp == false)
                        {
                                flag_slp = true;
                                u8g2.noDisplay();
                        }
                }
        }
}
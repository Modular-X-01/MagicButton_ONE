#ifndef __BOARD_DEF_H
#define __BOARD_DEF_H


#define VER_HW  "1.1"
#define VER_SW  "1.1"


#define MCU_NRF52840 0
#define MCU_ESP32C3 1


#define OLED_SSD1306 0
#define OLED_SSD1315    1

#define ACC_KXTJ3_1057  0
#define ACC_QMA7981     1
#define ACC_LSM6DS3TR   2

#define ENV_AHT2X   0
#define ENV_SHT30   1




#define HW_MCU  MCU_NRF52840
#define HW_OLED OLED_SSD1306  
#define HW_ACC  ACC_KXTJ3_1057
// #define HW_ENV  ENV_AHT2X

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


#define OLED_WIDTH 64
#define OLED_HEIGHT 48


#define MAX_ACC 28//退出操作的加速度最大阈值




#define OLED_MOSI 16 //0.30
#define OLED_CLK 21  //0.31
#define OLED_DC 20   //0.29

#define OLED_CS 46   //1.13
#define OLED_RESET 18 //0.02

#define PIN_SW01 17 //0.28


#define PIN_MOTOR 9  //0.26


#define PIN_NEOLED  24  //p0.15
#define PIN_NEOPWR  42  //low active  p1.06


// #define PIN_QSPI_SCK         1//0.24
// #define PIN_QSPI_CS          29//0.17
// #define PIN_QSPI_IO0         36//1.00
// #define PIN_QSPI_IO1         28//0.20
// #define PIN_QSPI_IO2         25//0.13
// #define PIN_QSPI_IO3         30//0.22



extern U8G2_SSD1306_64X48_ER_F_4W_HW_SPI u8g2;
extern U8G2LOG u8g2log;






#endif

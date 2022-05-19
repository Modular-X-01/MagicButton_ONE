#include "clock.h"
#include "board_def.h"
#include "app/app_key.h"
#include "app/app_motion.h"
#include "time.h"
#include "taikong_32/taikong_32_01.h"
#include "taikong_32/taikong_32_02.h"
#include "taikong_32/taikong_32_03.h"
#include "taikong_32/taikong_32_04.h"
#include "taikong_32/taikong_32_05.h"
#include "taikong_32/taikong_32_06.h"
#include "taikong_32/taikong_32_07.h"
#include "taikong_32/taikong_32_08.h"
#include "taikong_32/taikong_32_09.h"

uint8_t *img_taikong[] = {taikong_32_01_bits, taikong_32_02_bits,
						  taikong_32_03_bits, taikong_32_04_bits,
						  taikong_32_05_bits, taikong_32_06_bits,
						  taikong_32_07_bits, taikong_32_08_bits,
						  taikong_32_09_bits};

extern struct tm *get_time(void);

extern void set_time(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second);

static void disp_anim()
{
	uint8_t x = 0, y = 8;
	static uint8_t i = 0;
	u8g2.setDrawColor(0);
	u8g2.drawXBM(x, y, 32, 32, img_taikong[i]);

	// 转换时出现左和上边框白边
	u8g2.drawFrame(x, y, 32, 1);
	u8g2.drawFrame(x, y, 1, 32);

	i++;
	if (i == 9)
	{
		i = 0;
	}
}

static void disp_time(uint32_t hour, uint32_t minute)
{
	char buffer[20];

	u8g2.clearBuffer();

	disp_anim();

	u8g2.setFontPosTop();
	u8g2.setFont(u8g2_font_crox5hb_tn);

	memset(buffer, 0, 20);
	snprintf(buffer, 20, "%01d%01d", hour / 10, hour % 10);
	u8g2.setDrawColor(1);
	u8g2.drawStr(33, 0, buffer);

	u8g2.setFont(u8g2_font_crox5h_tn);
	memset(buffer, 0, 20);
	snprintf(buffer, 20, "%01d%01d", minute / 10, minute % 10);
	u8g2.setDrawColor(1);
	u8g2.drawStr(33, 24, buffer);

	u8g2.sendBuffer();
}

static void disp_date(uint32_t month, uint32_t day)
{
}

const char *str_settings[] =
	{
		"Year Setting",
		"Month Setting",
		"Day Setting",
		"Hour Setting",
		"Min Setting",
		"Sec Setting",
		"Save"

};
static int time_set[7] = {0};
static int8_t _set_index = 0;

static void drawSetting(int num)
{

	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_6x12_tr);
	u8g2.drawStr(0, 0, str_settings[_set_index]);
	// u8g2.drawStr(0, 0, "sdsd");

	if (_set_index != 6)
	{
		u8g2.setFont(u8g2_font_logisoso20_tn);
		u8g2.setCursor(5, 15);
		u8g2.printf("%0d", num);
	}
	else
	{
		u8g2.setFont(u8g2_font_9x18B_tr);
		u8g2.setCursor(5, 15);
		u8g2.print("Click ");
		u8g2.setCursor(5, 33);
		u8g2.print(" Button");
	}
	u8g2.sendBuffer();
}

static void switchNext()
{

	_set_index++;
	if (_set_index == 7)
		_set_index = 0;
}

static void switchPrev()
{

	_set_index--;
	if (_set_index < 0)
		_set_index = 6;
}
 

static void init(void *data)
{
}

static void enter(void *data)
{

	u8g2.clear();
	struct tm *time_now = get_time();
	disp_time(time_now->tm_hour, time_now->tm_min);

	manager_setBusy(false);
}

static void loop(void *data)
{
	KEY_TYPE key;
	struct tm *time_now;
	static bool f_setting = false;
	static uint32_t t_old = 0;

	if (f_setting == false)
	{
		if ((millis() - t_old) >= 100)
		{
			t_old = millis();
			//update screen
			time_now = get_time();
			disp_time(time_now->tm_hour, time_now->tm_min);
		}
		key = app_key_get();
		switch (key)
		{

		case KEY0_DOWN:

			break;

		case KEY0_SHORT:
			if (f_setting == false)
			{
				f_setting = true;
			}

			break;

		case KEY0_LONG:				  //长按
			manager_switchToParent(); //进入父项目 //退出
			break;
		default:
			break;
		}
	}

	else
	{
		static bool init = false;
		struct tm *time_setting;
		if (init == false)
		{
			manager.cur->sleep_enable = false;
			time_setting = get_time();

			app_motion_init(50, 2);
			_set_index = 0;
			time_set[0] = time_setting->tm_year+1900; //实际需要加1900
			time_set[1] = time_setting->tm_mon;
			time_set[2] = time_setting->tm_mday;
			time_set[3] = time_setting->tm_hour;
			time_set[4] = time_setting->tm_min;
			time_set[5] = time_setting->tm_sec;
			init = true;
		}

		uint8_t dir = app_getDirection();
		if (dir & DIR_RIGHT)
		{
			delay(200);
			if (app_getDirection() == DIR_NONE)
			{
				switchNext();
			}
		}
		else if (dir & DIR_LEFT)
		{
			delay(200);
			if (app_getDirection() == DIR_NONE)
			{
				switchPrev();
			}
		}
		if (dir & DIR_UP)
		{

			delay(500);

			time_set[_set_index]--;
		}
		else if (dir & DIR_DOWN)
		{
			delay(500);
			time_set[_set_index]++;
		}
		drawSetting(time_set[_set_index]);
		// drawSetting(time_setting->tm_year);

		key = app_key_get();
		if (key == KEY0_SHORT)
		{
			if (_set_index == 6) //保存
			{
				set_time(time_set[0], time_set[1], time_set[2], time_set[3], time_set[4], time_set[5]);
			}
			f_setting = false;
			init = false;
			manager.cur->sleep_enable = true;
			app_motion_deinit();
			app_key_clean();
			delay(100);
		}
	}
}

static void exit(void *data)
{

	manager_setBusy(true);
}
#include "img.h"
page_t page_clock = {
	.init = init,
	.enter = enter,
	.exit = exit,
	.loop = loop,
	.title_en = "Clock",
	.title_cn = "时间",
	.icon = img_bits,
	.icon_width = img_width,
	.icon_height = img_height,
	.sleep_enable = true,
	.wakeup_btn_effect_enable = false,

};
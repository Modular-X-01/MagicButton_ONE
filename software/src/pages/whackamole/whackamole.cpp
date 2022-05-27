#include "whackamole.h"
#include "board_def.h"
#include "app/app_key.h"
#include "time.h"
extern struct tm *get_time(void);

static void disp_title(void)
{

	u8g2.setFontPosTop();
	u8g2.setFont(u8g2_font_bubble_tr);
	u8g2.clearBuffer();

	u8g2.setDrawColor(1);
	u8g2.drawStr(0, 0, "Update");

	u8g2.sendBuffer();
}

 

static void init(void *data)
{
}

static void enter(void *data)
{

	u8g2.clear();
	disp_title();
	// enterUf2Dfu();
	enterOTADfu();
 

	manager_setBusy(false);
}

static void loop(void *data)
{

	KEY_TYPE key = app_key_get();
	switch (key)
	{

	case KEY0_DOWN:

		break;

	case KEY0_SHORT:

		break;

	case KEY0_LONG:				  //长按
		manager_switchToParent(); //进入父项目 //退出
		break;
	default:
		break;
	}
}

static void exit(void *data)
{

	manager_setBusy(true);
}
#include "img.h"
page_t page_whackamole = {
	.init = init,
	.enter = enter,
	.exit = exit,
	.loop = loop,
	.title_en = "Whackamole",
	.title_cn = "打地鼠",
	.icon = img_bits,
	.icon_width = img_width,
	.icon_height = img_height,
	.sleep_enable = true,
    .wakeup_btn_effect_enable = false,

};
#include "bird.h"
#include "board_def.h"
#include "app/app_key.h"

#define BUTTON_PIN PIN_SW01
#define HEIGHT 48
#define WIDTH 64
#define LINE 9
#define DELAY 50
#define DIFFICULTY 128 // 1..65535 (shorter = harder)
#define FRAMES_PER_MOVE 2
#define FRAMES_FLASH_TITLE 25
#define FRAMES_GAME_OVER 200
#define OBSTACLES 8
#define YMIN 5
#define APERTURE 20
#define SEPARATION 12
#define FLASHES 8

#define BLACK 0
#define WHITE 1

struct Obstacle {
	int x;
	int y;
};
enum Status {
	TITLE, ANIMATE_TITLE, PLAYING, GAME_OVER, WAIT_TITLE
};

int frames;
bool flag_flash;
unsigned int score;
unsigned int high;
int y;
Obstacle obstacles[OBSTACLES];
int last;
Status status;


void eepromIntWrite(int value) {
	// EEPROM.write(0, value & 0xFF);
	// EEPROM.write(1, value >> 8);
}

int eepromIntRead() {
	// return EEPROM.read(0) | (EEPROM.read(1) << 8);
    return 0;
}

void reset() {
    u8g2.clear();
    u8g2.setDrawColor(WHITE);	
    u8g2.setFont(u8g2_font_5x7_tr);
	y = HEIGHT / 2;
	flag_flash = false;
	frames = 0;
	score = 0;
	high = eepromIntRead();
	status = TITLE;
	randomSeed(analogRead(PIN_VBAT));
	for (int i = 0; i < OBSTACLES; i++) {
		obstacles[i].x = WIDTH + i * SEPARATION;
		obstacles[i].y = YMIN + random(HEIGHT - LINE + 1 - APERTURE - YMIN);
	}
	last = OBSTACLES - 1;
}

void title() {
	u8g2.clearBuffer();
    u8g2.setDrawColor(WHITE);	
	u8g2.setFont(u8g2_font_5x7_tr);
	// Top title
	u8g2.setCursor(6, 0);
	u8g2.print("MicroBird");

	// High score
	u8g2.setCursor(0, 40);
	u8g2.print("TOP ");
	u8g2.setCursor(24, 40);
	u8g2.print(high);

	u8g2.display();
    u8g2.sendBuffer();

	status = ANIMATE_TITLE;
}

void animateTitle() {
	// Animate "PRESS BUTTON" text
	if (frames == FRAMES_FLASH_TITLE) {
		String press = "PRESS";
		String button = "BUTTON";
		if (flag_flash) {
			press = "     ";
			button = "      ";
		}
        u8g2.setFont(u8g2_font_5x7_tr);
		u8g2.setCursor(17, 15);
		u8g2.print(press);
		u8g2.setCursor(14, 24);
		u8g2.print(button);
		u8g2.display();
        u8g2.sendBuffer();
		flag_flash = !flag_flash;
		frames = -1;
	}
	frames++;

	// Check button
	if (digitalRead(BUTTON_PIN) == HIGH) {
		frames = 0;
		status = PLAYING;
	}

	delay(DELAY);
}

void gameOver() {
	// Flash
	// for (int i = FLASHES; i > 0; i--) {
	// 	u8g2.invert(true);
        
	// 	delay(DELAY << 1);
	// 	u8g2.invert(false);
	// 	delay(DELAY << 1);
	// }

	// Game Over with black rectangle background
	u8g2.setDrawColor(BLACK);
	u8g2.drawBox(15, 15, 33, 25);
    u8g2.setFont(u8g2_font_5x7_tr);
	u8g2.setDrawColor(WHITE);	
	u8g2.setCursor(20, 20);
	u8g2.print("GAME");

	u8g2.setCursor(20, 28);
	u8g2.print("OVER");

	u8g2.display();
    u8g2.sendBuffer();

	// New high score?
	if (score > high) {
		high = score;
		eepromIntWrite(high);
	}

	frames = 0;

	status = WAIT_TITLE;
}

void waitTitle() {
	frames++;
	if (frames == FRAMES_GAME_OVER || digitalRead(BUTTON_PIN) == LOW) {
		reset();
	}
	delay(DELAY);
}

void playing() {
	// Update player
	if (frames == FRAMES_PER_MOVE) {
		frames = -1;
		if (digitalRead(BUTTON_PIN) == LOW) {
			y -=2;
		} else {
			y++;
		}
	}
	if (y <= LINE) {
		y = LINE + 1;
	} else if (y >= HEIGHT) {
		y = HEIGHT - 1;
	}
	frames++;

	// Show
	u8g2.clear();
	u8g2.drawLine(0, LINE, WIDTH, LINE);
 
	// Update obstacles
	for (int i = 0; i < OBSTACLES; i++) {
		obstacles[i].x--;
		if (obstacles[i].x <= -SEPARATION) {
			obstacles[i].x = obstacles[last].x + SEPARATION - (last > i ? 1 : 0);
			obstacles[i].y = YMIN + random(HEIGHT - LINE + 1 - APERTURE - YMIN);
			last = i;
			score++;
		}
		// Draw visible obstacles
		if (obstacles[i].x < WIDTH) {
			u8g2.drawBox(obstacles[i].x, LINE, SEPARATION, obstacles[i].y);
			u8g2.drawBox(obstacles[i].x, LINE + APERTURE + obstacles[i].y, SEPARATION, HEIGHT - (LINE + APERTURE + obstacles[i].y));
		}
		// Check collision
		if (obstacles[i].x <= 0 && (y < (LINE + obstacles[i].y) || y > (LINE + APERTURE + obstacles[i].y))) {
			status = GAME_OVER;
		}
	}

    u8g2.setFont(u8g2_font_5x7_tr);
	u8g2.setDrawColor(WHITE);
	u8g2.setCursor(0, 7);
	u8g2.print(score);
	u8g2.drawPixel(0, y);
	u8g2.sendBuffer();


	// Make it harder...
	int df = (int) (DELAY * (1 - (float) score / DIFFICULTY));
	if (df < 0) {
		df = 0;
	}
	delay(df);
}


























static void init(void *data)
{
    randomSeed(analogRead(PIN_VBAT));
 
}

static void enter(void *data)
{
    
    reset(); 
    manager_setBusy(false);
}

static void loop(void *data)
{
	switch (status) {
		case TITLE:
			title();
			break;

		case ANIMATE_TITLE:
			animateTitle();
			break;

		case PLAYING:
			playing();
			break;

		case GAME_OVER:
			gameOver();
			break;

		case WAIT_TITLE:
			waitTitle();
			break;
	}



    KEY_TYPE key = app_key_get();
    switch (key)
    {

    case KEY0_DOWN:

        break;




    case KEY0_SHORT:

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

    manager_setBusy(true);
}
#include "img.h"
page_t page_bird = {
    .init = init,
    .enter = enter,
    .exit = exit,
    .loop = loop,
    .title_en = "Bird",
    .title_cn = "鸟",
    .icon = img_bits,
    .icon_width = img_width,
    .icon_height = img_height,
	.sleep_enable = false,
    .wakeup_btn_effect_enable = false,

};
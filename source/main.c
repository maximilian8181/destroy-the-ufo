#include <tonc.h>

#include "hero_ship.h"
#include "foe_ship.h"
#include "energyBall.h"
#include "numbers.h"
#include "scoreLabel.h"
#include "timerLabel.h"

#include "titleCard.h"
#include "simpleBackground.h"
#include "endCard.h"
#include "badEndCard.h"

#define NUM_BALLS 10

OBJ_ATTR obj_buffer[128];

typedef struct object {
	OBJ_ATTR *obj;
	int x;
	int y;
	int width;
	int height;
	int delta;
	int active;
	u32 tid;
	u32 pb; 
} object;

enum gameState {
	START,
	GAME,
	END_GOOD,
	END_BAD
};

enum gameState state;

void load_titleCardData(void);
void runInitialState(void);

void firstSetUp(void);

void load_endCardData(void);
void runFinalState(void);

void load_badEndCardData(void);
void runBadFinalState(void);

void load_backgroundData(void);
void runGameState(void);

void load_spriteData(void);
void init_n_show_sprites(void);

void game(void);

void update(void);
void updatePlayer(void);
void updateUfo(void);
void updateNumbers(void);
void updateTimerNumbers(void);
void updateEnergyBall(object *o);

void fireEnergyBall(void);
void destroyEnergyBall(object *o);

void draw(void);
void drawPlayer(void);
void drawUfo(void);
void drawLabel(void);
void drawNumbers(void);
void drawTimerLabel(void);
void drawTimerNumbers(void);
void drawEnergyBall(object *o);

int detectCollision(object *a, object *b);

void performtitleCardAnimation(void);
void performEnergyBallAnimation(object *o);
void performBackgroundAnimation(void);
void performBadBackgroundAnimation(void);
void performBlinkingAnimation(void);

void hideAllSprites(void);
void showFinalPositions(void);

object starfighter;
object ufo;
object ball[NUM_BALLS];
object scoreLabel;
object numberLabel;
object timerLabel;
object timerNumberLabel;

int x = 0;
int y = 0;

int score = 0;
int timer = 10;

int frames1 = 0;
int frames2 = 0;
int frames3 = 0;
int frames4 = 0;
int frames5 = 0;
int frames6 = 0;

int main(void)
{
	irq_init(NULL);
	irq_enable(II_VBLANK);

	REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

	firstSetUp();

	while (1) {
		VBlankIntrWait();

		key_poll();

		switch(state) {
			case START:
				runInitialState();
				break;
			case GAME:
				runGameState();
				break;
			case END_GOOD:
				runFinalState();
				break;
			case END_BAD:
				runBadFinalState();
				break;
		}
	}
}

void load_titleCardData(void) {
	memcpy16(pal_bg_mem, titleCardPal, titleCardPalLen / 2);
	memcpy16(&tile_mem[0][0], titleCardTiles, titleCardTilesLen / 2);
	memcpy16(&se_mem[30][0], titleCardMap, titleCardMapLen / 2);
}

void runInitialState(void) {
	performtitleCardAnimation();

	if(key_hit(KEY_START)) {	
		load_backgroundData();
		load_spriteData();
		init_n_show_sprites();

		state = GAME;
	}
}

void firstSetUp(void) {
	hideAllSprites();

	load_titleCardData();

	state = START;
}

void load_endCardData(void) {
	memcpy16(pal_bg_mem, endCardPal, endCardPalLen / 2);
	memcpy16(&tile_mem[0][0], endCardTiles, endCardTilesLen / 2);
	memcpy16(&se_mem[30][0], endCardMap, endCardMapLen / 2);
}

void runFinalState(void) {
	hideAllSprites();

	load_endCardData();

	obj_unhide(starfighter.obj, 0);
	obj_unhide(ufo.obj, 0);

	performBackgroundAnimation();
	showFinalPositions();

	if(key_hit(KEY_START)) {
		firstSetUp();
	}
}

void load_badEndCardData(void) {
	memcpy16(pal_bg_mem, badEndCardPal, badEndCardPalLen / 2);
	memcpy16(&tile_mem[0][0], badEndCardTiles, badEndCardTilesLen / 2);
	memcpy16(&se_mem[30][0], badEndCardMap, badEndCardMapLen / 2);
}

void runBadFinalState(void) {
	hideAllSprites();

	load_badEndCardData();

	performBadBackgroundAnimation();

	if(key_hit(KEY_START)) {
		firstSetUp();
	}
}

void load_backgroundData(void) {
	memcpy16(pal_bg_mem, simpleBackgroundPal, simpleBackgroundPalLen / 2);
	memcpy16(&tile_mem[0][0], simpleBackgroundTiles, simpleBackgroundTilesLen / 2);
	memcpy16(&se_mem[30][0], simpleBackgroundMap, simpleBackgroundMapLen / 2);
}

void runGameState(void) {
	game();
}

void load_spriteData(void)
{
	memcpy16(&tile_mem[4][0], hero_shipTiles, hero_shipTilesLen / 2);
	memcpy16(pal_obj_mem, hero_shipPal, hero_shipPalLen / 2);

	memcpy16(&tile_mem[4][4], foe_shipTiles, foe_shipTilesLen / 2);
	memcpy16(pal_obj_mem + 32, foe_shipPal, foe_shipPalLen / 2);

	memcpy16(&tile_mem[4][12], energyBallTiles, energyBallTilesLen / 2);
	memcpy16(pal_obj_mem + 64, energyBallPal, energyBallPalLen / 2);

	memcpy16(&tile_mem[4][13], numbersTiles, numbersTilesLen / 2);
	memcpy16(pal_obj_mem + 96, numbersPal, numbersPalLen / 2);

	memcpy16(&tile_mem[4][24], scoreLabelTiles, scoreLabelTilesLen / 2);
	memcpy16(pal_obj_mem + 112, scoreLabelPal, scoreLabelPalLen / 2);

	memcpy16(&tile_mem[4][28], timerLabelTiles, timerLabelTilesLen / 2);
	memcpy16(pal_obj_mem + 128, timerLabelPal, timerLabelPalLen / 2);

	oam_init(obj_buffer, 128);
}

void init_n_show_sprites(void) {
	starfighter.obj = &obj_buffer[0];
	starfighter.x = (SCREEN_WIDTH >> 1) - 8;
	starfighter.y = SCREEN_HEIGHT - 16;
	starfighter.width = 16;
	starfighter.height = 16;
	starfighter.delta = 1;
	starfighter.active = 1;
	starfighter.tid = 0;
	starfighter.pb = 0;

	ufo.obj = &obj_buffer[1];
	ufo.x = SCREEN_WIDTH - 32;
	ufo.y = 17;
	ufo.width = 32;
	ufo.height = 16;
	ufo.delta = 1;
	ufo.active = 1;
	ufo.tid = 4;
	ufo.pb = 2;

	scoreLabel.obj = &obj_buffer[2];
	scoreLabel.x = 0;
	scoreLabel.y = SCREEN_HEIGHT - 8;
	scoreLabel.width = 32;
	scoreLabel.height = 8;
	scoreLabel.active = 1;
	scoreLabel.delta = 0;
	scoreLabel.tid = 24;
	scoreLabel.pb = 7;

	numberLabel.obj = &obj_buffer[3];
	numberLabel.x = scoreLabel.width + 8;
	numberLabel.y = scoreLabel.y;
	numberLabel.width = 8;
	numberLabel.height = 8;
	numberLabel.delta = 0;
	numberLabel.active = 1;
	numberLabel.tid = 13;
	numberLabel.pb = 6;

	for(int i = 0; i < NUM_BALLS; i++) {
		ball[i].obj = &obj_buffer[i + 4];
		ball[i].x = 0;
		ball[i].y = 0;
		ball[i].width = 8;
		ball[i].height = 8;
		ball[i].delta = 0;
		ball[i].active = 0;
		ball[i].tid = 12;
		ball[i].pb = 4;
	}

	timerLabel.obj = &obj_buffer[14];
	timerLabel.x = SCREEN_WIDTH - scoreLabel.width - 8;
	timerLabel.y = SCREEN_HEIGHT - 8;
	timerLabel.width = 32;
	timerLabel.height = 8;
	timerLabel.delta = 1;
	timerLabel.active = 1;
	timerLabel.tid = 28;
	timerLabel.pb = 8;

	timerNumberLabel.obj = &obj_buffer[15];
	timerNumberLabel.x = SCREEN_WIDTH - 8;;
	timerNumberLabel.y = SCREEN_HEIGHT - 8;
	timerNumberLabel.width = 8;
	timerNumberLabel.height = 8;
	timerNumberLabel.delta = 0;
	timerNumberLabel.active = 1;
	timerNumberLabel.tid = 23;
	timerNumberLabel.pb = 8;
}

void game(void) {
	update();
	draw();
}

void update(void) {
	updatePlayer();
	updateUfo();
	updateNumbers();
	updateTimerNumbers();

	for(int i = 0; i < NUM_BALLS; i++) {
		updateEnergyBall(&ball[i]);
	}
}

void updatePlayer(void) {
	x = starfighter.x + starfighter.delta * key_tri_horz();

	if(x >= 0 && x <= SCREEN_WIDTH - starfighter.width) {
		starfighter.x = x;
	}

	if(key_hit(KEY_A)) {
		fireEnergyBall();
	}
}

void updateUfo(void) {
	ufo.x = ufo.x - ufo.delta;

	if(ufo.x == 0 || ufo.x == SCREEN_WIDTH - ufo.width) {
		ufo.delta = -ufo.delta;
	}

	for(int i = 0; i < NUM_BALLS; i++) {
		if(ball[i].active == 1 && detectCollision(&ufo, &ball[i])) {
			ufo.x = SCREEN_WIDTH - 32;
			ufo.y = 17;
			ufo.delta = 1;
			score = score + 1;
			destroyEnergyBall(&ball[i]);
		}
	}
}

void updateNumbers(void) {
	switch(score) {
		case 0:
			numberLabel.tid = 13;
			break;
		case 1:
			numberLabel.tid = 14;
			break;
		case 2:
			numberLabel.tid = 15;
			break;
		case 3:
			numberLabel.tid = 16;
			break;
		case 4:
			numberLabel.tid = 17;
			break;
		case 5:
			numberLabel.tid = 18;
			break;
		case 6:
			numberLabel.tid = 19;
			break;
		case 7:
			numberLabel.tid = 20;
			break;
		case 8:
			numberLabel.tid = 21;
			break;
		case 9:
			numberLabel.tid = 22;
			break;
		case 10:
			timer = 10;
			score = 0;

			state = END_GOOD;
			break;
		default:
			timer = 10;
			score = 0;
			
			break;
	}
}

void updateTimerNumbers(void) {
	frames5++;

	if(frames5 == 90) {
		timer = timer - 1;

		switch(timer) {
			case 10:
				timerNumberLabel.tid = 23;
				break;
			case 9:
				timerNumberLabel.tid = 22;
				break;
			case 8:
				timerNumberLabel.tid = 21;
				break;
			case 7:
				timerNumberLabel.tid = 20;
				break;
			case 6:
				timerNumberLabel.tid = 19;
				break;
			case 5:
				timerNumberLabel.tid = 18;
				break;
			case 4:
				timerNumberLabel.tid = 17;
				break;
			case 3:
				timerNumberLabel.tid = 16;
				break;
			case 2:
				timerNumberLabel.tid = 15;
				break;
			case 1:
				timerNumberLabel.tid = 14;
				break;
			case 0:
				timer = 10;
				score = 0;

				state = END_BAD;
				break;
			default:
				timer = 10;
				score = 0;

				break;
		}

		frames5 = 0;
	}
}

void updateEnergyBall(object *o) {
	if(o->active == 1) {
		o->y = o->y - o->delta;

		performEnergyBallAnimation(o);

		if(o->y <= 0) {
			destroyEnergyBall(o);
		}
	}
}

void fireEnergyBall(void) {
	for(int i = 0; i < NUM_BALLS; i++) {
		if(ball[i].active == 0) {
			ball[i].x = starfighter.x + 4;
			ball[i].y = starfighter.y - 8;
			ball[i].delta = 2;
			ball[i].active = 1;
			break;
		}
	}
}

void destroyEnergyBall(object *o) {
	if(o->active == 1) {
		o->x = 0;
		o->y = 0;
		o->delta = 0;
		o->active = 0;
	}
}

void draw(void) {
	drawPlayer();
	drawUfo();
	drawLabel();
	drawNumbers();
	drawTimerLabel();
	drawTimerNumbers();

	for(int i = 0; i < NUM_BALLS; i++) {
		drawEnergyBall(&ball[i]);
	}

	oam_copy(oam_mem, obj_buffer, 16);
}

void drawPlayer(void) {
	obj_set_attr(starfighter.obj, ATTR0_SQUARE, ATTR1_SIZE_16, ATTR2_PALBANK(starfighter.pb) | starfighter.tid);
	starfighter.obj->attr2 = ATTR2_BUILD(starfighter.tid, starfighter.pb, 0);
	obj_set_pos(starfighter.obj, starfighter.x, starfighter.y);
}

void drawUfo(void) {
	obj_set_attr(ufo.obj, ATTR0_WIDE, ATTR1_SIZE_32x16, ATTR2_PALBANK(ufo.pb) | ufo.tid);
	ufo.obj->attr2 = ATTR2_BUILD(ufo.tid, ufo.pb, 0);
	obj_set_pos(ufo.obj, ufo.x, ufo.y);
}

void drawLabel(void) {
	obj_set_attr(scoreLabel.obj, ATTR0_WIDE, ATTR1_SIZE_32x8, ATTR2_PALBANK(scoreLabel.pb) | scoreLabel.tid);
	scoreLabel.obj->attr2 = ATTR2_BUILD(scoreLabel.tid, scoreLabel.pb, 0);
	obj_set_pos(scoreLabel.obj, scoreLabel.x, scoreLabel.y);
}

void drawNumbers(void) {
	obj_set_attr(numberLabel.obj, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(numberLabel.pb) | numberLabel.tid);
	numberLabel.obj->attr2 = ATTR2_BUILD(numberLabel.tid, numberLabel.pb, 0);
	obj_set_pos(numberLabel.obj, numberLabel.x, numberLabel.y);
}

void drawTimerLabel(void) {
	obj_set_attr(timerLabel.obj, ATTR0_WIDE, ATTR1_SIZE_32x8, ATTR2_PALBANK(timerLabel.pb) | timerLabel.tid);
	timerLabel.obj->attr2 = ATTR2_BUILD(timerLabel.tid, timerLabel.pb, 0);
	obj_set_pos(timerLabel.obj, timerLabel.x, timerLabel.y);
}

void drawTimerNumbers(void) {
	obj_set_attr(timerNumberLabel.obj, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(timerNumberLabel.pb) | timerNumberLabel.tid);
	timerNumberLabel.obj->attr2 = ATTR2_BUILD(timerNumberLabel.tid, timerNumberLabel.pb, 0);
	obj_set_pos(timerNumberLabel.obj, timerNumberLabel.x, timerNumberLabel.y);
}

void drawEnergyBall(object *o) {
	if(o->active == 1) {
		obj_set_attr(o->obj, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(o->pb) | o->tid);
		o->obj->attr2 = ATTR2_BUILD(o->tid, o->pb, 0);
		obj_set_pos(o->obj, o->x, o->y);
	}
	else {
		obj_hide(o->obj);
	}
}

int detectCollision(object *a, object *b) {
	if((a->x < b->x + b->width) && (a->x + a->width > b->x) && (a->y < b->y + b->height) && (a->y + a->height > b->y)) {
		return 1;
	}
	return 0;
}

void performtitleCardAnimation(void) {
	COLOR titleCard_red = RGB15(26, 3, 8);
	COLOR titleCard_blue = RGB15(5, 9, 25);
	COLOR titleCard_green = RGB15(6, 24, 3);
	COLOR titleCard_pink = RGB15(28, 6, 28);

	frames1++;

	if(frames1 <= 10) {
		pal_bg_bank[0][1] = titleCard_blue;
		pal_bg_bank[0][2] = titleCard_red;
		pal_bg_bank[0][3] = titleCard_green;
		pal_bg_bank[0][4] = titleCard_pink;
	}
	else if(frames1 <= 20) {
		pal_bg_bank[0][1] = titleCard_pink;
		pal_bg_bank[0][2] = titleCard_blue;
		pal_bg_bank[0][3] = titleCard_red;
		pal_bg_bank[0][4] = titleCard_green;
	}
	else if(frames1 <= 30) {
		pal_bg_bank[0][1] = titleCard_green;
		pal_bg_bank[0][2] = titleCard_pink;
		pal_bg_bank[0][3] = titleCard_blue;
		pal_bg_bank[0][4] = titleCard_red;
	}
	else if(frames1 <= 40) {
		pal_bg_bank[0][1] = titleCard_red;
		pal_bg_bank[0][2] = titleCard_green;
		pal_bg_bank[0][3] = titleCard_pink;
		pal_bg_bank[0][4] = titleCard_blue;
	}
	else {
		frames1 = 0;
	}
}

void performEnergyBallAnimation(object *o) {
	frames2++;

	if(frames2 < 5) {		
		o->pb = 4;
	}
	else if(frames2 >= 5 && frames2 < 10) {
		o->pb = 5;
	}
	else {
		o->pb = 4;
		frames2 = 0;
	}
}

void performBackgroundAnimation(void) {
	frames3++;

	if(frames3 <= 30) {						
		pal_bg_bank[0][1] = RGB15(frames3, 0, 0);
	}
	else if(frames3 <= 60) {
		pal_bg_bank[0][1] = RGB15(30, frames3 - 30, 0);
	}
	else if(frames3 <= 90) {
		pal_bg_bank[0][1] = RGB15(30 - (frames3 - 60), 30, 0);
	}
	else if(frames3 <= 120) {
		pal_bg_bank[0][1] = RGB15(0, 30, frames3 - 90);
	}
	else if(frames3 <= 150) {
		pal_bg_bank[0][1] = RGB15(0, 30 - (frames3 - 120), 30);
	}
	else if(frames3 <= 180) {
		pal_bg_bank[0][1] = RGB15(frames3 - 150, 0, 30);
	}
	else if(frames3 <= 210) {
		pal_bg_bank[0][1] = RGB15(30, 0, 30 - (frames3 - 180));
	}
	else if(frames3 <= 240) {
		pal_bg_bank[0][1] = RGB15(31 - (frames3 - 210), 0, 0);
	}
	else {
		pal_bg_bank[0][1] = RGB15(0, 0, 0);
		frames3 = 0;
	}
}

void performBadBackgroundAnimation(void) {
	frames6++;

	if(frames6 <= 20) {
		pal_bg_bank[0][2] = RGB15(31 - frames6, 28 - frames6, 0);
	}
	else if(frames6 <= 40) {
		pal_bg_bank[0][2] = RGB15(11 + (frames6 - 20), 8 + (frames6 - 20), 0);
	}
	else {
		frames6 = 0;
	}
}

void performBlinkingAnimation(void) {
	frames4++;

	if(frames4 < 30) {
		starfighter.pb = 0;
		ufo.pb = 2;
	}
	else if(frames4 >= 30 && frames4 <= 60) {
		starfighter.pb = 1;
		ufo.pb = 3;
	}
	else {
		frames4 = 0;
	}
}

void hideAllSprites(void) {
	obj_hide(starfighter.obj);
	obj_hide(ufo.obj);
	obj_hide(numberLabel.obj);
	obj_hide(scoreLabel.obj);
	obj_hide(timerLabel.obj);
	obj_hide(timerNumberLabel.obj);

	for(int i = 0; i < 10; i++) {
		obj_hide(ball[i].obj);
	}

	oam_copy(oam_mem, obj_buffer, 16);
}

void showFinalPositions(void) {
	obj_set_attr(starfighter.obj, ATTR0_SQUARE, ATTR1_SIZE_16, ATTR2_PALBANK(starfighter.pb) | starfighter.tid);
	starfighter.obj->attr2 = ATTR2_BUILD(starfighter.tid, starfighter.pb, 0);

	obj_set_attr(ufo.obj, ATTR0_WIDE, ATTR1_SIZE_32x16, ATTR2_PALBANK(ufo.pb) | ufo.tid);
	ufo.obj->attr2 = ATTR2_BUILD(ufo.tid, ufo.pb, 0);

	obj_set_pos(starfighter.obj, 59, 89);
	obj_set_pos(ufo.obj, 146, 44);

	performBlinkingAnimation();

	oam_copy(oam_mem, obj_buffer, 2);
}
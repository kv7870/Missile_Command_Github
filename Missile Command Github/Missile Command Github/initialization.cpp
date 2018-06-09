#include "stdafx.h"
#include <stdio.h>
#include <allegro5/allegro.h> // Include the allegro header file.
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "header.h"					
#include <time.h>
#include <stdlib.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

int initAllegro(ALLEGRO_DISPLAY ** display, ALLEGRO_TIMER ** timer, ALLEGRO_BITMAP ** imageCrosshair, ALLEGRO_EVENT_QUEUE ** event_queue, ALLEGRO_FONT ** font, ALLEGRO_BITMAP ** background,
	ALLEGRO_BITMAP ** imageUfo, Level * level, ALLEGRO_BITMAP ** imageBomb, ALLEGRO_BITMAP ** imageLauncher, ALLEGRO_BITMAP ** ground, ALLEGRO_BITMAP ** imageBase,
	ALLEGRO_FONT ** titleFont, Audio * audio) {

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	al_init_font_addon();
	al_init_ttf_addon();


	if (!al_install_audio()) {
		fprintf(stderr, "failed to initialize audio!\n");
		return -1;
	}

	if (!al_init_acodec_addon()) {
		fprintf(stderr, "failed to initialize audio codecs!\n");
		return -1;
	}

	if (!al_reserve_samples(300)) {
		fprintf(stderr, "failed to reserve samples!\n");
		return -1;
	}

	al_init_font_addon();
	al_init_ttf_addon();

	//load audio
	audio->missileLaunch = al_load_sample("missileLaunch.wav");
	audio->siren = al_load_sample("sirenLong.ogg");
	audio->explosion[0] = al_load_sample("explosion1.wav");
	audio->explosion[1] = al_load_sample("explosion2.wav");
	audio->explosion[2] = al_load_sample("explosion3.wav");
	audio->explosion[3] = al_load_sample("explosion4.wav");
	audio->explosion[4] = al_load_sample("explosion5.wav");
	audio->explosion[5] = al_load_sample("explosion6.wav");

	//create display
	*display = al_create_display(SCREEN_W, SCREEN_H);
	if (!(*display)) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(*timer);
		return -1;
	}

	//create timer
	*timer = al_create_timer(1.0 / FPS);   //1/60 seconds per frame, i.e. 60 fps
	if (!(*timer)) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	*font = al_load_ttf_font("Roboto-Regular.ttf", 24, 0);
	if (!font) {
		fprintf(stderr, "Could not load 'Roboto-Regular.ttf'.\n");
		return -1;
	}

	*titleFont = al_load_ttf_font("DAYPBL__.ttf", 64, 0);
	if (!titleFont) {
		fprintf(stderr, "Could not load DAYPBL__.ttf'.\n");
		return -1;
	}

	//create event queue 
	*event_queue = al_create_event_queue();
	if (!(*event_queue)) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_bitmap(*imageCrosshair);
		al_destroy_display(*display);
		al_destroy_timer(*timer);
		return -1;
	}

	//install keyboard 
	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}

	//install mouse 
	if (!al_install_mouse()) {
		fprintf(stderr, "failed to initialize the mouse!\n");
		return -1;
	}

	if (!al_init_primitives_addon()) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to initialize image addon!",
			nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	//initialize image addon 
	if (!al_init_image_addon()) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to initialize image addon!",
			nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	*imageCrosshair = al_load_bitmap("crosshair.png");
	if (!imageCrosshair) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}


	*background = al_load_bitmap("background.jpg");
	if (!background) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	*imageUfo = al_load_bitmap("ufoBlue.png");
	if (!background) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	imageBomb[0] = al_load_bitmap("greenBomb.png");
	if (!imageBomb) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	imageBomb[1] = al_load_bitmap("blueBomb.png");
	if (!imageBomb) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	imageBomb[2] = al_load_bitmap("pinkBomb.png");
	if (!imageBomb) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	imageBomb[3] = al_load_bitmap("yellowBomb.png");
	if (!imageBomb) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	imageBomb[4] = al_load_bitmap("whiteBomb.png");
	if (!imageBomb) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	*imageLauncher = al_load_bitmap("missile.png");
	if (!imageLauncher) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	*ground = al_load_bitmap("ground.png");
	if (!ground) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	*imageBase = al_load_bitmap("imageBase.png");
	if (!ground) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}





	level->ufoSize.x = al_get_bitmap_width(*imageUfo);
	level->ufoSize.y = al_get_bitmap_height(*imageUfo);

	level->bombSize.x = al_get_bitmap_width(*imageBomb);
	level->bombSize.y = al_get_bitmap_height(*imageBomb);

	level->baseSize.x = al_get_bitmap_width(*imageBase);
	level->baseSize.y = al_get_bitmap_width(*imageBase);

	//register 
	al_register_event_source(*event_queue, al_get_display_event_source(*display));
	al_register_event_source(*event_queue, al_get_timer_event_source(*timer));
	al_register_event_source(*event_queue, al_get_keyboard_event_source());
	al_register_event_source(*event_queue, al_get_mouse_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_start_timer(*timer);
}


void oneTimeInit(Level * level) {
	int i, j; 

	//enemy missile 
	level->enemySpeed = 2;
	level->lives = 6;
	level->spawnLimit = 10;
	level->spawnRate = 1000;
	level->splitRate = 1000;
	level->splitAngle = 50;
	level->score = 0;
	level->round = 1;

	//ufo
	level->spawnUfo = true;
	level->ufoSpeed = 0.5;
	level->ufoSpawnLimit = 2;
	level->ufoSpawnRate = 500;
	level->ufoMissileSpawnRate = 100;

	//bomb
	level->spawnBomb = true;
	level->bombSpawnLimit = 1;
	level->bombSpeed = 2;
	level->bombSpawnRate = 500;

	//max on screen simultaneously 
	level->maxEnemyOnScreen = 5;
	level->maxUfoOnScreen = 1;
	level->maxBombOnScreen = 1;
	//malloc for max enemy on screen; remove arrived

	//base coordinates
	for (i = 0, j = 145; i < 3; i++, j += 90)
		level->baseX[i] = j;
	for (i = 3, j = 530; i < 6; i++, j += 100)
		level->baseX[i] = j; 

	level->highScoreCount = 5; 
	for (i = 0; i < level->highScoreCount; i++)
		level->highScores[i] = 0;
}


//spawn crosshair at center of screen y
void initCrosshair(Crosshair * crosshair, ALLEGRO_BITMAP * imageCrosshair) {
	crosshair->pos.x = 450;
	crosshair->pos.y = 450;
	crosshair->height = al_get_bitmap_height(imageCrosshair);
	crosshair->width = al_get_bitmap_width(imageCrosshair);
}


void initLevel(Level * level) {
	//enemy missile
	level->num_spawned = 0;

	//ufo
	level->ufoNumSpawned = 0;
	level->ufoSpawnSide[LEFT] = 0 - level->ufoSize.x;
	level->ufoSpawnSide[RIGHT] = 900;

	//bomb
	level->bombNumSpawned = 0;

	//abm
	level->abmLeft = 30;
	for (int i = 0; i < 3; i++) {
		level->batteryAbmLeft[i] = 10;
	}
}

//initialize abm 
void initAbm(struct abmData * abm, Explosion * explosion) {
	int i;

	for (i = 0; i < ABM_COUNT; i++) {
		abm[i].dest.x = 0;
		abm[i].dest.y = 0;
		abm[i].launch.y = 800;
		abm[i].launched = false;
		abm[i].dx = 0;
		abm[i].dy = 0;
		abm[i].inc.x = 0;
		abm[i].inc.y = 0;
		abm[i].pos.x = 0;
		abm[i].pos.y = 0;
		abm[i].step = 0;
		abm[i].arrived = false;
		abm[i].num_increment = 1;

		explosion[i].ongoing = false;
		explosion[i].radius = 0;
		explosion[i].increaseRadius = true;
		explosion[i].expandedRadius = false;
		explosion[i].topRight.x = 0;
		explosion[i].topRight.y = 0;
		explosion[i].topLeft.x = 0;
		explosion[i].topLeft.y = 0;
		explosion[i].bottomLeft.x = 0;
		explosion[i].bottomLeft.y = 0;
		explosion[i].center.x = 0;
		explosion[i].center.y = 0;
		explosion[i].xNew = 0;
		explosion[i].yNew = 0;	
	};

	//1st battery (left)
	for (i = 0; i < 10; i++) {
		abm[i].launch.x = 40;
		abm[i].speed = 7;
	}

	//2nd battery (center)
	for (i = 10; i < 20; i++) {
		abm[i].launch.x = 450;
		abm[i].speed = 14;
	}

	//3rd battery (right)
	for (i = 20; i < 30; i++) {
		abm[i].launch.x = 850;
		abm[i].speed = 7;
	}
}

void initEnemy(Enemy ** enemy, Level * level, Ufo * ufo, Bomb * bomb) {
	for (int i = 0; i < level->maxEnemyOnScreen; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {

			//init main enemy missile 
			enemy[i][j].dest.x = 0;
			enemy[i][j].dest.y = 0;
			enemy[i][j].launched = false;
			enemy[i][j].dx = 0;
			enemy[i][j].dy = 0;
			enemy[i][j].inc.x = 0;
			enemy[i][j].inc.y = 0;
			enemy[i][j].pos.x = 0;
			enemy[i][j].pos.y = 0;
			enemy[i][j].step = 0;
			enemy[i][j].launch.y = 50;
			enemy[i][j].dest.y = 900;
			enemy[i][j].topRight.x = 0;
			enemy[i][j].topRight.y = 0;
			enemy[i][j].topLeft.x = 0;
			enemy[i][j].topLeft.y = 0;
			enemy[i][j].bottomLeft.x = 0;
			enemy[i][j].bottomLeft.y = 0;
		}
	}

	for (int i = 0; i < level->maxUfoOnScreen; i++) {
		ufo[i].spawned = false;
		ufo[i].arrived = false;
		ufo[i].pos.x = 0;
		ufo[i].pos.y = 0;

		//bounds
		ufo[i].topRight.x = 0;
		ufo[i].topRight.y = 0;
		ufo[i].topLeft.x = 0;
		ufo[i].topLeft.y = 0;
		ufo[i].bottomLeft.x = 0;
		ufo[i].bottomLeft.y = 0;
		ufo[i].origin = 0;

		for (int j = 0; j < 2; j++) {
			ufo[i].missile[j].launched = false;
			ufo[i].missile[j].pos.x = 0;
			ufo[i].missile[j].pos.y = 0;
			ufo[i].missile[j].dx = 0;
			ufo[i].missile[j].dy = 0;
			ufo[i].missile[j].inc.x = 0;
			ufo[i].missile[j].inc.y = 0;
			ufo[i].missile[j].step = 0;
			ufo[i].missile[j].launch.x = 0;
			ufo[i].missile[j].launch.y = 0;
			ufo[i].missile[j].dest.x = 0;
			ufo[i].missile[j].dest.y = 810;
			ufo[i].missile[j].topRight.x = 0;
			ufo[i].missile[j].topRight.y = 0;
			ufo[i].missile[j].topLeft.x = 0;
			ufo[i].missile[j].topLeft.y = 0;
			ufo[i].missile[j].bottomLeft.x = 0;
			ufo[i].missile[j].bottomLeft.y = 0;
		}
	}

	for (int i = 0; i < level->maxBombOnScreen; i++) {
		bomb[i].spawned = false;
		bomb[i].pos.x = 0;
		bomb[i].pos.y = 0;
		bomb[i].origin.x = 0;
		bomb[i].origin.y = 0;
		bomb[i].target.x = 0;
		bomb[i].target.y = 0;

		//bounds
		bomb[i].topRight.x = 0;
		bomb[i].topRight.y = 0;
		bomb[i].topLeft.x = 0;
		bomb[i].topLeft.y = 0;
		bomb[i].bottomLeft.x = 0;
		bomb[i].bottomLeft.y = 0;
	}
}


void initBase(Base * base, int baseCount, Level level) {
	int i, j;

	for (i = 0, j = 90; i < baseCount / 2; i++, j += 100) {
		base[i].pos.x = j;
		base[i].pos.y = 810;
	}

	for (i = 3, j = 505; i < 6; i++, j += 100) {
		base[i].pos.x = j;
		base[i].pos.y = 810;
	}

	//bounds
	for (i = 0; i < baseCount; i++) {
		base[i].destroyed = false;
		base[i].topLeft.x = base[i].pos.x;
		base[i].topLeft.y = base[i].pos.y;
		base[i].topRight.x = base[i].pos.x + level.baseSize.x;
		base[i].topRight.y = base[i].pos.y;
		base[i].bottomLeft.x = base[i].pos.x;
		base[i].bottomLeft.y = base[i].pos.y + level.baseSize.y;
	}
}


void initColorMap(int colorMap[][3]) {

	colorMap[NEON_RED][R] = 255;
	colorMap[NEON_RED][G] = 10;
	colorMap[NEON_RED][B] = 71;

	colorMap[NEON_GREEN][R] = 71;
	colorMap[NEON_GREEN][G] = 255;
	colorMap[NEON_GREEN][B] = 10;

	colorMap[NEON_YELLOW][R] = 194;
	colorMap[NEON_YELLOW][G] = 255;
	colorMap[NEON_YELLOW][B] = 10;

	colorMap[NEON_BLUE][R] = 10;
	colorMap[NEON_BLUE][G] = 194;
	colorMap[NEON_BLUE][B] = 255;

	colorMap[LIME][R] = 121;
	colorMap[LIME][G] = 255;
	colorMap[LIME][B] = 3;

	colorMap[NEON_PINK][R] = 255;
	colorMap[NEON_PINK][G] = 10;
	colorMap[NEON_PINK][B] = 194;

	colorMap[RED][R] = 255;
	colorMap[RED][G] = 0;
	colorMap[RED][B] = 25;

	colorMap[RED][R] = 255;
	colorMap[RED][G] = 0;
	colorMap[RED][B] = 25;

	colorMap[YELLOW][R] = 255;
	colorMap[YELLOW][G] = 222;
	colorMap[YELLOW][B] = 0;

	colorMap[TEAL][R] = 0;
	colorMap[TEAL][G] = 255;
	colorMap[TEAL][B] = 187;
}

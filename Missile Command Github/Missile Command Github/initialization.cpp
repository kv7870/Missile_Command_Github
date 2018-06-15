/*
MISSILE COMMAND SUMMATIVE BY KEVIN YANG & EDWARD LI
FINAL VERSION
*/

#include <stdio.h>
#include <allegro5/allegro.h> 
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

//initialize allegro
void initAllegro(ALLEGRO_DISPLAY ** display, ALLEGRO_TIMER ** timer, ALLEGRO_BITMAP ** imageCrosshair, ALLEGRO_EVENT_QUEUE ** event_queue, ALLEGRO_FONT ** font, ALLEGRO_BITMAP ** background,
	ALLEGRO_BITMAP ** imageUfo, Level * level, ALLEGRO_BITMAP ** imageScm, ALLEGRO_BITMAP ** imageMissile, ALLEGRO_BITMAP ** ground, ALLEGRO_BITMAP ** imageBase, Audio * audio) {


	al_init();

	al_install_keyboard();
	al_install_mouse();
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1000);
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	//load audio
	audio->missileLaunch = al_load_sample("missileLaunch.wav");
	audio->siren = al_load_sample("siren.ogg");
	audio->explosion[0] = al_load_sample("explosion1.wav");
	audio->explosion[1] = al_load_sample("explosion2.wav");
	audio->explosion[2] = al_load_sample("explosion3.wav");
	audio->explosion[3] = al_load_sample("explosion4.wav");
	audio->explosion[4] = al_load_sample("explosion5.wav");
	audio->explosion[5] = al_load_sample("explosion6.wav");

	//create Allegro stuff
	*display = al_create_display(SCREEN_W, SCREEN_H);
	*timer = al_create_timer(1.0 / FPS);   //1/60 seconds per frame, i.e. 60 fps
	//font[TEXT] = al_load_ttf_font("WorkSans-Medium.otf", 24, 0);
	font[TEXT] = al_load_ttf_font("Chivo-Regular.ttf", 24, 0);
	font[TITLE] = al_load_ttf_font("DAYPBL__.ttf", 64, 0);
	font[HEADING] = al_load_ttf_font("Timeline.ttf", 24, 0);
	font[BOLD] = al_load_ttf_font("Chivo-Bold.ttf", 24, 0);

	*event_queue = al_create_event_queue();

	//load images
	*imageCrosshair = al_load_bitmap("crosshair.png");
	*background = al_load_bitmap("background.jpg");
	imageUfo[0] = al_load_bitmap("ufoBlue.png");
	imageUfo[1] = al_load_bitmap("ufoRed.png");
	imageScm[0] = al_load_bitmap("greenBomb.png");
	imageScm[1] = al_load_bitmap("blueBomb.png");
	imageScm[2] = al_load_bitmap("pinkBomb.png");
	imageScm[3] = al_load_bitmap("yellowBomb.png");
	imageScm[4] = al_load_bitmap("whiteBomb.png");
	*imageMissile = al_load_bitmap("missile.png");
	*ground = al_load_bitmap("ground.png");
	*imageBase = al_load_bitmap("imageBase.png");


	//get dimensions of images
	level->ufoSize.x = al_get_bitmap_width(imageUfo[0]);
	level->ufoSize.y = al_get_bitmap_height(imageUfo[0]);
	level->scmSize.x = al_get_bitmap_width(*imageScm);
	level->scmSize.y = al_get_bitmap_height(*imageScm);
	level->baseSize.x = al_get_bitmap_width(*imageBase);
	level->baseSize.y = al_get_bitmap_height(*imageBase);

	//register 
	al_register_event_source(*event_queue, al_get_display_event_source(*display));
	al_register_event_source(*event_queue, al_get_timer_event_source(*timer));
	al_register_event_source(*event_queue, al_get_keyboard_event_source());
	al_register_event_source(*event_queue, al_get_mouse_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_start_timer(*timer);
}

//initialize variables that don't need to be reset each level 
void oneTimeInit(Level * level) {
	int i, j;

	level->newHighScore = false;
	level->initialSort = true;
	level->score = 0;
	level->round = 1;
	level->multiplier = 1; 

	//regular enemy missile 
	level->enemySpeed = 1.25;
	level->lives = 600;
	level->spawnLimit = 10;
	level->spawnRate = 1000;
	level->splitRate = 1000;
	level->splitAngle = 100;

	level->spawnRangeMin = 400;
	level->spawnRangeMax = 600;
	level->splitRangeMin = 498;
	level->splitRangeMax = 502;
	level->maxEnemyOnScreen = 8;


	//ufo (flying saucer)
	level->spawnUfo = true;
	level->ufoSpeed = 0.5;
	level->ufoSpawnLimit = 1;
	level->ufoSpawnRate = 500;
	level->ufoMissileSpawnRate = 1000;
	level->ufoSpawnRangeMin = 499;
	level->ufoSpawnRangeMax = 501;
	level->maxUfoOnScreen = 1;

	//smart cruise missile
	level->spawnScm = false; 
	level->scmSpawnLimit = 1;
	level->scmSpeed = 2;
	level->scmSpawnRate = 500;
	level->maxScmOnScreen = 1;

	//base x-coordinates (left)
	for (i = 0, j = 90; i < 3; i++, j += 100)
		level->base_x[i] = j + 45;
	//base x-coordinates (right)
	for (i = 3, j = 505; i < 6; i++, j += 100)
		level->base_x[i] = j + 45;

	level->highScoreCount = 5;
	for (i = 0; i < level->highScoreCount; i++)
		level->highScores[i] = 0;
}


//spawn crosshair at center of screen 
void initCrosshair(Crosshair * crosshair, ALLEGRO_BITMAP * imageCrosshair) {
	crosshair->pos.x = 450;
	crosshair->pos.y = 450;
	crosshair->height = al_get_bitmap_height(imageCrosshair);
	crosshair->width = al_get_bitmap_width(imageCrosshair);
}


//initialize level-related variables 
void initLevel(Level * level) {

	//enemy missile
	level->num_spawned = 0;

	//ufo
	level->ufoNumSpawned = 0;
	level->ufoSpawnSide[LEFT] = 0 - level->ufoSize.x;
	level->ufoSpawnSide[RIGHT] = 900; ; 

	//smart cruise missile
	level->scmNumSpawned = 0;

	//reset abm count
	level->abmLeft = 30;

	level->speedUp = false;

	//time
	level->enemyLastSpawned = 0;
	level->ufoLastSpawned = 0;
	level->scmLastSpawned = 0; 
}

//reset anti-ballistic missiles each level
void initAbm(Abm * abm, Explosion * explosion) {
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
	};

	//initialize 1st ABM battery (left)
	for (i = 0; i < 10; i++) {
		abm[i].launch.x = 40;
		abm[i].speed = 10;
	}

	//initialize 2nd ABM battery (center)
	for (i = 10; i < 20; i++) {
		abm[i].launch.x = 450;
		abm[i].speed = 20;
	}

	//initialize 3rd ABM battery (right)
	for (i = 20; i < 30; i++) {
		abm[i].launch.x = 850;
		abm[i].speed = 10;
	}
}

//reset enemies each level
void initEnemy(Enemy ** enemy, Level * level, Ufo * ufo, Scm * scm) {
	for (int i = 0; i < level->maxEnemyOnScreen/4; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {

			//initialize regular enemy missiles
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

	//initialize ufos
	for (int i = 0; i < level->maxUfoOnScreen; i++) {
		ufo[i].spawned = false;
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

		//intialize cruise missiles
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

	//initialize smart cruise missiles
	for (int i = 0; i < level->maxScmOnScreen; i++) {
		scm[i].spawned = false;
		scm[i].pos.x = 0;
		scm[i].pos.y = 0;
		scm[i].origin.x = 0;
		scm[i].origin.y = 0;
		scm[i].target.x = 0;
		scm[i].target.y = 0;
		scm[i].moveLeft = true;
		scm[i].timerCount = 0;

		//bounds
		scm[i].topRight.x = 0;
		scm[i].topRight.y = 0;
		scm[i].topLeft.x = 0;
		scm[i].topLeft.y = 0;
		scm[i].bottomLeft.x = 0;
		scm[i].bottomLeft.y = 0;
	}
}


//initialize coordinates of bases
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


//initialize colour palette for missiles
void initColorMap(int colorMap[][3]) {

	colorMap[GREEN][R] = 24;
	colorMap[GREEN][G] = 221;
	colorMap[GREEN][B] = 0;

	colorMap[BLUE][R] = 7;
	colorMap[BLUE][G] = 185;
	colorMap[BLUE][B] = 252;

	colorMap[YELLOW][R] = 255;
	colorMap[YELLOW][G] = 255;
	colorMap[YELLOW][B] = 0;

	colorMap[PINK][R] = 252;
	colorMap[PINK][G] = 89;
	colorMap[PINK][B] = 143;

	colorMap[RED][R] = 255;
	colorMap[RED][G] = 0;
	colorMap[RED][B] = 0;

	colorMap[ORANGE][R] = 253;
	colorMap[ORANGE][G] = 71;
	colorMap[ORANGE][B] = 3;

	colorMap[MAGENTA][R] = 182;
	colorMap[MAGENTA][G] = 16;
	colorMap[MAGENTA][B] = 191;
}
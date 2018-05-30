#include "stdafx.h"
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

int main(int argc, char **argv)
{

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *imageCrosshair = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_FONT * font;
	ALLEGRO_BITMAP *background = NULL; 
	ALLEGRO_BITMAP *imageUfo = NULL; 

	//int lives = 6;
	//int curr_enemy_count = 0;  //num of enemies on screen 
	//int num_spawned = 0;
	//int lvl_spawn_limit = 10;
	//int score = 0;
	//int batteryAbmLeft[3] = { 0 };
	int  colorMap[NUM_COLORS][3];
	int theme[COLORS_PER_THEME];

	Level level; 

	level.enemySpeed = 2;
	level.lives = 6;
	level.spawnLimit = 10;
	level.spawnRate = 1000;
	level.splitRate = 1000;
	level.splitAngle = 50; 
	level.score = 0; 
	level.round = 1; 

	level.spawnUfo = false;
	level.ufoSpeed = 1;
	level.ufoSpawnLimit = -1; 
	level.ufoSpawnRate = 1000; 


	Crosshair crosshair;
	Abm abm[ABM_COUNT];
	Explosion explosion[ABM_COUNT];
	Base base[6];

	Ufo * ufo = (Ufo *)malloc(level.ufoSpawnLimit * sizeof(Ufo)); 

	Enemy ** enemy = (Enemy **)malloc(level.spawnLimit * sizeof(Enemy *));
	for (int i = 0; i < level.spawnLimit; i++) {
		enemy[i] = (Enemy *)malloc(SPLIT_COUNT * sizeof(Enemy));
	}

	srand(time(0));

	initAllegro(&display, &timer, &imageCrosshair, &event_queue, &font, &background, &imageUfo);

	initCrosshair(&crosshair, imageCrosshair);

	initLevel(&level); 

	initAbm(abm, explosion);

	initEnemy(enemy, &level, ufo);

	initBase(base, 6);

	initColorMap(colorMap);

	generateTheme(theme);

	playerMovement(display, timer, imageCrosshair, event_queue, crosshair, abm, enemy, font, base, explosion, theme, colorMap, &level, background, imageUfo, ufo);


	//al_set_target_bitmap(image);

	//al_set_target_bitmap(al_get_backbuffer(display));


	return 0;
}
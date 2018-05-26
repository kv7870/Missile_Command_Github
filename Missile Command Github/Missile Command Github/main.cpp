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

	int lives = 6;
	int curr_enemy_count = 0;  //num of enemies on screen 
	int num_spawned = 0;
	int lvl_spawn_limit = 10;
	int level = 1;
	float enemySpeed = 2;
	int spawnRate = 1000; 
	int splitRate = 1000;
	int abmLeft = 30;
	int score = 0;
	int batteryAbmLeft[3] = { 0 };
	int  colorMap[NUM_COLORS][3];

	Crosshair crosshair;
	Abm abm[ABM_COUNT];
	Explosion explosion[ABM_COUNT]; 
	Base base[6];
	int theme[COLORS_PER_THEME];


	Enemy ** enemy = (Enemy **)malloc(lvl_spawn_limit * sizeof(Enemy *));
	for (int i = 0; i < lvl_spawn_limit; i++) {
		enemy[i] = (Enemy *)malloc(SPLIT_COUNT * sizeof(Enemy));
	}

	srand(time(0));

	

	initAllegro(&display, &timer, &imageCrosshair, &event_queue, &font);

	initCrosshair(&crosshair, imageCrosshair);

	initAbm(abm, &abmLeft, batteryAbmLeft, explosion);

	initEnemy(enemy, &lvl_spawn_limit);

	initBase(base, 6);

	initColorMap(colorMap); 

	generateTheme(theme);

	playerMovement(display, timer, imageCrosshair, event_queue, crosshair, abm, enemy, &curr_enemy_count, &num_spawned, &lvl_spawn_limit, &level,
		&enemySpeed, &spawnRate, &splitRate, font, &lives, &abmLeft, &score, base, batteryAbmLeft, explosion, theme, colorMap);


	//al_set_target_bitmap(image);

	//al_set_target_bitmap(al_get_backbuffer(display));


	return 0;
}

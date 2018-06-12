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
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

int main(int argc, char **argv)
{

	ALLEGRO_DISPLAY * display = NULL;
	ALLEGRO_TIMER * timer = NULL;
	ALLEGRO_BITMAP * imageCrosshair = NULL;
	ALLEGRO_EVENT_QUEUE * event_queue = NULL;
	ALLEGRO_FONT * font;
	ALLEGRO_FONT * titleFont;
	ALLEGRO_BITMAP * background = NULL;
	ALLEGRO_BITMAP * imageUfo = NULL;
	ALLEGRO_BITMAP * imageBomb[5] = { NULL };
	ALLEGRO_BITMAP * imageLauncher = NULL;
	ALLEGRO_BITMAP * ground = NULL;
	ALLEGRO_BITMAP * imageBase = NULL;
	FILE * fptr = NULL;

	//structs
	Level level;
	Crosshair crosshair;
	Abm abm[ABM_COUNT];
	Explosion explosion[ABM_COUNT];
	Base base[6];
	Audio audio;

	//variables to be malloc'd
	Enemy ** enemy = NULL;
	Ufo * ufo = NULL;
	Bomb * bomb = NULL;

	int  colorMap[NUM_COLORS][3];
	int theme[COLORS_PER_THEME];

	oneTimeInit(&level);

	//malloc arrays 
	enemy = (Enemy **)malloc(level.maxEnemyOnScreen * sizeof(Enemy *));
	for (int i = 0; i < level.maxEnemyOnScreen; i++) {
		enemy[i] = (Enemy *)malloc(SPLIT_COUNT * sizeof(Enemy));
	}

	ufo = (Ufo *)malloc(level.maxUfoOnScreen * sizeof(Ufo));
	bomb = (Bomb *)malloc(level.maxBombOnScreen * sizeof(Bomb));


	srand(time(0));

	initAllegro(&display, &timer, &imageCrosshair, &event_queue, &font, &background, &imageUfo, &level, imageBomb, &imageLauncher, &ground, &imageBase, &titleFont, &audio);

	initCrosshair(&crosshair, imageCrosshair);

	initLevel(&level);

	initAbm(abm, explosion);

	initEnemy(enemy, &level, ufo, bomb);

	initBase(base, 6, level);

	initColorMap(colorMap);

	generateTheme(theme);

	readScore(fptr, &level);
	sortScore(fptr, &level);

	titleScreen(imageBase, background, imageLauncher, ground, event_queue, titleFont, font, audio, level);

	playerMovement(display, timer, imageCrosshair, event_queue, crosshair, abm, enemy, font, base, explosion, theme, colorMap, &level, background,
		imageUfo, ufo, imageBomb, bomb, imageLauncher, ground, imageBase, titleFont, fptr, &audio);

	return 0;
}
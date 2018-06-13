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

int main(int argc, char **argv) {

	//allegro variables
	ALLEGRO_DISPLAY * display = NULL;
	ALLEGRO_TIMER * timer = NULL;
	ALLEGRO_BITMAP * imageCrosshair = NULL;  //player's reticle
	ALLEGRO_EVENT_QUEUE * event_queue = NULL;
	ALLEGRO_FONT * font;
	ALLEGRO_FONT * titleFont;  
	ALLEGRO_BITMAP * background = NULL;  
	ALLEGRO_BITMAP * imageUfo = NULL; 
	ALLEGRO_BITMAP * imageBomb[5] = { NULL };	//alternates between 5 bombs of different colours to create strobe effect 
	ALLEGRO_BITMAP * imageMissile = NULL;	//images of player missiles which dissapear upon being fired
	ALLEGRO_BITMAP * ground = NULL;	//planet surface texture
	ALLEGRO_BITMAP * imageBase = NULL;	//space bases player must defend
	FILE * fptr = NULL;

	//structs
	Level level;					 //stores miscellaneous data
	Crosshair crosshair; 
	Abm abm[ABM_COUNT];				 //player's anti-ballistic missiles used to shoot down enemies
	Explosion explosion[ABM_COUNT];  //explosion of abm 
	Base base[6];					 //player's bases
	Audio audio;					 //sound effects

	//arrays to be dynamically allocated
	Enemy ** enemy = NULL;	//enemy missiles
	Ufo * ufo = NULL;		//enemy ufos
	Bomb * bomb = NULL;		//enemy bombs

	int colorMap[NUM_COLORS][3];  //palette of colours for missiles
	int theme[COLORS_PER_THEME]; //3 colours per theme, e.g. [0] = blue, [1] = yellow, [2] = pink

	oneTimeInit(&level);  //initial initialization 

	//malloc arrays 
	enemy = (Enemy **)malloc(level.maxEnemyOnScreen * sizeof(Enemy *));  //malloc for only the max number of enemies that can be on the screen at once
	for (int i = 0; i < level.maxEnemyOnScreen; i++) {
		enemy[i] = (Enemy *)malloc(SPLIT_COUNT * sizeof(Enemy));
	}

	ufo = (Ufo *)malloc(level.maxUfoOnScreen * sizeof(Ufo));
	bomb = (Bomb *)malloc(level.maxBombOnScreen * sizeof(Bomb));


	srand(time(0));

	//initialize Allegro, load images, sounds
	initAllegro(&display, &timer, &imageCrosshair, &event_queue, &font, &background, &imageUfo, &level, imageBomb, &imageMissile, &ground, &imageBase, &titleFont, &audio);

	//initialize crosshair
	initCrosshair(&crosshair, imageCrosshair);

	//initialize data that changes every level 
	initLevel(&level);

	//initialize anti-ballistic missiles
	initAbm(abm, explosion);

	//initialize enemy missiles, ufos, bombs
	initEnemy(enemy, &level, ufo, bomb);

	//initialize player bases
	initBase(base, 6, level);

	//initialize RGB values of colours
	initColorMap(colorMap);

	//generate 3 colours 
	generateTheme(theme);

	//sort score from highest to lowest
	readScore(fptr, &level);
	sortScore(fptr, &level);

	//display title screen
	titleScreen(imageBase, background, imageMissile, ground, event_queue, titleFont, font, audio, level);

	//start actual game
	gameLoop(display, timer, imageCrosshair, event_queue, crosshair, abm, enemy, font, base, explosion, theme, colorMap, &level, background,
		imageUfo, ufo, imageBomb, bomb, imageMissile, ground, imageBase, titleFont, fptr, &audio);

	return 0;
}
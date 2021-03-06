/*
MISSILE COMMAND SUMMATIVE BY KEVIN YANG 
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

int main(int argc, char **argv) {

	ALLEGRO_DISPLAY * display = NULL;
	ALLEGRO_TIMER * timer = NULL;
	ALLEGRO_BITMAP * imageCrosshair = NULL;   //player's reticle
	ALLEGRO_EVENT_QUEUE * event_queue = NULL;
	ALLEGRO_FONT * font[4] = { NULL };	
	ALLEGRO_BITMAP * background = NULL;		//space background
	ALLEGRO_BITMAP * imageUfo[2] = { NULL };	//2 ufo pics
	ALLEGRO_BITMAP * imageScm[5] = { NULL }; //alternate between 5 different colours 
	ALLEGRO_BITMAP * imageMissile = NULL;  //images of player antiballistic missiles which dissapear upon being fired
	ALLEGRO_BITMAP * ground = NULL;  //planet surface texture
	ALLEGRO_BITMAP * imageBase = NULL;  //space bases player must defend
	FILE * fptr = NULL;

	//structs
	Level level;	//data related to the level
	Crosshair crosshair;	//data of reticle
	Abm abm[ABM_COUNT];		//player anti-ballistic missiles (abm) for shooting down enemies
	Explosion explosion[ABM_COUNT];	//explosion of abm 
	Base base[6];	//player's bases
	Audio audio;	//sound effects

	//pointers to be malloc'd
	Enemy ** enemy = NULL;  //regular enemy missiles
	Ufo * ufo = NULL;  //enemy ufos
	Scm * scm = NULL; //enemy smart cruise missiles

	int colorMap[NUM_COLORS][3];	//palette of rgb values
	int theme[COLORS_PER_THEME];	//3 colours per theme, e.g. [0] = blue, [1] = yellow, [2] = pink 

	oneTimeInit(&level);	//initial initialization 

	//dynamically allocate arrays 
	enemy = (Enemy **)malloc((level.maxEnemyOnScreen/4) * sizeof(Enemy *));
	for (int i = 0; i < level.maxEnemyOnScreen/4; i++) {
		enemy[i] = (Enemy *)malloc(SPLIT_COUNT * sizeof(Enemy));
	}

	ufo = (Ufo *)malloc(level.maxUfoOnScreen * sizeof(Ufo));
	scm = (Scm *)malloc(level.maxScmOnScreen * sizeof(Scm));


	srand(time(0));

	//initialize Allegro, load images, sounds
	initAllegro(&display, &timer, &imageCrosshair, &event_queue, font, &background, imageUfo, &level, imageScm, &imageMissile, &ground, &imageBase, &audio);

	//initialize crosshair
	initCrosshair(&crosshair, imageCrosshair);

	//initialize level data 
	initLevel(&level);

	//initialize anti-ballistic missiles
	initAbm(abm, explosion);

	//initialize enemy missiles, ufos, smart cruise missiles
	initEnemy(enemy, &level, ufo, scm);

	//initialize player bases
	initBase(base, 6, level);

	//initialize RGB values of colours
	initColorMap(colorMap);

	//generate 3 colours for current level
	generateTheme(theme);

	//read & sort highscores from highest to lowest 
	readScore(fptr, &level);
	sortScore(fptr, &level);

	//display title screen
	titleScreen(imageBase, background, imageMissile, ground, event_queue, font, audio, level);

	instructionScreen(font, event_queue); 

	//start actual game
	gameLoop(display, timer, imageCrosshair, event_queue, crosshair, abm, enemy, font, base, explosion, theme, colorMap, &level, background,
		imageUfo, ufo, imageScm, scm, imageMissile, ground, imageBase, fptr, &audio);

	return 0;
}

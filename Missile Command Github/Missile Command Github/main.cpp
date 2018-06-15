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

	ALLEGRO_DISPLAY * display = NULL;
	ALLEGRO_TIMER * timer = NULL;
	ALLEGRO_BITMAP * imageCrosshair = NULL;   //player's reticle
	ALLEGRO_EVENT_QUEUE * event_queue = NULL;
	ALLEGRO_FONT * font[4] = { NULL };
	ALLEGRO_BITMAP * background = NULL;
	ALLEGRO_BITMAP * imageUfo[2] = { NULL };
	ALLEGRO_BITMAP * imageScm[5] = { NULL }; //alternates between 5 different bombs of different colours
	ALLEGRO_BITMAP * imageMissile = NULL;  //images of player missiles which dissapear upon being fired
	ALLEGRO_BITMAP * ground = NULL;  //planet surface texture
	ALLEGRO_BITMAP * imageBase = NULL;  //space bases player must defend
	FILE * fptr = NULL;

	//structs
	Level level; //stores miscellaneous data
	Crosshair crosshair;
	Abm abm[ABM_COUNT];  //player's anti-ballistic missiles used to shoot down enemies
	Explosion explosion[ABM_COUNT]; //explosion of abm 
	Base base[6];  //player's bases
	Audio audio;

	//arrays to be dynamically allocated
	Enemy ** enemy = NULL;  //enemy missiles
	Ufo * ufo = NULL;  //enemy ufos
	Scm * scm = NULL; //enemy bombs

	int colorMap[NUM_COLORS][3];
	int theme[COLORS_PER_THEME]; //3 colours per theme, e.g. [0] = blue, [1] = yellow, [2] = pink 

	oneTimeInit(&level);  //initial initialization 

	//dynamically allocated arrays 
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

	//initialize data that changes every level 
	initLevel(&level);

	//initialize anti-ballistic missiles
	initAbm(abm, explosion);

	//initialize enemy missiles, ufos, smart cruise missiles
	initEnemy(enemy, &level, ufo, scm);

	//initialize player bases
	initBase(base, 6, level);

	//initialize RGB values of colours
	initColorMap(colorMap);

	//generate 3 colours
	generateTheme(theme);

	//generate 3 colours
	readScore(fptr, &level);
	sortScore(fptr, &level);

	//display title screen
	titleScreen(imageBase, background, imageMissile, ground, event_queue, font, audio, level);

	//start actual game
	gameLoop(display, timer, imageCrosshair, event_queue, crosshair, abm, enemy, font, base, explosion, theme, colorMap, &level, background,
		imageUfo, ufo, imageScm, scm, imageMissile, ground, imageBase, fptr, &audio);

	return 0;
}

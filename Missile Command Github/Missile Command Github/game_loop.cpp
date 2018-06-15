/*
MISSILE COMMAND SUMMATIVE BY KEVIN YANG & EDWARD LI
FINAL VERSION
*/

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


//actual game loop 
void gameLoop(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_EVENT_QUEUE *event_queue, 
	Crosshair crosshair, Abm * abm, Enemy ** enemy, ALLEGRO_FONT ** font, Base * base, Explosion * explosion, int * theme, int colorMap[][3], 
	Level * level, ALLEGRO_BITMAP * background, ALLEGRO_BITMAP ** imageUfo, Ufo * ufo, ALLEGRO_BITMAP ** imageBomb, Scm * scm, 
	ALLEGRO_BITMAP * imageLauncher, ALLEGRO_BITMAP * ground, ALLEGRO_BITMAP * imageBase, FILE * fptr, Audio * audio) {

	bool done = false;	//exit game if true
	bool draw = true;	
	int count = 0;
	int i;
	int round = 1;
	int numUnusedAbm = 0; 
	bool paused = false;

	al_hide_mouse_cursor(display);

	while (!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {  //update every 1/60 of a second 
			draw = true;

			spawnEnemy(enemy, level, ufo, scm, base, timer);

			updateAbm(abm);

			updateEnemy(enemy, level);

			abmArrival(abm, explosion);

			updateScm(level, scm, explosion);

			updateUfo(ufo, level);

			updateUfoMissile(ufo, level);

			spawnUfoMissile(ufo, level, base);

			enemyArrival(enemy, level, ufo, scm);

			baseCollision(base, enemy, 6, level, ufo, scm);

			collision(abm, enemy, explosion, level, ufo, audio);

			scmCollision(scm, explosion, level, audio);
		}

		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			done = true;
		}

		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
			if (ev.mouse.x >= 4 && ev.mouse.x <= SCREEN_W - crosshair.width)
				crosshair.pos.x = ev.mouse.x;
			if (ev.mouse.y >= 4 && ev.mouse.y <= SCREEN_H - crosshair.height/*-150*/)
				crosshair.pos.y = ev.mouse.y;
		}

		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (ev.mouse.button & 1) {
				crosshair.target.x = ev.mouse.x;   //get coordinate of target 
				crosshair.target.y = ev.mouse.y;
				fire(abm, crosshair, level, audio);
			}
		}


		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_ESCAPE:

				if (!paused) {
					paused = true;
					al_stop_timer(timer);
					al_draw_text(font[TEXT], al_map_rgb(255, 0, 0), 450, 400, ALLEGRO_ALIGN_CENTER, "PAUSED");
					al_flip_display();
				}
				else if (paused) {
					paused = false;
					al_start_timer(timer);
				}
				break;
			}

		}

		if (draw && al_is_event_queue_empty(event_queue)) {
			draw = false;
			round++;

			al_clear_to_color(al_map_rgb(0, 0, 0));  //clear screen to black to create illusion of animation; draw & clear screen, draw & clear screen... 

			drawObjects(base, 6, imageLauncher, abm, imageBase, background, ground);

			drawExplosion(abm, explosion, colorMap, level);

			drawAbm(abm, theme[0], colorMap);

			drawEnemy(enemy, theme, colorMap, level, ufo, imageUfo, imageBomb, scm);

			drawInfo(font, abm, level);

			drawCrosshair(imageCrosshair, &crosshair);

			al_flip_display();

			//game over 
			if (level->lives <= 0) {
				done = true;
				break; 
			}

			//check if ready to go to next level once all enemy missiles have spawned 
			if (level->num_spawned >= level->spawnLimit) {

				//check if anything is still on screen 
				if (levelProceed(level, explosion, enemy, ufo, scm)) {	

					al_stop_timer(timer);

					for (i = 0; i < level->maxEnemyOnScreen/4; i++) {
						free(enemy[i]);
					}

					free(enemy);
					free(ufo);
					free(scm);

					numUnusedAbm = level->abmLeft; 

					loadNextLevel(level, abm, base);

					enemy = (Enemy **)malloc((level->maxEnemyOnScreen/4) * sizeof(Enemy *));
					for (i = 0; i < level->maxEnemyOnScreen/4; i++) {
						enemy[i] = (Enemy *)malloc(SPLIT_COUNT * sizeof(Enemy));
					}

					ufo = (Ufo *)malloc((level->maxUfoOnScreen) * sizeof(Ufo));

					scm = (Scm *)malloc((level->maxScmOnScreen) * sizeof(Scm));

					initLevel(level);
					initAbm(abm, explosion);
					initEnemy(enemy, level, ufo, scm);
					generateTheme(theme);


					transition(font, timer, abm, level, numUnusedAbm);
					
					//al_start_timer(timer);
				}
			}
		}
	}

	calcScore(level);
	sortScore(fptr, level);

	for (i = 0; i < level->maxEnemyOnScreen/4; i++) {
		free(enemy[i]);
	}

	free(enemy);
	free(ufo);
	free(scm);

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(font[TITLE], al_map_rgb(255, 0, 0), 300, 400, 0, "THE END");
	al_flip_display();
	al_rest(5);

	al_destroy_bitmap(imageCrosshair);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	for (i = 0; i<5; i++)
		al_destroy_sample(audio->explosion[i]);
}


//return whether ready to proceed to next level 
bool levelProceed(Level * level, Explosion * explosion, Enemy ** enemy, Ufo * ufo, Scm * scm) {
	bool proceed = true;

	//cannot go to next level if a missile is still on screen
	for (int i = 0; i < level->maxEnemyOnScreen/4; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched)
				proceed = false;
		}
	}

	//cannot go to next level if an explosion is ongoing
	for (int i = 0; i < ABM_COUNT; i++) {
		if (explosion[i].ongoing)
			proceed = false;
	}

	//cannot go to next level if a ufo or cruise missile is still on screen
	for (int i = 0; i < level->maxUfoOnScreen; i++) {
		if (ufo[i].spawned) {
			proceed = false;
		}
		for (int j = 0; j < 2; j++) {
			if (ufo[i].missile[j].launched)
				proceed = false;
		}

	}

	//cannot go to next level if a smart cruise missile is still on screen 
	for (int i = 0; i < level->maxScmOnScreen; i++) {
		if (scm[i].spawned)
			proceed = false;
	}

	return proceed;
}


//increase difficulty 
void loadNextLevel(Level * level, Abm * abm, Base * base) {
	int i;

	//regular enemy missile

	level->spawnLimit += 5;
	level->enemySpeed += 0.1;
	level->spawnScm = true;


	if (level->splitAngle <= 600)
		level->splitAngle += 50;

	if (level->round % 2 == 0) {
		//regular missile 
		if (level->spawnRangeMin > 0)
			level->spawnRangeMin -= 5;
		if (level->spawnRangeMax < 1000)
			level->spawnRangeMax += 5;
		if (level->splitRangeMin > 0)
			level->splitRangeMin -= 2;
		if (level->splitRangeMax < 1000)
			level->splitRangeMax += 2;

		//ufo
		//level->ufoSpawnLimit += 1;
		if (level->maxUfoOnScreen <= 3)
			level->maxUfoOnScreen++;
		
		//smart cruise missile
		(level->scmSpawnLimit)++;
		(level->scmSpeed) += 0.25;
	}

	level->ufoSpawnLimit += 1;
	if(level->round % 4 == 0)
		level->maxEnemyOnScreen += 4;

	if (level->scmSpawnRate > 100)
		level->scmSpawnRate -= 50;


	//ufo 
	level->spawnUfo = true;
	level->ufoSpeed += 0.25;
	if (level->ufoSpawnRangeMin > 0)
		level->ufoSpawnRangeMin -= 10;
	if (level->ufoSpawnRangeMax < 1000)
		level->ufoSpawnRangeMax += 10;

	//scm
	if ((level->round % 3) == 0)
		(level->maxScmOnScreen)++;

	//100 bonus points for any remaining bases
	for (i = 0; i < 6; i++) {
		if (!base[i].destroyed)
			(level->score) += 100;
	}

	//25 bonus points for any unused missile
	for (i = 0; i < ABM_COUNT; i++) {
		if (!abm[i].launched && !abm[i].arrived)
			(level->score) += 5;
	}

	//increase level 
	level->round++;

	switch (level->round) {
	case 1:
	case 2:
		level->multiplier = 1;
		break;
	case 3:
	case 4:
		level->multiplier = 2;
		break;
	case 5:
	case 6:
		level->multiplier = 3;
		break;
	case 7:
	case 8:
		level->multiplier = 4;
		break;
	case 9:
	case 10:
		level->multiplier = 5;
		break;
	case 11:
		level->multiplier = 6;
	}

	if (level->round > 11)
		level->multiplier = 6;

	level->score *= level->multiplier;
}



//read top 5 highscores from text file
void readScore(FILE * fptr, Level * level) {
	if ((fptr = fopen("highScore.txt", "r")) == NULL)
		printf("Can't open highScore.txt.");

	for (int i = 0; i < level->highScoreCount; i++) {
		fscanf(fptr, "%d", &(level->highScores[i]));
	}

	fclose(fptr);
}

//calculate whether player's score is a new top 5 highscore
void calcScore(Level * level) {
	int lowest = 0;

	for (int i = 0; i < level->highScoreCount; i++) {
		if (level->highScores[i] < level->highScores[lowest]) {
			lowest = i;
		}
	}

	if (level->score > level->highScores[lowest]) {
		level->highScores[lowest] = level->score;	//player's score overwrites lowest highscore 
		level->newHighScore = true;
	}
}


//rank highscores from highest to lowest 
void sortScore(FILE * fptr, Level * level) {
	int i, j;
	int lowest;
	int highest;
	int temp;

	if (level->newHighScore || level->initialSort) {
		if ((fptr = fopen("highScore.txt", "w")) == NULL) {
			printf("Cannot open file");
		}

		for (i = 0; i < 2; i++) {
			highest = i;
			lowest = 4 - i;

			for (j = i; j < 5 - i; j++) {
				if (level->highScores[j] > level->highScores[highest]) {
					temp = level->highScores[highest];
					level->highScores[highest] = level->highScores[j];
					level->highScores[j] = temp;
				}

				if (level->highScores[j] < level->highScores[lowest]) {
					temp = level->highScores[lowest];
					level->highScores[lowest] = level->highScores[j];
					level->highScores[j] = temp;
				}
			}
		}

		for (int i = 0; i < level->highScoreCount; i++)
			fprintf(fptr, "%d\n", level->highScores[i]);
		fclose(fptr);
	}

	level->initialSort = false;
}
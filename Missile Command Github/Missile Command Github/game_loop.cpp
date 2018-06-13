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

void playerMovement(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_EVENT_QUEUE *event_queue, Crosshair crosshair,
	struct abmData * abm, Enemy ** enemy, ALLEGRO_FONT * font, Base * base, Explosion * explosion, int * theme, int colorMap[][3], Level * level, ALLEGRO_BITMAP * background,
	ALLEGRO_BITMAP * imageUfo, Ufo * ufo, ALLEGRO_BITMAP ** imageBomb, Bomb * bomb, ALLEGRO_BITMAP * imageLauncher, ALLEGRO_BITMAP * ground, ALLEGRO_BITMAP * imageBase,
	ALLEGRO_FONT * titleFont, FILE * fptr, Audio * audio) {

	bool done = false;
	bool draw = true;
	int count = 0;
	int i;
	int round = 1;
	bool proceedLevel = true;
	bool paused = false;

	al_hide_mouse_cursor(display);

	while (!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {  //update every 1/60 of a second 
			draw = true;

			spawnEnemy(enemy, level, ufo, bomb, base);

			updateAbm(abm);

			updateEnemy(enemy, level);

			hitDetection(abm, enemy, explosion, level, ufo, audio);

			abmArrival(abm, explosion);

			updateBomb(level, bomb, explosion);

			updateUfo(ufo, level);

			updateUfoMissile(ufo, level);

			spawnUfoMissile(ufo, level);

			enemyArrival(enemy, level, ufo, bomb);

			baseCollision(base, enemy, 6, level, ufo, bomb);

			bombHitDetection(bomb, explosion, level, audio);


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
					al_draw_text(font, al_map_rgb(255, 0, 0), 450, 400, ALLEGRO_ALIGN_CENTER, "PAUSED");
					al_flip_display();
				}
				else if (paused) {
					paused = false;
					al_start_timer(timer);
				}
				break;
			}

		}

		/*else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		switch (ev.keyboard.keycode) {

		case ALLEGRO_KEY_SPACE:
		key[KEY_SPACE] = true;
		crosshair.target.x = ev.mouse.x;
		crosshair.target.y = ev.mouse.y;
		printf("(%d, %d) ", ev.mouse.x, ev.mouse.y);
		//fire(abm, crosshair);
		break;
		}
		}*/


		//if key is released 
		/*else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
		switch (ev.keyboard.keycode) {
		case ALLEGRO_KEY_UP:
		key[KEY_UP] = false;
		break;

		case ALLEGRO_KEY_DOWN:
		key[KEY_DOWN] = false;
		break;

		case ALLEGRO_KEY_LEFT:
		key[KEY_LEFT] = false;
		break;

		case ALLEGRO_KEY_RIGHT:
		key[KEY_RIGHT] = false;
		break;

		case ALLEGRO_KEY_SPACE:
		key[KEY_SPACE] = false;
		break;

		case ALLEGRO_KEY_ESCAPE:
		done = true;
		break;
		}
		}*/

		//must draw everything every tick of timer      
		if (draw && al_is_event_queue_empty(event_queue)) {
			draw = false;
			round++;

			al_clear_to_color(al_map_rgb(0, 0, 0));  //clear screen to black to create illusion of animation; draw & clear screen, draw & clear screen... 

			drawObjects(base, 6, imageLauncher, abm, imageBase, background, ground);

			drawExplosion(abm, explosion, colorMap);

			drawAbm(abm, theme[0], colorMap);

			drawEnemy(enemy, theme, colorMap, level, ufo, imageUfo, imageBomb, bomb);

			drawInfo(font, abm, level);

			drawCrosshair(imageCrosshair, &crosshair);

			al_flip_display();

			//game over 
			if (level->lives == 0) {
				done = true;
			}

			//next level 
			if (level->num_spawned >= level->spawnLimit) {

				if (levelProceed(level, explosion, enemy, ufo, bomb)) {

					al_stop_timer(timer);

					for (i = 0; i < level->maxEnemyOnScreen; i++) {
						free(enemy[i]);
					}

					free(enemy);
					free(ufo);
					free(bomb);

					loadNextLevel(level, abm, base);

					enemy = (Enemy **)malloc((level->maxEnemyOnScreen) * sizeof(Enemy *));
					for (i = 0; i < level->maxEnemyOnScreen; i++) {
						enemy[i] = (Enemy *)malloc(SPLIT_COUNT * sizeof(Enemy));
					}

					ufo = (Ufo *)malloc((level->maxUfoOnScreen) * sizeof(Ufo));

					bomb = (Bomb *)malloc((level->maxBombOnScreen) * sizeof(Bomb));

					initLevel(level);
					initAbm(abm, explosion);
					initEnemy(enemy, level, ufo, bomb);
					generateTheme(theme);
					transition(font, timer, abm, level);

					al_start_timer(timer);
				}
			}
		}
	}

	calcScore(level);
	sortScore(fptr, level);

	for (i = 0; i < level->maxEnemyOnScreen; i++) {
		free(enemy[i]);
	}

	free(enemy);
	free(ufo);
	free(bomb);

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(titleFont, al_map_rgb(255, 0, 0), 300, 400, 0, "THE END");
	al_draw_textf(font, al_map_rgb(255, 0, 0), 300, 500, ALLEGRO_ALIGN_CENTRE, "%d", level->score);
	al_flip_display();
	al_rest(5);

	al_destroy_bitmap(imageCrosshair);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	for (i = 0; i<5; i++)
		al_destroy_sample(audio->explosion[i]);
}


bool levelProceed(Level * level, Explosion * explosion, Enemy ** enemy, Ufo * ufo, Bomb * bomb) {
	bool proceed = true;

	for (int i = 0; i < level->maxEnemyOnScreen; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched)
				proceed = false;
		}
	}

	for (int i = 0; i < ABM_COUNT; i++) {
		if (explosion[i].ongoing)
			proceed = false;
	}

	for (int i = 0; i < level->maxUfoOnScreen; i++) {
		if (ufo[i].spawned)
			proceed = false;
	}

	for (int i = 0; i < level->maxBombOnScreen; i++) {
		if (bomb[i].spawned)
			proceed = false;
	}

	return proceed;
}

void loadNextLevel(Level * level, Abm * abm, Base * base) {
	int i;

	//enemy missiles
	level->enemySpeed += 0.25;
	level->spawnLimit += 5;
	level->round++;

	if (level->round % 5 == 0)
		level->maxEnemyOnScreen += 5;

	if(level->spawnRangeMin > 0)
		level->spawnRangeMin -= 2;
	if (level->spawnRangeMax < 2000)
		level->spawnRangeMax += 2;

	if(level->splitRangeMin > 0)
		level->splitRangeMin -= 2;
	if(level->splitRangeMax < 2000)
		level->splitRangeMax += 2; 

	

	/*if (level->splitRate > 100)
		level->splitRate -= 100;*/

	if (level->splitAngle <= 600)
		level->splitAngle += 50;


	//ufo 
	level->spawnUfo = true;
	level->ufoSpeed += 0.5;
	level->ufoSpawnLimit += 1;
	(level->maxUfoOnScreen)++;
	(level->ufoSpawnLimit)++;
	if (level->ufoSpawnRangeMin > 0)
		level->ufoSpawnRangeMin -= 10;
	if (level->ufoSpawnRangeMax < 1000)
		level->ufoSpawnRangeMax += 10;


	//bomb
	(level->bombSpawnLimit)++;
	(level->maxBombOnScreen)++;
	(level->bombSpeed) += 1;


	//100 points for any remaining bases
	for (i = 0; i < 6; i++) {
		if (!base[i].destroyed)
			(level->score) += 100;
	}

	//5 points for any unused missile
	for (i = 0; i < ABM_COUNT; i++) {
		if (!abm[i].launched && !abm[i].arrived)
			(level->score) += 5;
	}

}

void readScore(FILE * fptr, Level * level) {
	if ((fptr = fopen("highScore.txt", "r")) == NULL)
		printf("Can't open highScore.txt.");

	for (int i = 0; i < level->highScoreCount; i++) {
		fscanf(fptr, "%d", &(level->highScores[i]));
		printf("%d\n", level->highScores[i]);
	}


	fclose(fptr);
}

void calcScore(Level * level) {
	int lowest = 0;

	for (int i = 0; i < level->highScoreCount; i++) {
		if (level->highScores[i] < level->highScores[lowest]) {
			lowest = i;
		}
	}

	if (level->score > level->highScores[lowest]) {
		level->highScores[lowest] = level->score;	//lowest high score overwritten by new high score
		level->newHighScore = true;
	}
}


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
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

void playerMovement(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_EVENT_QUEUE *event_queue, Crosshair crosshair,
	struct abmData * abm, Enemy ** enemy, ALLEGRO_FONT * font, Base * base, Explosion * explosion, int * theme, int colorMap[][3], Level * level, ALLEGRO_BITMAP * background,
	ALLEGRO_BITMAP * imageUfo, Ufo * ufo, ALLEGRO_BITMAP ** imageBomb, Bomb * bomb, ALLEGRO_BITMAP * imageLauncher, ALLEGRO_BITMAP * ground, ALLEGRO_BITMAP * imageBase,
	ALLEGRO_FONT * titleFont, FILE * fptr, int * highScore, int highScoreCount) {

	bool done = false;
	bool draw = true;
	int count = 0;
	int i;
	int round = 1;
	bool proceedLevel = true;
	bool paused = false; 

	bool key[5] = { false, false, false, false, false };  //array with members KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT; each member is true or false 

	al_hide_mouse_cursor(display);


	while (!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {  //update every 1/60 of a second 
			draw = true;

			spawnEnemy(enemy, level, ufo, bomb);

			updateAbm(abm);
		
			updateEnemy(enemy, level);

			hitDetection(abm, enemy, explosion, level, ufo);
			
			abmArrival(abm, explosion);
		
			updateBomb(level, bomb, explosion);
			
			updateUfo(ufo, level);
			
			updateUfoMissile(ufo, level);
			
			spawnUfoMissile(ufo, level); 
			
			enemyArrival(enemy, level, ufo, bomb);
			
			baseCollision(base, enemy, 6, level, ufo, bomb);
			
			bombHitDetection(bomb, explosion, level);
			
 
		}

		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			//done = true; 
			break;
		}

		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
			if (ev.mouse.x >= 4 && ev.mouse.x <= SCREEN_W - crosshair.width)
				crosshair.x = ev.mouse.x;
			if (ev.mouse.y >= 4 && ev.mouse.y <= SCREEN_H - crosshair.height/*-150*/)
				crosshair.y = ev.mouse.y;
		}

		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (ev.mouse.button & 1) {
				crosshair.target_x = ev.mouse.x;   //get coordinate of target 
				crosshair.target_y = ev.mouse.y;
				fire(abm, crosshair, level);
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
		crosshair.target_x = ev.mouse.x;
		crosshair.target_y = ev.mouse.y;
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

			drawObjects(base, 6, &(theme[2]), colorMap, imageLauncher, abm, imageBase, background, ground);

			drawExplosion(abm, explosion, colorMap);
		
			drawAbm(abm, &(theme[0]), colorMap);

			drawEnemy(enemy, &(theme[1]), colorMap, level, ufo, imageUfo, imageBomb, bomb);
		
			drawInfo(font, abm, level);

			drawCrosshair(imageCrosshair, &crosshair);

			al_flip_display();

			//game over 
			if (level->lives == 0) {
				done = true;
			}

			//next level 
			if (level->num_spawned >= level->spawnLimit) {

				proceedLevel = true;

				for (int i = 0; i < level->spawnLimit; i++) {
					for (int j = 0; j < SPLIT_COUNT; j++) {
						if (enemy[i][j].launched)
							proceedLevel = false;
					}
				}

				for (int i = 0; i < ABM_COUNT; i++) {
					if (explosion[i].ongoing)
						proceedLevel = false;
				}

				for (int i = 0; i < level->ufoSpawnLimit; i++) {
					if (ufo[i].spawned)
						proceedLevel = false;
				}

				for (int i = 0; i < level->bombSpawnLimit; i++) {
					if (bomb[i].spawned)
						proceedLevel = false;
				}


				if (proceedLevel) {

					al_stop_timer(timer);

					for (i = 0; i < level->spawnLimit; i++) {
						free(enemy[i]);
					}
					free(enemy);

					free(ufo); 

					(level->spawnLimit) += 5;
					(level->round)++;
					(level->spawnRate) -= 100;

					level->spawnUfo = true;
					level->ufoSpeed += 0.5; 
					level->ufoSpawnLimit += 2; 

					if(level->ufoSpawnRate > 10)
						level->ufoSpawnRate -= 10;

					if (level->spawnRate <= 0)
						level->spawnRate = 10;

					(level->splitRate) -= 100;

					if (level->splitRate <= 0)
						level->splitRate = 10;
					
					if(level->splitAngle >= 80)
						(level->splitAngle) += 10; 
					
					(level->enemySpeed) += 0.5;

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

					enemy = (Enemy **)malloc((level->spawnLimit) * sizeof(Enemy *));
					for (i = 0; i < level->spawnLimit; i++) {
						enemy[i] = (Enemy *)malloc(SPLIT_COUNT * sizeof(Enemy));
					}

					ufo = (Ufo *)malloc(level->ufoSpawnLimit * sizeof(Ufo));

					initLevel(level); 
					initEnemy(enemy, level, ufo, bomb);
					initAbm(abm, explosion);
					generateTheme(theme);

					transition(font, timer, abm, level);
				}
			}
		}
	}

	calcScore(fptr, highScore, highScoreCount, level);

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(titleFont, al_map_rgb(255, 0, 0), 300, 400, 0, "THE END");
	al_flip_display();
	al_rest(5);

	//al_get_time
	//al_current_time
	//al_init_timeout

	/*while (1) {
	r = rand() % 100 + 1;
	g = rand() % 100 + 1;
	b = rand() % 100 + 1;
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(font, al_map_rgb(r, g, b), 400, 450, 0, "THE END");
	al_flip_display();
	}*/

	al_destroy_bitmap(imageCrosshair);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
}


void calcScore(FILE * fptr, int * highScore, int highScoreCount, Level * level) {
	int lowest = 0;

	for (int i = 0; i < highScoreCount; i++) {
		if (highScore[i] < highScore[lowest]) {
			lowest = i;
		}
	}

	if (level->score > highScore[lowest]) {
		highScore[lowest] = level->score;
		fptr = fopen("highScore.txt", "w");

		for (int i = 0; i < 5; i++) 
			fprintf(fptr, "%d\n", highScore[i]);

		fclose(fptr);
	}
}
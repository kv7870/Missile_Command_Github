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
	ALLEGRO_BITMAP * imageUfo, Ufo * ufo, ALLEGRO_BITMAP ** imageBomb, Bomb * bomb, ALLEGRO_BITMAP * imageLauncher, ALLEGRO_BITMAP * ground) {

	bool done = false;
	bool draw = true;
	int count = 0;
	int i;
	int round = 1;
	bool doneUpdate = true;

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
			
			baseCollision(base, enemy, 6, level);
			
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

			al_draw_bitmap(background, 0, 0, 0);

			al_draw_bitmap(ground, -100, 750, 0); 

			drawObjects(base, 6, &(theme[2]), colorMap, imageLauncher, abm);

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

				doneUpdate = true;

				for (int i = 0; i < level->spawnLimit; i++) {
					for (int j = 0; j < SPLIT_COUNT; j++) {
						if (enemy[i][j].launched)
							doneUpdate = false;
					}
				}

				for (int i = 0; i < ABM_COUNT; i++) {
					if (explosion[i].ongoing)
						doneUpdate = false;
				}

				for (int i = 0; i < level->ufoSpawnLimit; i++) {
					if (ufo[i].spawned)
						doneUpdate = false;
				}

				for (int i = 0; i < level->bombSpawnLimit; i++) {
					if (bomb[i].spawned)
						doneUpdate = false;
				}


				if (doneUpdate) {

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


	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(font, al_map_rgb(255, 0, 0), 400, 450, 0, "THE END");
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

void drawCrosshair(ALLEGRO_BITMAP *imageCrosshair, Crosshair * crosshair) {
	al_draw_bitmap(imageCrosshair, crosshair->x - 27, crosshair->y - 23, 0);
}

void drawInfo(ALLEGRO_FONT * font, Abm * abm, Level * level) {
	//char printString[200];
	//sprintf(printString, "%s", string); 
	//al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_CENTRE, printString);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 10, 10, 0, "Bases: %d", level->lives);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 170, 10, 0, "Missiles: %d", level->abmLeft);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 370, 10, 0, "Score: %d", level->score);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 530, 10, 0, "Level: %d", level->round);


	//# abm left per battery 
	al_draw_textf(font, al_map_rgb(0, 0, 0), 35, 860, 0, "%d", level->batteryAbmLeft[0]);
	al_draw_textf(font, al_map_rgb(0, 0, 0), 415, 860, 0, "%d", level->batteryAbmLeft[1]);
	al_draw_textf(font, al_map_rgb(0, 0, 0), 850, 860, 0, "%d", level->batteryAbmLeft[2]);
}

void transition(ALLEGRO_FONT * font, ALLEGRO_TIMER * timer, Abm * abm, Level * level) {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_textf(font, al_map_rgb(255, 0, 0), 400, 300, 0, "Level: %d", level->round);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 390, 400, 0, "Score: %d", level->score);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 380, 500, 0, "Bases left: %d", level->lives);

	al_flip_display();
	al_rest(3);

	al_resume_timer(timer);
}


void drawObjects(Base * base, int baseCount, int * theme, int colorMap[][3], ALLEGRO_BITMAP * imageLauncher, Abm * abm) {

	int colorId2 = theme[0];
	int colorId3 = theme[1];

	//planet surface
	//al_draw_filled_ellipse(450, 900, 500, 60, al_map_rgb(69, 190, 247)); 
	//69 190 247
	//94, 242, 104

	//al_draw_bitmap(texture, 0, 800, 0); 
	//al_draw_filled_rectangle(0, 850, 900, 900, al_map_rgb(153, 102, 51)); 

	//abm pics 

	//first battery (left)
	if(!abm[0].launched && !abm[0].arrived)
		al_draw_bitmap(imageLauncher, -10, 800, 0); 
	if (!abm[1].launched && !abm[1].arrived)
		al_draw_bitmap(imageLauncher, 0, 800, 0);
	if (!abm[2].launched && !abm[2].arrived)
		al_draw_bitmap(imageLauncher, 10, 800, 0);
	if (!abm[3].launched && !abm[3].arrived)
		al_draw_bitmap(imageLauncher, 20, 800, 0);
	if (!abm[4].launched && !abm[4].arrived)
		al_draw_bitmap(imageLauncher, 30, 800, 0);
	if (!abm[5].launched && !abm[5].arrived)
		al_draw_bitmap(imageLauncher, 40, 800, 0);
	if (!abm[6].launched && !abm[6].arrived)
		al_draw_bitmap(imageLauncher, 50, 800, 0);
	if (!abm[7].launched && !abm[7].arrived)
		al_draw_bitmap(imageLauncher, 60, 800, 0);
	if (!abm[8].launched && !abm[8].arrived)
		al_draw_bitmap(imageLauncher, 70, 800, 0);
	if (!abm[9].launched && !abm[9].arrived)
		al_draw_bitmap(imageLauncher, 80, 800, 0);


	//second battery (center)
	if (!abm[10].launched && !abm[10].arrived)
		al_draw_bitmap(imageLauncher, 370, 800, 0);
	if (!abm[11].launched && !abm[11].arrived)
		al_draw_bitmap(imageLauncher, 380, 800, 0);
	if (!abm[12].launched && !abm[12].arrived)
		al_draw_bitmap(imageLauncher, 390, 800, 0);
	if (!abm[13].launched && !abm[13].arrived)
		al_draw_bitmap(imageLauncher, 400, 800, 0);
	if (!abm[14].launched && !abm[14].arrived)
		al_draw_bitmap(imageLauncher, 410, 800, 0);
	if (!abm[15].launched && !abm[15].arrived)
		al_draw_bitmap(imageLauncher, 420, 800, 0);
	if (!abm[16].launched && !abm[16].arrived)
		al_draw_bitmap(imageLauncher, 430, 800, 0);
	if (!abm[17].launched && !abm[17].arrived)
		al_draw_bitmap(imageLauncher, 440, 800, 0);
	if (!abm[18].launched && !abm[18].arrived)
		al_draw_bitmap(imageLauncher, 450, 800, 0);
	if (!abm[19].launched && !abm[19].arrived)
		al_draw_bitmap(imageLauncher, 460, 800, 0);

	//third battery (right) 
	if (!abm[20].launched && !abm[20].arrived)
		al_draw_bitmap(imageLauncher, 770, 800, 0);
	if (!abm[21].launched && !abm[21].arrived)
		al_draw_bitmap(imageLauncher, 780, 800, 0);
	if (!abm[22].launched && !abm[22].arrived)
		al_draw_bitmap(imageLauncher, 790, 800, 0);
	if (!abm[23].launched && !abm[23].arrived)
		al_draw_bitmap(imageLauncher, 800, 800, 0);
	if (!abm[24].launched && !abm[24].arrived)
		al_draw_bitmap(imageLauncher, 810, 800, 0);
	if (!abm[25].launched && !abm[25].arrived)
		al_draw_bitmap(imageLauncher, 820, 800, 0);
	if (!abm[26].launched && !abm[26].arrived)
		al_draw_bitmap(imageLauncher, 830, 800, 0);
	if (!abm[27].launched && !abm[27].arrived)
		al_draw_bitmap(imageLauncher, 840, 800, 0);
	if (!abm[28].launched && !abm[28].arrived)
		al_draw_bitmap(imageLauncher, 850, 800, 0);
	if (!abm[29].launched && !abm[29].arrived)
		al_draw_bitmap(imageLauncher, 860, 800, 0);


	//al_draw_filled_rectangle(10, 850, 80, 900, al_map_rgb(colorMap[colorId2][R], colorMap[colorId2][G], colorMap[colorId2][B]));
	//al_draw_filled_rectangle(390, 850, 460, 900, al_map_rgb(colorMap[colorId2][R], colorMap[colorId2][G], colorMap[colorId2][B]));
	//al_draw_filled_rectangle(820, 850, 890, 900, al_map_rgb(colorMap[colorId2][R], colorMap[colorId2][G], colorMap[colorId2][B]));

	//bases

	//left
	if (!base[0].destroyed)
		al_draw_filled_rectangle(120, 870, 170, 900, al_map_rgb(colorMap[colorId3][R], colorMap[colorId3][G], colorMap[colorId3][B]));
	if (!base[1].destroyed)
		al_draw_filled_rectangle(210, 870, 260, 900, al_map_rgb(colorMap[colorId3][R], colorMap[colorId3][G], colorMap[colorId3][B]));
	if (!base[2].destroyed)
		al_draw_filled_rectangle(300, 870, 350, 900, al_map_rgb(colorMap[colorId3][R], colorMap[colorId3][G], colorMap[colorId3][B]));

	//right
	if (!base[3].destroyed)
		al_draw_filled_rectangle(505, 870, 555, 900, al_map_rgb(colorMap[colorId3][R], colorMap[colorId3][G], colorMap[colorId3][B]));
	if (!base[4].destroyed)
		al_draw_filled_rectangle(605, 870, 655, 900, al_map_rgb(colorMap[colorId3][R], colorMap[colorId3][G], colorMap[colorId3][B]));
	if (!base[5].destroyed)
		al_draw_filled_rectangle(705, 870, 755, 900, al_map_rgb(colorMap[colorId3][R], colorMap[colorId3][G], colorMap[colorId3][B]));

}

void generateOneColor(int * theme, int i)
{
	theme[i] = rand() % NUM_COLORS;

	if (i > 1) {
		for (int j = i - 1; j >= 0; j--) {
			if (theme[i] == theme[j]) {
				generateOneColor(theme, i);
				break;
			}
		}
	}
}

void generateTheme(int * theme)
{
	for (int i = 0; i < COLORS_PER_THEME; i++) {
		generateOneColor(theme, i);
	}
}
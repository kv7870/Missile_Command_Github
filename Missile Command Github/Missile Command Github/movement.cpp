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
	struct abmData * abm, Enemy ** enemy, int  * curr_enemy_count, int * num_spawned, int * lvl_spawn_limit, int * level, float * enemySpeed, 
	int * spawnRate, int * splitRate, ALLEGRO_FONT * font, int * lives, int * abmLeft, int * score, Base * base, int * batteryAbmLeft) {

	//ship.x = SCREEN_W / 2.0 - BOUNCER_SIZE / 2.0;
	//ship.y = SCREEN_H / 2.0 - BOUNCER_SIZE / 2.0;
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

			updateAbm(abm);
			abmArrival(abm);
			hitDetection(abm, enemy, curr_enemy_count, lvl_spawn_limit, score);
			spawnEnemy(enemy, curr_enemy_count, num_spawned, lvl_spawn_limit, spawnRate, splitRate);
			updateEnemy(enemy, lvl_spawn_limit, enemySpeed);
			enemyArrival(enemy, curr_enemy_count, lvl_spawn_limit);
			baseCollision(base, enemy, lvl_spawn_limit, 6, lives); 
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
				printf("(%d, %d) ", ev.mouse.x, ev.mouse.y);
				printf("fired");
				fire(abm, crosshair, abmLeft, batteryAbmLeft);
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

			drawCrosshair(imageCrosshair, &crosshair);

			drawAbm(abm);

			drawExplosion(abm);

			drawEnemy(enemy, lvl_spawn_limit);

			drawObjects(base, 6);

			drawInfo(font, abm, lives, level, abmLeft, score, batteryAbmLeft); 

			printf("Round: %d Level: %d Spawned: %d\n", round, *level, *num_spawned);

			al_flip_display();

			if (*num_spawned >= *lvl_spawn_limit) {

				doneUpdate = true;

				for (int i = 0; i < *lvl_spawn_limit; i++) {
					for (int j = 0; j < SPLIT_COUNT; j++) {
						if (enemy[i][j].launched)
							doneUpdate = false;
					}
				}

				for (int i = 0; i < ABM_COUNT; i++) {
					if (abm[i].arrived && !abm[i].doneExploding)
						doneUpdate = false;
				}

				if (doneUpdate) {

					al_stop_timer(timer); 

					for (i = 0; i < *lvl_spawn_limit; i++) {
						free(enemy[i]);
					}
					free(enemy);

					(*lvl_spawn_limit) += 10;
					(*level)++;
					(*spawnRate) -= 100;
					(*splitRate) -= 100; 
					*num_spawned = 0;
					*curr_enemy_count = 0;
					(*enemySpeed) += 0.5; 
					

					enemy = (Enemy **)malloc((*lvl_spawn_limit) * sizeof(Enemy *));
					for (i = 0; i < *lvl_spawn_limit; i++) {
						enemy[i] = (Enemy *)malloc(SPLIT_COUNT * sizeof(Enemy));
					}

					initEnemy(enemy, lvl_spawn_limit);
					initAbm(abm, abmLeft);

					transition(font, timer, abm, lives, level, score); 
				}
			}
		}
	}

	al_destroy_bitmap(imageCrosshair);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
}

void drawCrosshair(ALLEGRO_BITMAP *imageCrosshair, Crosshair * crosshair) {
	al_draw_bitmap(imageCrosshair, crosshair->x - 27, crosshair->y - 23, 0);
}

void drawInfo(ALLEGRO_FONT * font, Abm * abm, int * lives, int * level, int * abmLeft, int * score, int * batteryAbmLeft) {
	//char printString[200];
	//sprintf(printString, "%s", string); 
	//al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_CENTRE, printString);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 10, 10, 0, "Bases: %d", *lives);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 170, 10, 0, "Missiles: %d", *abmLeft);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 370, 10, 0, "Score: %d", *score);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 530, 10, 0, "Level: %d", *level);


	//# abm left per battery 
	al_draw_textf(font, al_map_rgb(255, 0, 0), 35, 860, 0, "%d", batteryAbmLeft[0]);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 415, 860, 0, "%d", batteryAbmLeft[1]);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 850, 860, 0, "%d", batteryAbmLeft[2]);

}

void transition(ALLEGRO_FONT * font, ALLEGRO_TIMER * timer, Abm * abm, int * lives, int * level, int * score) {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_textf(font, al_map_rgb(255, 0, 0), 400, 300, 0, "Level: %d", *level);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 390, 400, 0, "Score: %d", *score);
	al_draw_textf(font, al_map_rgb(255, 0, 0), 380, 500, 0, "Bases left: %d", *lives);

	al_flip_display(); 
	al_rest(3); 

	al_resume_timer(timer); 
}


void drawObjects(Base * base, int baseCount) {

	//launchpads 
	al_draw_filled_rectangle(10, 850, 80, 900, al_map_rgb(255, 255, 0));
	al_draw_filled_rectangle(390, 850, 460, 900, al_map_rgb(255, 255, 0));
	al_draw_filled_rectangle(820, 850, 890, 900, al_map_rgb(255, 255, 0));

	//bases
	al_draw_filled_rectangle(120, 870, 170, 900, al_map_rgb(base[0].color.r, base[0].color.g, base[0].color.b));
	al_draw_filled_rectangle(210, 870, 260, 900, al_map_rgb(base[1].color.r, base[1].color.g, base[1].color.b));
	al_draw_filled_rectangle(300, 870, 350, 900, al_map_rgb(base[2].color.r, base[2].color.g, base[2].color.b));

	//bases
	al_draw_filled_rectangle(505, 870, 555, 900, al_map_rgb(base[3].color.r, base[3].color.g, base[3].color.b));
	al_draw_filled_rectangle(605, 870, 655, 900, al_map_rgb(base[4].color.r, base[4].color.g, base[4].color.b));
	al_draw_filled_rectangle(705, 870, 755, 900, al_map_rgb(base[5].color.r, base[5].color.g, base[5].color.b));
	

}
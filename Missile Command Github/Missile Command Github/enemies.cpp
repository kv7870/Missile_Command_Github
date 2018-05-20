#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h> // Include the allegro header file.
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "header.h"
#include <time.h>
#include <stdlib.h>

void spawnEnemy(Enemy * enemy, int * curr_enemy_num, int * num_spawned) {
	int i;
	int spawnTiming = rand() % 10 + 1;

	if (spawnTiming == 5) {
		if (*curr_enemy_num <= MAX_ENEMY && *num_spawned < ENEMY_COUNT) {
			for (i = 0; i < ENEMY_COUNT; i++) {
				if (!enemy[i].launched && !enemy[i].arrived) {
					enemy[i].launch_x = rand() % 861 + 20;
					enemy[i].dest_x = rand() % 801 + 50;
					enemy[i].x_pos = enemy[i].launch_x;
					enemy[i].y_pos = enemy[i].launch_y;
					enemy[i].launched = true;
					(*curr_enemy_num)++;
					(*num_spawned)++;
					break; 
				}
			}
		}
	}
}


void drawEnemy(Enemy * enemy) {

	for (int i = 0; i < ENEMY_COUNT; i++) {
		if (enemy[i].launched) {
			al_draw_filled_circle(enemy[i].x_pos, enemy[i].y_pos, 10, al_map_rgb(255, 255, 255));
			al_draw_line(enemy[i].x_pos, enemy[i].y_pos, enemy[i].launch_x, enemy[i].launch_y, al_map_rgb(255, 255, 255), 3);
		}
	}
}


void updateEnemy(Enemy * enemy) {

	for (int i = 0; i < ENEMY_COUNT; i++) {
		if (enemy[i].launched) {

			enemy[i].dx = fabs(enemy[i].dest_x - enemy[i].launch_x);
			enemy[i].dy = fabs(enemy[i].dest_y - enemy[i].launch_y);

			if (enemy[i].dx >= enemy[i].dy) {
				enemy[i].step = enemy[i].dx;
			}
			else {
				enemy[i].step = enemy[i].dy;
			}

			enemy[i].x_inc = enemy[i].dx / enemy[i].step;
			enemy[i].y_inc = enemy[i].dy / enemy[i].step;

			enemy[i].y_pos += 2 * enemy[i].y_inc;

			if (enemy[i].dest_x > enemy[i].launch_x) {
				enemy[i].x_pos += 2 * enemy[i].x_inc;
			}
			else if (enemy[i].dest_x < enemy[i].launch_x) {
				enemy[i].x_pos -= 2 * enemy[i].x_inc;
			}

		}
	}
}


void enemyArrival(Enemy * enemy, int * curr_num_enemy) {

	for (int i = 0; i < 30; i++) {
		if (enemy[i].launched) {
			enemy[i].distance = sqrt(pow((enemy[i].dest_x - enemy[i].x_pos), 2) + pow((enemy[i].dest_y - enemy[i].y_pos), 2));
			if (enemy[i].distance <= 10) {
				enemy[i].launched = false;
				enemy[i].arrived = true;
				(*curr_num_enemy)--;
			}
		}
	}
}

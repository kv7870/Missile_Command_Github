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

void spawnEnemy(Enemy enemy[ENEMY_COUNT][SPLIT_COUNT], int * curr_enemy_count, int * num_spawned) {
	int spawnTiming = 0;

	if (*curr_enemy_count <= MAX_ENEMY && *num_spawned < (ENEMY_COUNT * SPLIT_COUNT)) {
		for (int i = 0; i < ENEMY_COUNT; i++) {
			if (!enemy[i][0].launched && !enemy[i][0].arrived) {  //main missile
				spawnTiming = rand() % 100 + 1;
				if (spawnTiming == 50) {
					enemy[i][0].launch_x = rand() % 861 + 20;
					enemy[i][0].dest_x = rand() % 801 + 50;
					enemy[i][0].x_pos = enemy[i][0].launch_x;
					enemy[i][0].y_pos = enemy[i][0].launch_y;
					enemy[i][0].launched = true;
					(*curr_enemy_count)++;
					(*num_spawned)++;
					calcEnemyInc(&(enemy[i][0]));
					break;  //prevents all enemies spawning at once
				}
			}

			for (int j = 1; j < SPLIT_COUNT; j++) {
				if (enemy[i][j - 1].launched && !enemy[i][j].launched) {
					spawnTiming = rand() % 10 + 1;
					if (spawnTiming == 5) {
						enemy[i][j].launch_x = enemy[i][j - 1].x_pos;
						enemy[i][j].launch_y = enemy[i][j - 1].y_pos;
						enemy[i][j].dest_x = enemy[i][j - 1].dest_x + 50;
						enemy[i][j].x_pos = enemy[i][j].launch_x;
						enemy[i][j].y_pos = enemy[i][j].launch_y;
						enemy[i][j].launched = true;
						(*num_spawned)++;
						calcEnemyInc(&(enemy[i][j]));
						break;
					}
				}
			}
		}
	}
}


void calcEnemyInc(Enemy * enemy) {
	enemy->dx = fabs(enemy->dest_x - enemy->launch_x);
	enemy->dy = fabs(enemy->dest_y - enemy->launch_y);

	if (enemy->dx >= enemy->dy) {
		enemy->step = enemy->dx;
	}
	else {
		enemy->step = enemy->dy;
	}

	enemy->x_inc = enemy->dx / enemy->step;
	enemy->y_inc = enemy->dy / enemy->step;
}


void updateEnemy(Enemy enemy[ENEMY_COUNT][SPLIT_COUNT]) {
	for (int i = 0; i < ENEMY_COUNT; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {

				if (enemy[i][j].dest_x > enemy[i][j].launch_x) {
					enemy[i][j].x_pos += 2*enemy[i][j].x_inc;
				}
				else if (enemy[i][j].dest_x < enemy[i][j].launch_x) {
					enemy[i][j].x_pos -= 2*enemy[i][j].x_inc;
				}

				enemy[i][j].y_pos += 2*enemy[i][j].y_inc;

				//calculate bounds
				enemy[i][j].topLeft.x = enemy[i][j].x_pos - SIZE;
				enemy[i][j].topLeft.y = enemy[i][j].y_pos - SIZE;
				enemy[i][j].topRight.x = enemy[i][j].x_pos + SIZE;
				enemy[i][j].topRight.y = enemy[i][j].y_pos - SIZE;
				enemy[i][j].bottomLeft.x = enemy[i][j].x_pos - SIZE;
				enemy[i][j].bottomLeft.y = enemy[i][j].y_pos + SIZE;
			}
		}
	}
}


void drawEnemy(Enemy enemy[ENEMY_COUNT][SPLIT_COUNT]) {
	for (int i = 0; i < ENEMY_COUNT; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {
				al_draw_filled_rectangle(enemy[i][j].x_pos - 3, enemy[i][j].y_pos - 3, enemy[i][j].x_pos + 3, enemy[i][j].y_pos + 3, al_map_rgb(255, 255, 255));
				al_draw_line(enemy[i][j].x_pos, enemy[i][j].y_pos - 3, enemy[i][j].launch_x, enemy[i][j].launch_y, al_map_rgb(255, 0, 0), 4);
			}
		}
	}
}


void enemyArrival(Enemy enemy[ENEMY_COUNT][SPLIT_COUNT], int * curr_enemy_count) {
	for (int i = 0; i < ENEMY_COUNT; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {
				if (enemy[i][j].y_pos >= enemy[i][j].dest_y) {
					enemy[i][j].launched = false;
					enemy[i][j].arrived = true; 
					
					if (j == 0)
						(*curr_enemy_count)--;
				}
			}
		}
	}
}
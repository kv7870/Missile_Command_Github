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
	int spawnTiming = rand() % 100 + 1;

	if (spawnTiming == 50) {
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
					break;  //break prevents spawning all enemies at once 
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

			enemy[i].splitNum = rand() % 100 + 1; 

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


void spawnMirv(Enemy * enemy, Mirv * mirv) {
	int j;
	int dest_dx; 
	int splitNum; 

	for (int i = 0; i < ENEMY_COUNT; i++) {
		splitNum = rand() % 1000 + 1; 
		if (enemy[i].launched && !enemy[i].split && splitNum == 50) {
			for (j = 0, dest_dx = 50; j < 3; j++, dest_dx += 50) {
				mirv[i].branch[j].launched = true;
				mirv[i].branch[j].launch_x = enemy[i].x_pos;
				mirv[i].branch[j].launch_y = enemy[i].y_pos;
				mirv[i].branch[j].x_pos = mirv[i].branch[j].launch_x;  //split from location of main missile 
				mirv[i].branch[j].y_pos = mirv[i].branch[j].launch_y;  
				mirv[i].branch[j].dest_x = enemy[i].dest_x + dest_dx; 
				enemy[i].split = true; 
			}
			//break; //prevents all missiles from splitting at same time 
		}
		//break; //prevents all missiles from splitting at same time 
	}
}

void updateMirv(Mirv * mirv) {
	for (int i = 0; i < ENEMY_COUNT; i++) {
		for (int j = 0; j < 3; j++) {
			if (mirv[i].branch[j].launched) {

				mirv[i].branch[j].dx = fabs(mirv[i].branch[j].dest_x - mirv[i].branch[j].launch_x);
				mirv[i].branch[j].dy = fabs(mirv[i].branch[j].dest_y - mirv[i].branch[j].launch_y);

				if (mirv[i].branch[j].dx >= mirv[i].branch[j].dy) {
					mirv[i].branch[j].step = mirv[i].branch[j].dx;
				}
				else {
					mirv[i].branch[j].step = mirv[i].branch[j].dy;
				}

				mirv[i].branch[j].x_inc = mirv[i].branch[j].dx / mirv[i].branch[j].step;
				mirv[i].branch[j].y_inc = mirv[i].branch[j].dy / mirv[i].branch[j].step;

				mirv[i].branch[j].y_pos += 2 * mirv[i].branch[j].y_inc;

				if (mirv[i].branch[j].dest_x > mirv[i].branch[j].launch_x) {
					mirv[i].branch[j].x_pos += 2 * mirv[i].branch[j].x_inc;
				}
				else if (mirv[i].branch[j].dest_x < mirv[i].branch[j].launch_x) {
					mirv[i].branch[j].x_pos -= 2 * mirv[i].branch[j].x_inc;
				}
			}
		}
	}
}

void drawMirv(Mirv * mirv) {
	for (int i = 0; i < ENEMY_COUNT; i++) {
		for (int j = 0; j < 3; j++) {
			if (mirv[i].branch[j].launched) {
				al_draw_filled_circle(mirv[i].branch[j].x_pos, mirv[i].branch[j].y_pos, 10, al_map_rgb(255, 255, 255));
				al_draw_line(mirv[i].branch[j].x_pos, mirv[i].branch[j].y_pos, mirv[i].branch[j].launch_x, mirv[i].branch[j].launch_y, al_map_rgb(255, 255, 255), 3);
			}
		}
	}
}
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

void spawnEnemy(Enemy ** enemy, Level * level, Ufo * ufo) {
	int spawnTiming = 0;
	int ufoSpawnTiming = 0;
	int i, j; 

	if (level->curr_enemy_count < MAX_ENEMY && level->num_spawned < level->spawnLimit) {
		for (i = 0; i < level->spawnLimit; i++) {
			if (!enemy[i][0].launched && !enemy[i][0].arrived) {  //original missile 
				spawnTiming = rand() % level->spawnRate + 1;
				if (spawnTiming == 5) {
					enemy[i][0].launch_x = rand() % 861 + 20;
					enemy[i][0].dest_x = rand() % 801 + 50;
					enemy[i][0].x_pos = enemy[i][0].launch_x;
					enemy[i][0].y_pos = enemy[i][0].launch_y;
					enemy[i][0].launched = true;
					(level->curr_enemy_count)++;
					(level->num_spawned)++;
					calcEnemyInc(&(enemy[i][0]));
					break;  //prevents all enemies spawning at once
				}
			}

			spawnTiming = rand() % level->splitRate + 1;
			if (spawnTiming == 50) {
				for (j = 1; j < SPLIT_COUNT; j++) {
					if (enemy[i][j - 1].launched && !enemy[i][j].launched) {
						enemy[i][j].launch_x = enemy[i][j - 1].x_pos;
						enemy[i][j].launch_y = enemy[i][j - 1].y_pos;
						enemy[i][j].dest_x = enemy[i][j - 1].dest_x + level->splitAngle;
						enemy[i][j].x_pos = enemy[i][j].launch_x;
						enemy[i][j].y_pos = enemy[i][j].launch_y;
						enemy[i][j].launched = true;
						(level->num_spawned)++;
						(level->curr_enemy_count)++;
						calcEnemyInc(&(enemy[i][j]));
						break;
					}
				}
			}
		}
	}

	if (level->spawnUfo) {
		if (level->curr_ufo_count < MAX_UFO && level->ufoNumSpawned < level->ufoSpawnLimit) {
			spawnTiming = rand() % level->ufoSpawnRate + 1;
			if (spawnTiming == 5) {
				for (i = 0; i < level->ufoSpawnLimit; i++) {
					if (!ufo[i].spawned && !ufo[i].arrived) {
						ufo[i].spawned = true;

						if (i == 0) {
							ufo[i].origin = level->ufoSpawnSide[rand() % 2]; //& 1
						}
						else {
							if (ufo[i - 1].origin == 0 - level->ufoSize.x)
								ufo[i].origin = 900;
							else if (ufo[i - 1].origin == 900)
								ufo[i].origin == 0 - level->ufoSize.x; 
						}
							
						ufo[i].pos.x = ufo[i].origin;
						ufo[i].pos.y = rand() % 100 + 50;
						(level->ufoNumSpawned)++;
						(level->curr_ufo_count)++;
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


void updateEnemy(Enemy ** enemy, Level * level, Ufo * ufo) {
	for (int i = 0; i < level->spawnLimit; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {

				if (enemy[i][j].dest_x > enemy[i][j].launch_x) {
					enemy[i][j].x_pos += (level->enemySpeed) * enemy[i][j].x_inc;
				}
				else if (enemy[i][j].dest_x < enemy[i][j].launch_x) {
					enemy[i][j].x_pos -= (level->enemySpeed) * enemy[i][j].x_inc;
				}

				enemy[i][j].y_pos += (level->enemySpeed) * enemy[i][j].y_inc;

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

	for (int i = 0; i < level->ufoSpawnLimit; i++) {
	
		if (ufo[i].spawned) {
			if (ufo[i].origin == 900)
				ufo[i].pos.x-=2; 
			else if (ufo[i].origin == -150) {
				ufo[i].pos.x+=2;

				ufo[i].topLeft.x = ufo[i].pos.x; 
				ufo[i].topLeft.y = ufo[i].pos.y;
				ufo[i].topRight.x = ufo[i].pos.x + level->ufoSize.x;
				ufo[i].topRight.y = ufo[i].pos.y; 
				ufo[i].bottomLeft.x = ufo[i].pos.x;
				ufo[i].bottomLeft.y = ufo[i].pos.y + level->ufoSize.y;

			}
		}
	}
}


void drawEnemy(Enemy ** enemy, int * theme, int colorMap[][3], Level * level, Ufo * ufo, ALLEGRO_BITMAP * imageUfo)
{
	int colorId = *theme;  //theme[0] 

	for (int i = 0; i < level->spawnLimit; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {
				al_draw_filled_rectangle(enemy[i][j].x_pos - 3, enemy[i][j].y_pos - 3, enemy[i][j].x_pos + 3, enemy[i][j].y_pos + 3, al_map_rgb(colorMap[colorId][R], colorMap[colorId][G], colorMap[colorId][B]));
				al_draw_line(enemy[i][j].x_pos, enemy[i][j].y_pos - 3, enemy[i][j].launch_x, enemy[i][j].launch_y, al_map_rgb(colorMap[colorId][R], colorMap[colorId][G], colorMap[colorId][B]), 4);
			}
		}
	}

	for (int i = 0; i < level->ufoSpawnLimit; i++) {
		if (ufo[i].spawned) {
			al_draw_bitmap(imageUfo, ufo[i].pos.x, ufo[i].pos.y, 0);
		}

	}
}


void enemyArrival(Enemy ** enemy, Level * level, Ufo * ufo) {
	for (int i = 0; i < level->spawnLimit; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {
				if (enemy[i][j].y_pos >= enemy[i][j].dest_y) {
					enemy[i][j].launched = false;
					enemy[i][j].arrived = true;

					(level->curr_enemy_count)--;
				}
			}
		}
	}

	for (int i = 0; i < level->ufoSpawnLimit; i++) {
		if(ufo[i].spawned) {
			if (ufo[i].origin == -150) {
				if (ufo[i].pos.x > 900) {
					ufo[i].arrived = true; 
					level->curr_ufo_count--; 
				}
			}
			else if (ufo[i].origin == 900) {
				if (ufo[i].pos.x < -150) {
					ufo[i].arrived = true;
					level->curr_ufo_count--; 
				}

			}

		}

	}
}
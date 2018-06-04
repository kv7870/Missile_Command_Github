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

void spawnEnemy(Enemy ** enemy, Level * level, Ufo * ufo, Bomb * bomb)
{
	int spawnTiming = 0;
	int ufoSpawnTiming = 0;
	int i, j;

	//spawn enemy missile
	if (level->curr_enemy_count < level->maxEnemyOnScreen && level->num_spawned < level->spawnLimit) {
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


	//spawn ufo
	if (level->spawnUfo) {

		if (level->curr_ufo_count < level->maxUfoOnScreen) {
			if (level->ufoNumSpawned < level->ufoSpawnLimit) {

				spawnTiming = rand() % level->ufoSpawnRate + 1;

				if (spawnTiming == 100) {

					for (i = 0; i < level->ufoSpawnLimit; i++) {

						if (!ufo[i].spawned) {
							if (!ufo[i].arrived) {
								ufo[i].spawned = true;

								if (i == 0) {
									ufo[i].origin = level->ufoSpawnSide[rand() % 2]; //& 1
								}

								else if (i > 0) {
									if (ufo[i - 1].origin == 0 - level->ufoSize.x) {
										ufo[i].origin = 900;
									}
									else if (ufo[i - 1].origin == 900) {
										ufo[i].origin = 0 - level->ufoSize.x;
									}
								}

								ufo[i].pos.x = ufo[i].origin;
								ufo[i].pos.y = rand() % 100 + 50;

								printf("ufo[%d].spawned: %d\n", i, ufo[i].spawned);
								printf("ufo[%d].pos.x: %d\n", i, ufo[i].pos.x);

								(level->ufoNumSpawned)++;
								(level->curr_ufo_count)++;
								break;
							}
						}
					}
				}
			}
		}

	}



	//spawn bomb
	if (level->spawnBomb) {
		if (level->curr_bomb_count < level->maxBombOnScreen && level->bombNumSpawned < level->bombSpawnLimit) {

			spawnTiming = rand() % level->bombSpawnRate + 1;

			//if(spawnTiming == rand%level->bombSpawnRate + 1); 
			if (spawnTiming == 10) {
				for (i = 0; i < level->bombSpawnLimit; i++) {
					if (!bomb[i].spawned && !bomb[i].arrived) {
						bomb[i].spawned = true;
						bomb[i].origin.x = rand() % 600 + 200;
						bomb[i].pos.x = bomb[i].origin.x;
						bomb[i].pos.y = bomb[i].origin.y;
						(level->curr_bomb_count)++;
						(level->bombNumSpawned)++;
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


void updateBomb(Level * level, Bomb * bomb, Explosion * explosion) {

	for (int i = 0; i < level->bombSpawnLimit; i++) {

		if (bomb[i].spawned) {	

			bomb[i].pos.y += 2; 
			//calculate bounds 
			bomb[i].topLeft.x = bomb[i].pos.x;
			bomb[i].topLeft.y = bomb[i].pos.y;
			bomb[i].topRight.x = bomb[i].pos.x + level->bombSize.x;
			bomb[i].topRight.y = bomb[i].pos.y;
			bomb[i].bottomLeft.x = bomb[i].pos.x;
			bomb[i].bottomLeft.y = bomb[i].pos.y + level->bombSize.y;

			for (int j = 0; j < ABM_COUNT; j++) {
				if (explosion[j].ongoing) {

					//CASE 1: explosion diameter < bomb (evade horizontally)
					if (explosion[j].radius * 2 < level->bombSize.y) {
						//check if on bomb and explosion are on same y range 
						if ((explosion[j].center.y + explosion[j].radius >= bomb[i].pos.y && explosion[j].center.y + explosion[j].radius <= bomb[i].pos.y + level->bombSize.y) ||
							explosion[j].center.y - explosion[j].radius >= bomb[i].pos.y && explosion[j].center.y - explosion[j].radius <= bomb[i].pos.y + level->bombSize.y) {

							horizontalEvasion(&(bomb[i]), &(explosion[j]), level); 
						}
					}



					//CASE 2: explosion diameter > bomb (evade horizontally) 
					else if (explosion[j].radius * 2 > level->bombSize.y) {

						//check if bomb and explosion are on same y plane
						if ( (bomb[i].pos.y + level->bombSize.y >= explosion[j].center.y - explosion[j].radius && bomb[i].pos.y <= explosion[j].center.y + explosion[j].radius) ||
							(bomb[i].pos.y >= explosion[j].center.y - explosion[j].radius && bomb[i].pos.y <= explosion[j].center.y + explosion[j].radius) ) {

							horizontalEvasion(&(bomb[i]), &(explosion[j]), level); 
						}
					}


					//CASE 3: explosion diameter > bomb (evade vertically)
					if (explosion[j].radius * 2 < level->bombSize.x) {
						//check if bomb and explosion are on same x plane
						if ((explosion[j].center.x + explosion[j].radius >= bomb[i].pos.x && explosion[j].center.x + explosion[j].radius <= bomb[i].pos.x + level->bombSize.x) ||
							(explosion[j].center.x - explosion[j].radius >= bomb[i].pos.x && explosion[j].center.x + explosion[j].radius <= bomb[i].pos.x + level->bombSize.x)) {
							verticalEvasion(&(bomb[i]), &(explosion[j]), level);
						}
					}


					//CASE 4: explosion diameter < bomb (evade vertically)
					else if (explosion[j].radius * 2 > level->bombSize.x) {
						//check if bomb and explosion are on same x plane
						if ((bomb[i].pos.x >= explosion[j].center.x - explosion[j].radius && bomb[i].pos.x <= explosion[j].center.x + explosion[j].radius) ||
							(bomb[i].pos.x + level->bombSize.x >= explosion[j].center.x - explosion[j].radius && bomb[i].pos.x <= explosion[j].center.x + explosion[j].radius)) {
							verticalEvasion(&(bomb[i]), &(explosion[j]), level);

						}
					}
				}
			}
		}
	}
}


void horizontalEvasion(Bomb * bomb, Explosion * explosion, Level * level) {
	//bomb is to left of explosion
	if ((bomb->pos.x + 0.5*level->bombSize.x) < explosion->center.x) {
		if (fabs((explosion->center.x - explosion->radius) - (bomb->pos.x + level->bombSize.x)) < 50) {   //move bomb if too close
			bomb->pos.x -= 3;
		}
	}

	//bomb is to right of explosion	
	else /*if ((bomb[i].pos.x + 0.5*level->bombSize.x) > explosion[j].center.x)*/ {
		if (fabs((explosion->center.x + explosion->radius) - (bomb->pos.x)) < 50) {   //move bomb if too close
			bomb->pos.x += 3;
		}
	}
}


void verticalEvasion(Bomb * bomb, Explosion * explosion, Level * level) {

	if (fabs((explosion->center.y + explosion->radius) - (bomb->pos.y + level->bombSize.y)) < 100) {
		bomb->pos.y -= 3;
	}
}



void updateEnemy(Enemy ** enemy, Level * level) {
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
}


void updateUfo(Ufo * ufo, Level * level) {

	for (int i = 0; i < level->ufoSpawnLimit; i++) {

		if (ufo[i].spawned) {
			if (ufo[i].origin == 900) {
				ufo[i].pos.x -= 2;
			}
			else if (ufo[i].origin == 0 - level->ufoSize.x) {
				ufo[i].pos.x += 2;
			}

			//calculate bounds
			ufo[i].topLeft.x = ufo[i].pos.x;
			ufo[i].topLeft.y = ufo[i].pos.y;
			ufo[i].topRight.x = ufo[i].pos.x + level->ufoSize.x;
			ufo[i].topRight.y = ufo[i].pos.y;
			ufo[i].bottomLeft.x = ufo[i].pos.x;
			ufo[i].bottomLeft.y = ufo[i].pos.y + level->ufoSize.y; 

		}
	}
}


void spawnUfoMissile(Ufo * ufo, Level * level) {
	int spawnTiming;
	int baseX[6] = { 145, 235, 325, 530, 630, 730 }; 

	spawnTiming = rand() % level->ufoMissileSpawnRate + 1;

	if (spawnTiming == 100) {

		for (int i = 0; i < level->ufoSpawnLimit; i++) {

			if (ufo[i].spawned) {

				for (int j = 0; j < 2; j++) {
					if (!ufo[i].missile[j].launched) {
						if (!ufo[i].missile[j].arrived) {
							ufo[i].missile[j].launched = true;
							ufo[i].missile[j].launch_x = ufo[i].pos.x + 0.5 * level->ufoSize.x;
							ufo[i].missile[j].launch_y = ufo[i].pos.y + level->ufoSize.y;
							ufo[i].missile[j].dest_x = baseX[(rand() % 6)];
							ufo[i].missile[j].x_pos = ufo[i].missile[j].launch_x;
							ufo[i].missile[j].y_pos = ufo[i].missile[j].launch_y;

							calcUfoMissileInc(&(ufo[i].missile[j]));
						}
					}
				}

				break;
			}
		}
	}
}


void calcUfoMissileInc(Enemy * missile) { 

		missile->dx = fabs(missile->dest_x - missile->launch_x);
		missile->dy = fabs(missile->dest_y - missile->launch_y);

		if (missile->dx >= missile->dy) {
			missile->step = missile->dx;
		}
		else {
			missile->step = missile->dy;
		}

		missile->x_inc = missile->dx / missile->step;
		missile->y_inc = missile->dy / missile->step;
}


void updateUfoMissile(Ufo * ufo, Level * level) {

	for (int i = 0; i < level->ufoSpawnLimit; i++) {
		if (ufo[i].spawned) {
			for (int j = 0; j < 2; j++) {
				if (ufo[i].missile[j].launched) {

					if (ufo[i].missile[j].dest_x > ufo[i].missile[j].launch_x) {
						ufo[i].missile[j].x_pos += (level->enemySpeed) * ufo[i].missile[j].x_inc;
					}
					else if (ufo[i].missile[j].dest_x < ufo[i].missile[j].launch_x) {
						ufo[i].missile[j].x_pos -= (level->enemySpeed) * ufo[i].missile[j].x_inc;
					}

					ufo[i].missile[j].y_pos += (level->enemySpeed) * ufo[i].missile[j].y_inc;

					//calculate bounds
					ufo[i].missile[j].topLeft.x = ufo[i].missile[j].x_pos - SIZE;
					ufo[i].missile[j].topLeft.y = ufo[i].missile[j].y_pos - SIZE;
					ufo[i].missile[j].topRight.x = ufo[i].missile[j].x_pos + SIZE;
					ufo[i].missile[j].topRight.y = ufo[i].missile[j].y_pos - SIZE;
					ufo[i].missile[j].bottomLeft.x = ufo[i].missile[j].x_pos - SIZE;
					ufo[i].missile[j].bottomLeft.y = ufo[i].missile[j].y_pos + SIZE;
				}
			}
		}
	}

}

void drawEnemy(Enemy ** enemy, int * theme, int colorMap[][3], Level * level, Ufo * ufo, ALLEGRO_BITMAP * imageUfo, ALLEGRO_BITMAP ** imageBomb, Bomb * bomb)
{
	int colorId = *theme;  //theme[0] 
	int i, j;

	for (i = 0; i < level->spawnLimit; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {
				al_draw_filled_rectangle(enemy[i][j].x_pos - 3, enemy[i][j].y_pos - 3, enemy[i][j].x_pos + 3, enemy[i][j].y_pos + 3, al_map_rgb(colorMap[colorId][R], colorMap[colorId][G], colorMap[colorId][B]));
				al_draw_line(enemy[i][j].x_pos, enemy[i][j].y_pos - 3, enemy[i][j].launch_x, enemy[i][j].launch_y, al_map_rgb(colorMap[colorId][R], colorMap[colorId][G], colorMap[colorId][B]), 4);
			}
		}
	}

	for (i = 0; i < level->ufoSpawnLimit; i++) {
		if (ufo[i].spawned) {
			al_draw_bitmap(imageUfo, ufo[i].pos.x, ufo[i].pos.y, 0);

			for (j = 0; j < 2; j++) {
				if (ufo[i].missile[j].launched) {
					al_draw_filled_rectangle(ufo[i].missile[j].x_pos - 3, ufo[i].missile[j].y_pos - 3, ufo[i].missile[j].x_pos + 3, ufo[i].missile[j].y_pos + 3, al_map_rgb(colorMap[colorId][R], colorMap[colorId][G], colorMap[colorId][B]));
					al_draw_line(ufo[i].missile[j].x_pos, ufo[i].missile[j].y_pos - 3, ufo[i].missile[j].launch_x, ufo[i].missile[j].launch_y, al_map_rgb(colorMap[colorId][R], colorMap[colorId][G], colorMap[colorId][B]), 4);
				}
			}
		}
	}

	for (i = 0; i < level->bombSpawnLimit; i++) {
		if (bomb[i].spawned) {
			al_draw_bitmap(imageBomb[rand() % 5], bomb[i].pos.x, bomb[i].pos.y, 0);
			//al_draw_circle(bomb[i].pos.x, bomb[i].pos.y, 50, al_map_rgb(255, 255, 255), 5); 
		}
	}

}


void enemyArrival(Enemy ** enemy, Level * level, Ufo * ufo, Bomb * bomb) {
	int i; 

	//enemy missiles 
	for (i = 0; i < level->spawnLimit; i++) {
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

	//ufo
	for (i = 0; i < level->ufoSpawnLimit; i++) {
		if(ufo[i].spawned) {
			if (ufo[i].origin == 0 - level->ufoSize.x) {
				if (ufo[i].pos.x > 900) {
					ufo[i].arrived = true; 
					ufo[i].spawned = false; 
					(level->curr_ufo_count)--; 
				}
			}
			else if (ufo[i].origin == 900) {
				if (ufo[i].pos.x < 0 - level->ufoSize.x) {
					ufo[i].arrived = true;
					ufo[i].spawned = false;
					(level->curr_ufo_count)--; 
				}

			}

		}
	}

	//bomb
	for (i = 0; i < level->bombSpawnLimit; i++) {
		if (bomb[i].spawned) {
			if (bomb[i].pos.y >= 900) {
				bomb[i].spawned = false;
				bomb[i].arrived = true;
			}
		}
	}
}
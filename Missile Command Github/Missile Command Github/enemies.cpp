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
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

void spawnEnemy(Enemy ** enemy, Level * level, Ufo * ufo, Bomb * bomb, Base * base)
{
	int spawnTiming = 0;
	int ufoSpawnTiming = 0;
	int i, j;

	//spawn enemy missile
	for (i = 0; i < level->maxEnemyOnScreen; i++) {
		if (level->num_spawned < level->spawnLimit) {
			if (!enemy[i][0].launched) {  //original missile 

				spawnTiming = rand() % level->spawnRate;
				//(rand()%level->spawnRate) > 14 && rand()%level->spawnRate < 20)
				if ((spawnTiming > level->spawnRangeMin && spawnTiming < level->spawnRangeMax) || (level->abmLeft == 0)) {
					enemy[i][0].launch.x = rand() % 861 + 20;
					enemy[i][0].launch.y = 50;
					enemy[i][0].dest.x = rand() % 801 + 50;
					enemy[i][0].pos.x = enemy[i][0].launch.x;
					enemy[i][0].pos.y = enemy[i][0].launch.y;
					enemy[i][0].launched = true;
					(level->num_spawned)++;
					calcEnemyInc(&(enemy[i][0]));
					break;  //prevents all enemies from spawning at once
				}
			}


			for (j = 1; j < SPLIT_COUNT; j++) {
				if (level->num_spawned < level->spawnLimit) {
					spawnTiming = rand() % level->splitRate;
					if (spawnTiming > level->splitRangeMin && spawnTiming <level->splitRangeMax) {

						if (enemy[i][j - 1].launched && !enemy[i][j].launched) {
							if (enemy[i][0].pos.y < 700) {
								enemy[i][j].launch.x = enemy[i][j - 1].pos.x;
								enemy[i][j].launch.y = enemy[i][j - 1].pos.y;
								enemy[i][j].dest.x = enemy[i][j - 1].dest.x + (rand() % level->splitAngle + 50);
								enemy[i][j].pos.x = enemy[i][j].launch.x;
								enemy[i][j].pos.y = enemy[i][j].launch.y;
								enemy[i][j].launched = true;
								(level->num_spawned)++;
								calcEnemyInc(&(enemy[i][j]));
								break;
							}
						}
					}
				}
			}
		}
	}


	//spawn ufo
	if (level->spawnUfo) {
		if (level->ufoNumSpawned < level->ufoSpawnLimit) {

			spawnTiming = rand() % level->ufoSpawnRate + 1;

			if (spawnTiming > level->ufoSpawnRangeMin && spawnTiming < level->ufoSpawnRangeMax) {

				for (i = 0; i < level->maxUfoOnScreen; i++) {

					if (!ufo[i].spawned) {
						ufo[i].spawned = true;

						if (i == 0) {
							ufo[i].origin = level->ufoSpawnSide[rand() % 2]; //& 1
						}

						else if (i > 0) {
							if (ufo[i - 1].origin == level->ufoSpawnSide[LEFT]) {
								ufo[i].origin = level->ufoSpawnSide[RIGHT];
							}
							else if (ufo[i - 1].origin == level->ufoSpawnSide[RIGHT]) {
								ufo[i].origin = level->ufoSpawnSide[LEFT];
							}
						}

						ufo[i].pos.x = ufo[i].origin;
						ufo[i].pos.y = rand() % 100 + 50;

						(level->ufoNumSpawned)++;
						break;
					}
				}
			}
		}
	}


	//spawn bomb
	if (level->spawnBomb) {
		if (level->bombNumSpawned < level->bombSpawnLimit) {
			for (i = 0; i < level->bombSpawnLimit; i++) {

				if ((rand() % level->bombSpawnRate + 1) == 10) {
					if (!bomb[i].spawned) {
						bomb[i].spawned = true;
						bomb[i].origin.x = level->baseX[pickTarget(base)];
						bomb[i].pos.x = bomb[i].origin.x;
						bomb[i].pos.y = bomb[i].origin.y;
						(level->bombNumSpawned)++;
						break;
					}
				}
			}
		}
	}
}

//recursion
int pickTarget(Base * base) {
	int target = rand() % 6;

	if (base[target].destroyed)
		pickTarget(base);

	else
		return target;
}


void calcEnemyInc(Enemy * enemy) {
	enemy->dx = fabs(enemy->dest.x - enemy->launch.x);
	enemy->dy = fabs(enemy->dest.y - enemy->launch.y);

	if (enemy->dx >= enemy->dy) {
		enemy->step = enemy->dx;
	}
	else {
		enemy->step = enemy->dy;
	}

	enemy->inc.x = enemy->dx / enemy->step;
	enemy->inc.y = enemy->dy / enemy->step;
}


void updateBomb(Level * level, Bomb * bomb, Explosion * explosion) {

	for (int i = 0; i < level->bombSpawnLimit; i++) {

		if (bomb[i].spawned) {
			bomb[i].timerCount++; 

			if (level->abmLeft == 0)
				bomb[i].pos.y += 6;
			else 
				bomb[i].pos.y += 2;

			if (bomb[i].timerCount <= 5) {
				if (bomb[i].moveLeft) {
					bomb[i].pos.x -= 2.5;
				}
				else {
					bomb[i].pos.x += 2.5;
			
				}

				if (bomb[i].timerCount == 5) {
					bomb[i].timerCount = 0;
					if (bomb[i].moveLeft)
						bomb[i].moveLeft = false;
					else
						bomb[i].moveLeft = true;
				}
			}
				

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
						if ((bomb[i].pos.y + level->bombSize.y >= explosion[j].center.y - explosion[j].radius && bomb[i].pos.y <= explosion[j].center.y + explosion[j].radius) ||
							(bomb[i].pos.y >= explosion[j].center.y - explosion[j].radius && bomb[i].pos.y <= explosion[j].center.y + explosion[j].radius)) {

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
	for (int i = 0; i < level->maxEnemyOnScreen; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {

			if (enemy[i][j].launched) {
				if (enemy[i][j].dest.x > enemy[i][j].launch.x) {
					if (level->abmLeft == 0)
						enemy[i][j].pos.x += 6 * enemy[i][j].inc.x;
					else
						enemy[i][j].pos.x += (level->enemySpeed) * enemy[i][j].inc.x;
				}
				else if (enemy[i][j].dest.x < enemy[i][j].launch.x) {
					if (level->abmLeft == 0)
						enemy[i][j].pos.x -= 6 * enemy[i][j].inc.x;
					else
						enemy[i][j].pos.x -= (level->enemySpeed) * enemy[i][j].inc.x;
				}


				if (level->abmLeft == 0)
					enemy[i][j].pos.y += 6 * enemy[i][j].inc.y;
				else
					enemy[i][j].pos.y += (level->enemySpeed) * enemy[i][j].inc.y;

				//calculate bounds
				enemy[i][j].topLeft.x = enemy[i][j].pos.x - SIZE;
				enemy[i][j].topLeft.y = enemy[i][j].pos.y - SIZE;
				enemy[i][j].topRight.x = enemy[i][j].pos.x + SIZE;
				enemy[i][j].topRight.y = enemy[i][j].pos.y - SIZE;
				enemy[i][j].bottomLeft.x = enemy[i][j].pos.x - SIZE;
				enemy[i][j].bottomLeft.y = enemy[i][j].pos.y + SIZE;
			}
		}
	}
}


void updateUfo(Ufo * ufo, Level * level) {

	for (int i = 0; i < level->ufoSpawnLimit; i++) {

		if (ufo[i].spawned) {
			if (ufo[i].origin == 900) {
				if (level->abmLeft == 0)
					ufo[i].pos.x -= 6;
				else
					ufo[i].pos.x -= 2;
			}

			else if (ufo[i].origin == 0 - level->ufoSize.x) {
				if (level->abmLeft == 0)
					ufo[i].pos.x += 6;
				else
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

	for (int i = 0; i < level->maxUfoOnScreen; i++) {
		if (ufo[i].spawned) {

			spawnTiming = rand() % level->ufoMissileSpawnRate + 1;
			if (spawnTiming > 50 && spawnTiming < 70) {

				for (int j = 0; j < 2; j++) {
					if (!ufo[i].missile[j].launched) {
						if (level->num_spawned < level->spawnLimit) {

							if (ufo[i].pos.x > 100 && ufo[i].pos.x < 800) {
								ufo[i].missile[j].launched = true;


								if (ufo[i].origin == level->ufoSpawnSide[LEFT])
									ufo[i].missile[j].launch.x = ufo[i].pos.x + 0.6 * level->ufoSize.x;
								else
									ufo[i].missile[j].launch.x = 1.1 * ufo[i].pos.x;

								//ufo[i].missile[j].launch.x = ufo[i].pos.x + 0.5 * level->ufoSize.x;
								ufo[i].missile[j].launch.y = ufo[i].pos.y + level->ufoSize.y;
								ufo[i].missile[j].dest.x = baseX[(rand() % 6)];
								ufo[i].missile[j].pos.x = ufo[i].missile[j].launch.x;
								ufo[i].missile[j].pos.y = ufo[i].missile[j].launch.y;

								level->num_spawned++;

								calcUfoMissileInc(&(ufo[i].missile[j]));
							}
						}
					}
				}
				//break;
			}
		}
	}
}



void calcUfoMissileInc(Enemy * missile) {

	missile->dx = fabs(missile->dest.x - missile->launch.x);
	missile->dy = fabs(missile->dest.y - missile->launch.y);

	if (missile->dx >= missile->dy) {
		missile->step = missile->dx;
	}
	else {
		missile->step = missile->dy;
	}

	missile->inc.x = missile->dx / missile->step;
	missile->inc.y = missile->dy / missile->step;
}


void updateUfoMissile(Ufo * ufo, Level * level) {

	for (int i = 0; i < level->maxUfoOnScreen; i++) {
		for (int j = 0; j < 2; j++) {
			if (ufo[i].missile[j].launched) {

				if (ufo[i].missile[j].dest.x > ufo[i].missile[j].launch.x) {
					if (level->abmLeft == 0)
						ufo[i].missile[j].pos.x += 6 * ufo[i].missile[j].inc.x;
					else
						ufo[i].missile[j].pos.x += (level->enemySpeed) * ufo[i].missile[j].inc.x;
				}
				else if (ufo[i].missile[j].dest.x < ufo[i].missile[j].launch.x) {
					if (level->abmLeft == 0)
						ufo[i].missile[j].pos.x -= 6 * ufo[i].missile[j].inc.x;
					else
						ufo[i].missile[j].pos.x -= (level->enemySpeed) * ufo[i].missile[j].inc.x;
				}

				if (level->abmLeft == 0)
					ufo[i].missile[j].pos.y += 6 * ufo[i].missile[j].inc.y;
				else
					ufo[i].missile[j].pos.y += (level->enemySpeed) * ufo[i].missile[j].inc.y;

				//calculate bounds
				ufo[i].missile[j].topLeft.x = ufo[i].missile[j].pos.x - SIZE;
				ufo[i].missile[j].topLeft.y = ufo[i].missile[j].pos.y - SIZE;
				ufo[i].missile[j].topRight.x = ufo[i].missile[j].pos.x + SIZE;
				ufo[i].missile[j].topRight.y = ufo[i].missile[j].pos.y - SIZE;
				ufo[i].missile[j].bottomLeft.x = ufo[i].missile[j].pos.x - SIZE;
				ufo[i].missile[j].bottomLeft.y = ufo[i].missile[j].pos.y + SIZE;
			}
		}
	}
}

void drawEnemy(Enemy ** enemy, int * theme, int colorMap[][3], Level * level, Ufo * ufo, ALLEGRO_BITMAP * imageUfo, ALLEGRO_BITMAP ** imageBomb, Bomb * bomb)
{
	int enemyColour= theme[1]; 
	int ufoMissileColour = theme[2];

	int i, j;

	for (i = 0; i < level->maxEnemyOnScreen; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {
				al_draw_filled_rectangle(enemy[i][j].pos.x - 3, enemy[i][j].pos.y - 3, enemy[i][j].pos.x + 3, enemy[i][j].pos.y + 3, al_map_rgb(colorMap[enemyColour][R], colorMap[enemyColour][G], colorMap[enemyColour][B]));
				al_draw_line(enemy[i][j].pos.x, enemy[i][j].pos.y - 3, enemy[i][j].launch.x, enemy[i][j].launch.y, al_map_rgb(colorMap[enemyColour][R], colorMap[enemyColour][G], colorMap[enemyColour][B]), 4);
			}
		}
	}

	for (i = 0; i < level->maxUfoOnScreen; i++) {
		if (ufo[i].spawned) {
			al_draw_bitmap(imageUfo, ufo[i].pos.x, ufo[i].pos.y, 0);
		}

		for (j = 0; j < 2; j++) {
			if (ufo[i].missile[j].launched) {
				al_draw_filled_rectangle(ufo[i].missile[j].pos.x - 3, ufo[i].missile[j].pos.y - 3, ufo[i].missile[j].pos.x + 3, ufo[i].missile[j].pos.y + 3, al_map_rgb(colorMap[ufoMissileColour][R], colorMap[ufoMissileColour][G], colorMap[ufoMissileColour][B]));
				al_draw_line(ufo[i].missile[j].pos.x, ufo[i].missile[j].pos.y - 3, ufo[i].missile[j].launch.x, ufo[i].missile[j].launch.y, al_map_rgb(colorMap[ufoMissileColour][R], colorMap[ufoMissileColour][G], colorMap[ufoMissileColour][B]), 4);
			}
		}
	}

	for (i = 0; i < level->maxBombOnScreen; i++) {
		if (bomb[i].spawned) {
			al_draw_bitmap(imageBomb[rand() % 5], bomb[i].pos.x, bomb[i].pos.y, 0);
		}
	}
}


void enemyArrival(Enemy ** enemy, Level * level, Ufo * ufo, Bomb * bomb) {
	int i, j;

	//enemy missiles 
	for (i = 0; i < level->maxEnemyOnScreen; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {
				if (enemy[i][j].pos.y >= enemy[i][j].dest.y) {
					enemy[i][j].launched = false;
				}
			}
		}
	}

	//ufo
	for (i = 0; i < level->maxUfoOnScreen; i++) {
		if (ufo[i].spawned) {
			if (ufo[i].origin == 0 - level->ufoSize.x) {
				if (ufo[i].pos.x > 900) {
					ufo[i].spawned = false;
				}
			}
			else if (ufo[i].origin == 900) {
				if (ufo[i].pos.x < 0 - level->ufoSize.x) {
					ufo[i].spawned = false;
				}

			}
		}

		//ufo missiles
		for (j = 0; j < 2; j++) {
			if (ufo[i].missile[j].pos.y >= 900) {
				ufo[i].missile[j].launched = false;
			}
		}
	}

	//bomb
	for (i = 0; i < level->maxBombOnScreen; i++) {
		if (bomb[i].spawned) {
			if (bomb[i].pos.y >= 900) {
				bomb[i].spawned = false;
			}
		}
	}
}

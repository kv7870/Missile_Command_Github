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

//spawn enemy missiles, ufos, and bombs
void spawnEnemy(Enemy ** enemy, Level * level, Ufo * ufo, Bomb * bomb, Base * base) {
	int spawnTiming = 0;	//determines whether to spawn enemy
	int ufoSpawnTiming = 0;
	int i, j;

	//spawn enemy missile
	for (i = 0; i < level->maxEnemyOnScreen; i++) {
		if (level->num_spawned < level->spawnLimit) {
			if (!enemy[i][0].launched) {  //original missile 

				spawnTiming = rand() % level->spawnRate;

				//spawn new missile branch if previous missile is launched 
				if ((spawnTiming > level->spawnRangeMin && spawnTiming < level->spawnRangeMax) || (level->abmLeft == 0)) {
					enemy[i][0].launch.x = rand() % 861 + 20;	//enemy[i][0] is the main missile from which 3 more can branch 
					enemy[i][0].launch.y = 50;
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

			//spawn branching missiles 
			for (j = 1; j < SPLIT_COUNT; j++) {
				if (level->num_spawned < level->spawnLimit) {
					spawnTiming = rand() % level->splitRate;
					if (spawnTiming > level->splitRangeMin && spawnTiming < level->splitRangeMax) {

						if (enemy[i][j - 1].launched && !enemy[i][j].launched) {
							if (enemy[i][j - 1].pos.y < 500) {
								enemy[i][j].launch.x = enemy[i][j - 1].pos.x;	//new missile branches off from the previous missile
								enemy[i][j].launch.y = enemy[i][j - 1].pos.y;
								enemy[i][j].dest.x = enemy[i][j - 1].dest.x + (rand() % level->splitAngle + 50);
								enemy[i][j].pos.x = enemy[i][j].launch.x;
								enemy[i][j].pos.y = enemy[i][j].launch.y;
								enemy[i][j].launched = true;
								(level->num_spawned)++;
								calcEnemyInc(&(enemy[i][j]));	//calculate trajectory 
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

			//spawn ufo is random number generated falls within certain range 
			if (spawnTiming > level->ufoSpawnRangeMin && spawnTiming < level->ufoSpawnRangeMax) {

				//the first ufo can spawn on the right or left side of the screen 
				for (i = 0; i < level->maxUfoOnScreen; i++) {

					if (!ufo[i].spawned) {
						ufo[i].spawned = true;

						//the first ufo can spawn on the right or left side of the screen 
						if (i == 0) {
							ufo[i].origin = level->ufoSpawnSide[rand() % 2]; //& 1
						}

						//the next ufo spawns on the opposite side of the screen as the previous ufo 
						else if (i > 0) {
							if (ufo[i - 1].origin == level->ufoSpawnSide[LEFT]) {
								ufo[i].origin = level->ufoSpawnSide[RIGHT];
							}
							else if (ufo[i - 1].origin == level->ufoSpawnSide[RIGHT]) {
								ufo[i].origin = level->ufoSpawnSide[LEFT];
							}
						}

						ufo[i].pos.x = ufo[i].origin;
						ufo[i].pos.y = rand() % 100 + 50;	//random y-spawn 

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
			for (i = 0; i < level->maxBombOnScreen; i++) {

				if ((rand() % level->bombSpawnRate + 1) == 10) {
					if (!bomb[i].spawned) {
						bomb[i].spawned = true;
						bomb[i].origin.x = level->baseX[pickTarget(base)];	//bomb targets a randomly selected base; since bomb travels in straight line, its origin.x = its target.x
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


//randomly select a standing base for bomb to target; if selected base is destroyed, the function calls itself until a standing base is picked 
int pickTarget(Base * base) {
	int target = rand() % 6;

	if (base[target].destroyed)
		pickTarget(base);	//recursion

	else
		return target;
}


//calculate trajectory enemy missile must take using digital differential analyzer algorithm 
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


//update position of bomb
void updateBomb(Level * level, Bomb * bomb, Explosion * explosion) {

	for (int i = 0; i < level->maxBombOnScreen; i++) {

		if (bomb[i].spawned) {
			bomb[i].timerCount++;	//keeps track of how many timer ticks have elapsed (i.e. frames)

			if (level->abmLeft == 0)	//if ABMs are out, speed up bomb to finish level quicker 
				bomb[i].pos.y += 6;
			else
				bomb[i].pos.y += 2;

			if (bomb[i].timerCount <= 5) {	//bomb moves left or right for 5 frames to create zigzaing effect 
				if (bomb[i].moveLeft) {
					bomb[i].pos.x -= 2.5;
				}
				else {
					bomb[i].pos.x += 2.5;

				}

				if (bomb[i].timerCount == 5) {
					bomb[i].timerCount = 0;
					if (bomb[i].moveLeft)
						bomb[i].moveLeft = false;	//if bomb moved left previously, it will now move right 
					else
						bomb[i].moveLeft = true;	//if bomb moved right previously, it will now move left
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

					//calculate which direction bomb should move to dodge nearby explosion

					//CASE 1: explosion diameter < bomb (evade horizontally)
					if (explosion[j].radius * 2 < level->bombSize.y) {
						//check if on bomb and explosion are on same y plane
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
							//if(bomb[i].bottomLeft.y > explosion[i].center.y)	//if bomb is above an explosion
							verticalEvasion(&(bomb[i]), &(explosion[j]), level);
						}
					}


					//CASE 4: explosion diameter < bomb (evade vertically)
					else if (explosion[j].radius * 2 > level->bombSize.x) {
						//check if bomb and explosion are on same x plane
						if ((bomb[i].pos.x >= explosion[j].center.x - explosion[j].radius && bomb[i].pos.x <= explosion[j].center.x + explosion[j].radius) ||
							(bomb[i].pos.x + level->bombSize.x >= explosion[j].center.x - explosion[j].radius && bomb[i].pos.x <= explosion[j].center.x + explosion[j].radius)) {
							//if (bomb[i].bottomLeft.y > explosion[i].center.y)	//if bomb is above an explosion
							verticalEvasion(&(bomb[i]), &(explosion[j]), level);

						}
					}
				}
			}
		}
	}
}


//calculate if bomb should move horizontally to to evade nearby explosion 
void horizontalEvasion(Bomb * bomb, Explosion * explosion, Level * level) {
	//if bomb is to left of an explosion (and on same y plane)
	if ((bomb->pos.x + 0.5*level->bombSize.x) < explosion->center.x) {
		//bomb dodge left if an explosion is within 50 pixels right of bomb
		if (fabs((explosion->center.x - explosion->radius) - (bomb->pos.x + level->bombSize.x)) < 50) {   //move bomb if too close
			bomb->pos.x -= 3;
		}
	}

	//if bomb is to right of explosion (and on same y plane)
	else /*if ((bomb[i].pos.x + 0.5*level->bombSize.x) > explosion[j].center.x)*/ {
		//bomb dodge right if an explosion is within 50 pixels left of bomb 
		if (fabs((explosion->center.x + explosion->radius) - (bomb->pos.x)) < 50) {   //move bomb if too close
			bomb->pos.x += 3;
		}
	}
}


//calculate if bomb should move upward to evade an explosion below bomb
void verticalEvasion(Bomb * bomb, Explosion * explosion, Level * level) {

	if (bomb->bottomLeft.y < explosion->center.y) {

		//bomb move upward if an explosion is within 100 pixels below the bomb (and on the same x range) 
		if (fabs((explosion->center.y + explosion->radius) - (bomb->pos.y + level->bombSize.y)) < 100) {
			bomb->pos.y -= 3;
		}
	}
}


//update position of enemies 
void updateEnemy(Enemy ** enemy, Level * level) {
	for (int i = 0; i < level->maxEnemyOnScreen; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {

			if (enemy[i][j].launched) {
				if (enemy[i][j].dest.x > enemy[i][j].launch.x) {	//enemy missile move left by predetermined increment if its destination.x is greater than its launch.x
					if (level->abmLeft == 0)	//speed up missile if player ran out of ABMs to finish level quicker
						enemy[i][j].pos.x += 6 * enemy[i][j].inc.x;
					else
						enemy[i][j].pos.x += (level->enemySpeed) * enemy[i][j].inc.x;
				}
				else if (enemy[i][j].dest.x < enemy[i][j].launch.x) {	//enemy missile move right by predetermined increment if its destination.x is smaller than its launch.x
					if (level->abmLeft == 0)
						enemy[i][j].pos.x -= 6 * enemy[i][j].inc.x;
					else
						enemy[i][j].pos.x -= (level->enemySpeed) * enemy[i][j].inc.x;
				}


				if (level->abmLeft == 0)	//speed up missile if player ran out of ABMs
					enemy[i][j].pos.y += 6 * enemy[i][j].inc.y;	//increase y-coordinate by pre-determined increment 
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


//update position of flying saucer 
void updateUfo(Ufo * ufo, Level * level) {

	for (int i = 0; i < level->maxUfoOnScreen; i++) {

		if (ufo[i].spawned) {
			if (ufo[i].origin == 900) {		//if ufo spawned on right, move left
				if (level->abmLeft == 0)	//speed up if out of ABMs
					ufo[i].pos.x -= 6;
				else
					ufo[i].pos.x -= 2;
			}

			else if (ufo[i].origin == 0 - level->ufoSize.x) {	//if ufo spawned on left, move right
				if (level->abmLeft == 0)	//speed up if out of ABMs
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


//spawn cruise missile (fired by flying saucer, aka ufo) 
void spawnUfoMissile(Ufo * ufo, Level * level) {
	int spawnTiming;
	int baseX[6] = { 145, 235, 325, 530, 630, 730 };	//x-coordinates of bases which cruise missiles target

	for (int i = 0; i < level->maxUfoOnScreen; i++) {
		if (ufo[i].spawned) {	//cruise missile fired from ufo, so can't spawn it if ufo hasn't spawned 

			spawnTiming = rand() % level->ufoMissileSpawnRate + 1;

			//fire cruise missile if random number falls within certain range 
			if (spawnTiming > 50 && spawnTiming < 70) {

				for (int j = 0; j < 2; j++) {
					if (!ufo[i].missile[j].launched) {

						//ufo can only fire cruise missile if the level's missile spawn limit hasn't been exceeded 
						if (level->num_spawned < level->spawnLimit) {

							//ufo can only fire cruise missile if it's in middile of screen 
							if (ufo[i].pos.x > 100 && ufo[i].pos.x < 800) {
								ufo[i].missile[j].launched = true;

								//for asthetics 
								if (ufo[i].origin == level->ufoSpawnSide[LEFT])
									ufo[i].missile[j].launch.x = ufo[i].pos.x + 0.6 * level->ufoSize.x;
								else
									ufo[i].missile[j].launch.x = 1.1 * ufo[i].pos.x;

								//ufo[i].missile[j].launch.x = ufo[i].pos.x + 0.5 * level->ufoSize.x;
								ufo[i].missile[j].launch.y = ufo[i].pos.y + level->ufoSize.y;
								ufo[i].missile[j].dest.x = baseX[(rand() % 6)];	//select a base for cruise missile to target
								ufo[i].missile[j].pos.x = ufo[i].missile[j].launch.x;
								ufo[i].missile[j].pos.y = ufo[i].missile[j].launch.y;

								level->num_spawned++;

								calcUfoMissileInc(&(ufo[i].missile[j]));	//calculate cruise missile's trajectory 
							}
						}
					}
				}
				//break;
			}
		}
	}
}


//calculate trajectory of cruise missile (fired by ufo) 
void calcUfoMissileInc(Enemy * missile) {

	//same digital differential analyzer algorithm 
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


//update position of cruise missile
void updateUfoMissile(Ufo * ufo, Level * level) {

	for (int i = 0; i < level->maxUfoOnScreen; i++) {
		for (int j = 0; j < 2; j++) {
			if (ufo[i].missile[j].launched) {

				if (ufo[i].missile[j].dest.x > ufo[i].missile[j].launch.x) {
					if (level->abmLeft == 0)	//speed up if out of ABMs
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

				if (level->abmLeft == 0)	//speed up if out of ABMs
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


//draw enemy missiles & flying saucer
void drawEnemy(Enemy ** enemy, int * theme, int colorMap[][3], Level * level, Ufo * ufo, ALLEGRO_BITMAP * imageUfo, ALLEGRO_BITMAP ** imageBomb, Bomb * bomb) {
	int enemyColour = theme[1];			//colour of regular enemym missile
	int ufoMissileColour = theme[2];	//colour of cruise missile

	int i, j;

	//draw regular enemy missiles
	for (i = 0; i < level->maxEnemyOnScreen; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {
				al_draw_filled_rectangle(enemy[i][j].pos.x - 3, enemy[i][j].pos.y - 3, enemy[i][j].pos.x + 3, enemy[i][j].pos.y + 3, al_map_rgb(colorMap[enemyColour][R], colorMap[enemyColour][G], colorMap[enemyColour][B]));
				al_draw_line(enemy[i][j].pos.x, enemy[i][j].pos.y - 3, enemy[i][j].launch.x, enemy[i][j].launch.y, al_map_rgb(colorMap[enemyColour][R], colorMap[enemyColour][G], colorMap[enemyColour][B]), 4);
			}
		}
	}

	//draw flying saucers
	for (i = 0; i < level->maxUfoOnScreen; i++) {
		if (ufo[i].spawned) {
			al_draw_bitmap(imageUfo, ufo[i].pos.x, ufo[i].pos.y, 0);
		}

		//draw cruise missiles fired by flying saucers
		for (j = 0; j < 2; j++) {
			if (ufo[i].missile[j].launched) {
				al_draw_filled_rectangle(ufo[i].missile[j].pos.x - 3, ufo[i].missile[j].pos.y - 3, ufo[i].missile[j].pos.x + 3, ufo[i].missile[j].pos.y + 3, al_map_rgb(colorMap[ufoMissileColour][R], colorMap[ufoMissileColour][G], colorMap[ufoMissileColour][B]));
				al_draw_line(ufo[i].missile[j].pos.x, ufo[i].missile[j].pos.y - 3, ufo[i].missile[j].launch.x, ufo[i].missile[j].launch.y, al_map_rgb(colorMap[ufoMissileColour][R], colorMap[ufoMissileColour][G], colorMap[ufoMissileColour][B]), 4);
			}
		}
	}

	//draw smart cruise missile
	for (i = 0; i < level->maxBombOnScreen; i++) {
		if (bomb[i].spawned) {
			al_draw_bitmap(imageBomb[rand() % 5], bomb[i].pos.x, bomb[i].pos.y, 0);
		}
	}
}


//calculate if enemy has arrived 
void enemyArrival(Enemy ** enemy, Level * level, Ufo * ufo, Bomb * bomb) {
	int i, j;

	//regular missiles 
	for (i = 0; i < level->maxEnemyOnScreen; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {
				if (enemy[i][j].pos.y >= enemy[i][j].dest.y) {	//missile disappears when it reaches bottom of screen 
					enemy[i][j].launched = false;
				}
			}
		}
	}

	//flying saucer (ufo)
	for (i = 0; i < level->maxUfoOnScreen; i++) {
		if (ufo[i].spawned) {
			if (ufo[i].origin == 0 - level->ufoSize.x) {	//if ufo spawned left of screen, it disappears when it reaches right side 
				if (ufo[i].pos.x > 900) {
					ufo[i].spawned = false;
				}
			}
			else if (ufo[i].origin == 900) {	//if ufo spawned on right of screen, it disappears when it reaches left side 
				if (ufo[i].pos.x < 0 - level->ufoSize.x) {
					ufo[i].spawned = false;
				}

			}
		}

		//cruise missiles (fired by ufos)
		for (j = 0; j < 2; j++) {
			if (ufo[i].missile[j].pos.y >= 900) {	//cruise missile disappears when it reaches bottom of screen
				ufo[i].missile[j].launched = false;
			}
		}
	}

	//smart cruise missile 
	for (i = 0; i < level->maxBombOnScreen; i++) {
		if (bomb[i].spawned) {
			if (bomb[i].pos.y >= 900) {		//smart cruise missile disappears when it reaches bottom of screen
				bomb[i].spawned = false;
			}
		}
	}
}
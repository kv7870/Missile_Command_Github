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


void hitDetection(struct abmData * abm, Enemy ** enemy, Explosion * explosion, Level * level, Ufo * ufo) {

	//enemy is...
	bool left = false;
	bool right = false;
	bool below = false;
	bool above = false;
	Vector distance = { 0, 0 };
	Vector clamp = { 0, 0 };

	//distance.x = 900;
	//distance.y = 900;
	//clamp.x = 0;
	//clamp.y = 0; 

	for (int i = 0; i < ABM_COUNT; i++) {

		if (explosion[i].ongoing) {

			for (int j = 0; j < level->spawnLimit; j++) {
				for (int k = 0; k < SPLIT_COUNT; k++) {
					if (enemy[j][k].launched) {

						//check bounds
						//if (explosion[i].topRight.x >= enemy[j][k].topLeft.x &&
						//	explosion[i].topLeft.x <= enemy[j][k].topRight.x &&
						//	explosion[i].bottomLeft.y >= enemy[j][k].topLeft.y &&
						//	explosion[i].topLeft.y <= enemy[j][k].bottomLeft.y) {

						clampSquare(&(explosion[i]), &(enemy[j][k]), &clamp);

						calcDistance(distance, explosion[i], clamp);

						if (calcDistance(distance, explosion[i], clamp) == true) {
							enemy[j][k].launched = false;
							//enemy[j][k].arrived = true; 
							explosion[i].expandedRadius = true;
							//explosion[i].increaseRadius = true;
							//explosion[i].center.x = enemy[j][k].x_pos;
							//explosion[i] .center.y = enemy[j][k].y_pos;

							level->score += 25;
							(level->curr_enemy_count)--;
						}
					}
				}
			}


			for (int j = 0; j < level->ufoSpawnLimit; j++) {
				if (ufo[j].spawned) {
					if (explosion[i].topRight.x >= ufo[j].topLeft.x &&
						explosion[i].topLeft.x <= ufo[j].topRight.x &&
						explosion[i].bottomLeft.y >= ufo[j].topLeft.y &&
						explosion[i].topLeft.y <= ufo[j].bottomLeft.y) {

						ufo[j].spawned = false;
						ufo[j].arrived = true; 
						explosion[i].expandedRadius = true; 

						level->score += 100;
						(level->curr_ufo_count)--;
					}

					for (int k = 0; k < 2; k++) {
						if (ufo[j].missile[k].launched) {

							clampSquare(&(explosion[i]), &(ufo[j].missile[k]), &clamp);

							if (calcDistance(distance, explosion[i], clamp) == true) {
								ufo[j].missile[k].launched = false;
								ufo[j].missile[k].arrived = true; 
								explosion[i].expandedRadius = true;
								//explosion[i].increaseRadius = true;
								//explosion[i].center.x = enemy[j][k].x_pos;
								//explosion[i].center.y = enemy[j][k].y_pos;

								level->score += 25;
							}
						}
					}
				}
			}
		}
	}
}


bool calcDistance(Vector distance, Explosion explosion, Vector clamp) {
	bool collided = false;

	distance.x = fabs(explosion.center.x - clamp.x);
	distance.y = fabs(explosion.center.y - clamp.y);

	if (pow(distance.x, 2) + pow(distance.y, 2) <= pow(explosion.radius, 2))
		collided = true; 

	return collided; 
}

void clampSquare(Explosion * explosion, Enemy * enemy, Vector * clamp) {

	//clamp x
	if (explosion->center.x < enemy->topLeft.x) {
		clamp->x = enemy->topLeft.x;
	}
	else if (explosion->center.x > enemy->topRight.x) {
		clamp->x = enemy->topRight.x; 
	}
	else {
		clamp->x = explosion->center.x; 
	}

	//clamp y
	if (explosion->center.y < enemy->topLeft.y) {
		clamp->x = enemy->topLeft.y;
	}
	else if (explosion->center.y > enemy->bottomLeft.y) {
		clamp->y = enemy->bottomLeft.y;
	}
	else {
		clamp->y = explosion->center.y;
	}
}


void bombHitDetection(Bomb * bomb, Explosion * explosion, Level * level) {
	int i, j;
	const double alpha = 0.25* 3.14159265359; 
	Vector clamp = { 0, 0 }; 
	Vector distance = { 0, 0 }; 

	for (i = 0; i < level->bombSpawnLimit; i++) {
		if (bomb[i].spawned) {
			for (j = 0; j < ABM_COUNT; j++) {
				if (explosion[j].ongoing) {

					rotateBomb(&bomb[i], &explosion[j], level, alpha);

					clampBomb(&explosion[j], &bomb[i], &clamp);

					//explosion[j].xNew
					distance.x = fabs(explosion[j].center.x - clamp.x);
					distance.y = fabs(explosion[j].center.y - clamp.y);

					if (pow(distance.x, 2) + pow(distance.y, 2) <= pow(explosion[j].radius, 2)) {
						bomb[i].spawned = false;
						explosion[j].expandedRadius = true;
						explosion[j].increaseRadius = true;
						//explosion[i].center.x = enemy[j][k].x_pos;
						//explosion[i].center.y = enemy[j][k].y_pos;

						level->score += 100;
						(level->curr_bomb_count)--;
					}
				}
			}
		}
	}
}


void rotateBomb(Bomb * bomb, Explosion * explosion, Level * level, double alpha) 
{
	double xe = explosion->center.x;
	double ye = explosion->center.y; 

	double xb = bomb->pos.x + 0.5 * level->bombSize.x;
	double yb = bomb->pos.y + 0.5 * level->bombSize.y;

	double r = sqrt((xe - xb) * (xe - xb) + (ye - yb) * (ye - yb));
	double theta = atan2(-(xe - xb), (ye - yb));

	if (theta < 0.)
		theta += 2. * 3.14159265359; 

	explosion->xNew = xb - r * sin(theta + alpha);
	explosion->yNew = yb - r * sin(theta + alpha); 
}


void clampBomb(Explosion * explosion, Bomb * bomb, Vector * clamp) {
	
	//clamp x (xNew = new rotated center.x)
	if (explosion->center.x < bomb->topLeft.x) {
		clamp->x = bomb->topLeft.x;
	}
	else if (explosion->center.x > bomb->topRight.x) {
		clamp->x = bomb->topRight.x;
	}
	else {
		clamp->x = explosion->center.x;
	}

	//clamp y
	if (explosion->center.y < bomb->topLeft.y) {
		clamp->x = bomb->topLeft.y;
	}
	else if (explosion->center.y > bomb->bottomLeft.y) {
		clamp->y = bomb->bottomLeft.y;
	}
	else {
		clamp->y = explosion->center.y;
	}

	/*
	//clamp x (xNew = new rotated center.x)
	if (explosion->xNew < bomb->topLeft.x) {
		clamp->x = bomb->topLeft.x;
	}
	else if (explosion->xNew > bomb->topRight.x) {
		clamp->x = bomb->topRight.x;
	}
	else {
		clamp->x = explosion->xNew;
	}

	//clamp y
	if (explosion->yNew < bomb->topLeft.y) {
		clamp->x = bomb->topLeft.y;
	}
	else if (explosion->yNew > bomb->bottomLeft.y) {
		clamp->y = bomb->bottomLeft.y;
	}
	else {
		clamp->y = explosion->yNew;
	}
	*/
}


void baseCollision(Base * base, Enemy ** enemy, int baseCount, Level * level, Ufo * ufo, Bomb * bomb) {
	int i, j, k;


	for (i = 0; i < baseCount; i++) {
		if (!base[i].destroyed) {

			for (j = 0; j < level->spawnLimit; j++)
				for (k = 0; k < SPLIT_COUNT; k++) {

					if (enemy[j][k].launched) {
						if ((calcBoundingBox(base[i], enemy[j][k])) == true) {
							base[i].destroyed = true;
							(level->lives)--;
							enemy[j][k].arrived = true;
							enemy[j][k].launched = false;
						}
					}
				}



			for (j = 0; j < level->ufoSpawnLimit; j++) {
				if (ufo[j].spawned) {
					for (k = 0; k < 2; k++) {
						if (ufo[j].missile[k].launched) {

							if ((calcBoundingBox(base[i], ufo[j].missile[k])) == true) {

								base[i].destroyed = true;
								(level->lives)--;
								ufo[j].missile[k].arrived = true;
								ufo[j].missile[k].launched = false;
							}
						}
					}
				}
			}


			for (j = 0; j < level->bombSpawnLimit; j++) {
				if (bomb[j].spawned) {

					if (bomb[j].topRight.x >= base[i].topLeft.x &&
						bomb[j].topLeft.x <= base[i].topRight.x &&
						bomb[j].bottomLeft.y >= base[i].topLeft.y &&
						bomb[j].topLeft.y <= base[i].bottomLeft.y) {

						base[i].destroyed = true;
						(level->lives)--;
						bomb[j].arrived = true;
						bomb[j].spawned = false; 
					}
				}
			}
		}
	}
}



bool calcBoundingBox(Base base, Enemy enemy) {
	bool collided = false; 

	if (enemy.topRight.x >= base.topLeft.x &&
		enemy.topLeft.x <= base.topRight.x &&
		enemy.bottomLeft.y >= base.topLeft.y &&
		enemy.topLeft.y <= base.bottomLeft.y) {

		collided = true;
	}
	
	return collided; 
}


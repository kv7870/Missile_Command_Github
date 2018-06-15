//hit.cpp
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


void collision(Abm * abm, Enemy ** enemy, Explosion * explosion, Level * level, Ufo * ufo, Audio * audio) {

	bool left = false;
	bool right = false;
	bool below = false;
	bool above = false;
	Vector distance = { 0, 0 };
	Vector clamp = { 0, 0 };
	int audioSelection = 0;

	//distance.x = 900;
	//distance.y = 900;
	//clamp.x = 0;
	//clamp.y = 0; 

	for (int i = 0; i < ABM_COUNT; i++) {

		if (explosion[i].ongoing) {

			//check for collision between explosion and regular enemy missile
			for (int j = 0; j < level->maxEnemyOnScreen/4; j++) {
				for (int k = 0; k < SPLIT_COUNT; k++) {
					if (enemy[j][k].launched) {

						//check bounds
						//if (explosion[i].topRight.x >= enemy[j][k].topLeft.x &&
						//explosion[i].topLeft.x <= enemy[j][k].topRight.x &&
						//explosion[i].bottomLeft.y >= enemy[j][k].topLeft.y &&
						//explosion[i].topLeft.y <= enemy[j][k].bottomLeft.y) {

						//enemy[j][k].launched = false;
						//explosion[i].expandedRadius = true;

						//}

						clampSquare(&(explosion[i]), enemy[j][k], &clamp);

						if ((calcDistance(distance, explosion[i], clamp)) == true) {

							explosion[i].expandedRadius = true;

							explosion[i].increaseRadius = true;

							audioSelection = rand() % 6;
							al_play_sample(audio->explosion[audioSelection], 2.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

							enemy[j][k].launched = false;
							level->score += 25;
						}
					}
				}
			}


			//check for collision between explosion and missile using bounding boxes
			for (int j = 0; j < level->maxUfoOnScreen; j++) {
				if (ufo[j].spawned) {
					if (explosion[i].topRight.x >= ufo[j].topLeft.x &&
						explosion[i].topLeft.x <= ufo[j].topRight.x &&
						explosion[i].bottomLeft.y >= ufo[j].topLeft.y &&
						explosion[i].topLeft.y <= ufo[j].bottomLeft.y) {

						explosion[i].expandedRadius = true;
						explosion[i].increaseRadius = true;

						audioSelection = rand() % 6;
						al_play_sample(audio->explosion[audioSelection], 2.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

						ufo[j].spawned = false;
						level->score += 100;
					}
				}

				//check for collision between explosion and cruise missile
				for (int k = 0; k < 2; k++) {
					if (ufo[j].missile[k].launched) {

						clampSquare(&(explosion[i]), ufo[j].missile[k], &clamp);

						if ((calcDistance(distance, explosion[i], clamp)) == true) {

							explosion[i].expandedRadius = true;
							explosion[i].increaseRadius = true;

							audioSelection = rand() % 6;
							al_play_sample(audio->explosion[audioSelection], 2.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

							ufo[j].missile[k].launched = false;
							level->score += 25;
						}
					}
				}
			}
		}
	}
}


void clampSquare(Explosion * explosion, Enemy enemy, Vector * clamp) {

	//clamp x
	if (explosion->center.x < enemy.topLeft.x)
		clamp->x = enemy.topLeft.x;

	else if (explosion->center.x > enemy.topRight.x)
		clamp->x = enemy.topRight.x;

	else
		clamp->x = explosion->center.x;


	//clamp y
	if (explosion->center.y < enemy.topLeft.y)
		clamp->x = enemy.topLeft.y;

	else if (explosion->center.y > enemy.bottomLeft.y)
		clamp->y = enemy.bottomLeft.y;

	else
		clamp->y = explosion->center.y;
}


bool calcDistance(Vector distance, Explosion explosion, Vector clamp) {
	bool collided = false;

	distance.x = fabs(explosion.center.x - clamp.x);
	distance.y = fabs(explosion.center.y - clamp.y);

	if (pow(distance.x, 2) + pow(distance.y, 2) <= pow(explosion.radius, 2))
		collided = true;

	return collided;
}

void scmCollision(Scm * scm, Explosion * explosion, Level * level, Audio * audio) {
	int i, j;
	const double alpha = 0.25* 3.14159265359;
	int selection;

	Vector clamp = { 0, 0 };
	Vector distance = { 0, 0 };

	for (i = 0; i < level->maxScmOnScreen; i++) {
		if (scm[i].spawned) {
			for (j = 0; j < ABM_COUNT; j++) {
				if (explosion[j].ongoing) {

					clampScm(&explosion[j], &scm[i], &clamp);

					distance.x = fabs(explosion[j].center.x - clamp.x);
					distance.y = fabs(explosion[j].center.y - clamp.y);

					if (pow(distance.x, 2) + pow(distance.y, 2) <= pow(explosion[j].radius, 2)) {

						explosion[j].expandedRadius = true;
						explosion[j].increaseRadius = true;

						selection = rand() % 6;
						al_play_sample(audio->explosion[selection], 2.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

						scm[i].spawned = false;
						level->score += 100;
					}
				}
			}
		}
	}
}


void clampScm(Explosion * explosion, Scm * scm, Vector * clamp) {

	//clamp x (xNew = new rotated center.x)
	if (explosion->center.x < scm->topLeft.x)
		clamp->x = scm->topLeft.x;

	else if (explosion->center.x > scm->topRight.x)
		clamp->x = scm->topRight.x;

	else
		clamp->x = explosion->center.x;


	//clamp y
	if (explosion->center.y < scm->topLeft.y)
		clamp->x = scm->topLeft.y;

	else if (explosion->center.y > scm->bottomLeft.y)
		clamp->y = scm->bottomLeft.y;

	else
		clamp->y = explosion->center.y;
}


void baseCollision(Base * base, Enemy ** enemy, int baseCount, Level * level, Ufo * ufo, Scm * scm){
	int i, j, k;


	for (i = 0; i < baseCount; i++) {
		if (!base[i].destroyed) {

			for (j = 0; j < level->maxEnemyOnScreen/4; j++)
				for (k = 0; k < SPLIT_COUNT; k++) {

					if (enemy[j][k].launched) {
						if ((calcBoundingBox(base[i], enemy[j][k])) == true) {
							(level->lives)--;
							base[i].destroyed = true;
							enemy[j][k].launched = false;
						}
					}
				}

			for (j = 0; j < level->maxUfoOnScreen; j++) {
				for (k = 0; k < 2; k++) {
					if (ufo[j].missile[k].launched) {

						if ((calcBoundingBox(base[i], ufo[j].missile[k])) == true) {
							(level->lives)--;
							base[i].destroyed = true;
							ufo[j].missile[k].launched = false;
						}
					}
				}
			}


			for (j = 0; j < level->maxScmOnScreen; j++) {
				if (scm[j].spawned) {

					if (scm[j].topRight.x >= base[i].topLeft.x &&
						scm[j].topLeft.x <= base[i].topRight.x &&
						scm[j].bottomLeft.y >= base[i].topLeft.y &&
						scm[j].topLeft.y <= base[i].bottomLeft.y) {

						(level->lives)--;
						base[i].destroyed = true;
						scm[j].spawned = false;
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
/*
MISSILE COMMAND SUMMATIVE BY KEVIN YANG & EDWARD LI
FINAL VERSION
*/

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

	Vector distance = { 0, 0 };	//distance between enemy missile and explosion
	Vector clamp = { 0, 0 };	//pixel of enemy missile closest to explosion center
	int audioSelection = 0;

	for (int i = 0; i < ABM_COUNT; i++) {

		if (explosion[i].ongoing) {

			//check for collision between regular enemy missile and explosion using clamp method
			for (int j = 0; j < level->maxEnemyOnScreen/4; j++) {
				for (int k = 0; k < SPLIT_COUNT; k++) {
					if (enemy[j][k].launched) {

						//calculate pixel of enemy missile closest to the nearest explosion center
						clampSquare(&(explosion[i]), enemy[j][k], &clamp);

						//enemy missile is hit if its closest pixel is touching explosion
						if ((calcDistance(distance, explosion[i], clamp)) == true) {

							explosion[i].expandedRadius = true;	//explosion grows bigger

							explosion[i].increaseRadius = true;	//explosion grows bigger

							audioSelection = rand() % 6;	//play random explosion sound 
							al_play_sample(audio->explosion[audioSelection], 2.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

							enemy[j][k].launched = false;	//enemy missile is destroyed
							level->score += 25;	
						}
					}
				}
			}

			//check for collision between ufo and explosion using bounding boxes
			for (int j = 0; j < level->maxUfoOnScreen; j++) {
				if (ufo[j].spawned) {
					//collided if bounding boxes are touching
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

				//check for collision between cruise missile and explosion
				for (int k = 0; k < 2; k++) {
					if (ufo[j].missile[k].launched) {

						//calculate pixel of cruise missile closest to the nearest explosion center
						clampSquare(&(explosion[i]), ufo[j].missile[k], &clamp);

						//cruise missile is hit if its closest pixel is touching explosion
						if ((calcDistance(distance, explosion[i], clamp)) == true) {

							explosion[i].expandedRadius = true;
							explosion[i].increaseRadius = true;

							audioSelection = rand() % 6;
							al_play_sample(audio->explosion[audioSelection], 2.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

							ufo[j].missile[k].launched = false;
							level->score += 125;	//125 points for cruise missile
						}
					}
				}
			}
		}
	}
}


//check for collision between enemy missile and explosion by reducing missile to a single pixel
void clampSquare(Explosion * explosion, Enemy enemy, Vector * clamp) {

	//clamp x
	//if missile is to right of an explosion, left side of missile is closest to explosion center
	if (explosion->center.x < enemy.topLeft.x)	
		clamp->x = enemy.topLeft.x;

	//if missile is to left of an explosion, right side of missile is closest to explosion
	else if (explosion->center.x > enemy.topRight.x)
		clamp->x = enemy.topRight.x;

	//if missile is inside an explosion, x-coordinate of explosion center is closest to missile
	else
		clamp->x = explosion->center.x;


	//clamp y
	//if missile is below an explosion, top side of missile is closest to explosion center
	if (explosion->center.y < enemy.topLeft.y)
		clamp->x = enemy.topLeft.y;

	//if missile is above an explosion, bottom side of missile is closest to explosion
	else if (explosion->center.y > enemy.bottomLeft.y)
		clamp->y = enemy.bottomLeft.y;

	//if missile is inside an explosion, y-coordinate of explosion center is closest to missile
	else
		clamp->y = explosion->center.y;

	//enemy missile is now reduced to a single pixel closest to the explosion center
}


//return whether enemy missile is touching an explosion 
bool calcDistance(Vector distance, Explosion explosion, Vector clamp) {
	bool collided = false;

	distance.x = fabs(explosion.center.x - clamp.x);	//clamp = enemy missile reduced to closest pixel 
	distance.y = fabs(explosion.center.y - clamp.y);

	if (pow(distance.x, 2) + pow(distance.y, 2) <= pow(explosion.radius, 2))
		collided = true;

	return collided;
}


//calculate if a smart cruise missile is touching explosion using clamp; same method as regular enemy missiles
void scmCollision(Scm * scm, Explosion * explosion, Level * level, Audio * audio) {
	int i, j;
	int selection;	//audio 

	Vector clamp = { 0, 0 };
	Vector distance = { 0, 0 };

	for (i = 0; i < level->maxScmOnScreen; i++) {
		if (scm[i].spawned) {
			for (j = 0; j < ABM_COUNT; j++) {
				if (explosion[j].ongoing) {

					//calculate pixel of smart cruise missile closest to the nearest explosion center
					clampScm(&explosion[j], &scm[i], &clamp);

					//scm is hit if its closest pixel is touching an explosion
					if((calcDistance(distance, explosion[i], clamp)) == true) {

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


//calculate pixel of scm closest to an explosion center
void clampScm(Explosion * explosion, Scm * scm, Vector * clamp) {

	//clamp x 
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


//determine if base is hit by enemy missile 
void baseCollision(Base * base, Enemy ** enemy, int baseCount, Level * level, Ufo * ufo, Scm * scm){
	int i, j, k;


	for (i = 0; i < baseCount; i++) {
		if (!base[i].destroyed) {

			//check collision with regular missiles
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

			//check collision with cruise missiles
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


			//check collision with smart cruise missiles
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


//return whether a base is hit by enemy missile using bounding boxes
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
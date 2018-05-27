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


void hitDetection(struct abmData * abm, Enemy ** enemy, int * curr_enemy_count, int * lvl_spawn_limit, int * score, Explosion * explosion) {

	//enemy is...
	bool left = false;
	bool right = false;
	bool below = false; 
	bool above = false; 
	Vector distance;
	Vector clamp; 

	distance.x = 900;
	distance.y = 900;
	clamp.x = 0;
	clamp.y = 0; 

	for (int i = 0; i < ABM_COUNT; i++) {

		if (explosion[i].ongoing) {

			for (int j = 0; j < *lvl_spawn_limit; j++) {
				for (int k = 0; k < SPLIT_COUNT; k++) {
					if (enemy[j][k].launched) {

						//check bounds
						if (explosion[i].topRight.x >= enemy[j][k].topLeft.x &&
							explosion[i].topLeft.x <= enemy[j][k].topRight.x &&
							explosion[i].bottomLeft.y >= enemy[j][k].topLeft.y &&
							explosion[i].topLeft.y <= enemy[j][k].bottomLeft.y) {

							clampSquare(&(explosion[i]), &(enemy[j][k]), &clamp);
							 
							distance.x = fabs(explosion[i].center.x - clamp.x);
							distance.y = fabs(explosion[i].center.y - clamp.y); 

							if (distance.x <= explosion[i].radius && distance.y <= explosion[i].radius) {
								enemy[j][k].launched = false;
								explosion[i].expandedRadius = true;
								explosion[i].increaseRadius = true;
								explosion[i].center.x = enemy[j][k].x_pos;
								explosion[i].center.y = enemy[j][k].y_pos;

								(*score) += 25;
								(*curr_enemy_count)--;
							}
						}
					}
				}
			}
		}
	}
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


void baseCollision(Base * base, Enemy ** enemy, int * lvl_spawn_limit, int baseCount, int * lives) {
	for (int i = 0; i < *lvl_spawn_limit; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {
			if (enemy[i][j].launched) {
				for (int k = 0; k < baseCount; k++) {
					if (!base[k].destroyed) {
						if (enemy[i][j].topRight.x >= base[k].topLeft.x &&
							enemy[i][j].topLeft.x <= base[k].topRight.x &&
							enemy[i][j].bottomLeft.y >= base[k].topLeft.y &&
							enemy[i][j].topLeft.y <= base[k].bottomLeft.y) {

							base[k].destroyed = true;
							(*lives)--;
						}
					}
				}
			}
		}
	}
}

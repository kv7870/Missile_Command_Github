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


void hitDetection(struct abmData * abm, Enemy ** enemy, int * curr_enemy_count, int * lvl_spawn_limit, int * score) {
	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].arrived && !abm[i].doneExploding) {

			for (int j = 0; j < *lvl_spawn_limit; j++) {
				for (int k = 0; k < SPLIT_COUNT; k++) {
					if (enemy[j][k].launched) {

						//check bounds
						if (abm[i].topRight.x >= enemy[j][k].topLeft.x &&
							abm[i].topLeft.x <= enemy[j][k].topRight.x &&
							abm[i].bottomLeft.y >= enemy[j][k].topLeft.y &&
							abm[i].topLeft.y <= enemy[j][k].bottomLeft.y) {

							//left edge
							if (abm[i].dest_x < enemy[j][k].x_pos - SIZE) {
								enemy[j][k].relativeX = enemy[j][k].x_pos - SIZE;
							}

							//right edge 
							else if (abm[i].dest_x > enemy[j][k].x_pos + SIZE) {
								enemy[j][k].relativeX = enemy[j][k].x_pos + SIZE;
							}

							//top edge
							if (abm[i].dest_y < enemy[j][k].y_pos - SIZE) {
								enemy[j][k].relativeY = enemy[j][k].y_pos - SIZE;
							}

							//bottom edge 
							else if (abm[i].dest_y > enemy[j][k].y_pos + SIZE) {
								enemy[j][k].relativeY = enemy[j][k].y_pos + SIZE;
							}

							enemy[j][k].distX = abm[i].dest_x - enemy[j][k].relativeX;
							enemy[j][k].distY = abm[i].dest_y - enemy[j][k].relativeY;

							enemy[j][k].distTotal = sqrt((pow(enemy[j][k].distX, 2) + pow(enemy[j][k].distY, 2)));

							if (enemy[j][k].distTotal <= abm[i].explosionRadius) {
								enemy[j][k].launched = false;

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

							base[k].color.r = 255;
							base[k].color.g = 0;
							base[k].color.b = 0; 
						}
					}
				}
			}
		}
	}
}
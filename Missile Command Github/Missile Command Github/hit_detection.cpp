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

/*void hitDetection(struct abmData * abm, Enemy * enemy) {

	//check collision between abm and main enemy missile (not mirv's)
	for (int i = 0; i < 20; i++) {
		if (abm[i].arrived && !abm[i].exploded) {
			for (int j = 0; j < ENEMY_COUNT; j++) {
				if (enemy[j].launched) {
					if (abm[i].topRight.x >= enemy[i].topLeft.x &&
						abm[i].topLeft.x <= enemy[i].topRight.x &&
						abm[i].bottomLeft.y >= enemy[i].topLeft.y &&
						abm[i].topLeft.y <= enemy[i].bottomLeft.y) {
						printf("Hit");

					}
				}

			}
		}
	}
}*/

void hitDetection(struct abmData * abm, Enemy * enemy) {
	for (int i = 0; i < ABM_COUNT; i++) {
		printf("%d", i); 
		if (abm[i].arrived && !abm[i].exploded) {
			for (int j = 0; j < ENEMY_COUNT; j++) {
				if (enemy[j].launched) {
					//left edge
					if (abm[i].dest_x < enemy[j].x_pos - SIZE) {
						enemy[j].relativeX = enemy[j].x_pos - SIZE;
					}
					//right edge 
					else if (abm[i].dest_x > enemy[j].x_pos + SIZE) {
						enemy[j].relativeX = enemy[j].x_pos + SIZE;
					}

					//top edge
					if(abm[i].dest_y < enemy[j].y_pos-SIZE) {
						enemy[j].relativeY = enemy[j].y_pos - SIZE;
					}
					//bottom edge 
					else if (abm[i].dest_y > enemy[j].y_pos + SIZE) {
						enemy[j].relativeY = enemy[j].y_pos + SIZE; 
					}

					enemy[j].distX = abm[i].dest_x - enemy[j].relativeX;
					enemy[j].distY = abm[i].dest_y - enemy[j].relativeY;
					enemy[j].distTotal = sqrt((pow(enemy[j].distX, 2) + pow(enemy[j].distY, 2))); 
					
					if (enemy[j].distTotal <= abm[i].explosionRadius) {
						printf("hit");
						enemy[j].launched = false; 
					}

				}

			}
		}
	}
}


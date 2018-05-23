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


void hitDetection(struct abmData * abm, Enemy * enemy, Mirv * mirv) {
	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].arrived && !abm[i].exploded) {

			//hit detection for main enemy missiles 
			for (int j = 0; j < ENEMY_COUNT; j++) {
				if (enemy[j].launched) {
					//check bounds
					if (abm[i].topRight.x >= enemy[j].topLeft.x &&
						abm[i].topLeft.x <= enemy[j].topRight.x &&
						abm[i].bottomLeft.y >= enemy[j].topLeft.y &&
						abm[i].topLeft.y <= enemy[j].bottomLeft.y) {

						//left edge
						if (abm[i].dest_x < enemy[j].x_pos - SIZE) {
							enemy[j].relativeX = enemy[j].x_pos - SIZE;
						}
						//right edge 
						else if (abm[i].dest_x > enemy[j].x_pos + SIZE) {
							enemy[j].relativeX = enemy[j].x_pos + SIZE;
						}

						//top edge
						if (abm[i].dest_y < enemy[j].y_pos - SIZE) {
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
							enemy[j].launched = false;
						}
					}

					//hit detection for mirvs
					if(enemy[j].split) {
						for (int k = 0; k < 3; k++) {
							//check bounds
							if (abm[i].topRight.x >= mirv[j].branch[k].topLeft.x &&
								abm[i].topLeft.x <= mirv[j].branch[k].topRight.x &&
								abm[i].bottomLeft.y >= mirv[j].branch[k].topLeft.y &&
								abm[i].topLeft.y <= mirv[j].branch[k].bottomLeft.y) {

								//left edge
								if (abm[i].dest_x < mirv[j].branch[k].x_pos - SIZE) {
									mirv[j].branch[k].relativeX = mirv[j].branch[k].x_pos - SIZE;
								}
								//right edge
								else if (abm[i].dest_x > mirv[j].branch[k].x_pos + SIZE) {
									mirv[j].branch[k].relativeX = mirv[j].branch[k].x_pos + SIZE;
								}
								//top edge 
								else if (abm[i].dest_y < mirv[j].branch[k].y_pos - SIZE) {
									mirv[j].branch[k].relativeY = mirv[j].branch[k].y_pos - SIZE;
								}
								//bottom edge
								else if (abm[i].dest_y > mirv[j].branch[k].y_pos + SIZE) {
									mirv[j].branch[k].relativeY = mirv[j].branch[k].y_pos + SIZE;
								}

								mirv[j].branch[k].distX = abm[i].dest_x - mirv[j].branch[k].relativeX;
								mirv[j].branch[k].distY = abm[i].dest_y - mirv[j].branch[k].relativeY;

								mirv[j].branch[k].distTotal = sqrt((pow(mirv[j].branch[k].distX, 2) + pow(mirv[j].branch[k].distY, 2)));

								if (mirv[j].branch[k].distTotal <= abm[i].explosionRadius) {
									mirv[j].branch[k].launched = false;
								}
							}
						}
					}
				}
			}
		}
	}
}


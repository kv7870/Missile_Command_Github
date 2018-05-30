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


void fire(Abm * abm, Crosshair crosshair, Level * level) {
	int i;
	bool closestLaunchSuccess = false;

	if (crosshair.target_x <= 300) {
		for (i = 0; i < 10; i++) {
			if (!abm[i].launched && !abm[i].arrived) {
				abm[i].x_pos = abm[i].launch_x;
				abm[i].y_pos = abm[i].launch_y;
				abm[i].dest_x = crosshair.target_x;
				abm[i].dest_y = crosshair.target_y;
				abm[i].launched = true;
				closestLaunchSuccess = true;
				(level->abmLeft)--;
				(level->batteryAbmLeft[0])--;
				calcAbmInc(&(abm[i]));
				break;
			}
		}
	}

	else if (crosshair.target_x > 300 && crosshair.x <= 600) {
		for (i = 10; i < 20; i++) {
			if (!abm[i].launched && !abm[i].arrived) {
				abm[i].x_pos = abm[i].launch_x;
				abm[i].y_pos = abm[i].launch_y;
				abm[i].dest_x = crosshair.target_x;
				abm[i].dest_y = crosshair.target_y;
				abm[i].launched = true;
				closestLaunchSuccess = true;
				(level->abmLeft)--;
				(level->batteryAbmLeft[1])--;
				calcAbmInc(&(abm[i]));
				break;
			}
		}
	}

	else if (crosshair.target_x > 600) {
		for (i = 20; i < 30; i++) {
			if (!abm[i].launched && !abm[i].arrived) {
				abm[i].x_pos = abm[i].launch_x;
				abm[i].y_pos = abm[i].launch_y;
				abm[i].dest_x = crosshair.target_x;
				abm[i].dest_y = crosshair.target_y;
				abm[i].launched = true;
				closestLaunchSuccess = true;
				(level->abmLeft)--;
				(level->batteryAbmLeft[2])--;
				calcAbmInc(&(abm[i]));
				break;
			}
		}
	}


	//if cannot fire from nearest battery
	if (!closestLaunchSuccess) {
		for (i = 0; i < ABM_COUNT; i++) {
			if (!abm[i].launched && !abm[i].arrived) {
				abm[i].x_pos = abm[i].launch_x;
				abm[i].y_pos = abm[i].launch_y;
				abm[i].dest_x = crosshair.target_x;
				abm[i].dest_y = crosshair.target_y;
				abm[i].launched = true;
				(level->abmLeft)--;

				if (i < 10)
					(level->batteryAbmLeft[0])--;

				else if (i >= 10 && i < 20)
					(level->batteryAbmLeft[1])--;

				else if (i >= 20)
					(level->batteryAbmLeft[2])--;

				calcAbmInc(&(abm[i]));
				break;
			}
		}
	}
}


void calcAbmInc(Abm * abm) {
	abm->dx = fabs(abm->dest_x - abm->launch_x);
	abm->dy = fabs(abm->dest_y - abm->launch_y);

	if (abm->dx >= abm->dy) {
		abm->step = abm->dx;
	}
	else {
		abm->step = abm->dy;
	}

	abm->x_inc = abm->dx / abm->step;
	abm->y_inc = abm->dy / abm->step;
}

void drawAbm(struct abmData * abm, int * theme, int colorMap[][3]) {
	int r, g, b;

	r = rand() % 255 + 1;
	g = rand() % 255 + 1;
	b = rand() % 255 + 1;

	int colorId = *theme;  //theme[0] 


	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].launched) {

			al_draw_filled_rectangle(abm[i].x_pos - 3, abm[i].y_pos - 3, abm[i].x_pos + 3, abm[i].y_pos + 3, 
				al_map_rgb(colorMap[colorId][R], colorMap[colorId][G], colorMap[colorId][B]));
			al_draw_line(abm[i].x_pos, abm[i].y_pos + 3, abm[i].launch_x, abm[i].launch_y, al_map_rgb(colorMap[colorId][R], colorMap[colorId][G], colorMap[colorId][B]), 4);

			//hitmarkers
			al_draw_line(abm[i].dest_x - 7, abm[i].dest_y + 7, abm[i].dest_x + 7, abm[i].dest_y - 7, al_map_rgb(r, g, b), 3);
			al_draw_line(abm[i].dest_x - 7, abm[i].dest_y - 7, abm[i].dest_x + 7, abm[i].dest_y + 7, al_map_rgb(r, g, b), 3);
		}
	}
}


//update in buffer 
void updateAbm(struct abmData * abm) {
	int i;

	for (i = 0; i < ABM_COUNT; i++) {

		if (abm[i].launched) {  //only update launched & alive abm's 

			if (abm[i].dest_x > abm[i].launch_x) {
				abm[i].x_pos += abm[i].speed * abm[i].x_inc;
				//abm[i].num_increment += 10; 
			}
			else if (abm[i].dest_x < abm[i].launch_x) {
				abm[i].x_pos -= abm[i].speed * abm[i].x_inc;
				//abm[i].num_increment += 10; 
			}

			abm[i].y_pos -= abm[i].speed * abm[i].y_inc;
			abm[i].num_increment += abm[i].speed;
		}
	}
}


void abmArrival(Abm * abm, Explosion * explosion) {
	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].launched) {
			if (abm[i].num_increment > abm[i].step) {
				abm[i].launched = false;
				abm[i].arrived = true;
				explosion[i].ongoing = true; 
				explosion[i].center.x = abm[i].dest_x;
				explosion[i].center.y = abm[i].dest_y; 
			}
		}
	}
}


void drawExplosion(Abm * abm, Explosion * explosion, int colorMap[][3]) {
	
	int r, g, b;

	int explosionColor = rand() % NUM_COLORS;
	
	r = rand() % 255 + 1;
	g = rand() % 255 + 1;
	b = rand() % 255 + 1;

	for (int i = 0; i < ABM_COUNT; i++) {
		if (explosion[i].ongoing) {

			//printf("Destination: (%d, %d): arrive: %d\n", abm[i].dest_x, abm[i].dest_y, abm[i].arrived); 
			if (explosion[i].radius >= 40 && !explosion[i].expandedRadius) {
				explosion [i].increaseRadius = false;
			}
			else if (explosion[i].radius >= 80 && explosion[i].expandedRadius) {
				explosion[i].increaseRadius = false; 
			}

			if (explosion[i].increaseRadius) {
				explosion[i].radius += 1;
			}

			else if (!explosion[i].increaseRadius) {
				explosion[i].radius -= 1;
			}

			al_draw_filled_circle(abm[i].dest_x, abm[i].dest_y, explosion[i].radius, al_map_rgb(colorMap[explosionColor][R], colorMap[explosionColor][G], colorMap[explosionColor][B]));

			//calculate bounds of explosion 
			explosion[i].topLeft.x = explosion[i].center.x - explosion[i].radius;
			explosion[i].topLeft.y = explosion[i].center.y - explosion[i].radius;
			explosion[i].topRight.x = explosion[i].center.x + explosion[i].radius;
			explosion[i].topRight.y = explosion[i].center.y - explosion[i].radius;
			explosion[i].bottomLeft.x = explosion[i].center.x - explosion[i].radius;
			explosion[i].bottomLeft.y = explosion[i].center.y + explosion[i].radius;

			if (explosion[i].radius < 0) {
				explosion[i].ongoing = false; 
			}
		}
	}
}
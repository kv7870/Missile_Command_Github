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


void fire(Abm * abm, Crosshair crosshair, int * abmLeft, int * batteryAbmLeft) {
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
				(*abmLeft)--; 
				(batteryAbmLeft[0])--;
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
				(*abmLeft)--;
				(batteryAbmLeft[1])--;
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
				(*abmLeft)--;
				(batteryAbmLeft[2])--;
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
				(*abmLeft)--;

				if (i < 10) 
					(batteryAbmLeft[0])--;
				
				else if (i >= 10 && i < 20)
					(batteryAbmLeft[1])--;

				else if(i>=20)
					(batteryAbmLeft[2])--;

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

void drawAbm(struct abmData * abm) {
	int r, g, b;
	r = rand() % 255 + 1;
	g = rand() % 255 + 1;
	b = rand() % 255 + 1;

	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].launched) {
			al_draw_filled_rectangle(abm[i].x_pos - 3, abm[i].y_pos - 3, abm[i].x_pos + 3, abm[i].y_pos + 3, al_map_rgb(255, 255, 255));
			al_draw_line(abm[i].x_pos, abm[i].y_pos + 3, abm[i].launch_x, abm[i].launch_y, al_map_rgb(0, 0, 255), 4);

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


void abmArrival(Abm * abm) {
	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].launched) {
			if (abm[i].num_increment > abm[i].step) {
				abm[i].launched = false;
				abm[i].arrived = true;
			}
		}
	}
}


void drawExplosion(Abm * abm) {
	int r, g, b;

	r = rand() % 255 + 1;
	g = rand() % 255 + 1;
	b = rand() % 255 + 1;

	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].arrived && !abm[i].doneExploding) {

			//printf("Destination: (%d, %d): arrive: %d\n", abm[i].dest_x, abm[i].dest_y, abm[i].arrived); 
			if (abm[i].explosionRadius >= 40) {
				abm[i].increaseRadius = false;
			}

			if (abm[i].increaseRadius) {
				abm[i].explosionRadius += 1;
			}

			else if (!abm[i].increaseRadius) {
				abm[i].explosionRadius -= 1;
			}

			al_draw_filled_circle(abm[i].dest_x, abm[i].dest_y, abm[i].explosionRadius, al_map_rgb(r, g, b));

			//calculate bounds of explosion 
			abm[i].topLeft.x = abm[i].dest_x - abm[i].explosionRadius;
			abm[i].topLeft.y = abm[i].dest_y - abm[i].explosionRadius;
			abm[i].topRight.x = abm[i].dest_x + abm[i].explosionRadius;
			abm[i].topRight.y = abm[i].dest_y - abm[i].explosionRadius;
			abm[i].bottomLeft.x = abm[i].dest_x - abm[i].explosionRadius;
			abm[i].bottomLeft.y = abm[i].dest_y + abm[i].explosionRadius;

			if (abm[i].explosionRadius < 0) {
				abm[i].doneExploding = true;
			}
		}
	}
}
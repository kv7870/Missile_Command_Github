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

/*void fire(Crosshair crosshair, struct abmData * abm) {

	for (int i = 0; i < 30; i++) {
		if (!abm[i].used) {  //only fire unused abm 
			abm[i].dest_x = crosshair.target_x;
			abm[i].dest_y = crosshair.target_y;
			abm[i].launch_x = 100;
			abm[i].launch_y = 700;
			abm[i].launched = true;
			abm[i].used = false;
			break;
		}
	}

	drawAbm(abm, crosshair);
}*/


void fire(struct abmData * abm, Crosshair crosshair) {
	int i;
	bool closestLaunchSuccess = false;
	bool launchSuccess = false; //if any 1 abm is fired upon mouseclick, only for diagnosis 

	if(crosshair.target_x <= 300) {
		for (i = 0; i < 10; i++) {
			if (!abm[i].launched && !abm[i].used) {
				abm[i].x_pos = abm[i].launch_x;
				abm[i].y_pos = abm[i].launch_y;
				abm[i].dest_x = crosshair.target_x;
				abm[i].dest_y = crosshair.target_y;
				abm[i].launched = true; 
				closestLaunchSuccess = true;
				launchSuccess = true;
				break;
			}
		}
	}

	else if(crosshair.target_y > 300 && crosshair.x <= 600) {
		for (i = 10; i < 20; i++) {
			if (!abm[i].launched && !abm[i].used) {
				abm[i].x_pos = abm[i].launch_x;
				abm[i].y_pos = abm[i].launch_y;
				abm[i].dest_x = crosshair.target_x;
				abm[i].dest_y = crosshair.target_y;
				abm[i].launched = true;
				closestLaunchSuccess = true;
				launchSuccess = true;
				break;
			}
		}
	}

	else if (crosshair.target_x > 600) {
		for (i = 20; i < 30; i++) {
			if (!abm[i].launched && !abm[i].used) {
				abm[i].x_pos = abm[i].launch_x;
				abm[i].y_pos = abm[i].launch_y;
				abm[i].dest_x = crosshair.target_x;
				abm[i].dest_y = crosshair.target_y; 
				abm[i].launched = true;
				closestLaunchSuccess = true;
				launchSuccess = true;
				break;
			}
		}
	}


	//if cannot fire from nearest battery
	if (!closestLaunchSuccess) {
		for (i = 0; i < 30; i++) {
			if (!abm[i].launched && !abm[i].used) {
				abm[i].x_pos = abm[i].launch_x;
				abm[i].y_pos = abm[i].launch_y;
				abm[i].dest_x = crosshair.target_x;
				abm[i].dest_y = crosshair.target_y;
				abm[i].launched = true;
				launchSuccess = true;
				break;
			}
		}
	}

	if (!launchSuccess) {
		printf("Out of ABMs!"); 
	}

}


void drawAbm(struct abmData * abm) {
	al_draw_filled_rectangle(10, 830, 100, 900, al_map_rgb(255, 255, 255));   //(55, 830)
	al_draw_filled_rectangle(400, 830, 490, 900, al_map_rgb(255, 255, 255));  //(445, 830)
	al_draw_filled_rectangle(800, 830, 890, 900, al_map_rgb(255, 255, 255));  //(845, 830)

	for (int i = 0; i < 30; i++) {
		if (abm[i].launched) {
			al_draw_filled_circle(abm[i].x_pos, abm[i].y_pos, 10, al_map_rgb(255, 255, 255));
			al_draw_line(abm[i].x_pos, abm[i].y_pos, abm[i].launch_x, abm[i].launch_y, al_map_rgb(255, 255, 255), 3);
		}
	}
}


//update in buffer 
void updateAbm(struct abmData * abm) {

	for (int i = 0; i < 30; i++) {
		if (abm[i].launched) {  //only update launched abm's 

			if(abm[i].y_pos <100) {
			//if (abm[i].x_pos == abm[i].dest_x && abm[i].y_pos == abm[i].dest_y) {
				printf("arrived");
				abm[i].launched = false;
				abm[i].used = true;
			}

			abm[i].dx = fabs(abm[i].dest_x - abm[i].launch_x);
			abm[i].dy = fabs(abm[i].dest_y - abm[i].launch_y);

			if (abm[i].dx >= abm[i].dy) {
				abm[i].step = abm[i].dx;
			}
			else {
				abm[i].step = abm[i].dy;
			}

			abm[i].x_inc = abm[i].dx / abm[i].step;
			abm[i].y_inc = abm[i].dy / abm[i].step;

			abm[i].y_pos -= 5 * abm[i].y_inc;

			if (abm[i].dest_x > abm[i].launch_x) {
				abm[i].x_pos += 5 * abm[i].x_inc; 
			}
			else if (abm[i].dest_x < abm[i].launch_x) {
				abm[i].x_pos -= 5 * abm[i].x_inc;
			}
		}
	}
}


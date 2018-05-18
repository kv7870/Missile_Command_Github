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

void fire(struct crosshairData * crosshair, struct abmData * abm) {

	for (int i = 0; i < 30; i++) {
		if (!abm[i].used) {  //only fire unused abm 
			abm[i].dest_x = crosshair->target_x;
			abm[i].dest_y = crosshair->target_y;
			abm[i].launch_x = 100;
			abm[i].launch_y = 700;
			abm[i].launched = true;
			//abm[i].used = false;
			break;
		}
	}

	drawAbm(abm);
}


void drawAbm(struct abmData * abm) {
	for (int i = 0; i < 30; i++) {
		if (abm[i].launched && !abm[i].used) {
			al_draw_filled_circle(abm[i].x_pos, abm[i].y_pos, 10, al_map_rgb(255, 255, 255));
			abm[i].x_pos = abm[i].launch_x;
			abm[i].y_pos = abm[i].launch_y;
		}
	}
}


void updateAbm(struct abmData * abm) {

	al_draw_filled_rectangle(50, 800, 100, 850, al_map_rgb(255, 255, 255));

	for (int i = 0; i < 30; i++) {
		if (abm[i].launched && !abm[i].arrived) {  //only update launched abm's 

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

			abm[i].x_pos += 10 * abm[i].x_inc;
			abm[i].y_pos -= 10 * abm[i].y_inc;

			if (abm[i].x_pos == abm[i].dest_x && abm[i].y_pos == abm[i].dest_y) {
				abm[i].arrived = true;
			}


			al_draw_filled_circle(abm[i].x_pos, abm[i].y_pos + 45, 10, al_map_rgb(255, 255, 255));
			al_draw_line(abm[i].x_pos, abm[i].y_pos + 45, abm[i].launch_x - 5, abm[i].launch_y + 45, al_map_rgb(255, 255, 255), 3);
		}
	}
}
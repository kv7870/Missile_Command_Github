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
			abm[i].launched = true;
			abm[i].used = true;
			break;
		}
	}
}



void updateAbm(struct abmData * abm) {

	al_draw_filled_rectangle(50, 800, 100, 850, al_map_rgb(255, 255, 255));

	for (int i = 0; i < 30; i++) {
		if (abm[i].launched) {  //only update launched abm's 

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

			abm[i].x += abm[i].x_inc;
			abm[i].y += abm[i].y_inc;


			al_draw_filled_rectangle(abm[i].x, abm[i].y, (abm[i].x) - 500, (abm[i].y) - 500, al_map_rgb(255, 255, 255));
		}
	}
}
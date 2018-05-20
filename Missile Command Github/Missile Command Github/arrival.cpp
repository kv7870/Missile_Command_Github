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

void abmArrival(struct abmData * abm) {
	int i; 

	for (i = 0; i < 30; i++) {
		if (abm[i].launched) {
			abm[i].distance = sqrt(pow((abm[i].dest_x - abm[i].x_pos), 2) + pow((abm[i].dest_y - abm[i].y_pos), 2)); 
			if (abm[i].distance <= 10) {
				abm[i].launched = false; 
				abm[i].arrived = true; 
			}
		}
	}
}


void drawExplosion(struct abmData * abm) {
	
	int r, g, b;

		r = rand() % 255 + 1;
		g = rand() % 255 + 1;
		b = rand() % 255 + 1;

	for (int i = 0; i < 30; i++) {
		if (abm[i].arrived && !abm[i].exploded) {

			if (abm[i].explosionRadius == 40) {
				abm[i].increaseRadius = false; 
			}

			if (abm[i].increaseRadius) {
				abm[i].explosionRadius +=0.5;
				al_draw_filled_circle(abm[i].dest_x, abm[i].dest_y, abm[i].explosionRadius, al_map_rgb(r, g, b));
			}

			else if (!abm[i].increaseRadius)
				abm[i].explosionRadius -=0.5; 
				al_draw_filled_circle(abm[i].dest_x, abm[i].dest_y, abm[i].explosionRadius, al_map_rgb(r, g, b));
		}
	}
}


/*void drawExplosion(struct abmData * abm, ALLEGRO_BITMAP ** frameExplosion) {

	for (int i = 0; i < 30; i++) {
		if (abm[i].arrived && !abm[i].exploded) {
			al_draw_bitmap(frameExplosion[(abm[i].curr)], abm[i].dest_x, abm[i].dest_y, 0);
			abm[i].curr++;
			abm[i].curr %= 3;
		}

		if (abm[i].curr == 33) {
			abm[i].exploded = true;
		}
	}
}*/

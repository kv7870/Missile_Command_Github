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

void abmArrival(Abm * abm) {
	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].launched) {
			if (abm[i].num_increment > abm[i].step) {
				abm[i].launched = false;
				abm[i].arrived = true;
				//abm[i].num_increment = 1; 
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
		if (abm[i].arrived && !abm[i].exploded) {

			printf("radius: %f\n", abm[i].explosionRadius); 
			//printf("Destination: (%d, %d): arrive: %d\n", abm[i].dest_x, abm[i].dest_y, abm[i].arrived); 
			if (abm[i].explosionRadius >= 40) {
				abm[i].increaseRadius = false; 
			}

			if (abm[i].increaseRadius) {
				abm[i].explosionRadius +=1;
			}

			else if (!abm[i].increaseRadius) {
				abm[i].explosionRadius -= 1;
			}
				
			al_draw_filled_circle(abm[i].dest_x, abm[i].dest_y, abm[i].explosionRadius, al_map_rgb(r, g, b));

			//calculate bounds of explosion 
			/*abm[i].topLeft.x = abm[i].dest_x - abm[i].explosionRadius;
			abm[i].topLeft.y = abm[i].dest_y - abm[i].explosionRadius;
			abm[i].topRight.x = abm[i].dest_x + abm[i].explosionRadius;
			abm[i].topRight.y = abm[i].dest_y - abm[i].explosionRadius;
		
			abm[i].bottomLeft.x = abm[i].dest_x - abm[i].explosionRadius;
			abm[i].bottomLeft.y = abm[i].dest_y + abm[i].explosionRadius;
			abm[i].bottomRight.x = abm[i].dest_x + abm[i].explosionRadius;
			abm[i].bottomRight.y = abm[i].dest_y + abm[i].explosionRadius;*/

			if (abm[i].explosionRadius < 0) {
				abm[i].exploded = true;
			}
		}
	}
}


/*void drawExplosion(struct abmData * abm, ALLEGRO_BITMAP ** frameExplosion) {

	for (int i = 0; i < ABM_COUNT; i++) {
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

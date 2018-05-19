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

void abmArrival(struct abmData * abm) {
	int i; 

	for (i = 0; i < 30; i++) {
		if (abm[i].launched) {
			abm[i].distance = sqrt(pow((abm[i].dest_x - abm[i].x_pos), 2) + pow((abm[i].dest_y - abm[i].y_pos), 2)); 
			if (abm[i].distance <= 10) {
				abm[i].launched = false; 
				abm[i].used = true; 
			}
		}




	}
	



}
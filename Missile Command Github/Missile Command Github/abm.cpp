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
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


//launch an anti-ballistic missile when left mouse button is pressed
void fire(Abm * abm, Crosshair crosshair, Level * level, Audio * audio) {
	int i;
	bool closestLaunchSuccess = false;

	//if possible, only fire from the ABM battery closest to the coordinates of the mouse click 

	//launch from left battery if target is closest to the left side of the screen (< 300 px) 
	if (crosshair.target.x <= 300) {
		for (i = 0; i < 10; i++) {
			if (!abm[i].launched && !abm[i].arrived) {
				abm[i].pos.x = abm[i].launch.x;
				abm[i].pos.y = abm[i].launch.y;
				abm[i].dest.x = crosshair.target.x;
				abm[i].dest.y = crosshair.target.y;
				abm[i].launched = true;
				closestLaunchSuccess = true;
				(level->abmLeft)--;
				calcAbmInc(&(abm[i]));
				al_play_sample(audio->missileLaunch, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;
			}
		}
	}

	//launch from middle battery if target is closest to center portion of screen (between 300 and 600 px)
	else if (crosshair.target.x > 300 && crosshair.pos.x <= 600) {
		for (i = 10; i < 20; i++) {
			if (!abm[i].launched && !abm[i].arrived) {
				abm[i].pos.x = abm[i].launch.x;
				abm[i].pos.y = abm[i].launch.y;
				abm[i].dest.x = crosshair.target.x;
				abm[i].dest.y = crosshair.target.y;
				abm[i].launched = true;
				closestLaunchSuccess = true;
				(level->abmLeft)--;
				calcAbmInc(&(abm[i]));
				al_play_sample(audio->missileLaunch, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;
			}
		}
	}

	//launch from right battery if target is closest to right side of screen (>600 px)
	else if (crosshair.target.x > 600) {
		for (i = 20; i < 30; i++) {
			if (!abm[i].launched && !abm[i].arrived) {
				abm[i].pos.x = abm[i].launch.x;
				abm[i].pos.y = abm[i].launch.y;
				abm[i].dest.x = crosshair.target.x;
				abm[i].dest.y = crosshair.target.y;
				abm[i].launched = true;
				closestLaunchSuccess = true;
				(level->abmLeft)--;
				calcAbmInc(&(abm[i]));
				al_play_sample(audio->missileLaunch, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;
			}
		}
	}


	//if closest battery has run out of ABMs
	if (!closestLaunchSuccess) {
		for (i = 0; i < ABM_COUNT; i++) {
			if (!abm[i].launched && !abm[i].arrived) {
				abm[i].pos.x = abm[i].launch.x;
				abm[i].pos.y = abm[i].launch.y;
				abm[i].dest.x = crosshair.target.x;
				abm[i].dest.y = crosshair.target.y;
				abm[i].launched = true;
				(level->abmLeft)--;

				calcAbmInc(&(abm[i]));
				al_play_sample(audio->missileLaunch, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;
			}
		}
	}
}


//calculate the trajectory of the ABM using the digital differential algorithm 
void calcAbmInc(Abm * abm) {

	abm->dx = fabs(abm->dest.x - abm->launch.x);
	abm->dy = fabs(abm->dest.y - abm->launch.y);
	
	//increase x coordinates 
	if (abm->dx >= abm->dy) 
		abm->step = abm->dx;
	
	else 
		abm->step = abm->dy;
	
	//the # of pixels which the x and y coordinates of the ABM must increase by (every 1/60 of a second) 
	abm->inc.x = abm->dx / abm->step;
	abm->inc.y = abm->dy / abm->step;
}


//draw abm and trailing smoke 
void drawAbm(struct abmData * abm, int abmColour, int colorMap[][3]) {
	int r, g, b;

	r = rand() % 255 + 1;
	g = rand() % 255 + 1;
	b = rand() % 255 + 1;

	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].launched) {

			al_draw_filled_rectangle(abm[i].pos.x - 3, abm[i].pos.y - 3, abm[i].pos.x + 3, abm[i].pos.y + 3, al_map_rgb(colorMap[abmColour][R], colorMap[abmColour][G], colorMap[abmColour][B]));
			al_draw_line(abm[i].pos.x, abm[i].pos.y + 3, abm[i].launch.x, abm[i].launch.y, al_map_rgb(colorMap[abmColour][R], colorMap[abmColour][G], colorMap[abmColour][B]), 4);

			//hitmarkers
			al_draw_line(abm[i].dest.x - 7, abm[i].dest.y + 7, abm[i].dest.x + 7, abm[i].dest.y - 7, al_map_rgb(r, g, b), 3);
			al_draw_line(abm[i].dest.x - 7, abm[i].dest.y - 7, abm[i].dest.x + 7, abm[i].dest.y + 7, al_map_rgb(r, g, b), 3);
		}
	}
}


//update in buffer 
void updateAbm(struct abmData * abm) {
	int i;

	for (i = 0; i < ABM_COUNT; i++) {

		if (abm[i].launched) {  //only update launched & alive abm's 

			if (abm[i].dest.x > abm[i].launch.x) {
				abm[i].pos.x += abm[i].speed * abm[i].inc.x;
				//abm[i].num_increment += 10; 
			}
			else if (abm[i].dest.x < abm[i].launch.x) {
				abm[i].pos.x -= abm[i].speed * abm[i].inc.x;
				//abm[i].num_increment += 10; 
			}

			abm[i].pos.y -= abm[i].speed * abm[i].inc.y;
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
				explosion[i].center.x = abm[i].dest.x;
				explosion[i].center.y = abm[i].dest.y;
			}
		}
	}
}


void drawExplosion(Abm * abm, Explosion * explosion, int colorMap[][3]) {

	int palette[7][3] = { { 255, 0, 0 },{ 0, 255, 0 },{ 0, 0, 255 },{ 128, 128, 128 },{ 248, 6, 248 },{ 0, 255, 255 } };

	int color = rand() % NUM_COLORS;

	for (int i = 0; i < ABM_COUNT; i++) {
		if (explosion[i].ongoing) {

			//printf("Destination: (%d, %d): arrive: %d\n", abm[i].dest.x, abm[i].dest.y, abm[i].arrived); 
			if (explosion[i].radius >= 40 && !explosion[i].expandedRadius) {
				explosion[i].increaseRadius = false;
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

			al_draw_filled_circle(abm[i].dest.x, abm[i].dest.y, explosion[i].radius, al_map_rgb(palette[color][0], palette[color][1], palette[color][2]));

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
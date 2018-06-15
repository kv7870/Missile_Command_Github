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


//fire an anti-ballistic missile when left mouse button pressed
void fire(Abm * abm, Crosshair crosshair, Level * level, Audio * audio) {
	int i;
	bool closestLaunchSuccess = false;	//true if able to launch ABM from closest battery

	//if possible, fire from ABM battery that is closest to coordinates of mouse click 

	//launch from left battery if target is in left side of the screen (< 300 px) 
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
				calcAbmInc(&(abm[i]));	//calculate trajectory
				al_play_sample(audio->missileLaunch, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;
			}
		}
	}

	//launch from middle battery if target is is in center portion of screen (between 300 and 600 px)
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

	//launch from right battery if target is on right side of screen (>600 px)
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

	//if battery closest to target has run out of ABMs, fire an unused ABM from any battery
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

	//greater increase in x coordinate for every increase in y if delta x between launchpoint and target is greater than delta y 
	if (abm->dx >= abm->dy) {
		abm->step = abm->dx;
	}
	//greater increase in y coordinate for every increase in x if delta y between launchpoint and target is greater than delta x
	else {
		abm->step = abm->dy;
	}

	//the # of pixels which the x and y coordinates of the ABM must increase by (every 1/60 of a second)
	abm->inc.x = abm->dx / abm->step;	//either inc.x or inc.y is 1, i.e. either the x or y coordinate of the abm increases by 1 every every 1/60 s
	abm->inc.y = abm->dy / abm->step;	
}


//draw abm and trailing smoke 
void drawAbm(Abm * abm, int abmColour, int colorMap[][3]) {
	int r, g, b;

	r = rand() % 255 + 1;
	g = rand() % 255 + 1;
	b = rand() % 255 + 1;

	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].launched) {

			//draw abm at current position of pixel as calculated by digital differential analyzer 
			al_draw_filled_rectangle(abm[i].pos.x - 3, abm[i].pos.y - 3, abm[i].pos.x + 3, abm[i].pos.y + 3, al_map_rgb(colorMap[abmColour][R], colorMap[abmColour][G], colorMap[abmColour][B]));
			//draw line between current position of abm and its launch point to represent trailing smoke 
			al_draw_line(abm[i].pos.x, abm[i].pos.y + 3, abm[i].launch.x, abm[i].launch.y, al_map_rgb(colorMap[abmColour][R], colorMap[abmColour][G], colorMap[abmColour][B]), 4);

			//hitmarkers
			al_draw_line(abm[i].dest.x - 7, abm[i].dest.y + 7, abm[i].dest.x + 7, abm[i].dest.y - 7, al_map_rgb(r, g, b), 3);
			al_draw_line(abm[i].dest.x - 7, abm[i].dest.y - 7, abm[i].dest.x + 7, abm[i].dest.y + 7, al_map_rgb(r, g, b), 3);
		}
	}
}


//update position of abm  
void updateAbm(Abm * abm) {
	int i;

	for (i = 0; i < ABM_COUNT; i++) {

		if (abm[i].launched) {  //only update ABMs currently en route 

			if (abm[i].dest.x > abm[i].launch.x) {
				abm[i].pos.x += abm[i].speed * abm[i].inc.x;	//move ABM right by predetermined increment if its destination is right of launch point
																
			}
			else if (abm[i].dest.x < abm[i].launch.x) {
				abm[i].pos.x -= abm[i].speed * abm[i].inc.x;	//move ABM left by predetermined increment if its destination is left of launch point 
																
			}

			abm[i].pos.y -= abm[i].speed * abm[i].inc.y;	//increase y position by predetermined increment
			abm[i].num_increment += abm[i].speed;
		}
	}
}


//determine whether ABM has arrived at its destination 
void abmArrival(Abm * abm, Explosion * explosion) {
	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].launched) {

			//if an ABM has completed the number of increments necessary to reach target, it has arrived
			if (abm[i].num_increment > abm[i].step) {	
				abm[i].launched = false;
				abm[i].arrived = true;
				explosion[i].ongoing = true;	//explosion occurs at destination of abm 
				explosion[i].center.x = abm[i].dest.x;
				explosion[i].center.y = abm[i].dest.y;
			}
		}
	}
}


//draw explosion when an ABM arrives at target 
void drawExplosion(Abm * abm, Explosion * explosion, int colorMap[][3], Level * level) {
	if (level->abmLeft == 0 )
		level->speedUp = true;	//speed up enemy missiles if all abms used up 

	//flashing colours for explosion 
	int palette[7][3] = { { 255, 0, 0 },{ 0, 255, 0 },{ 0, 0, 255 },{ 128, 128, 128 },{ 248, 6, 248 },{ 0, 255, 255 } };

	int color = rand() % NUM_COLORS;

	for (int i = 0; i < ABM_COUNT; i++) {
		if (abm[i].launched) {
			level->speedUp = false;
		}

		if (explosion[i].ongoing) {
			level->speedUp = false; 

			//explosion expands & contracts 
			if (explosion[i].radius >= 40 && !explosion[i].expandedRadius) {
				explosion[i].increaseRadius = false;
			}
			//explosion radius can increase to 80 px if it destroys an enemy 
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
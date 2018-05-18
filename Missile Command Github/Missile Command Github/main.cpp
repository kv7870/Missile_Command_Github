// Missile Command Summative.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h> 
#include <allegro5/allegro.h> 
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "header.h"

int main(int argc, char **argv)
{
	//allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *imageCrosshair = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;

	//object variable 
	struct crosshairData crosshair;
	struct abmData abm[30]; //30 anti-ballstic missiles

	int lives = 3;

	initAllegro(&display, &timer, &imageCrosshair, &event_queue);

	initCrosshair(&crosshair, imageCrosshair);

	initAbm(abm);

	playerMovement(display, timer, imageCrosshair, event_queue, &crosshair, abm);


	//al_set_target_bitmap(image);

	//al_set_target_bitmap(al_get_backbuffer(display));


	return 0;
}

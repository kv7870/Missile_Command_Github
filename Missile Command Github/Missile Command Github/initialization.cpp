#include "stdafx.h"
#include <stdio.h>
#include <allegro5/allegro.h> // Include the allegro header file.
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "header.h"

int initAllegro(ALLEGRO_DISPLAY **display, ALLEGRO_TIMER **timer, ALLEGRO_BITMAP **imageCrosshair, ALLEGRO_EVENT_QUEUE **event_queue) {

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	//create display
	*display = al_create_display(SCREEN_W, SCREEN_H);
	if (!(*display)) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(*timer);
		return -1;
	}

	//create timer
	*timer = al_create_timer(1.0 / FPS);   //1/60 seconds per frame, i.e. 60 fps
	if (!(*timer)) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	//create event queue 
	*event_queue = al_create_event_queue();
	if (!(*event_queue)) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_bitmap(*imageCrosshair);
		al_destroy_display(*display);
		al_destroy_timer(*timer);
		return -1;
	}

	//install keyboard 
	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}

	//install mouse 
	if (!al_install_mouse()) {
		fprintf(stderr, "failed to initialize the mouse!\n");
		return -1;
	}

	if (!al_init_primitives_addon()) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to initialize image addon!",
			nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	//initialize image addon 
	if (!al_init_image_addon()) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to initialize image addon!",
			nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	//create ship image
	*imageCrosshair = al_load_bitmap("crosshair.png");
	if (!imageCrosshair) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	//register 
	al_register_event_source(*event_queue, al_get_display_event_source(*display));
	al_register_event_source(*event_queue, al_get_timer_event_source(*timer));
	al_register_event_source(*event_queue, al_get_keyboard_event_source());
	al_register_event_source(*event_queue, al_get_mouse_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_start_timer(*timer);
}

//spawn crosshair at center of screen y
void initCrosshair(struct crosshairData * crosshair, ALLEGRO_BITMAP * imageCrosshair) {
	crosshair->x = 450;
	crosshair->y = 450;
	crosshair->height = al_get_bitmap_height(imageCrosshair);
	crosshair->width = al_get_bitmap_width(imageCrosshair);
}

//initialize abm 
void initAbm(struct abmData * abm) {
	int i;

	for (i = 0; i < 30; i++) {
		abm[i].dest_x = 0;
		abm[i].dest_y = 0;
		abm[i].used = false;
		abm[i].launched = false;
	}

	for (i = 0; i < 10; i++) {
		abm[i].launch_x = 20;
		abm[i].launch_y = 50;
		abm[i].speed = 5;
	}

	for (i = 10; i < 20; i++) {
		abm[i].launch_x = 250;
		abm[i].launch_y = 50;
		abm[i].speed = 10;
	}
	for (i = 20; i < 30; i++) {
		abm[i].launch_x = 850;
		abm[i].launch_y = 50;
		abm[i].speed = 5;
	}
}